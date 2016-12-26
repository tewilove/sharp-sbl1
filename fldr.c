
#include "fldr.h"
#include "string.h"
#include "insn.h"
#if AS87 || AS97 || PA21 || PA23 || PB25 || PA28 || DL40 || DL75
#include "consts_msm8974.h"
#else
#error "Not implemented."
#endif

typedef int (*usb_tx_t)(const char *, int);

extern void _abort();

extern void fldr_init_ng();

usb_tx_t _usb_tx = 0;
shdiag_read_t shdiag_read = 0;
shdiag_write_t shdiag_write = 0;

sh_reboot_t sh_reboot = 0;
sh_power_off_t sh_power_off = 0;

spmi_ext_register_readl_t spmi_ext_register_readl = 0;
spmi_ext_register_writel_t spmi_ext_register_writel = 0;

struct t32_bl_pat {
    unsigned char *data;
    int size;
};

/* mov   r1, r0
 * movs  r3, #0x80
 * movs  r0, #2
 * mov.w r2, #0x5800
 */
static unsigned char shdiag_read1[] = {
    0x01, 0x46, 0x80, 0x23, 0x02, 0x20, 0x4f, 0xf4, 0xb0, 0x42
};

static struct t32_bl_pat pat_shdiag_read[] = {
    { shdiag_read1, sizeof(shdiag_read1) },
    { 0, 0 }
};

/* movs  r3, #0xd
 * movs  r2, #0xf0
 * movs  r0, #1
 * mov   r1, sp
 */
static unsigned char shdiag_write1[] = {
    0x0d, 0x23, 0xf0, 0x22, 0x01, 0x20, 0x69, 0x46
};

/* movs  r3, #1
 * movs  r0, #3
 * mov.w r2, #0x607
 * mov   r1, sp
 */
static unsigned char shdiag_write2[] = {
    0x01, 0x23, 0x03, 0x20, 0x40, 0xf2, 0x07, 0x62, 0x69, 0x46
};

/* movs  r3, #0xd
 * movs  r0, #3
 * mov.w r2, #0x100
 * mov   r1, sp
 */
static unsigned char shdiag_write3[] = {
    0x0d, 0x23, 0x03, 0x20, 0x4f, 0xf4, 0x80, 0x72, 0x69, 0x46
};

static struct t32_bl_pat pat_shdiag_write[] = {
    { shdiag_write1, sizeof(shdiag_write1) },
    { shdiag_write2, sizeof(shdiag_write2) },
    { shdiag_write3, sizeof(shdiag_write3) },
    { 0, 0 }
};

static unsigned char fixed_usb_resp[] = {
    0x01, 0x02, 0x00, 0xfc,
    0x01, 0x02, 0x01, 0xfb,
    0x01, 0x02, 0x02, 0xfa,
    0x01, 0x02, 0x03, 0xf9,
};

void abort() {
    _abort();
}

/* fixed_usb_tx:
 *     ldr r1, pp_resp_tbl
 *     ldr.w r0, [r1, r0, lsl #2]
 *     movs r1, #4
 *     b.w usb_tx
 * pp_resp_tbl:
 *     .word p_resp_tbl
 */
static uint32_t find_usb_tx() {
    void *tmp;
    uint32_t p_resp_tbl = 0;
    uint32_t *p;
    int i;

    tmp = memmem((const void *) SBL1_BASE, SBL1_SIZE, fixed_usb_resp, sizeof(fixed_usb_resp));
    if (!tmp)
        return 0;
    for (p = (uint32_t *)(tmp - 4), i = 32; ((uint32_t) p >= SBL1_BASE) && i; p--, i--) {
        if (*p == (uint32_t) tmp) {
            p_resp_tbl = (uint32_t) p;
            break;
        }
    }
    if (!p_resp_tbl)
        return 0;
    tmp = memmem((const void *) SBL1_BASE, SBL1_SIZE, &p_resp_tbl, sizeof(p_resp_tbl));
    if (!tmp)
        return 0;

    return ((uint32_t) tmp - 4);
}

static uint32_t find_t32_bl(struct t32_bl_pat *pat) {
    void *tmp = NULL;

    if (!pat)
        return 0;
    while (pat->data) {
        tmp = memmem((void *) SBL1_BASE, SBL1_SIZE, pat->data, pat->size);
        if (tmp)
            break;
        pat++;
    }
    if (!tmp)
        return 0;
    return decode_t32_bl((uint32_t)(tmp + pat->size),
        *((uint32_t *)(tmp + pat->size)));
}

int usb_tx(const char *data, int size) {
    if (_usb_tx) {
        uint32_t head = (uint32_t) size;
#if 1
        int nb = 0, c;

        _usb_tx((const char *) &head, sizeof(head));
        while (size) {
            c = size - nb;
            if (c > 512)
                c = 512;
            _usb_tx(data + nb, c);
            nb += c;
            size -= c;
        }
#else
        _usb_tx((const char *) &head, sizeof(head));
        _usb_tx(data, size);
#endif
        return 0;
    }
    return -1;
}

void fldr_init() {
    uint32_t addr;

    // maybe reusable
    addr = find_usb_tx();
    if (addr)
        _usb_tx = (usb_tx_t)(addr | 1);
    addr = find_t32_bl(pat_shdiag_read);
    if (addr)
        shdiag_read = (shdiag_read_t)(addr | 1);
    addr = find_t32_bl(pat_shdiag_write);
    if (addr)
        shdiag_write = (shdiag_write_t)(addr | 1);
    // maybe device specific
    fldr_init_ng();
}

