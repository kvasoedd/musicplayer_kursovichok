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
        ui->buttonPlayPause->setText("⏸");  // Можно заменить на значок "⏸"
    } else {
        ui->buttonPlayPause->setText("▶️");   // Можно заменить на значок "▶️"
    }
}

void MusicPlayer::on_buttonNext_clicked()
{
    musicController.next();
}

void MusicPlayer::on_buttonPrevious_clicked()
{
    musicController.previous();
}

void MusicPlayer::on_buttonRandom_clicked()
{
    musicController.toggleRandom();

    if (musicController.isRandomEnabled()) {
        ui->buttonRandom->setText("Random: ON");
    } else {
        ui->buttonRandom->setText("Random: OFF");
    }
}

// Слот для включения/выключения зацикливания
void MusicPlayer::on_buttonLoop_clicked() {
    musicController.toggleLoop();

    if (musicController.isLoopEnabled()) {
        ui->buttonLoop->setText("Loop: ON");
    } else {
        ui->buttonLoop->setText("Loop: OFF");
    }
}


void MusicPlayer::on_buttonAdd_clicked()
{
    //новая реализация с добавлением папки и сортивкой по типу файла
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select a folder with audio files");
    if (folderPath.isEmpty())
        return;

    QDir dir(folderPath);
    // Устанавливаем фильтры для нужных аудиоформатов
    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.flac" << "*.aac";
    dir.setNameFilters(filters);

    // Получаем список файлов в выбранной папке
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);

    // Добавляем каждый найденный файл в плейлист
    for (const QFileInfo &fileInfo : fileList) {
        Track newTrack;
        newTrack.filePath = fileInfo.absoluteFilePath();
        newTrack.title = fileInfo.baseName();
        // При необходимости можно добавить извлечение метаданных

        playlist.addTrack(newTrack);
    }

    // Обновляем пользовательский интерфейс (например, QListWidget)
    updatePlaylistUI();

    //старая реализация с добавлением треков поштучно
    /*
    QString filePath = QFileDialog::getOpenFileName(this, "Select an audio file", "", "Audio Files (*.mp3 *.wav *.flac *.aac);; All Files (*.*)");
    if (!filePath.isEmpty()) {
        Track newTrack;
        newTrack.filePath = filePath;
        newTrack.title = QFileInfo(filePath).baseName();
        playlist.addTrack(newTrack);
        updatePlaylistUI();
    } */
}

void MusicPlayer::on_buttonRemove_clicked()
{
    // Получаем индекс выбранного элемента в QListWidget
    int selectedIndex = ui->listWidget->currentRow();
    if (selectedIndex >= 0) {
        // Удаляем трек из плейлиста
        playlist.removeTrack(selectedIndex);
        // Обновляем UI, чтобы отобразить актуальный список треков
        updatePlaylistUI();
    }
}

void MusicPlayer::on_buttonClear_clicked()
{
    playlist.clear();
    ui->listWidget->clear();
    emit musicController.trackChanged();
}


void MusicPlayer::on_volumeSlider_valueChanged(int value)
{
    // Преобразуем значение из диапазона [0, 100] в [0.0, 1.0]
    double volume = value / 100.0;
    musicController.setVolume(volume);
}

void MusicPlayer::on_positionSlider_sliderMoved(int position) {
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


void MusicPlayer::updateCurrentTrackInfo() {
    Track* currentTrack = playlist.getCurrentTrack();
    if (currentTrack) {
        QString trackInfo = QString("%1").arg(currentTrack->title);
        ui->currentTrackLabel->setText(trackInfo);  // Установим текст метки
    } else {
        ui->currentTrackLabel->setText("No tracks to play =(");
    }
}

