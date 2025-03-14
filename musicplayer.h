#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
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

private:
    Ui::MusicPlayer *ui;
    Playlist playlist;
    MusicController musicController;

    void updatePlaylistUI();
};
#endif // MUSICPLAYER_H
