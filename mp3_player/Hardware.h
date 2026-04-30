#ifndef MP3_PLAYER_HARDWARE_H
#define MP3_PLAYER_HARDWARE_H

#include "Config.h"
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <Audio.h>
#include <Adafruit_NeoPixel.h>
#include <lvgl.h>

#ifndef LV_LABEL_LONG_MODE_SCROLL_CIRCULAR
#define LV_LABEL_LONG_MODE_SCROLL_CIRCULAR LV_LABEL_LONG_SCROLL_CIRCULAR
#endif

#ifndef LV_LABEL_LONG_MODE_DOTS
#define LV_LABEL_LONG_MODE_DOTS LV_LABEL_LONG_DOT
#endif

#ifndef LV_LABEL_LONG_MODE_WRAP
#define LV_LABEL_LONG_MODE_WRAP LV_LABEL_LONG_WRAP
#endif

#ifndef LV_INDEV_STATE_PRESSED
#define LV_INDEV_STATE_PRESSED LV_INDEV_STATE_PR
#endif

#ifndef LV_INDEV_STATE_RELEASED
#define LV_INDEV_STATE_RELEASED LV_INDEV_STATE_REL
#endif

extern TFT_eSPI tft;
extern Audio audio;
extern Adafruit_NeoPixel strip;
extern SPIClass spiSD;

void initDisplayHardware();
void initLvglPort();
void initLedRing();
void initSdBus();
void tickLvgl();

void setBacklight(bool on);
void updateBattery();
void updateLedRing();
void handleTimers();

#endif
