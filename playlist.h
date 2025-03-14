#ifndef PLAYLIST_H
#define PLAYLIST_H

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

    // Новый геттер для доступа к трекам
    const QVector<Track>& getTracks() const;

private:
    QVector<Track> tracks;
    int currentIndex;
};

#endif // PLAYLIST_H
