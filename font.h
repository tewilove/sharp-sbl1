
#ifndef _FONT_H_
#define _FONT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_WIDTH 40
#define FONT_HEIGHT 64
// size in pixels
#define FONT_SIZE (95 * FONT_WIDTH * FONT_HEIGHT)

// compressed RGB565 font data
extern const char font_data[];
extern const int font_size;

int font_init();

#ifdef __cplusplus
}
#endif

#endif

