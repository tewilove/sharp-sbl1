
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#ifdef __cplusplus
extern "C" {
#endif

struct image {
    void *data;
    short src_width;
    short src_x;
    short src_y;
    short width;
    short height;
    short dst_x;
    short dst_y;
};

struct rect {
    int color;
    short left;
    short top;
    short right;
    short bottom;
};

int display_init();

int draw_bgn();
int draw_end();
int draw_image(const struct image *);
int draw_rect(const struct rect *);
int draw_text(int, int, const char*, int, int);

#ifdef __cplusplus
}
#endif

#endif

