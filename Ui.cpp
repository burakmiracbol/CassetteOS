#include "Ui.h"
#include "../Model.h"
#include "../Player.h"
#include "../Storage.h"
#include "UiLvglPro.h"
#include <vector>
#include <cstdint>

static lv_obj_t *screenBoot = nullptr;
lv_obj_t *screenMain = nullptr;
lv_obj_t *screenSearch = nullptr;
lv_obj_t *screenLibrary = nullptr;
lv_obj_t *screenFavorites = nullptr;
lv_obj_t *screenSaver = nullptr;
lv_obj_t *screenSettings = nullptr;
lv_obj_t *screenError = nullptr;

static lv_obj_t *bootLabel = nullptr;
static lv_obj_t *errorLabel = nullptr;

static std::vector<size_t> searchResultIndexes;
static std::vector<size_t> libraryTrackIndexes;
static std::vector<size_t> favoritesTrackIndexes;


// Main screen
static lv_obj_t *labelTitle = nullptr;
static lv_obj_t *labelSubtitle = nullptr;
static lv_obj_t *labelStatus = nullptr;
static lv_obj_t *labelBattery = nullptr;
static lv_obj_t *labelTime = nullptr;
static lv_obj_t *sliderSeek = nullptr;
static lv_obj_t *sliderVolume = nullptr;
static lv_obj_t *labelVolume = nullptr;
static lv_obj_t *labelPlayBtn = nullptr;
static lv_obj_t *labelShuffleBtn = nullptr;
static lv_obj_t *labelRepeatBtn = nullptr;
static lv_obj_t *labelFavoriteBtn = nullptr;
static lv_obj_t *labelSleepBtn = nullptr;
static lv_obj_t *labelEqBtn = nullptr;
static lv_obj_t *labelThemeBtn = nullptr;
static lv_obj_t *labelSettingsBtn = nullptr;

// Search screen
static lv_obj_t *searchTextArea = nullptr;
static lv_obj_t *searchResultList = nullptr;

// Library screen
static lv_obj_t *libraryAlbumLabel = nullptr;
static lv_obj_t *libraryScopeBtn = nullptr;
static lv_obj_t *libraryAlbumPrevBtn = nullptr;
static lv_obj_t *libraryAlbumNextBtn = nullptr;
static lv_obj_t *libraryTrackList = nullptr;

// Favorites screen
static lv_obj_t *favoritesLabel = nullptr;
static lv_obj_t *favoritesList = nullptr;

static lv_obj_t *btnShuffle = nullptr;
static lv_obj_t *btnRepeat = nullptr;
static lv_obj_t *btnFavorite = nullptr;
static lv_obj_t *btnSleep = nullptr;
static lv_obj_t *btnTheme = nullptr;

// Settings screen
static lv_obj_t *btnSettingsTheme = nullptr;
static lv_obj_t *btnSettingsEq = nullptr;
static lv_obj_t *btnSettingsSleep = nullptr;
static lv_obj_t *sliderBrightness = nullptr;
static lv_obj_t *labelBrightness = nullptr;
static lv_obj_t *btnClearFavorites = nullptr;

// Screensaver
static lv_obj_t *saverTitleLabel = nullptr;
static lv_obj_t *saverStateLabel = nullptr;
static lv_obj_t *saverArcLeft = nullptr;
static lv_obj_t *saverArcRight = nullptr;
static unsigned long screensaverPhase = 0;

static void play_event_cb(lv_event_t *e);
static void next_event_cb(lv_event_t *e);
static void previous_event_cb(lv_event_t *e);
static void shuffle_event_cb(lv_event_t *e);
static void repeat_event_cb(lv_event_t *e);
static void favorite_event_cb(lv_event_t *e);
static void search_open_event_cb(lv_event_t *e);
static void library_open_event_cb(lv_event_t *e);
static void favorites_open_event_cb(lv_event_t *e);
static void volume_event_cb(lv_event_t *e);
static void seek_event_cb(lv_event_t *e);
static void sleep_event_cb(lv_event_t *e);
static void eq_event_cb(lv_event_t *e);
static void theme_event_cb(lv_event_t *e);

static void search_key_event_cb(lv_event_t *e);
static void search_result_event_cb(lv_event_t *e);
static void search_exit_event_cb(lv_event_t *e);
static void refreshSearchResults();
static void refreshSearchList();

static void library_track_event_cb(lv_event_t *e);
static void library_scope_event_cb(lv_event_t *e);
static void library_album_prev_event_cb(lv_event_t *e);
static void library_album_next_event_cb(lv_event_t *e);
static void library_exit_event_cb(lv_event_t *e);
static void refreshLibraryTracks();

static void favorites_track_event_cb(lv_event_t *e);
static void favorites_clear_event_cb(lv_event_t *e);
static void favorites_exit_event_cb(lv_event_t *e);
static void refreshFavoritesList();

static void favorites_track_event_cb(lv_event_t *e);
static void favorites_clear_event_cb(lv_event_t *e);
static void favorites_exit_event_cb(lv_event_t *e);
static void settings_open_event_cb(lv_event_t *e);
static void settings_back_event_cb(lv_event_t *e);
static void settings_theme_event_cb(lv_event_t *e);
static void settings_eq_event_cb(lv_event_t *e);
static void settings_sleep_event_cb(lv_event_t *e);
static void brightness_event_cb(lv_event_t *e);
static void settings_clear_favorites_event_cb(lv_event_t *e);

static void screensaver_event_cb(lv_event_t *e);
static void error_exit_event_cb(lv_event_t *e);
static bool lvglProReady = false;
static lv_obj_t *createPlaceholderScreen(const char *titleText, const char *bodyText, lv_event_cb_t backCb);

void loadScreen(lv_obj_t *screen) {
#if LVGL_VERSION_MAJOR >= 9
    lv_screen_load(screen);
#else
    lv_scr_load(screen);
#endif
}

bool tryLoadLvglProScreen(AppState state) {
    if(!lvglProReady) {
        lvglProReady = lvglProInit();
    }

    if(!lvglProReady) return false;

    lv_obj_t *screen = lvglProScreenForState(state);
    if(!screen) return false;

    loadScreen(screen);
    return true;
}

void syncLvglProUi() {
    if(!lvglProReady) return;
    lvglProSyncFromModel();
}

static lv_obj_t *eventTargetObj(lv_event_t *e) {
#if LVGL_VERSION_MAJOR >= 9
    return lv_event_get_target_obj(e);
#else
    return lv_event_get_target(e);
#endif
}

static lv_obj_t *makeButton(lv_obj_t *parent, int w, int h, int x, int y, const char *text, lv_event_cb_t cb) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, x, y);
    if(cb) lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return btn;
}

static lv_obj_t *createPlaceholderScreen(const char *titleText, const char *bodyText, lv_event_cb_t backCb) {
    lv_obj_t *screen = lv_obj_create(nullptr);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, titleText);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    lv_obj_t *body = lv_label_create(screen);
    lv_obj_set_width(body, 296);
    lv_label_set_long_mode(body, LV_LABEL_LONG_MODE_WRAP);
    lv_label_set_text(body, bodyText);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, 8, 40);

    makeButton(screen, 60, 28, 252, 4, "BACK", backCb);
    return screen;
}

static void styleToggleButton(lv_obj_t *btn, bool on, lv_color_t active, lv_color_t inactive) {
    if(!btn) return;
    lv_obj_set_style_bg_color(btn, on ? active : inactive, 0);
}

static void applyThemeToMainScreen() {
    if(!screenMain) return;

    lv_color_t bg = lv_color_hex(0x0A0A0A);
    lv_color_t fg = lv_color_hex(0xF2F2F2);
    lv_color_t muted = lv_color_hex(0xA9B1BA);
    lv_color_t accent = lv_color_hex(0x3A8DFF);
    lv_color_t active = lv_color_hex(0x24B36B);
    lv_color_t inactive = lv_color_hex(0x364052);

    if(model.themeMode == ThemeMode::Neon) {
        bg = lv_color_hex(0x04151A);
        fg = lv_color_hex(0xD4FFF7);
        muted = lv_color_hex(0x8EC9C9);
        accent = lv_color_hex(0x00D5FF);
        active = lv_color_hex(0x05FF9C);
        inactive = lv_color_hex(0x16404A);
    } else if(model.themeMode == ThemeMode::Minimal) {
        bg = lv_color_hex(0xF4F5F6);
        fg = lv_color_hex(0x15202B);
        muted = lv_color_hex(0x4A596B);
        accent = lv_color_hex(0x2B6CB0);
        active = lv_color_hex(0x2F855A);
        inactive = lv_color_hex(0xCBD5E0);
    }

    lv_obj_set_style_bg_color(screenMain, bg, 0);
    lv_obj_set_style_bg_color(sliderSeek, accent, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(sliderVolume, accent, LV_PART_INDICATOR);

    lv_obj_set_style_text_color(labelTitle, fg, 0);
    lv_obj_set_style_text_color(labelSubtitle, muted, 0);
    lv_obj_set_style_text_color(labelStatus, fg, 0);
    lv_obj_set_style_text_color(labelBattery, muted, 0);
    lv_obj_set_style_text_color(labelTime, muted, 0);
    lv_obj_set_style_text_color(labelVolume, fg, 0);

    styleToggleButton(btnShuffle, model.shuffle, active, inactive);
    styleToggleButton(btnRepeat, model.repeatMode != RepeatMode::Off, active, inactive);
    styleToggleButton(btnFavorite, hasTracks() && isTrackIndexValid(model.currentIndex) && isFavorite(model.tracks[model.currentIndex].path), active, inactive);
    styleToggleButton(btnSleep, model.sleepMode != SleepTimerMode::Off, active, inactive);
}

void showBootMessage(const char *message) {
    if(!screenBoot) {
        screenBoot = lv_obj_create(nullptr);
        lv_obj_set_style_bg_color(screenBoot, lv_color_black(), 0);

        bootLabel = lv_label_create(screenBoot);
        lv_label_set_text(bootLabel, "ESP32 MP3 Player");
        lv_obj_set_style_text_color(bootLabel, lv_color_white(), 0);
        lv_obj_align(bootLabel, LV_ALIGN_CENTER, 0, 0);
    }

    loadScreen(screenBoot);
    lv_label_set_text(bootLabel, message);
    lv_timer_handler();
}

void showError(const char *message) {
    createErrorScreen();
    lv_label_set_text(errorLabel, message ? message : "Error");
    setAppState(AppState::Error);
}

void createMainScreen() {
    if(screenMain) return;

    screenMain = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenMain, LV_OBJ_FLAG_SCROLLABLE);

    labelTitle = lv_label_create(screenMain);
    lv_obj_set_width(labelTitle, 304);
    lv_label_set_long_mode(labelTitle, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_align(labelTitle, LV_ALIGN_TOP_LEFT, 8, 6);

    labelSubtitle = lv_label_create(screenMain);
    lv_obj_set_width(labelSubtitle, 304);
    lv_label_set_long_mode(labelSubtitle, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_align(labelSubtitle, LV_ALIGN_TOP_LEFT, 8, 28);

    sliderSeek = lv_slider_create(screenMain);
    lv_obj_set_size(sliderSeek, 304, 12);
    lv_slider_set_range(sliderSeek, 0, 1000);
    lv_obj_align(sliderSeek, LV_ALIGN_TOP_LEFT, 8, 50);
    lv_obj_add_event_cb(sliderSeek, seek_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);
    lv_obj_add_event_cb(sliderSeek, seek_event_cb, LV_EVENT_PRESSED, nullptr);
    lv_obj_add_event_cb(sliderSeek, seek_event_cb, LV_EVENT_RELEASED, nullptr);
    lv_obj_add_event_cb(sliderSeek, seek_event_cb, LV_EVENT_PRESS_LOST, nullptr);

    labelTime = lv_label_create(screenMain);
    lv_obj_align(labelTime, LV_ALIGN_TOP_LEFT, 8, 66);

    labelStatus = lv_label_create(screenMain);
    lv_obj_align(labelStatus, LV_ALIGN_TOP_RIGHT, -8, 66);

    makeButton(screenMain, 48, 28, 8, 88, "LIB", library_open_event_cb);
    makeButton(screenMain, 48, 28, 62, 88, "SRH", search_open_event_cb);
    makeButton(screenMain, 48, 28, 116, 88, "FLS", favorites_open_event_cb);

    labelBattery = lv_label_create(screenMain);
    lv_obj_align(labelBattery, LV_ALIGN_TOP_RIGHT, -8, 90);

    makeButton(screenMain, 66, 36, 34, 122, "PREV", previous_event_cb);
    lv_obj_t *btnPlay = makeButton(screenMain, 88, 36, 116, 122, "PLAY", play_event_cb);
    makeButton(screenMain, 66, 36, 220, 122, "NEXT", next_event_cb);
    labelPlayBtn = lv_obj_get_child(btnPlay, 0);

    sliderVolume = lv_slider_create(screenMain);
    lv_obj_set_size(sliderVolume, 215, 14);
    lv_slider_set_range(sliderVolume, 0, 21);
    lv_slider_set_value(sliderVolume, model.volume, LV_ANIM_OFF);
    lv_obj_align(sliderVolume, LV_ALIGN_TOP_LEFT, 8, 168);
    lv_obj_add_event_cb(sliderVolume, volume_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);

    labelVolume = lv_label_create(screenMain);
    lv_obj_align(labelVolume, LV_ALIGN_TOP_RIGHT, -8, 161);

    btnShuffle = makeButton(screenMain, 48, 30, 8, 196, "SHF", shuffle_event_cb);
    btnRepeat = makeButton(screenMain, 48, 30, 60, 196, "REP", repeat_event_cb);
    btnFavorite = makeButton(screenMain, 48, 30, 112, 196, "FAV", favorite_event_cb);
    btnSleep = makeButton(screenMain, 48, 30, 164, 196, "SLP", sleep_event_cb);
    lv_obj_t *btnEq = makeButton(screenMain, 48, 30, 216, 196, "EQ", eq_event_cb);
    btnTheme = makeButton(screenMain, 48, 30, 268, 196, "SET", settings_open_event_cb);

    labelShuffleBtn = lv_obj_get_child(btnShuffle, 0);
    labelRepeatBtn = lv_obj_get_child(btnRepeat, 0);
    labelFavoriteBtn = lv_obj_get_child(btnFavorite, 0);
    labelSleepBtn = lv_obj_get_child(btnSleep, 0);
    labelEqBtn = lv_obj_get_child(btnEq, 0);
    labelThemeBtn = lv_obj_get_child(btnTheme, 0);
}

void createSearchScreen() {
    if(screenSearch) return;

    screenSearch = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenSearch, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screenSearch);
    lv_label_set_text(title, "Search");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    searchTextArea = lv_textarea_create(screenSearch);
    lv_obj_set_width(searchTextArea, 304);
    lv_textarea_set_one_line(searchTextArea, true);
    lv_textarea_set_placeholder_text(searchTextArea, "Type to search...");
    lv_obj_align(searchTextArea, LV_ALIGN_TOP_LEFT, 8, 36);
    lv_obj_add_event_cb(searchTextArea, search_key_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);

    searchResultList = lv_list_create(screenSearch);
    lv_obj_set_size(searchResultList, 304, 154);
    lv_obj_align(searchResultList, LV_ALIGN_TOP_LEFT, 8, 70);

    makeButton(screenSearch, 96, 34, 216, 230, "BACK", search_exit_event_cb);
    refreshSearchResults();
}

void createLibraryScreen() {
    if(screenLibrary) return;

    screenLibrary = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenLibrary, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screenLibrary);
    lv_label_set_text(title, "Library");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    libraryScopeBtn = makeButton(screenLibrary, 100, 34, 8, 36, model.libraryScope == LibraryScope::AllTracks ? "ALL" : "ALB", library_scope_event_cb);
    libraryAlbumPrevBtn = makeButton(screenLibrary, 40, 34, 116, 36, "<", library_album_prev_event_cb);
    libraryAlbumNextBtn = makeButton(screenLibrary, 40, 34, 164, 36, ">", library_album_next_event_cb);
    libraryAlbumLabel = lv_label_create(screenLibrary);
    lv_label_set_long_mode(libraryAlbumLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(libraryAlbumLabel, 136);
    lv_obj_align(libraryAlbumLabel, LV_ALIGN_TOP_LEFT, 212, 44);

    libraryTrackList = lv_list_create(screenLibrary);
    lv_obj_set_size(libraryTrackList, 304, 160);
    lv_obj_align(libraryTrackList, LV_ALIGN_TOP_LEFT, 8, 80);

    makeButton(screenLibrary, 96, 34, 216, 230, "BACK", library_exit_event_cb);
    refreshLibraryTracks();
}

void createFavoritesScreen() {
    if(screenFavorites) return;

    screenFavorites = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenFavorites, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screenFavorites);
    lv_label_set_text(title, "Favorites");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    favoritesLabel = lv_label_create(screenFavorites);
    lv_obj_align(favoritesLabel, LV_ALIGN_TOP_LEFT, 8, 44);

    favoritesList = lv_list_create(screenFavorites);
    lv_obj_set_size(favoritesList, 304, 160);
    lv_obj_align(favoritesList, LV_ALIGN_TOP_LEFT, 8, 70);

    btnClearFavorites = makeButton(screenFavorites, 120, 34, 8, 230, "CLEAR FAV", favorites_clear_event_cb);
    makeButton(screenFavorites, 96, 34, 216, 230, "BACK", favorites_exit_event_cb);
    refreshFavoritesList();
}

void createSettingsScreen() {
    if(screenSettings) return;

    screenSettings = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenSettings, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screenSettings);
    lv_label_set_text(title, "Settings");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    btnSettingsTheme = makeButton(screenSettings, 100, 34, 8, 44, themeModeName(model.themeMode), settings_theme_event_cb);
    btnSettingsEq = makeButton(screenSettings, 100, 34, 8, 86, eqModeName(model.eqMode), settings_eq_event_cb);
    btnSettingsSleep = makeButton(screenSettings, 100, 34, 8, 128, sleepModeName(model.sleepMode), settings_sleep_event_cb);

    lv_obj_t *brightnessLabelTitle = lv_label_create(screenSettings);
    lv_label_set_text(brightnessLabelTitle, "Brightness");
    lv_obj_align(brightnessLabelTitle, LV_ALIGN_TOP_LEFT, 8, 176);

    sliderBrightness = lv_slider_create(screenSettings);
    lv_obj_set_size(sliderBrightness, 220, 14);
    lv_slider_set_range(sliderBrightness, 0, 255);
    lv_slider_set_value(sliderBrightness, model.brightness, LV_ANIM_OFF);
    lv_obj_align(sliderBrightness, LV_ALIGN_TOP_LEFT, 8, 198);
    lv_obj_add_event_cb(sliderBrightness, brightness_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);

    labelBrightness = lv_label_create(screenSettings);
    lv_obj_align(labelBrightness, LV_ALIGN_TOP_RIGHT, -8, 192);

    btnClearFavorites = makeButton(screenSettings, 120, 34, 8, 230, "CLEAR FAV", settings_clear_favorites_event_cb);
    makeButton(screenSettings, 96, 34, 216, 230, "BACK", settings_back_event_cb);
    updateSettingsScreen();
}

void updateSettingsScreen() {
    if(!screenSettings) return;

    lv_label_set_text(lv_obj_get_child(btnSettingsTheme, 0), themeModeName(model.themeMode));
    lv_label_set_text(lv_obj_get_child(btnSettingsEq, 0), eqModeName(model.eqMode));
    lv_label_set_text(lv_obj_get_child(btnSettingsSleep, 0), sleepModeName(model.sleepMode));
    lv_slider_set_value(sliderBrightness, model.brightness, LV_ANIM_OFF);

    char brightnessBuf[24];
    snprintf(brightnessBuf, sizeof(brightnessBuf), "BRT %u", model.brightness);
    lv_label_set_text(labelBrightness, brightnessBuf);
}

void createScreensaverScreen() {
    if(screenSaver) return;

    screenSaver = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenSaver, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(screenSaver, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(screenSaver, lv_color_black(), 0);
    lv_obj_add_event_cb(screenSaver, screensaver_event_cb, LV_EVENT_CLICKED, nullptr);

    saverArcLeft = lv_arc_create(screenSaver);
    lv_obj_set_size(saverArcLeft, 90, 90);
    lv_obj_align(saverArcLeft, LV_ALIGN_CENTER, -58, -16);
    lv_arc_set_range(saverArcLeft, 0, 100);
    lv_obj_clear_flag(saverArcLeft, LV_OBJ_FLAG_CLICKABLE);

    saverArcRight = lv_arc_create(screenSaver);
    lv_obj_set_size(saverArcRight, 90, 90);
    lv_obj_align(saverArcRight, LV_ALIGN_CENTER, 58, -16);
    lv_arc_set_range(saverArcRight, 0, 100);
    lv_obj_clear_flag(saverArcRight, LV_OBJ_FLAG_CLICKABLE);

    saverTitleLabel = lv_label_create(screenSaver);
    lv_obj_set_width(saverTitleLabel, 300);
    lv_label_set_long_mode(saverTitleLabel, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_color(saverTitleLabel, lv_color_white(), 0);
    lv_obj_align(saverTitleLabel, LV_ALIGN_BOTTOM_MID, 0, -34);

    saverStateLabel = lv_label_create(screenSaver);
    lv_obj_set_style_text_color(saverStateLabel, lv_color_hex(0x9BE7FF), 0);
    lv_obj_align(saverStateLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void createErrorScreen() {
    if(screenError) return;
    screenError = createPlaceholderScreen("Error", "An error occurred. Press BACK to return to Now Playing.", error_exit_event_cb);
    errorLabel = lv_obj_get_child(screenError, 1);
}

void updateMainScreen() {
    if(!screenMain) return;

    applyThemeToMainScreen();

    lv_label_set_text(labelTitle, activeTrackTitle().c_str());
    lv_label_set_text(labelSubtitle, activeTrackSubtitle().c_str());

    char batteryBuf[20];
    snprintf(batteryBuf, sizeof(batteryBuf), "BAT %u%%", model.batteryPercent);
    lv_label_set_text(labelBattery, batteryBuf);

    char statusBuf[48];
    snprintf(statusBuf, sizeof(statusBuf), "%s %s", playerStateName(model.playerState), appStateName(appState));
    lv_label_set_text(labelStatus, statusBuf);

    uint32_t total = audio.getAudioFileDuration();
    uint32_t current = audio.getAudioCurrentTime();

    if(!model.seekActive) {
        int pos = total > 0 ? (int)((current * 1000UL) / total) : 0;
        lv_slider_set_value(sliderSeek, pos, LV_ANIM_OFF);
    }

    String timeText = formatTime(current) + " / " + formatTime(total);
    lv_label_set_text(labelTime, timeText.c_str());

    lv_slider_set_value(sliderVolume, model.volume, LV_ANIM_OFF);
    char volBuf[18];
    snprintf(volBuf, sizeof(volBuf), "VOL %u", model.volume);
    lv_label_set_text(labelVolume, volBuf);

    lv_label_set_text(labelPlayBtn, model.playerState == PlayerState::Playing ? "PAUSE" : "PLAY");
    lv_label_set_text(labelShuffleBtn, model.shuffle ? "SHF*" : "SHF");

    if(model.repeatMode == RepeatMode::Off) lv_label_set_text(labelRepeatBtn, "REP");
    else if(model.repeatMode == RepeatMode::One) lv_label_set_text(labelRepeatBtn, "RP1");
    else lv_label_set_text(labelRepeatBtn, "RPA");

    bool favorite = hasTracks() && isTrackIndexValid(model.currentIndex) && isFavorite(model.tracks[model.currentIndex].path);
    lv_label_set_text(labelFavoriteBtn, favorite ? "FAV*" : "FAV");
    lv_label_set_text(labelSleepBtn, sleepModeName(model.sleepMode));
    lv_label_set_text(labelEqBtn, eqModeName(model.eqMode));
    lv_label_set_text(labelThemeBtn, "SET");
    syncLvglProUi();
}

static void refreshSearchList() {
    if(!searchResultList) return;
    lv_obj_clean(searchResultList);

    for(size_t i = 0; i < searchResultIndexes.size(); i++) {
        size_t trackIndex = searchResultIndexes[i];
        const Track &track = model.tracks[trackIndex];
        String label = track.title + "\n" + track.album;
        lv_obj_t *btn = lv_list_add_btn(searchResultList, NULL, label.c_str());
        lv_obj_add_event_cb(btn, search_result_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)trackIndex);
    }
}

void updateSearchResults() {
    model.searchSelection = 0;
    if(model.searchQuery.length() == 0) {
        searchResultIndexes.clear();
        for(size_t i = 0; i < model.tracks.size(); i++) {
            searchResultIndexes.push_back(i);
        }
    } else {
        String query = model.searchQuery;
        query.toLowerCase();
        searchResultIndexes.clear();
        for(size_t i = 0; i < model.tracks.size(); i++) {
            String title = model.tracks[i].title;
            String album = model.tracks[i].album;
            String artist = "";
            title.toLowerCase();
            album.toLowerCase();
            artist.toLowerCase();
            if(title.indexOf(query) >= 0 || album.indexOf(query) >= 0 || artist.indexOf(query) >= 0) {
                searchResultIndexes.push_back(i);
            }
        }
    }
    refreshSearchList();
}

void updateSearchScreen() {
    if(!screenSearch) return;
    if(model.searchQuery.length() > 0) {
        lv_textarea_set_text(searchTextArea, model.searchQuery.c_str());
    }
    refreshSearchList();
}

static void refreshLibraryTracks() {
    if(!libraryTrackList) return;
    lv_obj_clean(libraryTrackList);
    libraryTrackIndexes.clear();

    if(model.libraryScope == LibraryScope::AllTracks) {
        for(size_t i = 0; i < model.tracks.size(); i++) {
            libraryTrackIndexes.push_back(i);
        }
    } else if(model.albumFilterIndex >= 0 && model.albumFilterIndex < (int)model.albums.size()) {
        const String &filterAlbum = model.albums[(size_t)model.albumFilterIndex];
        for(size_t i = 0; i < model.tracks.size(); i++) {
            if(model.tracks[i].album == filterAlbum) {
                libraryTrackIndexes.push_back(i);
            }
        }
    }

    for(size_t idx : libraryTrackIndexes) {
        const Track &track = model.tracks[idx];
        String label = track.title + "\n" + track.album;
        lv_obj_t *btn = lv_list_add_btn(libraryTrackList, NULL, label.c_str());
        lv_obj_add_event_cb(btn, library_track_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)idx);
    }
}

void updateLibraryScreen() {
    if(!screenLibrary) return;
    if(model.libraryScope == LibraryScope::AllTracks) {
        lv_label_set_text(libraryAlbumLabel, "All tracks");
        lv_label_set_text(lv_obj_get_child(libraryScopeBtn, 0), "ALL");
    } else if(model.albums.empty()) {
        lv_label_set_text(libraryAlbumLabel, "No albums");
        lv_label_set_text(lv_obj_get_child(libraryScopeBtn, 0), "ALL");
    } else {
        int albumIndex = model.albumFilterIndex < 0 ? 0 : model.albumFilterIndex;
        if(albumIndex >= (int)model.albums.size()) albumIndex = 0;
        lv_label_set_text(libraryAlbumLabel, model.albums[albumIndex].c_str());
        lv_label_set_text(lv_obj_get_child(libraryScopeBtn, 0), "ALB");
    }
    refreshLibraryTracks();
}

static void refreshFavoritesList() {
    if(!favoritesList) return;
    lv_obj_clean(favoritesList);

    for(size_t idx : favoritesTrackIndexes) {
        const Track &track = model.tracks[idx];
        String label = track.title + "\n" + track.album;
        lv_obj_t *btn = lv_list_add_btn(favoritesList, NULL, label.c_str());
        lv_obj_add_event_cb(btn, favorites_track_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)idx);
    }
}

void updateFavoritesScreen() {
    if(!screenFavorites) return;
    favoritesTrackIndexes.clear();
    for(size_t i = 0; i < model.tracks.size(); i++) {
        if(isFavorite(model.tracks[i].path)) {
            favoritesTrackIndexes.push_back(i);
        }
    }
    char labelBuf[32];
    snprintf(labelBuf, sizeof(labelBuf), "%u favorites", (unsigned)favoritesTrackIndexes.size());
    lv_label_set_text(favoritesLabel, labelBuf);
    refreshFavoritesList();
}

void updateScreensaverScreen() {
    if(!screenSaver) return;

    screensaverPhase = (screensaverPhase + 8) % 100;
    lv_arc_set_value(saverArcLeft, screensaverPhase);
    lv_arc_set_value(saverArcRight, 100 - screensaverPhase);

    lv_label_set_text(saverTitleLabel, activeTrackTitle().c_str());

    String state = String(playerStateName(model.playerState)) + " | " + String(model.tracks.size()) + " tracks";
    if(model.shuffle) state += " | SHF";
    if(model.repeatMode != RepeatMode::Off) state += " | REP";
    lv_label_set_text(saverStateLabel, state.c_str());
}

static void play_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    togglePlayPause();
}

static void next_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    playNextTrack();
}

static void previous_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    playPreviousTrack();
}

static void shuffle_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    model.shuffle = !model.shuffle;
    updateMainScreen();
    saveSettings();
}

static void repeat_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.repeatMode == RepeatMode::Off) model.repeatMode = RepeatMode::One;
    else if(model.repeatMode == RepeatMode::One) model.repeatMode = RepeatMode::All;
    else model.repeatMode = RepeatMode::Off;
    updateMainScreen();
    saveSettings();
}

static void favorite_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    toggleFavorite();
}

static void search_open_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::Search);
}

static void library_open_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::Library);
}

static void favorites_open_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::Favorites);
}

static void volume_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    model.volume = (uint8_t)lv_slider_get_value(sliderVolume);
    audio.setVolume(model.volume);
    updateMainScreen();
    saveSettings();
}

static void seek_event_cb(lv_event_t *e) {
    recordInteraction();

    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED) {
        model.seekActive = true;
        return;
    }

    if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        model.seekActive = false;
        uint32_t total = audio.getAudioFileDuration();
        int pos = lv_slider_get_value(sliderSeek);
        uint32_t sec = total > 0 ? (uint32_t)((pos * total) / 1000UL) : 0;
        seekToSecond(sec);
        updateMainScreen();
        return;
    }

    if(code == LV_EVENT_VALUE_CHANGED && model.seekActive) {
        uint32_t total = audio.getAudioFileDuration();
        int pos = lv_slider_get_value(sliderSeek);
        uint32_t sec = total > 0 ? (uint32_t)((pos * total) / 1000UL) : 0;
        String timeText = formatTime(sec) + " / " + formatTime(total);
        lv_label_set_text(labelTime, timeText.c_str());
    }
}

static void sleep_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    cycleSleepTimer();
}

static void eq_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    uint8_t next = (uint8_t)model.eqMode + 1;
    if(next > (uint8_t)EqMode::Jazz) next = 0;
    model.eqMode = (EqMode)next;
    applyEqMode();
    updateMainScreen();
}

static void theme_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();

    uint8_t next = (uint8_t)model.themeMode + 1;
    if(next > (uint8_t)ThemeMode::Minimal) next = 0;
    model.themeMode = (ThemeMode)next;
    updateMainScreen();
}

static void search_key_event_cb(lv_event_t *e) {
    if(!searchTextArea) return;
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        const char *text = lv_textarea_get_text(searchTextArea);
        model.searchQuery = text ? text : "";
        updateSearchResults();
    }
}

static void search_result_event_cb(lv_event_t *e) {
    recordInteraction();
    size_t trackIndex = (size_t)(uintptr_t)lv_event_get_user_data(e);
    if(isTrackIndexValid(trackIndex)) {
        startTrack(trackIndex);
    }
}

static void search_exit_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}

static void library_track_event_cb(lv_event_t *e) {
    recordInteraction();
    size_t trackIndex = (size_t)(uintptr_t)lv_event_get_user_data(e);
    if(isTrackIndexValid(trackIndex)) {
        startTrack(trackIndex);
    }
}

static void library_scope_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.libraryScope == LibraryScope::AllTracks) {
        model.libraryScope = LibraryScope::AlbumOnly;
        if(model.albumFilterIndex < 0 && !model.albums.empty()) {
            model.albumFilterIndex = 0;
        }
    } else {
        model.libraryScope = LibraryScope::AllTracks;
    }
    updateLibraryScreen();
}

static void library_album_prev_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.albums.empty()) return;
    if(model.albumFilterIndex <= 0) model.albumFilterIndex = (int)model.albums.size() - 1;
    else model.albumFilterIndex--;
    updateLibraryScreen();
}

static void library_album_next_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.albums.empty()) return;
    model.albumFilterIndex = (model.albumFilterIndex + 1) % (int)model.albums.size();
    updateLibraryScreen();
}

static void library_exit_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}

static void favorites_track_event_cb(lv_event_t *e) {
    recordInteraction();
    size_t trackIndex = (size_t)(uintptr_t)lv_event_get_user_data(e);
    if(isTrackIndexValid(trackIndex)) {
        startTrack(trackIndex);
    }
}

static void favorites_clear_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    clearFavorites();
    updateFavoritesScreen();
    updateMainScreen();
}

static void favorites_exit_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}

static void settings_open_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::Settings);
}

static void settings_back_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}

static void settings_theme_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    uint8_t next = (uint8_t)model.themeMode + 1;
    if(next > (uint8_t)ThemeMode::Minimal) next = 0;
    model.themeMode = (ThemeMode)next;
    updateSettingsScreen();
    updateMainScreen();
    saveSettings();
}

static void settings_eq_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    uint8_t next = (uint8_t)model.eqMode + 1;
    if(next > (uint8_t)EqMode::Jazz) next = 0;
    model.eqMode = (EqMode)next;
    applyEqMode();
    updateSettingsScreen();
    updateMainScreen();
    saveSettings();
}

static void settings_sleep_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    cycleSleepTimer();
    updateSettingsScreen();
    updateMainScreen();
    saveSettings();
}

static void brightness_event_cb(lv_event_t *e) {
    (void)e;
    if(!sliderBrightness) return;
    model.brightness = (uint8_t)lv_slider_get_value(sliderBrightness);
    setBacklight(true);
    updateSettingsScreen();
    saveSettings();
}

static void settings_clear_favorites_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    clearFavorites();
    updateFavoritesScreen();
    updateMainScreen();
}

static void screensaver_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}

static void error_exit_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}
