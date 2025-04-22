#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QRandomGenerator>
#include <QString>
#include <QVector>

struct Track {
    QString filePath;
    QString title;
    QString artist;
    QString duration;
};

class Playlist {
public:
    Playlist() : currentIndex(0) {}

    void addTrack(const Track& track);
    void removeTrack(int index);
    void clear();

    Track* getCurrentTrack();
    Track* getNextTrack();
    Track* getPreviousTrack();

    //Track* getRandomTrack();
    void enableShuffle();
    void disableShuffle();
    void setCurrentIndex(int index);

    // Новый геттер для доступа к трекам
    const QVector<Track>& getTracks() const;

private:
    QVector<Track> tracks;
    QVector<Track> originalTracks; // для хранения исходного порядка
    int currentIndex = 0;
    bool isShuffled = false;       // флаг, включен ли режим перемешивания
};

#endif // PLAYLIST_H
