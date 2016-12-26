
#include "mmc.h"
#include "consts_device.h"

#if PA23 || PA21 || AS97 || AS87 || DL40
#define OLD_MMC_CMD 1
#define MMC_READ  2
#define MMC_WRITE 0
#endif

#define MMC_SEND_EXT_CSD             8   /* adtc                    R1  */
#define MMC_CLR_WRITE_PROT          29   /* ac   [31:0] data addr   R1b */
#define MMC_SEND_WRITE_PROT_TYPE    31   /* adtc [31:0] data addr   R1  */
#define MMC_GEN_CMD                 56   /* adtc [0] RD/WR          R1  */

#define EXT_CSD_SEC_CNT             212
#define HC_WP_GRP_SIZE              221
#define HC_ERASE_GRP_SIZE           224

#if defined(OLD_MMC_CMD)
struct mmc_cmd {
    int cmd;
    int cmd_arg;
    int resp_type;
    int resp[4];
    int prog_scan;
};
#else
struct mmc_cmd {
    int cmd;
    int cmd_arg;
    int resp_type;
    int resp[4];
    int prog_scan;
    int status;
    int flags;
};
#endif

typedef int *(*mmc_init_t)(int, int);
typedef int (*mmc_fini_t)(int *);
typedef int (*mmc_send_cmd_t)(int, struct mmc_cmd *);
#if defined(OLD_MMC_CMD)
typedef int (*mmc_transfer_t)(int, struct mmc_cmd *, int, void *, int);
#else
typedef int (*mmc_transfer_t)(int, struct mmc_cmd *, void *, int);
#endif

typedef int (*mmc_func3_t)(int *, char *, int *);
typedef int (*mmc_func2_t)(int *, char *);
typedef int (*mmc_wpoff_t)(int *);

typedef int (*mmc_part_open_impl_t)(const char *);
typedef int (*mmc_part_close_impl_t)(int);
typedef int (*mmc_part_read_impl_t)(int, int, char *, int);
typedef int (*mmc_part_write_impl_t)(int, int, const char *, int);

const static mmc_init_t mmc_init = (mmc_init_t)(FUNC_MMC_INIT + 1);
const static mmc_fini_t mmc_fini = (mmc_fini_t)(FUNC_MMC_FINI + 1);
const static mmc_send_cmd_t mmc_send_cmd = (mmc_send_cmd_t)(FUNC_MMC_SEND_CMD + 1);
const static mmc_transfer_t mmc_transfer = (mmc_transfer_t)(FUNC_MMC_TRANSFER + 1);
const static mmc_func3_t mmc_func3 = (mmc_func3_t)(FUNC_MMC_FUNC3 + 1);
const static mmc_func2_t mmc_func2 = (mmc_func2_t)(FUNC_MMC_FUNC2 + 1);
const static mmc_wpoff_t mmc_wpoff = (mmc_wpoff_t)(FUNC_MMC_WPOFF + 1);
const static mmc_part_open_impl_t mmc_part_open_impl = (mmc_part_open_impl_t)(FUNC_MMC_PART_OPEN_IMPL + 1);
const static mmc_part_close_impl_t mmc_part_close_impl = (mmc_part_close_impl_t)(FUNC_MMC_PART_CLOSE_IMPL + 1);
const static mmc_part_read_impl_t mmc_part_read_impl = (mmc_part_read_impl_t)(FUNC_MMC_PART_READ_IMPL + 1);
const static mmc_part_write_impl_t mmc_part_write_impl = (mmc_part_write_impl_t)(FUNC_MMC_PART_WRITE_IMPL + 1);

static int get_gen_cmd(const char *pass, char *data) {
    const char *src;
    char *dst;
    int i;

    src = pass;
    dst = data;
    for (i = 0; i < 0x28; i++)
        dst[i] = 0;
    while (*src) {
        char i, off, cnt;

        off = *src >> 4;
        cnt = *src & 0x0F;
        src++;
        dst += off;
        for (i = 0; i < cnt; i++) {
            *dst++ = *src++;
        }
    }

    return 0;
}

int mmc_kill_shwp(const char *pass) {
    int *handle;
    int i, x = 0;
    int card_size, wpgrp_size;
    int out[2];
    struct mmc_cmd args;
    char data[512];
    char extcsd[512];

    get_gen_cmd(pass, data);
    handle = mmc_init(0, 0);
    mmc_func3(handle, data, &out[0]);
    mmc_func2(handle, data);
    mmc_wpoff(handle);
    //
    x = 0;
    args.cmd = MMC_SEND_EXT_CSD;
    args.cmd_arg = 0;
    args.resp_type = 1;
    args.prog_scan = 0;
#if defined(OLD_MMC_CMD)
    x += mmc_transfer(*handle, &args, MMC_READ, &extcsd[0], 1);
#else
    args.flags = 0;
    x += mmc_transfer(*handle, &args, &extcsd[0], 1);
#endif
    // no legacy support
    card_size = extcsd[EXT_CSD_SEC_CNT] |
        extcsd[EXT_CSD_SEC_CNT + 1] << 8 |
        extcsd[EXT_CSD_SEC_CNT + 2] << 16 |
        extcsd[EXT_CSD_SEC_CNT + 3] << 24;
    wpgrp_size = extcsd[HC_ERASE_GRP_SIZE] * extcsd[HC_WP_GRP_SIZE] * 1024;
    //
    if (card_size && wpgrp_size) {
        int n = card_size / wpgrp_size;
        int o;

        // 1
        for (o = 0; o < sizeof(data); o++)
            data[o] = 0;
        get_gen_cmd(pass, data);
        data[0x20] = 2;
        args.cmd = MMC_GEN_CMD;
        args.resp_type = 1;
        args.prog_scan = 0;
        args.cmd_arg = 0;
#if defined(OLD_MMC_CMD)
        x += mmc_transfer(*handle, &args, MMC_WRITE, data, 1);
#else
        args.flags = 1;
        x += mmc_transfer(*handle, &args, data, 1);
#endif
        args.cmd_arg = 1;
#if defined(OLD_MMC_CMD)
        x += mmc_transfer(*handle, &args, MMC_READ, data, 1);
#else
        args.flags = 2;
        x += mmc_transfer(*handle, &args, data, 1);
#endif
        // 2
        for (o = 0; o < sizeof(data); o++)
            data[o] = 0;
        get_gen_cmd(pass, data);
        data[0x20] = 1;
        args.cmd = MMC_GEN_CMD;
        args.resp_type = 1;
        args.prog_scan = 0;
        args.cmd_arg = 0;
#if defined(OLD_MMC_CMD)
        x += mmc_transfer(*handle, &args, MMC_WRITE, data, 1);
#else
        args.flags = 1;
        x += mmc_transfer(*handle, &args, data, 1);
#endif
        args.cmd_arg = 1;
#if defined(OLD_MMC_CMD)
        x += mmc_transfer(*handle, &args, MMC_READ, data, 1);
#else
        args.flags = 2;
        x += mmc_transfer(*handle, &args, data, 1);
#endif
        for (i = 0; i < n; i++) {
            // 3
            args.cmd = MMC_CLR_WRITE_PROT;
            args.cmd_arg = i * wpgrp_size;
            args.resp_type = 1;
            args.prog_scan = 1;
#if !(defined(OLD_MMC_CMD))
            args.flags = 0;
#endif
            x += mmc_send_cmd(*handle, &args);
        }
    }
    mmc_fini(handle);

    return x;
}

static int mmc_part_open(int part) {
	int i = 0;
	char device[32];

	if (part < 0 || part > 99)
		return 0;
	for (i = 0; i < sizeof(device); i++)
		device[i] = 0;
	i = 0;
	device[i++] = '/';
	device[i++] = 'h';
	device[i++] = 'd';
	device[i++] = 'e';
	device[i++] = 'v';
	device[i++] = '/';
	device[i++] = 's';
	device[i++] = 'd';
	device[i++] = 'c';
	device[i++] = '1';
	device[i++] = 'h';
	device[i++] = '0';
	device[i++] = 'p';
	if (part > 10)
		device[i++] = '0' + (part / 10);
	device[i++] = '0' + (part % 10);
	return mmc_part_open_impl(device);
}

static int mmc_part_close(int handle) {
	return mmc_part_close_impl(handle);
}

int mmc_part_read(int part, unsigned int off, char *data, unsigned int cnt) {
	int res = -1;
	int h;

	h = mmc_part_open(part);
	if (h) {
		res = mmc_part_read_impl(h, off, data, cnt);
		mmc_part_close(h);
	}
	return res;
}

int mmc_part_write(int part, unsigned int off, const char *data, unsigned int cnt) {
	int res = -1;
	int h;

	h = mmc_part_open(part);
	if (h) {
        res = mmc_part_write_impl(h, off, data, cnt);
        mmc_part_close(h);
    }
    return res;
}

