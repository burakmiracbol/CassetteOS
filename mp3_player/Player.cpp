#include "Player.h"
#include "Hardware.h"
#include "Storage.h"
#include "Ui.h"
#include <SD.h>

static String trimCopy(const String &in) {
    String out = in;
    out.trim();
    return out;
}

static String parseStreamTitle(const String &raw) {
    int first = raw.indexOf('\'');
    int second = raw.indexOf('\'', first + 1);
    if(first >= 0 && second > first) return raw.substring(first + 1, second);
    return raw;
}

static void applyId3Line(const String &line) {
    int sep = line.indexOf(':');
    if(sep < 0) return;

    String key = line.substring(0, sep);
    String value = line.substring(sep + 1);
    key = normalizeText(trimCopy(key));
    value = trimCopy(value);
    if(value.length() == 0) return;

    if(key.indexOf("title") >= 0 || key == "tit2") {
        model.id3Title = value;
        return;
    }
    if(key.indexOf("artist") >= 0 || key == "tpe1") {
        model.id3Artist = value;
        return;
    }
    if(key.indexOf("album") >= 0 || key == "talb") {
        model.id3Album = value;
        return;
    }
}

void initAudioCallbacks() {
    Audio::audio_info_callback = [](Audio::msg_t m) {
        if(!m.msg) return;

        String msg = String(m.msg);
        switch(m.e) {
            case Audio::evt_id3data:
                applyId3Line(msg);
                break;

            case Audio::evt_streamtitle:
                model.streamTitle = parseStreamTitle(msg);
                break;

            case Audio::evt_info:
                applyId3Line(msg);
                break;

            default:
                break;
        }
    };
}

void applyEqMode() {
    switch(model.eqMode) {
        case EqMode::Normal:
            audio.setTone(0.0f, 0.0f, 0.0f);
            break;
        case EqMode::Bass:
            audio.setTone(6.0f, 1.5f, -1.0f);
            break;
        case EqMode::Treble:
            audio.setTone(-1.5f, 1.5f, 6.0f);
            break;
        case EqMode::Pop:
            audio.setTone(3.0f, 2.0f, 3.0f);
            break;
        case EqMode::Rock:
            audio.setTone(4.0f, 2.0f, 4.0f);
            break;
        case EqMode::Jazz:
            audio.setTone(2.0f, 3.0f, 2.0f);
            break;
    }
}

bool startTrack(size_t index, int32_t startAtSecond) {
    if(!isTrackIndexValid(index)) return false;

    audio.stopSong();
    model.currentIndex = index;
    model.lastError = "";
    clearNowPlayingMetadata();

    setPlayerState(PlayerState::Loading);
    bool ok = audio.connecttoFS(SD, model.tracks[index].path.c_str(), startAtSecond);
    if(!ok) {
        setErrorMessage("Track open failed");
        setPlayerState(PlayerState::Error);
        showError(model.lastError.c_str());
        return false;
    }

    applyEqMode();
    setPlayerState(PlayerState::Playing);
    setAppState(AppState::NowPlaying);
    updateMainScreen();
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
    if(model.playerState == PlayerState::Loading) return;

    if(model.playerState == PlayerState::Stopped || model.playerState == PlayerState::Error) {
        startTrack(model.currentIndex);
        return;
    }

    bool ok = audio.pauseResume();
    if(!ok) {
        setErrorMessage("Pause/Resume failed");
        setPlayerState(PlayerState::Error);
        showError(model.lastError.c_str());
        return;
    }

    if(model.playerState == PlayerState::Playing) setPlayerState(PlayerState::Paused);
    else setPlayerState(PlayerState::Playing);
}

void stopPlayback() {
    audio.stopSong();
    setPlayerState(PlayerState::Stopped);
}

bool seekToSecond(uint32_t second) {
    if(model.playerState != PlayerState::Playing && model.playerState != PlayerState::Paused) return false;

    uint32_t total = audio.getAudioFileDuration();
    if(total > 0 && second > total) second = total;
    if(second > UINT16_MAX) second = UINT16_MAX;

    bool ok = audio.setAudioPlayTime((uint16_t)second);
    if(!ok) {
        setErrorMessage("Seek failed");
        return false;
    }
    return true;
}

void setSleepTimer(SleepTimerMode mode) {
    model.sleepMode = mode;
    switch(mode) {
        case SleepTimerMode::Off:
            model.sleepDeadlineMs = 0;
            break;
        case SleepTimerMode::Min30:
            model.sleepDeadlineMs = millis() + (30UL * 60UL * 1000UL);
            break;
        case SleepTimerMode::Min60:
            model.sleepDeadlineMs = millis() + (60UL * 60UL * 1000UL);
            break;
        case SleepTimerMode::Min90:
            model.sleepDeadlineMs = millis() + (90UL * 60UL * 1000UL);
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
    Serial.printf("EOF: %s\n", info ? info : "");

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
