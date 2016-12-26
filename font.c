
#include "font.h"
#include "consts_device.h"

int font_init() {
    unsigned short *p = (unsigned short *) font_data;
    unsigned char *out = (unsigned char *) FONT_BUFFER_ADDR;
    int i = 0, j;

    while (i < FONT_SIZE) {
        unsigned short n, c;
        unsigned char rr, gg, bb;

        if ((void *)(p) - (void *)(font_data) >= font_size)
            break;
        n = *p++;
        c = *p++;
        rr = c * 8 + 3;
        gg = 4 * ((c >> 5) & 0x3f) + 1;
        bb = 8 * (c >> 11) + 3;
        for (j = 0; j < n; j++) {
            *out++ = rr;
            *out++ = gg;
            *out++ = bb;
        }
        i += n;
    }
    if (i != FONT_SIZE)
        return - 1;
    return 0;
}

