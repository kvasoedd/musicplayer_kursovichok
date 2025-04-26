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

/* старый метод перемешивания
Track* Playlist::getRandomTrack() {
    if (tracks.isEmpty())
        return nullptr;
    int randomIndex = QRandomGenerator::global()->bounded(tracks.size());
    currentIndex = randomIndex;
    return &tracks[currentIndex];
}*/

void Playlist::setCurrentIndex(int index) {
    if (index >= 0 && index < tracks.size())
        currentIndex = index;
}
