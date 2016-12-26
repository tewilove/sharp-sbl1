
#include "fldr.h"
#include "mmc.h"

#define FLDR_PAYLOAD_SIZE (240 * 1024)

struct fldr_payload {
    int magic;
    int partition;
    unsigned int offset;
    unsigned int count;
    char data[0];
} __attribute__((packed));

static char payload[sizeof(struct fldr_payload) + FLDR_PAYLOAD_SIZE] = {
    'F', 'L', 'D', 'R',
};

#ifdef PA23
const static char password[0x28] = {
    0x01, 0x0d, 0x34, 0x44, 0x0d, 0x49, 0x1c, 0xf0, 0xd4, 0x7a, 0x67, 0xac, 0x09, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xae,
};
#else
#error "Not implemented yet."
#endif

int fldr_main() {
    int ret = 0;
    struct fldr_payload *p = (struct fldr_payload *) payload;

    if (p->magic == 0x52444C46 && 
        p->partition && p->count) {
        ret += mmc_kill_shwp(password);
        ret += mmc_part_write(p->partition, p->offset, &p->data[0], p->count);
    }
    usb_tx((const char *) &ret, sizeof(ret));

    return 0;
}

