#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include <QOpenGLWidget>
#include <QVector>
#include <QPainter>
#include <QObject>
#include <QAudioDecoder>
#include <QAudioBuffer>
#include <QUrl>
#include <QAudioFormat>
#include <QTimer>
#include "musiccontroller.h"

class AudioVisualizer : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit AudioVisualizer(QWidget *parent = nullptr);
    void updateSamples(const QVector<qint16> &samples);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QVector<qint16> m_samples;
};

class AudioDecoder : public QObject {
    Q_OBJECT
public:
    explicit AudioDecoder(QObject *parent = nullptr);
    void decodeFile(const QString &filePath);
    void stopDecoding();

signals:
    void samplesReady(const QVector<qint16> &samples);

private slots:
    void onBufferReady();
    void onFinished();

private:
    QAudioDecoder *audioDecoder;
    QTimer *decodeTimer;
    MusicController musicController;
};

#endif // AUDIOVISUALIZER_H
