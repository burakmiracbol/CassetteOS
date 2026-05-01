#ifndef MP3_PLAYER_MODEL_H
#define MP3_PLAYER_MODEL_H

#include <Arduino.h>
#include <vector>

enum class AppState : uint8_t {
    Boot,
    NowPlaying,
    Search,
    Library,
    Favorites,
    Screensaver,
    DisplayOff,
    Error
};

enum class PlayerState : uint8_t {
    Stopped,
    Loading,
    Playing,
    Paused,
    Error
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

enum class ThemeMode : uint8_t {
    Classic,
    Neon,
    Minimal
};

enum class LibraryScope : uint8_t {
    AllTracks,
    AlbumOnly
};

struct Track {
    String path;
    String title;
    String album;
};

struct PlayerModel {
    std::vector<Track> tracks;
    std::vector<String> favorites;
    std::vector<String> albums;

    size_t currentIndex = 0;
    size_t searchSelection = 0;

    int albumFilterIndex = -1; // -1 means all albums
    LibraryScope libraryScope = LibraryScope::AllTracks;

    PlayerState playerState = PlayerState::Stopped;
    RepeatMode repeatMode = RepeatMode::Off;
    EqMode eqMode = EqMode::Normal;
    SleepTimerMode sleepMode = SleepTimerMode::Off;
    ThemeMode themeMode = ThemeMode::Classic;

    bool shuffle = false;
    bool seekActive = false;

    uint8_t volume = 12;
    uint8_t brightness = 255;
    uint8_t batteryPercent = 100;

    unsigned long sleepDeadlineMs = 0;

    String id3Title;
    String id3Artist;
    String id3Album;
    String streamTitle;
    String lastError;
};

extern PlayerModel model;
extern AppState appState;
extern AppState previousVisibleState;
extern unsigned long lastInteractionMs;
extern unsigned long lastLvTickMs;

bool hasTracks();
bool isTrackIndexValid(size_t index);
bool timeReached(unsigned long deadline);
void recordInteraction();
void setErrorMessage(const String &message);
void clearNowPlayingMetadata();

void setAppState(AppState next);
void setPlayerState(PlayerState next);

String fileTitleFromPath(const String &path);
String albumFromPath(const String &path);
String normalizeText(const String &text);
String formatTime(uint32_t seconds);

String currentTrackTitle();
String currentTrackAlbum();
String currentTrackArtist();
String activeTrackTitle();
String activeTrackSubtitle();

const char *appStateName(AppState state);
const char *playerStateName(PlayerState state);
const char *repeatModeName(RepeatMode mode);
const char *eqModeName(EqMode mode);
const char *sleepModeName(SleepTimerMode mode);
const char *themeModeName(ThemeMode mode);

#endif
