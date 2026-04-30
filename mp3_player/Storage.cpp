#include "Storage.h"
#include "Config.h"
#include "Model.h"
#include "Ui.h"
#include <SD.h>

static bool endsWithMp3(const String &path) {
    String lower = path;
    lower.toLowerCase();
    return lower.endsWith(".mp3");
}

static bool isHiddenPath(const String &path) {
    return path.indexOf("/.") >= 0;
}

static bool trackPathExists(const String &path) {
    for(const Track &track : model.tracks) {
        if(track.path == path) return true;
    }
    return false;
}

void scanSDCard(fs::FS &fs, const char *dirname, uint8_t levels) {
    File root = fs.open(dirname);
    if(!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while(file) {
        String path = String(file.path());
        if(path.length() > 0 && path.length() <= MAX_TRACK_PATH_LEN && !isHiddenPath(path)) {
            if(file.isDirectory()) {
                if(levels > 0) {
                    scanSDCard(fs, file.path(), levels - 1);
                }
            } else if(endsWithMp3(path) && !trackPathExists(path)) {
                Track track;
                track.path = path;
                track.title = fileTitleFromPath(path);
                track.album = albumFromPath(path);
                model.tracks.push_back(track);
            }
        }
        file = root.openNextFile();
    }

    rebuildAlbumIndex();
}

void rebuildAlbumIndex() {
    model.albums.clear();

    for(const Track &track : model.tracks) {
        bool found = false;
        for(const String &album : model.albums) {
            if(album == track.album) {
                found = true;
                break;
            }
        }
        if(!found) model.albums.push_back(track.album);
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
    if(!hasTracks() || !isTrackIndexValid(model.currentIndex)) return;

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

void clearFavorites() {
    model.favorites.clear();
    saveFavorites();
}

void collectFavoriteTrackIndexes(std::vector<size_t> &out) {
    out.clear();
    for(size_t i = 0; i < model.tracks.size(); i++) {
        if(isFavorite(model.tracks[i].path)) out.push_back(i);
    }
}

void collectLibraryTrackIndexes(std::vector<size_t> &out, LibraryScope scope, int albumFilterIndex) {
    out.clear();

    if(scope == LibraryScope::AllTracks || albumFilterIndex < 0 || albumFilterIndex >= (int)model.albums.size()) {
        for(size_t i = 0; i < model.tracks.size(); i++) out.push_back(i);
        return;
    }

    const String &selectedAlbum = model.albums[(size_t)albumFilterIndex];
    for(size_t i = 0; i < model.tracks.size(); i++) {
        if(model.tracks[i].album == selectedAlbum) out.push_back(i);
    }
}
