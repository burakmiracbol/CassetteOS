#ifndef MP3_PLAYER_STORAGE_H
#define MP3_PLAYER_STORAGE_H

#include <Arduino.h>
#include <FS.h>
#include <vector>
#include "Model.h"

void scanSDCard(fs::FS &fs, const char *dirname, uint8_t levels);
void rebuildAlbumIndex();

void loadFavorites();
void saveFavorites();
bool isFavorite(const String &path);
void toggleFavorite();
void clearFavorites();

void collectFavoriteTrackIndexes(std::vector<size_t> &out);
void collectLibraryTrackIndexes(std::vector<size_t> &out, LibraryScope scope, int albumFilterIndex);

#endif
