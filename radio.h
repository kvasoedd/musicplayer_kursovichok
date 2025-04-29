#ifndef RADIO_H
#define RADIO_H

#include <QWidget>
#include <QProcess>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QAudioOutput>

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

private:
    QLineEdit *urlEdit;
    QPushButton *playButton;
    QSlider *streamVolume;
    QPushButton *addButton;
    QPushButton *removeButton;
    QListWidget *stationList;
    QProcess *playerProcess;
    void loadStations();
    void saveStations();
};

#endif // RADIO_H
