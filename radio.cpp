#include "radio.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

Radio::Radio(QWidget *parent)
    : QWidget(parent), playerProcess(new QProcess(this))
{
    urlEdit = new QLineEdit(this);
    playButton = new QPushButton("▶️", this);
    addButton = new QPushButton("❤️ Add to favorites", this);
    streamVolume = new QSlider(Qt::Vertical);
    removeButton = new QPushButton("❌ Delete from favorites", this);
    stationList = new QListWidget(this);
    nowPlayingLabel = new QLabel("🎵 Now playing: ---", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *controlsLayout = new QHBoxLayout();

    controlsLayout->addWidget(urlEdit);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(streamVolume);
    streamVolume->setRange(0, 100);
    streamVolume->setMaximumHeight(100);
    streamVolume->setValue(50);

    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(addButton);
    mainLayout->addWidget(removeButton);
    mainLayout->addWidget(stationList);
    mainLayout->addWidget(nowPlayingLabel);

    connect(playButton, &QPushButton::clicked, this, [=]() {
        startStream(urlEdit->text());
    });
    connect(addButton, &QPushButton::clicked, this, &Radio::addStation);
    connect(removeButton, &QPushButton::clicked, this, &Radio::removeStation);
    connect(stationList, &QListWidget::itemClicked, this, &Radio::stationSelected);
    connect(streamVolume, &QSlider::valueChanged, this, &Radio::setVolume);
    connect(playerProcess, &QProcess::readyReadStandardOutput, this, &Radio::readProcessOutput);
    connect(playerProcess, &QProcess::readyReadStandardError, this, &Radio::readProcessOutput);

    loadStations();
}

Radio::~Radio() {}

void Radio::startStream(const QString &url) {
    QUrl testUrl(url);
    if (!testUrl.isValid() || testUrl.scheme().isEmpty() || url.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Invalid URL");
        return;
    }
    if (playerProcess->state() != QProcess::NotRunning) {
        playerProcess->kill();
        playerProcess->waitForFinished();
        playButton->setText("▶️");
    }
    else {
        QString program = "ffplay";
        QStringList arguments;
        arguments << "-nodisp" << "-autoexit" << "-loglevel" << "info" << "-volume" << QString::number(streamVolume->value())<< url;
        playerProcess->start(program, arguments);
        if (!playerProcess->waitForStarted(3000)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось запустить ffplay!");
            playButton->setText("▶️");
        }
        else {
            playButton->setText("⏸");
        }
    }
}

void Radio::StopStream(){
    if (playerProcess->state() != QProcess::NotRunning) {
        playerProcess->kill();
        playerProcess->waitForFinished();
        playButton->setText("▶️");
    }
}

void Radio::addStation() {
    QString url = urlEdit->text();
    if (!url.isEmpty()) {
        stationList->addItem(url);
        saveStations();
    }
}

void Radio::removeStation() {
    auto item = stationList->currentItem();
    if (item) {
        delete item;
        saveStations();
    }
}

void Radio::stationSelected() {
    auto item = stationList->currentItem();
    if (item) {
        urlEdit->setText(item->text());
    }
}

void Radio::readProcessOutput() {
    QString output = QString::fromUtf8(playerProcess->readAllStandardOutput());
    QString errorOutput = QString::fromUtf8(playerProcess->readAllStandardError());
    QString combined = output + errorOutput;
    if (combined.contains("ICY Info:")) {
        int start = combined.indexOf("StreamTitle='") + QString("StreamTitle='").length();
        int end = combined.indexOf("';", start);
        QString title = combined.mid(start, end - start);
        nowPlayingLabel->setText("🎵 Now playing: " + title);
    }
}

void Radio::loadStations() {
    QFile file("stations.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                stationList->addItem(line);
            }
        }
    }
}

void Radio::saveStations() {
    QFile file("stations.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (int i = 0; i < stationList->count(); ++i) {
            out << stationList->item(i)->text() << "\n";
        }
    }
}

void Radio::setVolume(int volume) {
    if (playerProcess->state() != QProcess::NotRunning) {
        QString currentUrl = urlEdit->text();
        playerProcess->kill();
        playerProcess->waitForFinished();
        startStream(currentUrl);
    }
}
