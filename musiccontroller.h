#ifndef MUSICCONTROLLER_H
#define MUSICCONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "playlist.h"

class MusicController : public QObject {
    Q_OBJECT

public:
    explicit MusicController(QObject* parent = nullptr);
    void setPlaylist(Playlist* playlist);
    void play();
    void next();
    void previous();
    void pause();
    void setVolume(double volume);
    void setPosition(qint64 pos);  // Для перемотки по треку
    QMediaPlayer::PlaybackState getPlaybackState() const;


signals:
    // Сигналы для проброса информации о позиции и длительности трека
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void trackChanged();

private:
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    Playlist* playlist = nullptr;
};

#endif // MUSICCONTROLLER_H
