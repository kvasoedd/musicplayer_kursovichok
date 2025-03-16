#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include "playlist.h"
#include "musiccontroller.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MusicPlayer;
}
QT_END_NAMESPACE

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();

private slots:
    void on_buttonPlay_clicked();

    void on_buttonPause_clicked();

    void on_buttonNext_clicked();

    void on_buttonPrevious_clicked();

    void on_buttonAdd_clicked();

    void on_volumeSlider_valueChanged(int value);

    void on_positionSlider_sliderMoved(int position);  // Новый слот для перемотки

    void updatePosition(qint64 position);              // Слот для обновления положения слайдера

    void updateDuration(qint64 duration);              // Слот для установки максимального значения слайдера

    void on_buttonPlayPause_clicked();

    void on_buttonRemove_clicked();


private:
    Ui::MusicPlayer *ui;
    Playlist playlist;
    MusicController musicController;

    void updatePlaylistUI();
    void updatePlayPauseButton();
    void updateCurrentTrackInfo();

    // Новый метод для форматирования времени
    QString formatTime(qint64 timeMillis);
};
#endif // MUSICPLAYER_H
