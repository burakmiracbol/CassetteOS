#include "Config.h"
#include "Hardware.h"
#include "Model.h"
#include "Player.h"
#include "Storage.h"
#include "ui/Ui.h"

void setup() {
    Serial.begin(115200);
    delay(100);

    initDisplayHardware();
    initLvglPort();

    lastInteractionMs = millis();
    lastLvTickMs = millis();

    showBootMessage("LED ring baslatiliyor...");
    initLedRing();

    showBootMessage("SD kart okunuyor...");
    initSdBus();
    if(!SD.begin(SD_CS, spiSD)) {
        showError("SD kart baslatilamadi");
        return;
    }

    showBootMessage("MP3 kutuphanesi taraniyor...");
    scanSDCard(SD, "/", SD_SCAN_MAX_DEPTH);
    loadFavorites();
    updateBattery();

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(model.volume);
    initAudioCallbacks();
    applyEqMode();

    if(hasTracks()) {
        startTrack(0);
    } else {
        setErrorMessage("No MP3 files found on SD");
        setAppState(AppState::NowPlaying);
    }
}

void loop() {
    tickLvgl();

    audio.loop();
    lv_timer_handler();
    handleTimers();

    delay(5);
}
