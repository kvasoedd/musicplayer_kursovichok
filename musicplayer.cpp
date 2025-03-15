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

void MusicPlayer::on_volumeSlider_valueChanged(int value)
{
    // Преобразуем значение из диапазона [0, 100] в [0.0, 1.0]
    double volume = value / 100.0;
    musicController.setVolume(volume);
}
