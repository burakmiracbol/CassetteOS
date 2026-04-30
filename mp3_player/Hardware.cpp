#include "Hardware.h"
#include "Model.h"
#include "Player.h"
#include "Ui.h"

TFT_eSPI tft = TFT_eSPI();
Audio audio;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
SPIClass spiSD(FSPI);

static lv_color_t colorBuf[SCREEN_W * LVGL_BUF_LINES];

#if LVGL_VERSION_MAJOR < 9
static lv_disp_draw_buf_t drawBuf;
#endif

static unsigned long lastUiRefreshMs = 0;
static unsigned long lastLedRefreshMs = 0;
static uint8_t ledHue = 0;

static unsigned long lastBtnPrevMs = 0;
static unsigned long lastBtnNextMs = 0;
static unsigned long lastBtnPlayMs = 0;
static unsigned long lastBtnVolDownMs = 0;
static unsigned long lastBtnVolUpMs = 0;

static bool buttonPressed(int pin, unsigned long &lastMs) {
    if(pin < 0) return false;
    if(digitalRead(pin) != LOW) return false;
    if((unsigned long)(millis() - lastMs) < BUTTON_DEBOUNCE_MS) return false;
    lastMs = millis();
    return true;
}

#if LVGL_VERSION_MAJOR >= 9
static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)px_map, w * h, true);
    tft.endWrite();

    lv_display_flush_ready(disp);
}

static void my_touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
    (void)indev;
#else
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)color_p, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

static bool my_touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    (void)drv;
#endif
    uint16_t x = 0;
    uint16_t y = 0;

#ifdef TOUCH_CS
    bool touched = tft.getTouch(&x, &y);
#else
    bool touched = false;
#endif

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
#if LVGL_VERSION_MAJOR >= 9
        return;
#else
        return false;
#endif
    }

    recordInteraction();

    if(appState == AppState::DisplayOff) {
        setBacklight(true);
        setAppState(previousVisibleState);
        data->state = LV_INDEV_STATE_RELEASED;
#if LVGL_VERSION_MAJOR >= 9
        return;
#else
        return false;
#endif
    }

    if(appState == AppState::Screensaver) {
        setAppState(AppState::NowPlaying);
        data->state = LV_INDEV_STATE_RELEASED;
#if LVGL_VERSION_MAJOR >= 9
        return;
#else
        return false;
#endif
    }

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
#if LVGL_VERSION_MAJOR >= 9
    return;
#else
    return false;
#endif
}

void initDisplayHardware() {
    pinMode(TFT_BL, OUTPUT);
    pinMode(BAT_PIN, INPUT);
    setBacklight(true);

    if(BTN_PREV_PIN >= 0) pinMode(BTN_PREV_PIN, INPUT_PULLUP);
    if(BTN_NEXT_PIN >= 0) pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    if(BTN_PLAY_PIN >= 0) pinMode(BTN_PLAY_PIN, INPUT_PULLUP);
    if(BTN_VOL_DOWN_PIN >= 0) pinMode(BTN_VOL_DOWN_PIN, INPUT_PULLUP);
    if(BTN_VOL_UP_PIN >= 0) pinMode(BTN_VOL_UP_PIN, INPUT_PULLUP);

    randomSeed((uint32_t)analogRead(BAT_PIN));

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
}

void initLvglPort() {
    lv_init();

#if LVGL_VERSION_MAJOR >= 9
    lv_display_t *display = lv_display_create(SCREEN_W, SCREEN_H);
    lv_display_set_flush_cb(display, my_disp_flush);
    lv_display_set_buffers(display, colorBuf, nullptr, sizeof(colorBuf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read);
#else
    lv_disp_draw_buf_init(&drawBuf, colorBuf, nullptr, SCREEN_W * LVGL_BUF_LINES);

    static lv_disp_drv_t dispDrv;
    lv_disp_drv_init(&dispDrv);
    dispDrv.hor_res = SCREEN_W;
    dispDrv.ver_res = SCREEN_H;
    dispDrv.flush_cb = my_disp_flush;
    dispDrv.draw_buf = &drawBuf;
    lv_disp_drv_register(&dispDrv);

    static lv_indev_drv_t indevDrv;
    lv_indev_drv_init(&indevDrv);
    indevDrv.type = LV_INDEV_TYPE_POINTER;
    indevDrv.read_cb = my_touch_read;
    lv_indev_drv_register(&indevDrv);
#endif
}

void initLedRing() {
    strip.begin();
    strip.setBrightness(24);
    strip.clear();
    strip.show();
}

void initSdBus() {
    spiSD.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
}

void tickLvgl() {
    unsigned long now = millis();
    lv_tick_inc(now - lastLvTickMs);
    lastLvTickMs = now;
}

void setBacklight(bool on) {
    analogWrite(TFT_BL, on ? model.brightness : 0);
}

void updateBattery() {
    int adc = analogRead(BAT_PIN);
    float voltage = (adc / 4095.0f) * 3.3f * 2.0f;
    int percent = (int)(((voltage - 3.0f) / (4.2f - 3.0f)) * 100.0f);
    percent = constrain(percent, 0, 100);
    model.batteryPercent = (uint8_t)percent;
}

void updateLedRing() {
    if(model.playerState != PlayerState::Playing) {
        strip.clear();
        strip.show();
        return;
    }

    strip.rainbow(ledHue);
    strip.show();
    ledHue += 3;
}

static void handlePhysicalButtons() {
    if(buttonPressed(BTN_PREV_PIN, lastBtnPrevMs)) {
        recordInteraction();
        playPreviousTrack();
    }

    if(buttonPressed(BTN_NEXT_PIN, lastBtnNextMs)) {
        recordInteraction();
        playNextTrack();
    }

    if(buttonPressed(BTN_PLAY_PIN, lastBtnPlayMs)) {
        recordInteraction();
        togglePlayPause();
    }

    if(buttonPressed(BTN_VOL_DOWN_PIN, lastBtnVolDownMs)) {
        recordInteraction();
        if(model.volume > 0) model.volume--;
        audio.setVolume(model.volume);
        updateMainScreen();
    }

    if(buttonPressed(BTN_VOL_UP_PIN, lastBtnVolUpMs)) {
        recordInteraction();
        if(model.volume < audio.getVolumeSteps()) model.volume++;
        audio.setVolume(model.volume);
        updateMainScreen();
    }
}

void handleTimers() {
    unsigned long now = millis();

    handlePhysicalButtons();

    if(timeReached(model.sleepDeadlineMs)) {
        stopPlayback();
        setSleepTimer(SleepTimerMode::Off);
    }

    if(appState != AppState::DisplayOff && appState != AppState::Error) {
        unsigned long idle = now - lastInteractionMs;
        if(idle > DISPLAY_OFF_TIMEOUT_MS) {
            setAppState(AppState::DisplayOff);
        } else if(idle > SCREENSAVER_TIMEOUT_MS && appState == AppState::NowPlaying) {
            setAppState(AppState::Screensaver);
        }
    }

    if(now - lastUiRefreshMs > 500UL) {
        updateBattery();

        if(appState == AppState::NowPlaying) updateMainScreen();
        else if(appState == AppState::Screensaver) updateScreensaverScreen();

        lastUiRefreshMs = now;
    }

    if(now - lastLedRefreshMs > 80UL) {
        updateLedRing();
        lastLedRefreshMs = now;
    }
}
