#ifndef RADIO_H
#define RADIO_H

#include <QWidget>
#include <QProcess>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QListWidget>
#include <QAudioOutput>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>

class Radio : public QWidget
{
    Q_OBJECT

public:
    explicit Radio(QWidget *parent = nullptr);
    ~Radio();
    void setVolume(int volume);
    void StopStream();

private slots:
    void startStream(const QString &url);
    void addStation();
    void removeStation();
    void stationSelected();
    void renameStation();
    void showContextMenu(const QPoint &pos);

private:
    QLineEdit *urlEdit;
    QPushButton *playButton;
    QSlider *streamVolume;
    QPushButton *addButton;
    QListWidget *stationList;
    QProcess *playerProcess;
    void loadStations();
    void saveStations();
};

struct Station {
    QString name;
    QString url;
};

#endif // RADIO_H
