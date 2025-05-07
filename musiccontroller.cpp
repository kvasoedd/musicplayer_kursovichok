#include "musiccontroller.h"
#include <QUrl>
MusicController::MusicController(QObject* parent) : QObject(parent) {
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            if (isLoopEnabled()) {
                restartCurrentTrack();
            } else {
                next();
            }
        }
    });
    connect(player, &QMediaPlayer::positionChanged, this, &MusicController::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MusicController::durationChanged);
}
void MusicController::setPlaylist(Playlist* playlist) {
    this->playlist = playlist;
}
void MusicController::stop() {
    player->stop();
    player->setSource(QUrl());
}
void MusicController::play() {
    if (!playlist || playlist->getTracks().isEmpty()) return;
    if (player->playbackState() == QMediaPlayer::PausedState) {
        player->play();
        return;
    }
    if (!playlist || playlist->getTracks().isEmpty()) return;
    Track* track = playlist->getCurrentTrack();
    if (track) {
        player->setSource(QUrl::fromLocalFile(track->filePath));
        player->play();
        emit trackChanged();
    }
}
void MusicController::pause() {
    if (player->playbackState() == QMediaPlayer::PlayingState)
        player->pause();
}
QMediaPlayer* MusicController::getPlayer() const {
    return player;
}
QMediaPlayer::PlaybackState MusicController::getPlaybackState() const {
    return player->playbackState();
}
void MusicController::next() {
    if (!playlist) return;
    Track* track = playlist->getNextTrack();
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
    if (playlist) {
        if (randomEnabled) {
            playlist->enableShuffle();
        } else {
            playlist->disableShuffle();
        }
    }
}
bool MusicController::isRandomEnabled() const {
    return randomEnabled;
}
void MusicController::setVolume(double volume) {
    audioOutput->setVolume(volume);
    QSettings settings("IST", "MusicPlayer");
    settings.setValue("volume", volume);
}
void MusicController::setVolumeFromSlider(int sliderValue) {
    double volume = sliderValue / 100.0;
    setVolume(volume);
}
void MusicController::setPosition(qint64 position) {
    player->setPosition(position);
}
