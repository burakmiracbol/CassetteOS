#include "Ui.h"
#include "Model.h"
#include "Player.h"
#include "Storage.h"

static lv_obj_t *screenBoot = nullptr;
lv_obj_t *screenMain = nullptr;
lv_obj_t *screenSearch = nullptr;
lv_obj_t *screenSaver = nullptr;
lv_obj_t *screenError = nullptr;

static lv_obj_t *bootLabel = nullptr;
static lv_obj_t *errorLabel = nullptr;

static lv_obj_t *labelTitle = nullptr;
static lv_obj_t *labelAlbum = nullptr;
static lv_obj_t *labelStatus = nullptr;
static lv_obj_t *labelBattery = nullptr;
static lv_obj_t *labelTime = nullptr;
static lv_obj_t *progressBar = nullptr;
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

static lv_obj_t *searchInputLabel = nullptr;
static lv_obj_t *searchResultList = nullptr;
static String searchQuery;
static std::vector<size_t> searchResults;

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
static void volume_event_cb(lv_event_t *e);
static void sleep_event_cb(lv_event_t *e);
static void eq_event_cb(lv_event_t *e);
static void theme_event_cb(lv_event_t *e);
static void search_key_event_cb(lv_event_t *e);
static void search_result_event_cb(lv_event_t *e);
static void search_exit_event_cb(lv_event_t *e);
static void screensaver_event_cb(lv_event_t *e);

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
    lv_label_set_text(errorLabel, message);
    setAppState(AppState::Error);
}

void createMainScreen() {
    if(screenMain) return;

    screenMain = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenMain, LV_OBJ_FLAG_SCROLLABLE);

    labelTitle = lv_label_create(screenMain);
    lv_obj_set_width(labelTitle, 220);
    lv_label_set_long_mode(labelTitle, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_align(labelTitle, LV_ALIGN_TOP_LEFT, 8, 6);

    labelBattery = lv_label_create(screenMain);
    lv_obj_align(labelBattery, LV_ALIGN_TOP_RIGHT, -8, 6);

    labelAlbum = lv_label_create(screenMain);
    lv_obj_set_width(labelAlbum, 220);
    lv_label_set_long_mode(labelAlbum, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_align(labelAlbum, LV_ALIGN_TOP_LEFT, 8, 30);

    labelStatus = lv_label_create(screenMain);
    lv_obj_align(labelStatus, LV_ALIGN_TOP_RIGHT, -8, 30);

    progressBar = lv_bar_create(screenMain);
    lv_obj_set_size(progressBar, 304, 12);
    lv_bar_set_range(progressBar, 0, 1000);
    lv_obj_align(progressBar, LV_ALIGN_TOP_LEFT, 8, 62);

    labelTime = lv_label_create(screenMain);
    lv_obj_align(labelTime, LV_ALIGN_TOP_LEFT, 8, 80);

    makeButton(screenMain, 66, 38, 34, 112, "PREV", previous_event_cb);
    lv_obj_t *btnPlay = makeButton(screenMain, 88, 38, 116, 112, "PLAY", play_event_cb);
    makeButton(screenMain, 66, 38, 220, 112, "NEXT", next_event_cb);
    labelPlayBtn = lv_obj_get_child(btnPlay, 0);

    sliderVolume = lv_slider_create(screenMain);
    lv_obj_set_size(sliderVolume, 215, 16);
    lv_slider_set_range(sliderVolume, 0, 21);
    lv_slider_set_value(sliderVolume, model.volume, LV_ANIM_OFF);
    lv_obj_align(sliderVolume, LV_ALIGN_TOP_LEFT, 8, 166);
    lv_obj_add_event_cb(sliderVolume, volume_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);

    labelVolume = lv_label_create(screenMain);
    lv_obj_align(labelVolume, LV_ALIGN_TOP_RIGHT, -8, 160);

    makeButton(screenMain, 40, 34, 6, 198, "SRH", search_open_event_cb);
    btnShuffle = makeButton(screenMain, 40, 34, 51, 198, "SHF", shuffle_event_cb);
    btnRepeat = makeButton(screenMain, 40, 34, 96, 198, "REP", repeat_event_cb);
    btnFavorite = makeButton(screenMain, 40, 34, 141, 198, "FAV", favorite_event_cb);
    btnSleep = makeButton(screenMain, 40, 34, 186, 198, "SLP", sleep_event_cb);
    lv_obj_t *btnEq = makeButton(screenMain, 40, 34, 231, 198, "EQ", eq_event_cb);
    btnTheme = makeButton(screenMain, 40, 34, 276, 198, "DRK", theme_event_cb);

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
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

    makeButton(screenSearch, 54, 28, 258, 4, "BACK", search_exit_event_cb);

    searchInputLabel = lv_label_create(screenSearch);
    lv_obj_set_width(searchInputLabel, 300);
    lv_label_set_long_mode(searchInputLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_color(searchInputLabel, lv_color_hex(0x9BE7FF), 0);
    lv_obj_align(searchInputLabel, LV_ALIGN_TOP_LEFT, 10, 36);

    searchResultList = lv_list_create(screenSearch);
    lv_obj_set_size(searchResultList, 300, 84);
    lv_obj_align(searchResultList, LV_ALIGN_TOP_LEFT, 10, 58);

    static const char *keys[3][10] = {
        {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"},
        {"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T"},
        {"U", "V", "W", "X", "Y", "Z", "<", "SP", "OK", "CLR"}
    };

    for(uint8_t row = 0; row < 3; row++) {
        for(uint8_t col = 0; col < 10; col++) {
            lv_obj_t *btn = lv_btn_create(screenSearch);
            lv_obj_set_size(btn, 28, 28);
            lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 10 + (col * 30), 150 + (row * 30));
            lv_obj_add_event_cb(btn, search_key_event_cb, LV_EVENT_CLICKED, nullptr);

            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, keys[row][col]);
            lv_obj_center(label);
        }
    }
}

void createScreensaverScreen() {
    if(screenSaver) return;

    screenSaver = lv_obj_create(nullptr);
    lv_obj_clear_flag(screenSaver, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(screenSaver, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(screenSaver, lv_color_black(), 0);
    lv_obj_add_event_cb(screenSaver, screensaver_event_cb, LV_EVENT_CLICKED, nullptr);

    saverArcLeft = lv_arc_create(screenSaver);
    lv_obj_set_size(saverArcLeft, 86, 86);
    lv_obj_align(saverArcLeft, LV_ALIGN_CENTER, -55, -12);
    lv_arc_set_range(saverArcLeft, 0, 100);
    lv_obj_clear_flag(saverArcLeft, LV_OBJ_FLAG_CLICKABLE);

    saverArcRight = lv_arc_create(screenSaver);
    lv_obj_set_size(saverArcRight, 86, 86);
    lv_obj_align(saverArcRight, LV_ALIGN_CENTER, 55, -12);
    lv_arc_set_range(saverArcRight, 0, 100);
    lv_obj_clear_flag(saverArcRight, LV_OBJ_FLAG_CLICKABLE);

    saverTitleLabel = lv_label_create(screenSaver);
    lv_obj_set_width(saverTitleLabel, 292);
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
    lv_label_set_text(errorLabel, "Hata");
    lv_obj_align(errorLabel, LV_ALIGN_CENTER, 0, 0);
}

static void setLabelColor(lv_obj_t *label, lv_color_t color) {
    if(label) lv_obj_set_style_text_color(label, color, 0);
}

static void styleToggleButton(lv_obj_t *btn, bool on) {
    if(!btn) return;
    lv_obj_set_style_bg_color(btn, on ? lv_color_hex(0x20A36B) : lv_color_hex(0x3C4B64), 0);
}

static void applyMainTheme() {
    if(!screenMain) return;

    lv_color_t bg = model.darkMode ? lv_color_black() : lv_color_hex(0xF4F7FB);
    lv_color_t fg = model.darkMode ? lv_color_white() : lv_color_hex(0x172033);
    lv_color_t muted = model.darkMode ? lv_color_hex(0xAAB7C4) : lv_color_hex(0x516070);

    lv_obj_set_style_bg_color(screenMain, bg, 0);
    setLabelColor(labelTitle, fg);
    setLabelColor(labelAlbum, muted);
    setLabelColor(labelStatus, fg);
    setLabelColor(labelBattery, muted);
    setLabelColor(labelTime, muted);
    setLabelColor(labelVolume, fg);
}

void updateMainScreen() {
    if(!screenMain) return;

    applyMainTheme();

    lv_label_set_text(labelTitle, currentTrackTitle().c_str());
    lv_label_set_text(labelAlbum, currentTrackAlbum().c_str());

    char batteryBuf[16];
    snprintf(batteryBuf, sizeof(batteryBuf), "BAT %u%%", model.batteryPercent);
    lv_label_set_text(labelBattery, batteryBuf);

    char statusBuf[40];
    snprintf(statusBuf, sizeof(statusBuf), "%s %s", playerStateName(model.playerState), appStateName(appState));
    lv_label_set_text(labelStatus, statusBuf);

    uint32_t total = audio.getAudioFileDuration();
    uint32_t current = audio.getAudioCurrentTime();
    int progress = total > 0 ? (int)((current * 1000UL) / total) : 0;
    lv_bar_set_value(progressBar, progress, LV_ANIM_OFF);

    String timeText = formatTime(current) + " / " + formatTime(total);
    lv_label_set_text(labelTime, timeText.c_str());

    lv_slider_set_value(sliderVolume, model.volume, LV_ANIM_OFF);
    char volumeBuf[16];
    snprintf(volumeBuf, sizeof(volumeBuf), "VOL %u", model.volume);
    lv_label_set_text(labelVolume, volumeBuf);

    lv_label_set_text(labelPlayBtn, model.playerState == PlayerState::Playing ? "PAUSE" : "PLAY");
    lv_label_set_text(labelShuffleBtn, model.shuffle ? "SHF*" : "SHF");

    if(model.repeatMode == RepeatMode::Off) lv_label_set_text(labelRepeatBtn, "REP");
    else if(model.repeatMode == RepeatMode::One) lv_label_set_text(labelRepeatBtn, "RP1");
    else lv_label_set_text(labelRepeatBtn, "RPA");

    bool favorite = hasTracks() && isFavorite(model.tracks[model.currentIndex].path);
    lv_label_set_text(labelFavoriteBtn, favorite ? "FAV*" : "FAV");
    lv_label_set_text(labelSleepBtn, sleepModeName(model.sleepMode));
    lv_label_set_text(labelEqBtn, eqModeName(model.eqMode));
    lv_label_set_text(labelThemeBtn, model.darkMode ? "DRK" : "LIT");

    styleToggleButton(btnShuffle, model.shuffle);
    styleToggleButton(btnRepeat, model.repeatMode != RepeatMode::Off);
    styleToggleButton(btnFavorite, favorite);
    styleToggleButton(btnSleep, model.sleepMode != SleepTimerMode::Off);
    styleToggleButton(btnTheme, model.darkMode);
}

void updateSearchResults() {
    searchResults.clear();

    String query = normalizeText(searchQuery);
    for(size_t i = 0; i < model.tracks.size(); i++) {
        String haystack = normalizeText(model.tracks[i].title + " " + model.tracks[i].album + " " + model.tracks[i].path);
        if(query.length() == 0 || haystack.indexOf(query) >= 0) {
            searchResults.push_back(i);
        }
    }
}

void updateSearchScreen() {
    if(!screenSearch) return;

    String inputText = searchQuery.length() > 0 ? searchQuery : String("Type to filter tracks");
    lv_label_set_text(searchInputLabel, inputText.c_str());
    lv_obj_clean(searchResultList);

    uint8_t count = min((size_t)4, searchResults.size());
    for(uint8_t i = 0; i < count; i++) {
        size_t trackIndex = searchResults[i];
        String row = model.tracks[trackIndex].title + " - " + model.tracks[trackIndex].album;
        lv_obj_t *entry = lv_list_add_btn(searchResultList, nullptr, row.c_str());
        lv_obj_add_event_cb(entry, search_result_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)trackIndex);
    }
}

void updateScreensaverScreen() {
    if(!screenSaver) return;

    screensaverPhase = (screensaverPhase + 9) % 100;
    lv_arc_set_value(saverArcLeft, screensaverPhase);
    lv_arc_set_value(saverArcRight, 100 - screensaverPhase);
    lv_label_set_text(saverTitleLabel, currentTrackTitle().c_str());

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
    setAppState(AppState::Search);
}

static void volume_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    model.volume = (uint8_t)lv_slider_get_value(sliderVolume);
    audio.setVolume(model.volume);
    updateMainScreen();
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
    updateMainScreen();
}

static void theme_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    model.darkMode = !model.darkMode;
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

static void screensaver_event_cb(lv_event_t *e) {
    (void)e;
    recordInteraction();
    setAppState(AppState::NowPlaying);
}
