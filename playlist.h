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
    void clear(); //пока не используется. реализовать как очистку очереди

    Track* getCurrentTrack();
    Track* getNextTrack();
    Track* getPreviousTrack();

    Track* getRandomTrack();
    void setCurrentIndex(int index);

    // Новый геттер для доступа к трекам
    const QVector<Track>& getTracks() const;

private:
    QVector<Track> tracks;
    int currentIndex;
};

#endif // PLAYLIST_H
