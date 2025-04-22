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

// Метод для включения режима перемешивания
void Playlist::enableShuffle() {
    if (!isShuffled) {
        // Сохраняем исходный порядок, если ещё не сохранён
        originalTracks = tracks;
        // Сохраняем текущий трек, чтобы потом обновить индекс в новом порядке
        Track currentTrack = getCurrentTrack() ? *getCurrentTrack() : Track();
        // Перемешиваем список треков
        std::shuffle(tracks.begin(), tracks.end(), *QRandomGenerator::global());
        // Обновляем currentIndex так, чтобы он указывал на тот же трек в перемешанном списке
        for (int i = 0; i < tracks.size(); i++) {
            if (tracks[i].filePath == currentTrack.filePath) {
                currentIndex = i;
                break;
            }
        }
        isShuffled = true;
    }
}

// Метод для выключения режима перемешивания (восстанавливаем исходный порядок)
void Playlist::disableShuffle() {
    if (isShuffled) {
        // Сохраняем текущий трек из перемешанного списка
        Track currentTrack = getCurrentTrack() ? *getCurrentTrack() : Track();
        // Восстанавливаем исходный порядок
        tracks = originalTracks;
        // Обновляем currentIndex так, чтобы он указывал на текущий трек в исходном порядке
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

/* отключено для теста нового перемешивания
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
