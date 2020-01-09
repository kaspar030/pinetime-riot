#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

typedef struct display display_t;
struct display {
    int (*put)(display_t *dev, unsigned x, unsigned y, unsigned w, unsigned h,
               const uint16_t *picture);
    int (*fill)(display_t *dev, unsigned x, unsigned y, unsigned w, unsigned h,
                uint16_t color);
};

static inline int display_put(display_t * dev, unsigned x, unsigned y, unsigned w,
                          unsigned h, const uint16_t *picture)
{
    return dev->put(dev, x, y, w, h, picture);
}

static inline int display_fill(display_t * dev, unsigned x, unsigned y, unsigned w,
                           unsigned h, uint16_t color)
{
    return dev->fill(dev, x, y, w, h, color);
}

typedef struct {
    display_t *display;
    unsigned xpos;
    unsigned ypos;
    unsigned scale;
    unsigned width;
    unsigned height;
    uint16_t fg;
    uint16_t bg;
} display_txt_t;

void display_putchar(display_txt_t *dt, char c);
void display_txt_goto(display_txt_t *dt, int x, int y);

extern display_txt_t *stdio_display_dev;

#endif /* DISPLAY_H */
