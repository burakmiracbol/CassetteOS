#include "Player.h"
#include "Hardware.h"
#include "Ui.h"
#include <SD.h>

bool startTrack(size_t index) {
    if(index >= model.tracks.size()) return false;

    audio.stopSong();
    model.currentIndex = index;

    bool ok = audio.connecttoFS(SD, model.tracks[index].path.c_str());
    if(!ok) {
        setPlayerState(PlayerState::Stopped);
        showError("Parca acilamadi");
        return false;
    }

    setPlayerState(PlayerState::Playing);
    setAppState(AppState::NowPlaying);
    return true;
}

void playNextTrack() {
    if(!hasTracks()) return;

    size_t nextIndex = model.currentIndex;
    if(model.shuffle && model.tracks.size() > 1) {
        do {
            nextIndex = (size_t)random((long)model.tracks.size());
        } while(nextIndex == model.currentIndex);
    } else if(model.currentIndex + 1 < model.tracks.size()) {
        nextIndex = model.currentIndex + 1;
    } else if(model.repeatMode == RepeatMode::All) {
        nextIndex = 0;
    } else {
        stopPlayback();
        return;
    }

    startTrack(nextIndex);
}

void playPreviousTrack() {
    if(!hasTracks()) return;

    size_t previousIndex = model.currentIndex;
    if(model.shuffle && model.tracks.size() > 1) {
        do {
            previousIndex = (size_t)random((long)model.tracks.size());
        } while(previousIndex == model.currentIndex);
    } else if(model.currentIndex > 0) {
        previousIndex = model.currentIndex - 1;
    } else if(model.repeatMode == RepeatMode::All) {
        previousIndex = model.tracks.size() - 1;
    } else {
        previousIndex = 0;
    }

    startTrack(previousIndex);
}

void togglePlayPause() {
    if(!hasTracks()) return;

    if(model.playerState == PlayerState::Stopped) {
        startTrack(model.currentIndex);
        return;
    }

    audio.pauseResume();
    if(model.playerState == PlayerState::Playing) {
        setPlayerState(PlayerState::Paused);
    } else {
        setPlayerState(PlayerState::Playing);
    }
}

void stopPlayback() {
    audio.stopSong();
    setPlayerState(PlayerState::Stopped);
}

void setSleepTimer(SleepTimerMode mode) {
    model.sleepMode = mode;

    switch(mode) {
        case SleepTimerMode::Off:
            model.sleepDeadlineMs = 0;
            break;
        case SleepTimerMode::Min30:
            model.sleepDeadlineMs = millis() + 30UL * 60UL * 1000UL;
            break;
        case SleepTimerMode::Min60:
            model.sleepDeadlineMs = millis() + 60UL * 60UL * 1000UL;
            break;
        case SleepTimerMode::Min90:
            model.sleepDeadlineMs = millis() + 90UL * 60UL * 1000UL;
            break;
    }
    updateMainScreen();
}

void cycleSleepTimer() {
    if(model.sleepMode == SleepTimerMode::Off) setSleepTimer(SleepTimerMode::Min30);
    else if(model.sleepMode == SleepTimerMode::Min30) setSleepTimer(SleepTimerMode::Min60);
    else if(model.sleepMode == SleepTimerMode::Min60) setSleepTimer(SleepTimerMode::Min90);
    else setSleepTimer(SleepTimerMode::Off);
}

void audio_eof_mp3(const char *info) {
    Serial.printf("EOF: %s\n", info);

    if(!hasTracks()) {
        stopPlayback();
        return;
    }

    if(model.repeatMode == RepeatMode::One) {
        startTrack(model.currentIndex);
        return;
    }

    if(model.currentIndex + 1 < model.tracks.size() || model.shuffle || model.repeatMode == RepeatMode::All) {
        playNextTrack();
    } else {
        stopPlayback();
    }
}

void audio_info(const char *info) {
    Serial.println(info);
}
