#include "playlist.h"

void Playlist::addTrack(const Track& track) {
    tracks.append(track);
}

void Playlist::removeTrack(int index) {
    if (index >= 0 && index < tracks.size()) {
        tracks.remove(index);
        // Если удалённый трек находится перед текущим, уменьшаем currentIndex
        if (currentIndex > index) {
            currentIndex--;
        }
        // Если удалён текущий трек, можно установить currentIndex на 0 или оставить тот же индекс, если он валиден
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

// Реализация геттера
const QVector<Track>& Playlist::getTracks() const {
    return tracks;
}
