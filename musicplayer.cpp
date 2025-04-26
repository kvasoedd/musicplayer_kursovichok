#include "musicplayer.h"
#include "ui_musicplayer.h"
#include <QFileInfo>
#include <ctime>

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);

    mainLayout = new QVBoxLayout();
    radioPage = new Radio(this);
    mainLayout->addWidget(radioPage);
    ui->pageArea->setLayout(mainLayout);
    radioPage->setVisible(false);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    musicController.setPlaylist(&playlist);

    positionSlider = new SeekSlider(this);
    positionSlider->setOrientation(Qt::Horizontal);
    positionSlider->setRange(0, 0);
    ui->progressLayout->addWidget(positionSlider);

    connect(ui->playerButton, &QPushButton::clicked, this, &MusicPlayer::showPlayer);
    connect(ui->radioButton, &QPushButton::clicked, this, &MusicPlayer::showRadio);
    connect(positionSlider, &QSlider::sliderMoved, &musicController, &MusicController::setPosition);
    connect(&musicController, &MusicController::positionChanged, positionSlider, &QSlider::setValue);
    connect(&musicController, &MusicController::durationChanged, positionSlider, &QSlider::setMaximum);
    connect(&musicController, &MusicController::positionChanged, this, &MusicPlayer::updatePosition);
    connect(&musicController, &MusicController::durationChanged, this, &MusicPlayer::updateDuration);
    connect(&musicController, &MusicController::trackChanged, this, &MusicPlayer::updateCurrentTrackInfo);
    connect(musicController.getPlayer(), &QMediaPlayer::playbackStateChanged, this, &MusicPlayer::updatePlayPauseButton);
    updatePlayPauseButton();
    ui->volumeSlider->setValue(50);

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

    // Горячие клавиши:
    QShortcut* playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonPlayPause_clicked);

    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(nextShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonNext_clicked);

    QShortcut* prevShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(prevShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonPrevious_clicked);

    QShortcut* loopShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(loopShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonLoop_clicked);

    QShortcut* randomShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(randomShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonRandom_clicked);

    QShortcut* volUpShortcut = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(volUpShortcut, &QShortcut::activated, [=]() {
        int value = ui->volumeSlider->value();
        ui->volumeSlider->setValue(qMin(value + 10, 100));
    });

    QShortcut* volDownShortcut = new QShortcut(QKeySequence(Qt::Key_Down), this);
    connect(volDownShortcut, &QShortcut::activated, [=]() {
        int value = ui->volumeSlider->value();
        ui->volumeSlider->setValue(qMax(value - 10, 0));
    });

    QShortcut* muteShortcut = new QShortcut(QKeySequence(Qt::Key_M), this);
    connect(muteShortcut, &QShortcut::activated, this, &MusicPlayer::toggleMute);

    QShortcut* addfolderShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    connect(addfolderShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonAdd_clicked);

    QShortcut* clearShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(clearShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonClear_clicked);
}

MusicPlayer::~MusicPlayer() {
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
    } else {
        musicController.play();
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

void MusicPlayer::updatePlayPauseButton() {
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        ui->buttonPlayPause->setText("⏸");
    } else {
        ui->buttonPlayPause->setText("▶️");
    }
}

void MusicPlayer::on_buttonNext_clicked() {
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

void MusicPlayer::on_buttonPrevious_clicked() {
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

void MusicPlayer::on_buttonRandom_clicked() {
    musicController.toggleRandom();
    if (musicController.isRandomEnabled()) {
        ui->buttonRandom->setText("🔀: ON");
    } else {
        ui->buttonRandom->setText("🔀: OFF");
    }
}

void MusicPlayer::on_buttonLoop_clicked() {
    musicController.toggleLoop();
    if (musicController.isLoopEnabled()) {
        ui->buttonLoop->setText("🔄: ON");
    } else {
        ui->buttonLoop->setText("🔄: OFF");
    }
}


void MusicPlayer::on_buttonAdd_clicked() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select a folder with audio files");
    if (folderPath.isEmpty()) return;
    QDir dir(folderPath);
    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.flac" << "*.aac";
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
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

void MusicPlayer::on_buttonRemove_clicked() {
    int selectedIndex = ui->listWidget->currentRow();
    if (selectedIndex >= 0) {
        playlist.removeTrack(selectedIndex);
        updatePlaylistUI();
        if (playlist.tracks.size() == 0) {
            on_buttonClear_clicked();
            updatePlaylistUI();
        }
    }
}

void MusicPlayer::on_buttonClear_clicked() {
    playlist.clear();
    ui->listWidget->clear();
    musicController.stop();
    positionSlider->setValue(0);
    positionSlider->setRange(0, 0);
    ui->labelCurrentTime->setText("00:00");
    ui->labelTotalTime->setText("00:00");
    ui->currentTrackLabel->setText("No tracks to play =(");
    updatePlayPauseButton();
    updateCurrentTrackInfo();
    emit musicController.trackChanged();
}

void MusicPlayer::on_volumeSlider_valueChanged(int value) {
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
    ui->labelCurrentTime->setText(formatTime(position));
}

void MusicPlayer::updateDuration(qint64 duration) {
    ui->labelTotalTime->setText(formatTime(duration));
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
        QString gifPath = updateGifImage();
        if (gifMovie) {
            gifMovie->stop();
            delete gifMovie;
        }
        gifMovie = new QMovie(gifPath);
        ui->gifLabel->setMovie(gifMovie);
        ui->gifLabel->setVisible(true);
        gifMovie->start();
    } else {
        ui->currentTrackLabel->setText("No tracks to play =(");
        ui->gifLabel->setVisible(false);
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

void MusicPlayer::showRadio() {
    ui->listWidget->setVisible(false);
    ui->progressFrame->setVisible(false);
    ui->buttonPlayPause->setVisible(false);
    ui->buttonNext->setVisible(false);
    ui->buttonPrevious->setVisible(false);
    ui->buttonRandom->setVisible(false);
    ui->buttonLoop->setVisible(false);
    ui->buttonAdd->setVisible(false);
    ui->buttonRemove->setVisible(false);
    ui->buttonClear->setVisible(false);
    ui->volumeSlider->setVisible(false);
    ui->labelCurrentTime->setVisible(false);
    ui->labelTotalTime->setVisible(false);
    ui->currentTrackLabel->setVisible(false);
    ui->gifLabel->setVisible(false);
    radioPage->setVisible(true);
    ui->pageArea->raise();
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        musicController.pause();
        gifMovie->stop();
        updatePlayPauseButton();
    }
}

void MusicPlayer::showPlayer() {
    ui->listWidget->setVisible(true);
    ui->progressFrame->setVisible(true);
    ui->buttonPlayPause->setVisible(true);
    ui->buttonNext->setVisible(true);
    ui->buttonPrevious->setVisible(true);
    ui->buttonRandom->setVisible(true);
    ui->buttonLoop->setVisible(true);
    ui->buttonAdd->setVisible(true);
    ui->buttonRemove->setVisible(true);
    ui->buttonClear->setVisible(true);
    ui->volumeSlider->setVisible(true);
    ui->labelCurrentTime->setVisible(true);
    ui->labelTotalTime->setVisible(true);
    ui->currentTrackLabel->setVisible(true);
    ui->gifLabel->setVisible(true);
    radioPage->StopStream();
    radioPage->setVisible(false);
    ui->pageArea->lower();
}
