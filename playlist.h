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

    void enableShuffle();
    void disableShuffle();
    void resetShuffleState();
    void setCurrentIndex(int index);

    QVector<Track> tracks;

    void insertTrack(int index, const Track& track);
    int getCurrentIndex() const;

    QVector<Track>& getTracksMutable();
    const QVector<Track>& getTracks() const;
    const QVector<Track>& getOriginalTracks() const;

private:
    QVector<Track> originalTracks;
    int currentIndex = 0;
    bool isShuffled = false;
};

#endif // PLAYLIST_H
