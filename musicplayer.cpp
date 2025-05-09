#include "musicplayer.h"
#include "ui_musicplayer.h"
#include <QFileInfo>
#include <ctime>
#include <QtGlobal>

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

    ui->listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->listWidget->setDefaultDropAction(Qt::MoveAction);
    connect(ui->listWidget->model(), &QAbstractItemModel::rowsMoved, this, &MusicPlayer::on_listWidget_modelRowsMoved);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    musicController.setPlaylist(&playlist);

    positionSlider = new SeekSlider(this);
    positionSlider->setOrientation(Qt::Horizontal);
    positionSlider->setRange(0, 0);
    ui->progressLayout->addWidget(positionSlider);

    connect(ui->playerButton, &QPushButton::clicked, this, &MusicPlayer::showPlayer);
    connect(ui->radioButton, &QPushButton::clicked, this, &MusicPlayer::showRadio);

    loadState();
    if (!musicController.isRandomEnabled()) {
        playlist.resetShuffleState();
    }
    updatePlaylistUI();

    connect(positionSlider, &QSlider::sliderMoved, &musicController, &MusicController::setPosition);
    connect(&musicController, &MusicController::positionChanged, positionSlider, &QSlider::setValue);
    connect(&musicController, &MusicController::durationChanged, positionSlider, &QSlider::setMaximum);
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

    // Горячие клавиши:
    QShortcut* playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonPlayPause_clicked);

    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(nextShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonNext_clicked);

    QShortcut* prevShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(prevShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonPrevious_clicked);

    QShortcut* loopShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    connect(loopShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonLoop_clicked);

    QShortcut* randomShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
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

    QShortcut* addfolderShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), this);
    connect(addfolderShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonAdd_clicked);

    QShortcut* clearShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    connect(clearShortcut, &QShortcut::activated, this, &MusicPlayer::on_buttonClear_clicked);
}

MusicPlayer::~MusicPlayer() {
    delete ui;
    delete gifMovie;
    gifMovie = nullptr;
}

void MusicPlayer::updatePlaylistUI() {
    ui->listWidget->clear();
    for (const Track& track : playlist.getTracks()) {
        ui->listWidget->addItem(track.title);
    }
}

void MusicPlayer::closeEvent(QCloseEvent *event) {
    saveState();
    QMainWindow::closeEvent(event);
}

void MusicPlayer::saveState() {
    QSettings s("IST", "MusicPlayer");
    s.beginGroup("PlayerState");
    QStringList paths;
    if (musicController.isRandomEnabled()) {
        for (const Track &t : playlist.getOriginalTracks())
            paths << t.filePath;
    } else {
        for (const Track &t : playlist.getTracks())
            paths << t.filePath;
    }
    Track* cur = playlist.getCurrentTrack();
    if (cur)
        s.setValue("currentTrackPath", cur->filePath);
    else
        s.remove("currentTrackPath");
    s.setValue("queueOrder", paths);
    s.setValue("currentIndex", playlist.getCurrentIndex());
    s.setValue("randomEnabled", musicController.isRandomEnabled());
    s.setValue("loopEnabled",   musicController.isLoopEnabled());
    s.setValue("volume", ui->volumeSlider->value());
    s.endGroup();
}

void MusicPlayer::loadState() {
    QSettings s("IST", "MusicPlayer");
    s.beginGroup("PlayerState");
    QStringList paths = s.value("queueOrder").toStringList();
    playlist.clear();
    for (const QString &fp : paths) {
        Track t;
        t.filePath = fp;
        t.title    = QFileInfo(fp).baseName();
        playlist.addTrack(t);
    }
    bool randomOn = s.value("randomEnabled", false).toBool();
    musicController.setRandomEnabled(randomOn);
    playlist.resetShuffleState();
    if (randomOn) {
        playlist.enableShuffle();
    }
    bool loopOn = s.value("loopEnabled", false).toBool();
    musicController.setLoopEnabled(loopOn);
    QString savedPath = s.value("currentTrackPath", "").toString();
    if (!savedPath.isEmpty()) {
        const auto &trk = playlist.getTracks();
        int found = -1;
        for (int i = 0; i < trk.size(); ++i) {
            if (trk[i].filePath == savedPath) {
                found = i;
                break;
            }
        }
        if (found >= 0) {
            playlist.setCurrentIndex(found);
        }
    }
    int savedVolume = s.value("volume", 50).toInt();
    ui->volumeSlider->setValue(savedVolume);
    musicController.setVolumeFromSlider(savedVolume);
    s.endGroup();

    updatePlaylistUI();
    ui->listWidget->setCurrentRow(playlist.getCurrentIndex());
    ui->buttonRandom->setText(randomOn ? "🔀: ON" : "🔀: OFF");
    ui->buttonLoop->setText(loopOn     ? "🔄: ON"   : "🔄: OFF");
}

void MusicPlayer::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
    int row = ui->listWidget->row(item);
    playlist.setCurrentIndex(row);
    Track* track = playlist.getCurrentTrack();
    if (track) {
        musicController.setTrack();
        musicController.play();
    }
}

void MusicPlayer::on_listWidget_modelRowsMoved(const QModelIndex & /*parent*/,
                                               int sourceStart,
                                               int sourceEnd,      // Последняя строка диапазона, который переместили
                                               const QModelIndex & /*destinationParent*/,
                                               int destinationRow){
    Q_UNUSED(sourceEnd);
    // sourceStart — начальный индекс перемещаемой строки
    // destinationRow — место вставки (до какого индекса)
    int from = sourceStart;
    int to = destinationRow > from ? destinationRow - 1 : destinationRow;
    Track* currentTrack = playlist.getCurrentTrack();
    QString currentPath = currentTrack ? currentTrack->filePath : "";
    Track moved = playlist.getTracks().at(from);
    playlist.removeTrack(from);
    playlist.insertTrack(to, moved);
    ui->listWidget->setCurrentRow(to);
    const auto& tracks = playlist.getTracks();
    for (int i = 0; i < tracks.size(); ++i) {
        if (tracks[i].filePath == currentPath) {
            playlist.setCurrentIndex(i);
            break;
        }
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
    updatePlaylistUI();
    int idx = playlist.getCurrentIndex();
    ui->listWidget->setCurrentRow(idx);
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
    playlist.resetShuffleState();
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
    QSettings s("IST", "MusicPlayer");
    s.setValue("volume", value);
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
        ui->currentTrackLabel->setText(currentTrack->title);
        int idx = playlist.getCurrentIndex();
        ui->listWidget->setCurrentRow(idx);
        QString gifPath = updateGifImage();
        if (!gifPath.isEmpty()) {
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
    int previousGif = currentGif;
    do {
        currentGif = std::rand() % gifImages.size();
    } while (gifImages.size() > 1 && currentGif == previousGif);

    return gifImages[currentGif];
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
    if (!playlist.getTracks().isEmpty()) {
        ui->gifLabel->setVisible(true);
    }
    radioPage->StopStream();
    radioPage->setVisible(false);
    ui->pageArea->lower();
}
