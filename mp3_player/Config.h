#ifndef MP3_PLAYER_CONFIG_H
#define MP3_PLAYER_CONFIG_H

#include <Arduino.h>

// Optional board profile:
// #define MP3_PROFILE_ESP32_S3_DEV

// -------------------- Pins --------------------
#ifdef MP3_PROFILE_ESP32_S3_DEV
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
#else
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
#endif

// Optional physical buttons (active low). Keep -1 to disable.
#define BTN_PREV_PIN       -1
#define BTN_NEXT_PIN       -1
#define BTN_PLAY_PIN       -1
#define BTN_VOL_DOWN_PIN   -1
#define BTN_VOL_UP_PIN     -1

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
static constexpr unsigned long BUTTON_DEBOUNCE_MS = 220UL;

// -------------------- Scan settings --------------------
static constexpr uint8_t SD_SCAN_MAX_DEPTH = 4;
static constexpr size_t MAX_TRACK_PATH_LEN = 240;

#endif
