#ifndef EPAPER_H
#define EPAPER_H

#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bitmaps/fonts/fontStruct.h"

// Pin Definitions
#define PIN_MOSI    23
#define PIN_MISO    -1
#define PIN_CLK     18
#define PIN_CS      5
#define PIN_DC      0
#define PIN_RST     2
#define PIN_BUSY    15

// Screen Definitions
#define DISPLAY_WIDTH   250
#define DISPLAY_HEIGHT  122

//Color Definitions
#define BLACK 0
#define RED 1

//Detailed Debugs
extern uint8_t debugB;

// Function Prototypes
void epaper_init(void);
void epaper_writeBufferToDisplay(void);
void epaper_update(void);
void epaper_clear(void);
void epaper_deep_sleep(void);
void epaper_draw_blackBitmap(const unsigned char IMAGE[]);
void epaper_draw_redBitmap(const unsigned char IMAGE[]);
void epaper_draw_blackAndRedBitmaps(const unsigned char IMAGE_BLACK[], const unsigned char IMAGE_RED[]);
void epaper_draw_partial_blackAndRedBitmapsEnhanced(const unsigned char* black_bitmap, const unsigned char* red_bitmap, int x, int y, int width, int height, int bitmap_width, int bitmap_height);
void epaper_draw_partial_blackBitmapsEnhanced(const unsigned char* black_bitmap, int x, int y, int width, int height, int bitmap_width, int bitmap_height);
void place_image_into_buffer(int x, int y,const unsigned char* black_bitmap, int IMAGE_WIDTH, int IMAGE_HEIGHT);
void draw_pixel(int w, int h, uint8_t pixel, int x, int y);
void draw_line_horizontal(int x1, int x2, int y);
void draw_line_vertical(int x ,int y1, int y2);
void draw_pixel_color(int w, int h, uint8_t pixel, int x, int y, int color);
void printBinary2_color(unsigned char value, int length, int height, int x, int y, int color);
void draw_char_withFont_color(uint8_t letter, int x, int y, const GFXfont *font, uint8_t color);
void draw_word_withFont_color(const char* word, int x, int y, const GFXfont *font, uint8_t color);
void epaper_wait_milliseconds(uint32_t milliseconds);
void epaper_wait_seconds(uint32_t seconds);
void epaper_red_buffer_clear(void);
void epaper_bw_buffer_clear(void);
void epaper_reset();
#endif



