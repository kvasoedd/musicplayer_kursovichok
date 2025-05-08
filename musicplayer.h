#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include <QLabel>
#include <QMovie>
#include <QString>
#include <QStringList>
#include <QShortcut>
#include <QStackedWidget>
#include <QSettings>
#include <QCloseEvent>
#include "playlist.h"
#include "musiccontroller.h"
#include "seekslider.h"
#include "radio.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MusicPlayer;
}
QT_END_NAMESPACE

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();

private slots:
    void on_listWidget_modelRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
    void on_buttonPlayPause_clicked();
    void on_buttonNext_clicked();
    void on_buttonPrevious_clicked();
    void on_buttonRandom_clicked();
    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();
    void on_buttonClear_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_positionSlider_sliderMoved(int position);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void on_buttonLoop_clicked();
    void toggleMute();
    void showRadio();
    void showPlayer();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MusicPlayer *ui;
    QVBoxLayout *mainLayout;
    Playlist playlist;
    MusicController musicController;
    Radio *radioPage = nullptr;
    void updatePlaylistUI();
    void updatePlayPauseButton();
    void updateCurrentTrackInfo();
    SeekSlider* positionSlider;
    QString formatTime(qint64 timeMillis);
    QStringList gifImages;
    QLabel *gifLabel;
    QMovie* gifMovie = nullptr;
    int currentGif = 0;
    QString updateGifImage();
    QVector<int> gifQueue;
    int previousVolume = 100;
    bool isMuted = false;
    void saveState();
    void loadState();
};

#endif // MUSICPLAYER_H
