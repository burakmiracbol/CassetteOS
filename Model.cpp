#include "Model.h"
#include "Hardware.h"
#include "ui/Ui.h"

PlayerModel model;
AppState appState = AppState::Boot;
AppState previousVisibleState = AppState::NowPlaying;
unsigned long lastInteractionMs = 0;
unsigned long lastLvTickMs = 0;

bool hasTracks() {
    return !model.tracks.empty();
}

bool isTrackIndexValid(size_t index) {
    return index < model.tracks.size();
}

bool timeReached(unsigned long deadline) {
    return deadline > 0 && (long)(millis() - deadline) >= 0;
}

void recordInteraction() {
    lastInteractionMs = millis();
}

void setErrorMessage(const String &message) {
    model.lastError = message;
}

void clearNowPlayingMetadata() {
    model.id3Title = "";
    model.id3Artist = "";
    model.id3Album = "";
    model.streamTitle = "";
}

String fileTitleFromPath(const String &path) {
    int slash = path.lastIndexOf('/');
    String title = slash >= 0 ? path.substring(slash + 1) : path;
    if(title.endsWith(".mp3") || title.endsWith(".MP3")) title.remove(title.length() - 4);
    return title;
}

String albumFromPath(const String &path) {
    int slash = path.lastIndexOf('/');
    if(slash <= 0) return "Root";

    String folder = path.substring(0, slash);
    int previousSlash = folder.lastIndexOf('/');
    if(previousSlash >= 0) folder = folder.substring(previousSlash + 1);
    if(folder.length() == 0) return "Root";
    return folder;
}

String normalizeText(const String &text) {
    String out = text;
    out.toLowerCase();
    out.replace("ı", "i");
    out.replace("ğ", "g");
    out.replace("ü", "u");
    out.replace("ş", "s");
    out.replace("ö", "o");
    out.replace("ç", "c");
    return out;
}

String formatTime(uint32_t seconds) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%02lu:%02lu", seconds / 60UL, seconds % 60UL);
    return String(buf);
}

String currentTrackTitle() {
    if(!hasTracks() || !isTrackIndexValid(model.currentIndex)) return "No tracks";
    return model.tracks[model.currentIndex].title;
}

String currentTrackAlbum() {
    if(!hasTracks() || !isTrackIndexValid(model.currentIndex)) return "Empty library";
    return model.tracks[model.currentIndex].album;
}

String currentTrackArtist() {
    if(model.id3Artist.length() > 0) return model.id3Artist;
    return "Unknown artist";
}

String activeTrackTitle() {
    if(model.streamTitle.length() > 0) return model.streamTitle;
    if(model.id3Title.length() > 0) return model.id3Title;
    return currentTrackTitle();
}

String activeTrackSubtitle() {
    String artist = currentTrackArtist();
    String album = model.id3Album.length() > 0 ? model.id3Album : currentTrackAlbum();
    return artist + " | " + album;
}

const char *appStateName(AppState state) {
    switch(state) {
        case AppState::Boot: return "BOOT";
        case AppState::NowPlaying: return "NOW";
        case AppState::Search: return "SEARCH";
        case AppState::Library: return "LIB";
        case AppState::Favorites: return "FAVS";
        case AppState::Screensaver: return "SAVER";
        case AppState::DisplayOff: return "OFF";
        case AppState::Error: return "ERROR";
        case AppState::Settings: return "SET";
        default: return "?";
    }
}

const char *playerStateName(PlayerState state) {
    switch(state) {
        case PlayerState::Stopped: return "STOP";
        case PlayerState::Loading: return "LOAD";
        case PlayerState::Playing: return "PLAY";
        case PlayerState::Paused: return "PAUSE";
        case PlayerState::Error: return "ERR";
    }
    return "?";
}

const char *repeatModeName(RepeatMode mode) {
    switch(mode) {
        case RepeatMode::Off: return "OFF";
        case RepeatMode::One: return "ONE";
        case RepeatMode::All: return "ALL";
    }
    return "?";
}

const char *eqModeName(EqMode mode) {
    switch(mode) {
        case EqMode::Normal: return "EQ";
        case EqMode::Bass: return "BASS";
        case EqMode::Treble: return "TREB";
        case EqMode::Pop: return "POP";
        case EqMode::Rock: return "ROCK";
        case EqMode::Jazz: return "JAZZ";
    }
    return "?";
}

const char *sleepModeName(SleepTimerMode mode) {
    switch(mode) {
        case SleepTimerMode::Off: return "SLP";
        case SleepTimerMode::Min30: return "S30";
        case SleepTimerMode::Min60: return "S60";
        case SleepTimerMode::Min90: return "S90";
    }
    return "?";
}

const char *themeModeName(ThemeMode mode) {
    switch(mode) {
        case ThemeMode::Classic: return "TH1";
        case ThemeMode::Neon: return "TH2";
        case ThemeMode::Minimal: return "TH3";
    }
    return "?";
}

void setPlayerState(PlayerState next) {
    if(model.playerState == next) return;
    PlayerState previous = model.playerState;
    model.playerState = next;
    Serial.printf("PlayerState: %s -> %s\n", playerStateName(previous), playerStateName(next));
    updateMainScreen();
    syncLvglProUi();
}

void setAppState(AppState next) {
    if(appState == next && next != AppState::Boot) return;

    AppState previous = appState;
    if(previous != AppState::DisplayOff && previous != AppState::Boot && previous != AppState::Error) {
        previousVisibleState = previous;
    }

    appState = next;
    Serial.printf("AppState: %s -> %s\n", appStateName(previous), appStateName(next));

    switch(next) {
        case AppState::Boot:
            setBacklight(true);
            showBootMessage("ESP32 MP3 Player");
            break;

        case AppState::NowPlaying:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createMainScreen();
            loadScreen(screenMain);
            updateMainScreen();
            break;

        case AppState::Settings:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createSettingsScreen();
            updateSettingsScreen();
            loadScreen(screenSettings);
            break;

        case AppState::Search:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createSearchScreen();
            updateSearchResults();
            updateSearchScreen();
            loadScreen(screenSearch);
            break;

        case AppState::Library:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createLibraryScreen();
            updateLibraryScreen();
            loadScreen(screenLibrary);
            break;

        case AppState::Favorites:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createFavoritesScreen();
            updateFavoritesScreen();
            loadScreen(screenFavorites);
            break;

        case AppState::Screensaver:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createScreensaverScreen();
            updateScreensaverScreen();
            loadScreen(screenSaver);
            break;

        case AppState::DisplayOff:
            setBacklight(false);
            break;

        case AppState::Error:
            setBacklight(true);
            if(tryLoadLvglProScreen(next)) {
                syncLvglProUi();
                break;
            }
            createErrorScreen();
            loadScreen(screenError);
            break;
    }
}
