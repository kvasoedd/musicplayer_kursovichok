#include "musicplayer.h"
#include "ui_musicplayer.h"
#include <QFileInfo>


MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);
    musicController.setPlaylist(&playlist);

    // Подключаем сигналы от MusicController к слотам
    connect(&musicController, &MusicController::positionChanged, this, &MusicPlayer::updatePosition);
    connect(&musicController, &MusicController::durationChanged, this, &MusicPlayer::updateDuration);
    connect(&musicController, &MusicController::trackChanged, this, &MusicPlayer::updateCurrentTrackInfo);


    // Устанавливаем начальные значения для слайдера
    ui->positionSlider->setRange(0, 0);

    updatePlayPauseButton();
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

void MusicPlayer::on_positionSlider_sliderMoved(int position) {
    // Перемотка трека через MusicController
    musicController.setPosition(position);
}

void MusicPlayer::updatePosition(qint64 position) {
    ui->positionSlider->setValue(static_cast<int>(position));
    ui->labelCurrentTime->setText(formatTime(position));  // Обновление текущего времени
}

void MusicPlayer::updateDuration(qint64 duration) {
    ui->positionSlider->setRange(0, static_cast<int>(duration));
    ui->labelTotalTime->setText(formatTime(duration));  // Обновление общего времени
}

QString MusicPlayer::formatTime(qint64 timeMillis) {
    int seconds = (timeMillis / 1000) % 60;
    int minutes = (timeMillis / 60000) % 60;
    int hours = (timeMillis / 3600000);

    if (hours > 0)
        return QTime(hours, minutes, seconds).toString("hh:mm:ss");
    else
        return QTime(0, minutes, seconds).toString("mm:ss");
}

void MusicPlayer::on_buttonPlayPause_clicked() {
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        musicController.pause();
    } else {
        musicController.play();
        updateCurrentTrackInfo();  // Обновляем отображение текущего трека
    }
    updatePlayPauseButton();  // Обновление текста кнопки
}

void MusicPlayer::updatePlayPauseButton() {
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        ui->buttonPlayPause->setText("Pause");  // Можно заменить на значок "⏸"
    } else {
        ui->buttonPlayPause->setText("Play");   // Можно заменить на значок "▶️"
    }
}

void MusicPlayer::updateCurrentTrackInfo() {
    Track* currentTrack = playlist.getCurrentTrack();
    if (currentTrack) {
        QString trackInfo = QString("%1").arg(currentTrack->title);
        ui->currentTrackLabel->setText(trackInfo);  // Установим текст метки
    } else {
        ui->currentTrackLabel->setText("No tracks to play =(");
    }
}

