#include "musiccontroller.h"
#include <QUrl>

MusicController::MusicController(QObject* parent) : QObject(parent) {
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    // Подключаем сигнал изменения статуса медиаплеера
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            if (isLoopEnabled()) {
                restartCurrentTrack();
            } else {
                next();
            }
        }
    });

    // Пробрасываем сигналы обновления позиции и длительности
    connect(player, &QMediaPlayer::positionChanged, this, &MusicController::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MusicController::durationChanged);
}
void MusicController::setPlaylist(Playlist* playlist) {
    this->playlist = playlist;
}


void MusicController::play() {
    // Если плеер уже в состоянии паузы, просто продолжить воспроизведение
    if (player->playbackState() == QMediaPlayer::PausedState) {
        player->play();
        return;
    }

    if (!playlist || playlist->getTracks().isEmpty()) return;
    Track* track = playlist->getCurrentTrack();
    if (track) {
        // Загрузка файла в плеер
        player->setSource(QUrl::fromLocalFile(track->filePath));
        // Запуск воспроизведения
        player->play();
        emit trackChanged(); //Новый сигнал
    }
}

void MusicController::pause() {
    if (player->playbackState() == QMediaPlayer::PlayingState)
        player->pause();
}

QMediaPlayer::PlaybackState MusicController::getPlaybackState() const {
    return player->playbackState();
}

void MusicController::next() {
    if (!playlist) return;

    Track* track = nullptr;
    if (randomEnabled) {
        track = playlist->getRandomTrack();
    } else {
        track = playlist->getNextTrack();
    }

    if (track) {
        player->setSource(QUrl::fromLocalFile(track->filePath));
        player->play();
        emit trackChanged();
    }
}

void MusicController::previous() {
    if (!playlist) return;

    Track* track = playlist->getPreviousTrack();
    if (track) {
        player->setSource(QUrl::fromLocalFile(track->filePath));
        player->play();
        emit trackChanged();
    }
}

void MusicController::setLoopEnabled(bool enabled) {
    loopEnabled = enabled;
}

void MusicController::toggleLoop() {
    loopEnabled = !loopEnabled;
}

bool MusicController::isLoopEnabled() const {
    return loopEnabled;
}

void MusicController::restartCurrentTrack() {
    if (!playlist) return;
    Track* track = playlist->getCurrentTrack();
    if (track) {
        player->setPosition(0);
        player->play();
        emit trackChanged();
    }
}

void MusicController::setRandomEnabled(bool enabled) {
    randomEnabled = enabled;
}

void MusicController::toggleRandom() {
    randomEnabled = !randomEnabled;
}

bool MusicController::isRandomEnabled() const {
    return randomEnabled;
}

void MusicController::setVolume(double volume) {
    audioOutput->setVolume(volume);
}

void MusicController::setPosition(qint64 position) {
    player->setPosition(position);
}
