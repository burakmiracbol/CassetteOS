#ifndef MP3_PLAYER_MODEL_H
#define MP3_PLAYER_MODEL_H

#include <Arduino.h>
#include <vector>

enum class AppState : uint8_t {
    Boot,
    NowPlaying,
    Search,
    Screensaver,
    DisplayOff,
    Error
};

enum class PlayerState : uint8_t {
    Stopped,
    Playing,
    Paused
};

enum class RepeatMode : uint8_t {
    Off,
    One,
    All
};

enum class EqMode : uint8_t {
    Normal,
    Bass,
    Treble,
    Pop,
    Rock,
    Jazz
};

enum class SleepTimerMode : uint8_t {
    Off,
    Min30,
    Min60,
    Min90
};

struct Track {
    String path;
    String title;
    String album;
};

struct PlayerModel {
    std::vector<Track> tracks;
    std::vector<String> favorites;
    size_t currentIndex = 0;
    PlayerState playerState = PlayerState::Stopped;
    RepeatMode repeatMode = RepeatMode::Off;
    EqMode eqMode = EqMode::Normal;
    SleepTimerMode sleepMode = SleepTimerMode::Off;
    bool shuffle = false;
    bool darkMode = true;
    uint8_t volume = 12;
    uint8_t brightness = 255;
    uint8_t batteryPercent = 100;
    unsigned long sleepDeadlineMs = 0;
};

extern PlayerModel model;
extern AppState appState;
extern AppState previousVisibleState;
extern unsigned long lastInteractionMs;
extern unsigned long lastLvTickMs;

bool hasTracks();
bool timeReached(unsigned long deadline);
void recordInteraction();

void setAppState(AppState next);
void setPlayerState(PlayerState next);

String fileTitleFromPath(const String &path);
String albumFromPath(const String &path);
String normalizeText(const String &text);
String formatTime(uint32_t seconds);
String currentTrackTitle();
String currentTrackAlbum();

const char *appStateName(AppState state);
const char *playerStateName(PlayerState state);
const char *repeatModeName(RepeatMode mode);
const char *eqModeName(EqMode mode);
const char *sleepModeName(SleepTimerMode mode);

#endif
