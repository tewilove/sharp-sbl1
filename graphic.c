
#include "graphic.h"
#include "consts_device.h"
#include "font.h"
#include "string.h"

typedef int (*display_init_t)(void);

typedef int (*draw_bgn_t)(void);
typedef int (*draw_end_t)(void);
typedef int (*draw_image_t)(const struct image *);
typedef int (*draw_rect_t)(const struct rect *);

const static display_init_t __display_init = (display_init_t)(FUNC_DISP_INIT + 1);
const static draw_bgn_t __draw_bgn = (draw_bgn_t)(FUNC_DRAW_INIT + 1);
const static draw_end_t __draw_end = (draw_end_t)(FUNC_DRAW_FINI + 1);
const static draw_image_t __draw_image = (draw_image_t)(FUNC_DRAW_IMAGE + 1);
const static draw_rect_t __draw_rect = (draw_rect_t)(FUNC_DRAW_RECT + 1);

static int g_font_init = 0;

int display_init(void) {
    if (!g_font_init) {
        font_init();
        g_font_init = 1;
    }
    return __display_init();
}

int draw_bgn(void) {
    return __draw_bgn();
}

int draw_end(void) {
    return __draw_end();
}

int draw_image(const struct image *r) {
    return __draw_image(r);
}

int draw_rect(const struct rect *r) {
    return __draw_rect(r);
}

int draw_text(int x, int y, const char *str, int fg, int bg) {
    struct image r;
    int i, l;
    char ch, *image;

    if (!str)
        return 0;
    l = strlen(str);
    if (l == 0)
        return 0;
    draw_bgn();
    for (i = 0; i < strlen(str); i++) {
        ch = str[i];
        if (ch < 0x20 || ch > 0x7e)
            ch = '?';
        ch -= 0x20;
        image = (char *)(FONT_BUFFER_ADDR + ch * FONT_HEIGHT * FONT_WIDTH * 3);
        r.data = image;
        r.src_width = FONT_WIDTH;
        r.src_x = 0;
        r.src_y = 0;
        r.width = FONT_WIDTH;
        r.height = FONT_HEIGHT;
        r.dst_x = x + i * FONT_WIDTH;
        r.dst_y = y;
        draw_image(&r);
    }
    draw_end();
    return 0;
}

