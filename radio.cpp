#include "radio.h"

Radio::Radio(QWidget *parent)
    : QWidget(parent), playerProcess(new QProcess(this))
{
    urlEdit = new QLineEdit(this);
    playButton = new QPushButton("▶️", this);
    addButton = new QPushButton("❤️ Add to favorites", this);
    streamVolume = new QSlider(Qt::Vertical);
    stationList = new QListWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *controlsLayout = new QHBoxLayout();

    controlsLayout->addWidget(urlEdit);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(addButton);
    controlsLayout->addWidget(streamVolume);
    streamVolume->setRange(0, 100);
    streamVolume->setMaximumHeight(100);
    streamVolume->setValue(50);

    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(stationList);
    stationList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(stationList, &QListWidget::customContextMenuRequested, this, &Radio::showContextMenu);
    connect(playButton, &QPushButton::clicked, this, [=]() {
        startStream(urlEdit->text());
    });
    connect(addButton, &QPushButton::clicked, this, &Radio::addStation);
    connect(stationList, &QListWidget::itemClicked, this, &Radio::stationSelected);
    connect(streamVolume, &QSlider::valueChanged, this, &Radio::setVolume);

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
            QMessageBox::critical(this, "Error", "Failed to start ffplay!");
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
    QString url = urlEdit->text().trimmed();
    if (url.isEmpty())
        return;
    for (int i = 0; i < stationList->count(); ++i) {
        if (stationList->item(i)->data(Qt::UserRole).toString() == url) {
            QMessageBox::information(this, "Duplicate", "This url is already in favorites.");
            return;
        }
    }
    bool ok;
    QString name = QInputDialog::getText(this, "Add to favorites", "Station name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.trimmed().isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem(name.trimmed());
        item->setData(Qt::UserRole, url);
        stationList->addItem(item);
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
        StopStream();
        QString url = item->data(Qt::UserRole).toString();
        urlEdit->setText(url);
        startStream(url);
    }
}

void Radio::loadStations() {
    QFile file("stations.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("|");
            if (parts.size() == 2) {
                QListWidgetItem *item = new QListWidgetItem(parts[0]);
                item->setData(Qt::UserRole, parts[1]);
                stationList->addItem(item);
            }
        }
    }
}

void Radio::saveStations() {
    QFile file("stations.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int i = 0; i < stationList->count(); ++i) {
            QListWidgetItem *item = stationList->item(i);
            QString name = item->text();
            QString url = item->data(Qt::UserRole).toString();
            out << name << "|" << url << "\n";
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

void Radio::renameStation() {
    auto item = stationList->currentItem();
    if (!item) return;
    bool ok;
    QString currentName = item->text();
    QString newName = QInputDialog::getText(this, "Rename station",
                                            "New name:",
                                            QLineEdit::Normal,
                                            currentName, &ok);
    if (ok && !newName.trimmed().isEmpty()) {
        item->setText(newName.trimmed());
        saveStations();
    }
}

void Radio::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = stationList->itemAt(pos);
    if (!item) return;
    QMenu menu(this);
    QAction *renameAction = menu.addAction("Rename");
    QAction *deleteAction = menu.addAction("Delete");
    QAction *selectedAction = menu.exec(stationList->viewport()->mapToGlobal(pos));
    if (selectedAction == renameAction) {
        renameStation();
    } else if (selectedAction == deleteAction) {
        removeStation();
    }
}
