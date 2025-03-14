#include "musicplayer.h"
#include "ui_musicplayer.h"
#include <QFileInfo>

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);
    musicController.setPlaylist(&playlist);
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
}

void MusicPlayer::updatePlaylistUI() {
    ui->listWidget->clear();
    // Используем getTracks() для получения списка треков
    for (const Track& track : playlist.getTracks()) {
        ui->listWidget->addItem(track.title);
    }
}

void MusicPlayer::on_buttonPlay_clicked()
{
musicController.play();
}


void MusicPlayer::on_buttonPause_clicked()
{
    musicController.pause();
}


void MusicPlayer::on_buttonNext_clicked()
{
    musicController.next();
}


void MusicPlayer::on_buttonPrevious_clicked()
{
    musicController.previous();
}


void MusicPlayer::on_buttonAdd_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите аудиофайл", "", "Audio Files (*.mp3 *.wav)");
    if (!filePath.isEmpty()) {
        Track newTrack;
        newTrack.filePath = filePath;
        newTrack.title = QFileInfo(filePath).baseName();
        playlist.addTrack(newTrack);
        updatePlaylistUI();
    }
}


void MusicPlayer::on_volumeSlider_valueChanged(int value)
{
    // Преобразуем значение из диапазона [0, 100] в [0.0, 1.0]
    double volume = value / 100.0;
    musicController.setVolume(volume);
}

