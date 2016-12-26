
#include "screen.h"
#include "graphic.h"
#include "consts_device.h"

#if DISPLAY_1080
#include "logo_1080.c"
#elif DISPLAY_720
#include "logo_720.c"
#else
#error "Not implemented yet."
#endif

static void screen_fill_image(const void *data, int size) {
#if DISPLAY_1080
    int w = 1080, h = 1920;
#elif DISPLAY_720
    int w = 720, h = 1280;
#else
#error "Not implemented yet."
#endif
    struct image r;
    unsigned short *p = (unsigned short *) data;
    unsigned char *out = (unsigned char *) VIDEO_BUFFER_ADDR;
    int i = 0, j;

    while (i < w * h) {
        unsigned short n, c;
        unsigned char rr, gg, bb;

        if ((void *)p - data >= size)
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
    if (i != w * h)
        return;
    r.data= (void *) VIDEO_BUFFER_ADDR;
    r.src_width = w;
    r.src_x = 0;
    r.src_y = 0;
    r.width = w;
    r.height = h;
    r.dst_x = 0;
    r.dst_y = 0;
    draw_bgn();
    draw_image(&r);
    draw_end();
}

void screen_splash() {
    screen_fill_image(logo_data, logo_size);
}

void screen_fill_color(int c) {
    struct rect r;

    r.color = c;
    r.left = 0;
    r.top = 0;
#if DISPLAY_1080
    r.right = 1279;
    r.bottom = 1919;
#elif DISPLAY_720
    r.right = 719;
    r.bottom = 1279;
#else
#error "Not implemented yet."
#endif
    draw_bgn();
    draw_rect(&r);
    draw_end();
}

