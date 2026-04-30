#include "Storage.h"
#include "Model.h"
#include "Ui.h"
#include <SD.h>

void scanSDCard(fs::FS &fs, const char *dirname, uint8_t levels) {
    File root = fs.open(dirname);
    if(!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while(file) {
        if(file.isDirectory()) {
            if(levels > 0) scanSDCard(fs, file.path(), levels - 1);
        } else {
            String path = String(file.path());
            if(path.endsWith(".mp3") || path.endsWith(".MP3")) {
                Track track;
                track.path = path;
                track.title = fileTitleFromPath(path);
                track.album = albumFromPath(path);
                model.tracks.push_back(track);
            }
        }
        file = root.openNextFile();
    }
}

void loadFavorites() {
    model.favorites.clear();
    File f = SD.open("/favorites.txt", FILE_READ);
    if(!f) return;

    while(f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        if(line.length() > 0) model.favorites.push_back(line);
    }
    f.close();
}

void saveFavorites() {
    File f = SD.open("/favorites.txt", FILE_WRITE);
    if(!f) return;

    for(const String &path : model.favorites) {
        f.println(path);
    }
    f.close();
}

bool isFavorite(const String &path) {
    for(const String &favorite : model.favorites) {
        if(favorite == path) return true;
    }
    return false;
}

void toggleFavorite() {
    if(!hasTracks()) return;

    String path = model.tracks[model.currentIndex].path;
    for(size_t i = 0; i < model.favorites.size(); i++) {
        if(model.favorites[i] == path) {
            model.favorites.erase(model.favorites.begin() + i);
            saveFavorites();
            updateMainScreen();
            return;
        }
    }

    model.favorites.push_back(path);
    saveFavorites();
    updateMainScreen();
}
