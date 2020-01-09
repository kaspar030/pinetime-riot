/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     applications
 * @{
 *
 * @file
 * @brief       PineTime watch main application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <errno.h>

#include "board.h"
#include "ili9341.h"
#include "ili9341_params.h"
#include "display.h"
#include "riot_logo.h"
#include "periph/rtt.h"
#include "ztimer.h"
#include "ztimer/extend.h"
#include "ztimer/rtt.h"

typedef struct {
    display_t super;
    ili9341_t dev;
} display_ili9341_t;

static int _ili9341_display_put(display_t *dev, unsigned x, unsigned y, unsigned w, unsigned h, const uint16_t *picture)
{
    display_ili9341_t *ili_dev = (display_ili9341_t *)dev;
    ili9341_map(&ili_dev->dev, x, x+w, y, y+h, picture);
    return 0;
}

static int _ili9341_display_fill(display_t *dev, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t color)
{
    display_ili9341_t *ili_dev = (display_ili9341_t *)dev;
    ili9341_fill(&ili_dev->dev, x, x+w, y, y+h, color);
    return 0;
}

static int display_init_ili9341(display_ili9341_t *dev, const ili9341_params_t *params)
{
    return ili9341_init(&dev->dev, params);
}

static display_ili9341_t ili_lcd = {
    .super.put = _ili9341_display_put,
    .super.fill = _ili9341_display_fill,
};

static display_t *lcd = &ili_lcd.super;

static display_txt_t tw = {
    .display = &ili_lcd.super,
    .scale = 2,
    .width = 240,
    .height = 240,
    .fg = 0x0000,
    .bg = 0xffff,
    .xpos = 5,
    .ypos = 5,
};

/* set interval to 1 second */
#define INTERVAL (1U * US_PER_SEC)

/* ztimer configuration */
/* nrf5x RTT defaults to 1024kHz, 24bit. */

ztimer_rtt_t _rtt;
ztimer_extend_t _rtt_extended;
ztimer_dev_t *ZTIMER_RTT = (ztimer_dev_t *)&_rtt_extended;

int main(void)
{
    ztimer_rtt_init(&_rtt);
    ztimer_extend_init(&_rtt_extended, (ztimer_dev_t *)&_rtt, 24);

    if (display_init_ili9341(&ili_lcd, &ili9341_params[0]) != 0) {
        puts("error initializing display");
    }
    stdio_display_dev = &tw;
    display_fill(lcd, 0, 0, 240, 240, 0xffff);

    gpio_clear(VCC33);
    gpio_clear(LCD_BACKLIGHT_LOW);
    gpio_clear(LCD_BACKLIGHT_MID);
    gpio_set(VIBRATOR);


    while(1) {
        uint32_t now = ztimer_now(ZTIMER_RTT);
        display_txt_goto(stdio_display_dev, 5, 9);
        printf("%"PRIu32"\n", now);
    }

    return 0;
}
