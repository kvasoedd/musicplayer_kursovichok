#include "playlist.h"

void Playlist::addTrack(const Track& track) {
    tracks.append(track);
}

void Playlist::removeTrack(int index) {
    if (index >= 0 && index < tracks.size()) {
        tracks.remove(index);
        if (currentIndex > index) {
            currentIndex--;
        }
        else if (currentIndex == index) {
            if (tracks.isEmpty())
                currentIndex = 0;
            else if (currentIndex >= tracks.size())
                currentIndex = tracks.size() - 1;
        }
    }
}

void Playlist::clear() {
    tracks.clear();
    currentIndex = 0;
}

Track* Playlist::getCurrentTrack() {
    if (tracks.isEmpty()) return nullptr;
    return &tracks[currentIndex];
}

int Playlist::getCurrentIndex() const {
    return currentIndex;
}

void Playlist::insertTrack(int index, const Track& track) {
    // Проверка границ
    if (index < 0 || index > tracks.size())
        return;
    tracks.insert(index, track);
    // Если вставили перед или на текущем треке, смещаем currentIndex вправо
    if (index <= currentIndex) {
        ++currentIndex;
    }
}

Track* Playlist::getNextTrack() {
    if (tracks.isEmpty()) return nullptr;
    currentIndex = (currentIndex + 1) % tracks.size();
    return &tracks[currentIndex];
}

Track* Playlist::getPreviousTrack() {
    if (tracks.isEmpty()) return nullptr;
    currentIndex = (currentIndex - 1 + tracks.size()) % tracks.size();
    return &tracks[currentIndex];
}

const QVector<Track>& Playlist::getTracks() const {
    return tracks;
}

const QVector<Track>& Playlist::getOriginalTracks() const {
    return originalTracks;
}

// Метод для включения режима перемешивания
void Playlist::enableShuffle() {
    if (!isShuffled) {
        originalTracks = tracks;
        Track currentTrack = getCurrentTrack() ? *getCurrentTrack() : Track();
        std::shuffle(tracks.begin(), tracks.end(), *QRandomGenerator::global());
        for (int i = 0; i < tracks.size(); i++) {
            if (tracks[i].filePath == currentTrack.filePath) {
                currentIndex = i;
                break;
            }
        }
        isShuffled = true;
    }
}

void Playlist::disableShuffle() {
    if (isShuffled) {
        Track currentTrack = getCurrentTrack() ? *getCurrentTrack() : Track();
        tracks = originalTracks;
        for (int i = 0; i < tracks.size(); i++) {
            if (tracks[i].filePath == currentTrack.filePath) {
                currentIndex = i;
                break;
            }
        }
        originalTracks.clear();
        isShuffled = false;
    }
}


void Playlist::resetShuffleState() {
    // После загрузки из папки считаем, что мы в «чистом» порядке
    originalTracks.clear();
    isShuffled = false;
}


void Playlist::setCurrentIndex(int index) {
    if (index >= 0 && index < tracks.size())
        currentIndex = index;
}
