#ifndef MP3_PLAYER_STORAGE_H
#define MP3_PLAYER_STORAGE_H

#include <Arduino.h>
#include <FS.h>

void scanSDCard(fs::FS &fs, const char *dirname, uint8_t levels);
void loadFavorites();
void saveFavorites();
bool isFavorite(const String &path);
void toggleFavorite();

#endif
