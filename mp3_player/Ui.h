#ifndef MP3_PLAYER_UI_H
#define MP3_PLAYER_UI_H

#include "Hardware.h"

extern lv_obj_t *screenMain;
extern lv_obj_t *screenSearch;
extern lv_obj_t *screenLibrary;
extern lv_obj_t *screenFavorites;
extern lv_obj_t *screenSaver;
extern lv_obj_t *screenError;

void loadScreen(lv_obj_t *screen);

void showBootMessage(const char *message);
void showError(const char *message);

void createMainScreen();
void createSearchScreen();
void createLibraryScreen();
void createFavoritesScreen();
void createScreensaverScreen();
void createErrorScreen();

void updateMainScreen();
void updateSearchResults();
void updateSearchScreen();
void updateLibraryScreen();
void updateFavoritesScreen();
void updateScreensaverScreen();

#endif
