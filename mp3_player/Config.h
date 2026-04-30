#ifndef MP3_PLAYER_CONFIG_H
#define MP3_PLAYER_CONFIG_H

#include <Arduino.h>

// -------------------- Pins --------------------
#define I2S_BCLK      4
#define I2S_LRC       5
#define I2S_DOUT      6
#define SD_CS         10
#define SPI_MOSI      35
#define SPI_MISO      37
#define SPI_SCK       36
#define TFT_BL        38
#define LED_PIN       18
#define NUM_LEDS      60
#define BAT_PIN       39

// If TFT_eSPI User_Setup.h does not define TOUCH_CS, you can enable it here.
// Example for XPT2046 touch:
// #define MP3_PLAYER_TOUCH_CS 16
#ifdef MP3_PLAYER_TOUCH_CS
#ifndef TOUCH_CS
#define TOUCH_CS MP3_PLAYER_TOUCH_CS
#endif
#endif

// -------------------- Display --------------------
static constexpr uint16_t SCREEN_W = 320;
static constexpr uint16_t SCREEN_H = 240;
static constexpr uint16_t LVGL_BUF_LINES = 10;

// -------------------- App timing --------------------
static constexpr unsigned long SCREENSAVER_TIMEOUT_MS = 30000UL;
static constexpr unsigned long DISPLAY_OFF_TIMEOUT_MS = 45000UL;

#endif
