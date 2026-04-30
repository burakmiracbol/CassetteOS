#include "Ui.h"
#include "Model.h"
#include "Player.h"
#include "Storage.h"

static lv_obj_t *screenBoot = nullptr;
lv_obj_t *screenMain = nullptr;
lv_obj_t *screenSearch = nullptr;
lv_obj_t *screenLibrary = nullptr;
lv_obj_t *screenFavorites = nullptr;
lv_obj_t *screenSaver = nullptr;
lv_obj_t *screenError = nullptr;

static lv_obj_t *bootLabel = nullptr;
static lv_obj_t *errorLabel = nullptr;

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

static lv_obj_t *btnShuffle = nullptr;
static lv_obj_t *btnRepeat = nullptr;
static lv_obj_t *btnFavorite = nullptr;
static lv_obj_t *btnSleep = nullptr;
static lv_obj_t *btnTheme = nullptr;

// Search screen
static lv_obj_t *searchInputLabel = nullptr;
static lv_obj_t *searchResultList = nullptr;
static String searchQuery;
static std::vector<size_t> searchResults;

// Library screen
static lv_obj_t *libraryList = nullptr;
static lv_obj_t *libraryScopeLabel = nullptr;
static lv_obj_t *libraryAlbumLabel = nullptr;
static std::vector<size_t> libraryIndexes;

// Favorites screen
static lv_obj_t *favoritesList = nullptr;
static lv_obj_t *favoritesInfoLabel = nullptr;
static std::vector<size_t> favoriteIndexes;

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

static void library_track_event_cb(lv_event_t *e);
static void library_scope_event_cb(lv_event_t *e);
static void library_album_prev_event_cb(lv_event_t *e);
static void library_album_next_event_cb(lv_event_t *e);
static void library_exit_event_cb(lv_event_t *e);

static void favorites_track_event_cb(lv_event_t *e);
static void favorites_clear_event_cb(lv_event_t *e);
static void favorites_exit_event_cb(lv_event_t *e);

static void screensaver_event_cb(lv_event_t *e);
static void error_exit_event_cb(lv_event_t *e);

void loadScreen(lv_obj_t *screen) {
#if LVGL_VERSION_MAJOR >= 9
    lv_screen_load(screen);
#else
    lv_scr_load(screen);
#endif
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
    styleToggleButton(btnTheme, true, accent, accent);
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
    btnTheme = makeButton(screenMain, 48, 30, 268, 196, "TH1", theme_event_cb);

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
    lv_obj_set_style_bg_color(screenSearch, lv_color_black(), 0);

    lv_obj_t *title = lv_label_create(screenSearch);
    lv_label_set_text(title, "Search");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    makeButton(screenSearch, 54, 28, 258, 4, "BACK", search_exit_event_cb);

    searchInputLabel = lv_label_create(screenSearch);
    lv_obj_set_width(searchInputLabel, 300);
    lv_label_set_long_mode(searchInputLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_color(searchInputLabel, lv_color_hex(0x9BE7FF), 0);
    lv_obj_align(searchInputLabel, LV_ALIGN_TOP_LEFT, 10, 36);

    searchResultList = lv_list_create(screenSearch);
    lv_obj_set_size(searchResultList, 300, 88);
    lv_obj_align(searchResultList, LV_ALIGN_TOP_LEFT, 10, 56);

    static const char *keys[4][8] = {
        {"A", "B", "C", "D", "E", "F", "G", "H"},
        {"I", "J", "K", "L", "M", "N", "O", "P"},
        {"Q", "R", "S", "T", "U", "V", "W", "X"},
        {"Y", "Z", "<", "SP", "CH", "SH", "OK", "CLR"}
    };

    for(uint8_t row = 0; row < 4; row++) {
        for(uint8_t col = 0; col < 8; col++) {
            lv_obj_t *btn = lv_btn_create(screenSearch);
            lv_obj_set_size(btn, 34, 24);
            lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 10 + (col * 37), 152 + (row * 21));
            lv_obj_add_event_cb(btn, search_key_event_cb, LV_EVENT_CLICKED, nullptr);

            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, keys[row][col]);
            lv_obj_center(label);
        }
    }
}

void createLibraryScreen() {
    if(screenLibrary) return;

    screenLibrary = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenLibrary, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screenLibrary);
    lv_label_set_text(title, "Library");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    makeButton(screenLibrary, 54, 28, 258, 4, "BACK", library_exit_event_cb);

    lv_obj_t *scopeBtn = makeButton(screenLibrary, 56, 26, 8, 34, "SCOPE", library_scope_event_cb);
    libraryScopeLabel = lv_obj_get_child(scopeBtn, 0);

    makeButton(screenLibrary, 36, 26, 72, 34, "<", library_album_prev_event_cb);
    makeButton(screenLibrary, 36, 26, 276, 34, ">", library_album_next_event_cb);

    libraryAlbumLabel = lv_label_create(screenLibrary);
    lv_obj_set_width(libraryAlbumLabel, 160);
    lv_label_set_long_mode(libraryAlbumLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_align(libraryAlbumLabel, LV_ALIGN_TOP_MID, 0, 40);

    libraryList = lv_list_create(screenLibrary);
    lv_obj_set_size(libraryList, 300, 168);
    lv_obj_align(libraryList, LV_ALIGN_TOP_LEFT, 10, 66);
}

void createFavoritesScreen() {
    if(screenFavorites) return;

    screenFavorites = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenFavorites, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screenFavorites);
    lv_label_set_text(title, "Favorites");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    makeButton(screenFavorites, 54, 28, 258, 4, "BACK", favorites_exit_event_cb);
    makeButton(screenFavorites, 54, 28, 200, 4, "CLR", favorites_clear_event_cb);

    favoritesInfoLabel = lv_label_create(screenFavorites);
    lv_obj_set_width(favoritesInfoLabel, 220);
    lv_label_set_long_mode(favoritesInfoLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_align(favoritesInfoLabel, LV_ALIGN_TOP_LEFT, 8, 38);

    favoritesList = lv_list_create(screenFavorites);
    lv_obj_set_size(favoritesList, 300, 174);
    lv_obj_align(favoritesList, LV_ALIGN_TOP_LEFT, 10, 62);
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

    screenError = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screenError, lv_color_hex(0x210B0B), 0);

    errorLabel = lv_label_create(screenError);
    lv_obj_set_width(errorLabel, 286);
    lv_label_set_long_mode(errorLabel, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_color(errorLabel, lv_color_white(), 0);
    lv_label_set_text(errorLabel, "Error");
    lv_obj_align(errorLabel, LV_ALIGN_CENTER, 0, -20);

    makeButton(screenError, 90, 34, 114, 170, "HOME", error_exit_event_cb);
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
    lv_label_set_text(labelThemeBtn, themeModeName(model.themeMode));
}

void updateSearchResults() {
    searchResults.clear();

    String query = normalizeText(searchQuery);
    for(size_t i = 0; i < model.tracks.size(); i++) {
        String haystack = normalizeText(model.tracks[i].title + " " + model.tracks[i].album + " " + model.tracks[i].path);
        if(query.length() == 0 || haystack.indexOf(query) >= 0) searchResults.push_back(i);
    }

    if(searchResults.empty()) model.searchSelection = 0;
    else if(model.searchSelection >= searchResults.size()) model.searchSelection = searchResults.size() - 1;
}

void updateSearchScreen() {
    if(!screenSearch) return;

    String inputText = searchQuery.length() > 0 ? searchQuery : String("Type to filter");
    lv_label_set_text(searchInputLabel, inputText.c_str());
    lv_obj_clean(searchResultList);

    uint8_t count = min((size_t)8, searchResults.size());
    for(uint8_t i = 0; i < count; i++) {
        size_t trackIndex = searchResults[i];
        String row = model.tracks[trackIndex].title + " - " + model.tracks[trackIndex].album;
        lv_obj_t *entry = lv_list_add_btn(searchResultList, nullptr, row.c_str());
        lv_obj_add_event_cb(entry, search_result_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)trackIndex);
    }
}

void updateLibraryScreen() {
    if(!screenLibrary) return;

    collectLibraryTrackIndexes(libraryIndexes, model.libraryScope, model.albumFilterIndex);
    lv_label_set_text(libraryScopeLabel, model.libraryScope == LibraryScope::AllTracks ? "ALL" : "ALB");

    String albumText = "All albums";
    if(model.libraryScope == LibraryScope::AlbumOnly && model.albumFilterIndex >= 0 && model.albumFilterIndex < (int)model.albums.size()) {
        albumText = model.albums[(size_t)model.albumFilterIndex];
    }
    lv_label_set_text(libraryAlbumLabel, albumText.c_str());

    lv_obj_clean(libraryList);
    for(size_t idx : libraryIndexes) {
        String row = model.tracks[idx].title + " - " + model.tracks[idx].album;
        lv_obj_t *entry = lv_list_add_btn(libraryList, nullptr, row.c_str());
        lv_obj_add_event_cb(entry, library_track_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)idx);
    }
}

void updateFavoritesScreen() {
    if(!screenFavorites) return;

    collectFavoriteTrackIndexes(favoriteIndexes);
    String info = String(favoriteIndexes.size()) + " items";
    lv_label_set_text(favoritesInfoLabel, info.c_str());

    lv_obj_clean(favoritesList);
    for(size_t idx : favoriteIndexes) {
        String row = model.tracks[idx].title + " - " + model.tracks[idx].album;
        lv_obj_t *entry = lv_list_add_btn(favoritesList, nullptr, row.c_str());
        lv_obj_add_event_cb(entry, favorites_track_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)idx);
    }
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
}

static void repeat_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.repeatMode == RepeatMode::Off) model.repeatMode = RepeatMode::One;
    else if(model.repeatMode == RepeatMode::One) model.repeatMode = RepeatMode::All;
    else model.repeatMode = RepeatMode::Off;
    updateMainScreen();
}

static void favorite_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    toggleFavorite();
}

static void search_open_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    searchQuery = "";
    updateSearchResults();
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
    recordInteraction();

    lv_obj_t *btn = eventTargetObj(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    if(!label) return;

    String key = lv_label_get_text(label);
    if(key == "<") {
        if(searchQuery.length() > 0) searchQuery.remove(searchQuery.length() - 1);
    } else if(key == "SP") {
        searchQuery += ' ';
    } else if(key == "CLR") {
        searchQuery = "";
    } else if(key == "OK") {
        if(!searchResults.empty()) startTrack(searchResults[0]);
        return;
    } else if(key == "CH") {
        searchQuery += "CH";
    } else if(key == "SH") {
        searchQuery += "SH";
    } else {
        searchQuery += key;
    }

    updateSearchResults();
    updateSearchScreen();
}

static void search_result_event_cb(lv_event_t *e) {
    recordInteraction();
    size_t trackIndex = (size_t)(uintptr_t)lv_event_get_user_data(e);
    startTrack(trackIndex);
}

static void search_exit_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}

static void library_track_event_cb(lv_event_t *e) {
    recordInteraction();
    size_t trackIndex = (size_t)(uintptr_t)lv_event_get_user_data(e);
    startTrack(trackIndex);
}

static void library_scope_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.libraryScope == LibraryScope::AllTracks) {
        model.libraryScope = LibraryScope::AlbumOnly;
        if(model.albums.empty()) model.albumFilterIndex = -1;
        else if(model.albumFilterIndex < 0) model.albumFilterIndex = 0;
    } else {
        model.libraryScope = LibraryScope::AllTracks;
        model.albumFilterIndex = -1;
    }
    updateLibraryScreen();
}

static void library_album_prev_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.libraryScope != LibraryScope::AlbumOnly || model.albums.empty()) return;
    if(model.albumFilterIndex <= 0) model.albumFilterIndex = (int)model.albums.size() - 1;
    else model.albumFilterIndex--;
    updateLibraryScreen();
}

static void library_album_next_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    if(model.libraryScope != LibraryScope::AlbumOnly || model.albums.empty()) return;
    if(model.albumFilterIndex < 0 || model.albumFilterIndex >= (int)model.albums.size() - 1) model.albumFilterIndex = 0;
    else model.albumFilterIndex++;
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
    startTrack(trackIndex);
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
