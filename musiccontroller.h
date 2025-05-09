#ifndef MUSICCONTROLLER_H
#define MUSICCONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSettings>
#include "playlist.h"

class MusicController : public QObject {
    Q_OBJECT

public:
    explicit MusicController(QObject* parent = nullptr);
    void setPlaylist(Playlist* playlist);
    void setTrack();
    void play();
    void next();
    void previous();
    void pause();
    void stop();
    void setVolume(double volume);
    void setPosition(qint64 pos);
    QMediaPlayer::PlaybackState getPlaybackState() const;
    QMediaPlayer* getPlayer() const;
    void setRandomEnabled(bool enabled);
    void toggleRandom();
    bool isRandomEnabled() const;
    void setLoopEnabled(bool enabled);
    void toggleLoop();
    bool isLoopEnabled() const;
    void setPlaylist();
    void restartCurrentTrack();
    void setVolumeFromSlider(int sliderValue);

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void trackChanged();

private:
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    Playlist* playlist = nullptr;
    bool randomEnabled = false;
    bool loopEnabled = false;
};

#endif // MUSICCONTROLLER_H
