#include "musicplayer.h"
#include "ui_musicplayer.h"
#include <QFileInfo>
#include <ctime>

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    musicController.setPlaylist(&playlist);

    positionSlider = new SeekSlider(this);
    positionSlider->setOrientation(Qt::Horizontal);
    positionSlider->setRange(0, 0);
    ui->progressLayout->addWidget(positionSlider);

    musicController.setPlaylist(&playlist);

    // Подключаем сигнал перемотки
    connect(positionSlider, &QSlider::sliderMoved, &musicController, &MusicController::setPosition);
    connect(&musicController, &MusicController::positionChanged, positionSlider, &QSlider::setValue);
    connect(&musicController, &MusicController::durationChanged, positionSlider, &QSlider::setMaximum);

    // Подключаем сигналы от MusicController к слотам
    connect(&musicController, &MusicController::positionChanged, this, &MusicPlayer::updatePosition);
    connect(&musicController, &MusicController::durationChanged, this, &MusicPlayer::updateDuration);
    connect(&musicController, &MusicController::trackChanged, this, &MusicPlayer::updateCurrentTrackInfo);
    connect(musicController.getPlayer(), &QMediaPlayer::playbackStateChanged, this, &MusicPlayer::updatePlayPauseButton);


    updatePlayPauseButton();

    gifLabel = new QLabel(this);
    gifLabel->setVisible(false);
    gifImages << ":/imgs/gifs/christian-bale-american-psycho.gif"
              << ":/imgs/gifs/lofi-girl-lofi.gif"
              << ":/imgs/gifs/cat-headphones.gif"
              << ":/imgs/gifs/gyllenhaal.gif"
              << ":/imgs/gifs/dadada.gif"
              << ":/imgs/gifs/monke.gif"
              << ":/imgs/gifs/girl.gif"
              << ":/imgs/gifs/cant-sleep-to-the-beat.gif"
              << ":/imgs/gifs/bart.gif"
              << ":/imgs/gifs/djcat.gif"
              << ":/imgs/gifs/jakedog.gif"
              << ":/imgs/gifs/john-cena.gif"
              << ":/imgs/gifs/kid.gif"
              << ":/imgs/gifs/bateman.gif"
              << ":/imgs/gifs/spongebob.gif"
              << ":/imgs/gifs/starlord.gif";
    currentGif = std::rand() % gifImages.size();
    gifMovie = new QMovie(gifImages[currentGif]);
    ui->gifLabel->setMovie(gifMovie);

    // Воспроизведение / пауза по Space
    QShortcut* playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonPlayPause_clicked);

    // Следующий трек по Right
    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(nextShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonNext_clicked);

    // Предыдущий трек по Left
    QShortcut* prevShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(prevShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonPrevious_clicked);

    // Зацикливание трека по CTRL + R
    QShortcut* loopShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(loopShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonLoop_clicked);

    // Случайный порядок треков по CTRL + S
    QShortcut* randomShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(randomShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonRandom_clicked);

    // Громкость вверх по Up
    QShortcut* volUpShortcut = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(volUpShortcut, &QShortcut::activated, [=]() {
        int value = ui->volumeSlider->value();
        ui->volumeSlider->setValue(qMin(value + 10, 100));
    });

    // Громкость вниз по Down
    QShortcut* volDownShortcut = new QShortcut(QKeySequence(Qt::Key_Down), this);
    connect(volDownShortcut, &QShortcut::activated, [=]() {
        int value = ui->volumeSlider->value();
        ui->volumeSlider->setValue(qMax(value - 10, 0));
    });

    // Выклячить звук по M
    QShortcut* muteShortcut = new QShortcut(QKeySequence(Qt::Key_M), this);
    connect(muteShortcut, &QShortcut::activated, this, &MusicPlayer::toggleMute);

    // Добавить папку с треками Ctrl + A
    QShortcut* addfolderShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    connect(addfolderShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonAdd_clicked);

    // Очистить очередь Ctrl + C
    QShortcut* clearShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(clearShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonClear_clicked);

}

MusicPlayer::~MusicPlayer()
{
    delete ui;
}

void MusicPlayer::updatePlaylistUI() {
    ui->listWidget->clear();
    for (const Track& track : playlist.getTracks()) {
        ui->listWidget->addItem(track.title);
    }
}

void MusicPlayer::on_buttonPlayPause_clicked() {
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        musicController.pause();
        gifMovie->stop();
        // gifLabel->setVisible(false);
    } else {
        musicController.play();
        // updateCurrentTrackInfo();
        if (!playlist.getTracks().isEmpty()) {
            QString gifPath = gifImages[currentGif];
            if (!gifPath.isEmpty()) {
                gifLabel->setVisible(true);
                gifMovie->start();
            }
        } else {
            gifLabel->setVisible(false);
        }
    }
    updatePlayPauseButton();
}


// Остановить/воспроизвести
void MusicPlayer::updatePlayPauseButton() {
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        ui->buttonPlayPause->setText("⏸");
    } else {
        ui->buttonPlayPause->setText("▶️");
    }
}

// Включение следующего трека
void MusicPlayer::on_buttonNext_clicked()
{
    musicController.next();
    if (!playlist.getTracks().isEmpty()) {
        QString gifPath = updateGifImage();

        if (gifMovie) {
            gifMovie->stop();
            delete gifMovie;
        }

        gifMovie = new QMovie(gifPath);
        ui->gifLabel->setMovie(gifMovie);
        ui->gifLabel->setVisible(true);
        gifMovie->start();
    }
}

// Включение предыдущего трека
void MusicPlayer::on_buttonPrevious_clicked()
{
    musicController.previous();
    if (!playlist.getTracks().isEmpty()) {
        QString gifPath = updateGifImage();

        if (gifMovie) {
            gifMovie->stop();
            delete gifMovie;
        }

        gifMovie = new QMovie(gifPath);
        ui->gifLabel->setMovie(gifMovie);
        ui->gifLabel->setVisible(true);
        gifMovie->start();
    }
}

// Включения/выключения воспроизведения треков в случайном порядке
void MusicPlayer::on_buttonRandom_clicked()
{
    musicController.toggleRandom();
    if (musicController.isRandomEnabled()) {
        ui->buttonRandom->setText("Random: ON");
    } else {
        ui->buttonRandom->setText("Random: OFF");
    }
}

// Включения/выключения зацикливания
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
        playlist.addTrack(newTrack);
    }
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
    int selectedIndex = ui->listWidget->currentRow();
    if (selectedIndex >= 0) {
        playlist.removeTrack(selectedIndex);
        updatePlaylistUI();
    }
}

void MusicPlayer::on_buttonClear_clicked()
{
    playlist.clear();
    ui->listWidget->clear();
    musicController.stop();
    positionSlider->setValue(0);
    positionSlider->setRange(0, 0);
    ui->labelCurrentTime->setText("00:00");
    ui->labelTotalTime->setText("00:00");
    gifLabel->setVisible(false);
    ui->currentTrackLabel->setText("No tracks to play =(");
    updatePlayPauseButton();
    emit musicController.trackChanged();
}

void MusicPlayer::on_volumeSlider_valueChanged(int value)
{
    // Преобразуем значение из диапазона [0, 100] в [0.0, 1.0]
    double volume = value / 100.0;
    musicController.setVolume(volume);
}

void MusicPlayer::toggleMute() {
    if (!isMuted) {
        previousVolume = ui->volumeSlider->value();
        ui->volumeSlider->setValue(0);
        isMuted = true;
    } else {
        ui->volumeSlider->setValue(previousVolume);
        isMuted = false;
    }
}

void MusicPlayer::on_positionSlider_sliderMoved(int position) {
    musicController.setPosition(position);
}


void MusicPlayer::updatePosition(qint64 position) {
    ui->labelCurrentTime->setText(formatTime(position));  // Обновление текущего времени
}

void MusicPlayer::updateDuration(qint64 duration) {
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
        ui->currentTrackLabel->setText(trackInfo);
        if(QMediaPlayer::EndOfMedia){
            QString gifPath = updateGifImage();
            if (gifMovie) {
                gifMovie->stop();
                delete gifMovie;
            }
            gifMovie = new QMovie(gifPath);
            ui->gifLabel->setMovie(gifMovie);
            ui->gifLabel->setVisible(true);
            gifMovie->start();
        }
    } else {
        ui->currentTrackLabel->setText("No tracks to play =(");
        gifLabel->setVisible(false);
    }
}

QString MusicPlayer::updateGifImage() {
    if (gifImages.isEmpty()) {
        qWarning() << "gifImages list is empty!";
        return "";
    }
    if (playlist.getTracks().isEmpty()) {
        return "";
    }
    QString gifPath = gifImages[currentGif];
    currentGif = std::rand() % gifImages.size();
    return gifPath;
}
