#include "musiccontroller.h"
#include <QUrl>

MusicController::MusicController(QObject* parent) : QObject(parent) {
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    // Подключаем сигнал изменения статуса медиаплеера
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        // Если трек завершился, автоматически переходим к следующему
        if (status == QMediaPlayer::EndOfMedia) {
            next();
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
    }
}

void MusicController::pause() {
    if (player->playbackState() == QMediaPlayer::PlayingState)
        player->pause();
}

void MusicController::next() {
    if (!playlist) return;
    Track* track = playlist->getNextTrack();
    if (track) {
        player->setSource(QUrl::fromLocalFile(track->filePath));
        player->play();
    }
}

void MusicController::previous() {
    if (!playlist) return;
    Track* track = playlist->getPreviousTrack();
    if (track) {
        player->setSource(QUrl::fromLocalFile(track->filePath));
        player->play();
    }
}

void MusicController::setVolume(double volume) {
    audioOutput->setVolume(volume);
}
