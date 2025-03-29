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

    // Методы для режима случайного воспроизведения
    void setRandomEnabled(bool enabled);
    void toggleRandom();
    bool isRandomEnabled() const;

    // Методы для зацикливания
    void setLoopEnabled(bool enabled);
    void toggleLoop();
    bool isLoopEnabled() const;
    void setPlaylist();
    void restartCurrentTrack();

signals:
    // Сигналы для проброса информации о позиции и длительности трека
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void trackChanged();

private:
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    Playlist* playlist = nullptr;
    bool randomEnabled = false;
    bool loopEnabled = false;  // Флаг зацикливания
};

#endif // MUSICCONTROLLER_H
