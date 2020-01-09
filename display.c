#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "display.h"
#include "mineplex.h"

static void _display_putchar(display_t *d, char c, unsigned x, unsigned y, unsigned scale, uint16_t fg, uint16_t bg)
{
    unsigned ypos = y;
    const uint8_t *data = mineplex_char(c);
    for (unsigned row = 0; row < 5; row++) {
        unsigned xpos = x;
        uint8_t row_data = *data++;
        for (unsigned col = 0; col < 5; col++) {
            uint16_t color = (row_data & (1 << (col))) ? fg : bg;
            display_fill(d, xpos, ypos, scale, scale, color);
            xpos += scale;
        }
        ypos += scale;
    }
}

void display_putchar(display_txt_t *dt, char c)
{
    const unsigned char_width = (5+1) * dt->scale;
    switch (c) {
        case '\n':
            dt->xpos = 5;
            /* fall through */
        case '\r':
            dt->ypos += char_width;
            break;
        default:
            {
                _display_putchar(dt->display, c, dt->xpos, dt->ypos, dt->scale, dt->fg, dt->bg);
                dt->xpos += char_width;
                if ((dt->xpos + char_width) >= dt->width) {
                    dt->xpos = 5;
                    dt->ypos += char_width;
                }
            }
    }

    /* TODO: shift stuff up */
    if ((dt->ypos + char_width) >= (dt->height)) {
        dt->ypos = 5;
        display_fill(dt->display, 5, 5, dt->width - 5, 5+1, dt->bg);
    }
}

void display_txt_goto(display_txt_t *dt, int x, int y)
{
    /* make sure previously buffered stdio is printed before messing with the
     * position
     *
     * Fixes this:
     *
     * printf("foo"); // no \n, thus buffered
     * display_txt_goto(...);
     * printf("bar\n"); // libc flushes libc buffer, calls "stdio_write()" 
     *                  // at new position for whole text.
     *
     */
    fflush(stdout);

    const unsigned char_width = (5+1) * dt->scale;

    dt->xpos += x*(int)char_width;
    dt->ypos += y*(int)char_width;
}

display_txt_t *stdio_display_dev;

void stdio_init(void) {
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    if (stdio_display_dev == NULL) {
        return len;
    }

    const uint8_t *_buffer = buffer;
    size_t _len = len;
    while (_len--) {
        display_putchar(stdio_display_dev, *_buffer++);
    }

    return len;
}
ssize_t stdio_read(const void* buffer, size_t len)
{
    (void)buffer;
    (void)len;
    return -ENOTSUP;
}
