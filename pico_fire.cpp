/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico_display.hpp"
#include <string>

const uint32_t FPS = 1000 / 27; // DoomFire runs at 27FPS.
uint32_t frames = 0;
uint32_t last_time = 0;
uint16_t buffer[pimoroni::PicoDisplay::WIDTH * pimoroni::PicoDisplay::HEIGHT];
pimoroni::PicoDisplay pico_display(buffer);

uint8_t fire[pimoroni::PicoDisplay::WIDTH * pimoroni::PicoDisplay::HEIGHT];

uint16_t pallete[36] = { // 36 colours
        pico_display.create_pen(0x07, 0x07, 0x07), pico_display.create_pen(0x1f, 0x07, 0x07),
        pico_display.create_pen(0x2f, 0x0f, 0x07), pico_display.create_pen(0x47, 0x0f, 0x07),
        pico_display.create_pen(0x57, 0x17, 0x07), pico_display.create_pen(0x67, 0x1f, 0x07),
        pico_display.create_pen(0x77, 0x1f, 0x07), pico_display.create_pen(0x8f, 0x27, 0x07),
        pico_display.create_pen(0x9f, 0x2f, 0x07), pico_display.create_pen(0xaf, 0x3f, 0x07),
        pico_display.create_pen(0xbf, 0x47, 0x07), pico_display.create_pen(0xc7, 0x47, 0x07),
        pico_display.create_pen(0xDF, 0x4F, 0x07), pico_display.create_pen(0xDF, 0x57, 0x07),
        pico_display.create_pen(0xDF, 0x57, 0x07), pico_display.create_pen(0xD7, 0x5F, 0x07),
        pico_display.create_pen(0xD7, 0x67, 0x0F), pico_display.create_pen(0xcf, 0x6f, 0x0f),
        pico_display.create_pen(0xcf, 0x77, 0x0f), pico_display.create_pen(0xcf, 0x7f, 0x0f),
        pico_display.create_pen(0xCF, 0x87, 0x17), pico_display.create_pen(0xC7, 0x87, 0x17),
        pico_display.create_pen(0xC7, 0x8F, 0x17), pico_display.create_pen(0xC7, 0x97, 0x1F),
        pico_display.create_pen(0xBF, 0x9F, 0x1F), pico_display.create_pen(0xBF, 0x9F, 0x1F),
        pico_display.create_pen(0xBF, 0xA7, 0x27), pico_display.create_pen(0xBF, 0xA7, 0x27),
        pico_display.create_pen(0xBF, 0xAF, 0x2F), pico_display.create_pen(0xB7, 0xAF, 0x2F),
        pico_display.create_pen(0xB7, 0xB7, 0x2F), pico_display.create_pen(0xB7, 0xB7, 0x37),
        pico_display.create_pen(0xCF, 0xCF, 0x6F), pico_display.create_pen(0xDF, 0xDF, 0x9F),
        pico_display.create_pen(0xEF, 0xEF, 0xC7), pico_display.create_pen(0xFF, 0xFF, 0xFF)
};

int8_t wind = 0;

int posAt(int x, int y) {
    return y * pimoroni::PicoDisplay::WIDTH + x;
}

void init() {
    pico_display.init();
    pico_display.set_backlight(255);
    pico_display.set_pen(0, 0, 0);
    pico_display.clear();

    // Initialises the screen
    for (int x = 0; x < pimoroni::PicoDisplay::WIDTH; x++) {
        for (int y = 0; y < pimoroni::PicoDisplay::HEIGHT; y++) {
            uint32_t pos = posAt(x, y);
            fire[pos] = 0;
        }
    }

    // Initialises the bottom line
    for (int i = 0; i < pimoroni::PicoDisplay::WIDTH; i++) {
        uint32_t pos = posAt(i, pimoroni::PicoDisplay::HEIGHT - 1);
        fire[pos] = 35;
    }
    pico_display.update();
}

static unsigned int g_seed;

// Used to seed the generator.
inline void fast_srand(int seed) {
    g_seed = seed;
}

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
inline int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

void render_fps(uint32_t fps) {
    std::string text = std::to_string(fps) + "FPS";
//    pico_display.set_pen(100, 100, 100);
//    pico_display.text(text, pimoroni::Point(12, 12), 0);
    pico_display.set_pen(255, 255, 255);
    pico_display.text(text, pimoroni::Point(10, 10), 0);
}

void update_and_render(uint32_t time) {
    if (time - last_time < FPS) {
        return;
    }
    last_time = time;
    for (int y = 0; y < pimoroni::PicoDisplay::HEIGHT; y++) {
        // We precompute the rows for a small performance gain.
        int row = y * pimoroni::PicoDisplay::WIDTH;

        // For each pixel in each row, we calculate the colours that will be
        // rendered on the previous row, on the next call to update.
        int next_row = y == 0 ? 0 : (y - 1) * pimoroni::PicoDisplay::WIDTH;

        for (int x = 0; x < pimoroni::PicoDisplay::WIDTH; x++) {
            uint8_t color = fire[row + x];
            uint16_t pen = pallete[color];
            *pico_display.ptr(x, y) = pen;

            if (y > 0) {
                int new_x = x;
                int rand = fast_rand() % 3;
                new_x = (new_x + rand - 1 + wind);
                if (new_x >= pimoroni::PicoDisplay::WIDTH) {
                    new_x = new_x - pimoroni::PicoDisplay::WIDTH;
                } else if (new_x < 0) {
                    new_x = new_x + pimoroni::PicoDisplay::WIDTH;
                }
                color = color > 0 ? color - (rand & 1) : 0;
                fire[next_row + new_x] = color;
            }
        }
    }
    frames++;
    render_fps(frames * 1000 / time);
    pico_display.update();
}

//void update(uint32_t time) {
//    if (time - last_time < FPS) {
//        return;
//    }
//    last_time = time;
//
//    for (uint32_t src_y = pimoroni::PicoDisplay::HEIGHT; src_y > 1; src_y--) {
//        uint32_t src_row = src_y * pimoroni::PicoDisplay::WIDTH;
//
//        for (uint32_t src_x = 0; src_x < pimoroni::PicoDisplay::WIDTH; src_x++) {
//            int rand = std::rand() % 3;
//            uint32_t src_index = src_row - src_x;
//            uint8_t color = fire[src_index];
//            if (color > 0) {
//                color = color - (rand & 1);
//            }
//
//            uint32_t dst_x = src_x;
//
//            // Avoid propagating fire to the wrong place, when on the edges
//            if (dst_x == 0) {
//                dst_x = dst_x + std::max(rand - 1, 0);
//            } else if (dst_x == pimoroni::PicoDisplay::WIDTH - 1) {
//                dst_x = dst_x + std::min(rand - 1, 0);
//            } else {
//                dst_x = dst_x + rand - 1;
//            }
//
//            uint32_t dst_index = src_row - pimoroni::PicoDisplay::WIDTH - dst_x;
//            fire[dst_index] = color;
//        }
//    }
//}
//
//void render(uint32_t time) {
//    for (uint32_t x = 0; x < pimoroni::PicoDisplay::WIDTH; x++) {
//        for (uint32_t y = 0; y < pimoroni::PicoDisplay::HEIGHT; y++) {
//            uint32_t pos = posAt(x, y);
//            pico_display.set_pen(pallete[fire[pos]]);
//            pico_display.pixel(pimoroni::Point(x, y));
//        }
//    }
//    pico_display.update();
//}

int main() {
    init();
    uint32_t start_time = time_us_32();
    while (true) {

        if (pico_display.is_pressed(pimoroni::PicoDisplay::Y)) {
            wind++;
        }
        if (pico_display.is_pressed(pimoroni::PicoDisplay::B)) {
            wind--;
        }
        update_and_render((time_us_32() - start_time) / 1000);
    }
}
