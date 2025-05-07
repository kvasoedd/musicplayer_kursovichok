#include "audiovisualizer.h"

AudioVisualizer::AudioVisualizer(QWidget *parent)
    : QOpenGLWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

AudioDecoder::AudioDecoder(QObject *parent) : QObject(parent) {
    audioDecoder = new QAudioDecoder(this);
    decodeTimer = new QTimer(this);
    decodeTimer->setInterval(80); // примерно 30 мс — "псевдопоток"
    connect(decodeTimer, &QTimer::timeout, this, [this]() {
        if (audioDecoder->bufferAvailable()) {
            onBufferReady();
        }
    });
    connect(audioDecoder, &QAudioDecoder::finished, this, &AudioDecoder::onFinished);
    connect(audioDecoder, &QAudioDecoder::finished, this, &AudioDecoder::stopDecoding);
}

void AudioVisualizer::initializeGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // белый фон
}

void AudioVisualizer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, -h / 2, h / 2, -1, 1); // ортографическая проекция
    glMatrixMode(GL_MODELVIEW);
}

void AudioVisualizer::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (m_samples.isEmpty())
        return;
    glColor3f(0.0f, 1.0f, 0.0f); // зелёный цвет
    glBegin(GL_LINE_STRIP);
    int w = width();
    int count = m_samples.size();
    int step = std::max(1, count / w); // если больше ширины — пропускаем
    for (int i = 0; i < count; i += step) {
        float x = (float)i / step;
        float y = static_cast<float>(m_samples[i]) / 32768.0f * height() / 2.0f;
        glVertex2f(x, y);
    }
    glEnd();
}

void AudioVisualizer::updateSamples(const QVector<qint16> &samples) {
    m_samples = samples;
    update();
}

void AudioDecoder::decodeFile(const QString &filePath) {
    audioDecoder->setSource(QUrl::fromLocalFile(filePath));
    audioDecoder->start();
    decodeTimer->start();
}

void AudioDecoder::stopDecoding() {
    if (musicController.getPlaybackState() == QMediaPlayer::PlayingState) {
        decodeTimer->stop();
        audioDecoder->stop();
    }
}

void AudioDecoder::onBufferReady() {
    QAudioBuffer buffer = audioDecoder->read();
    if (!buffer.isValid()) return;
    QVector<qint16> samples;
    QAudioFormat::SampleFormat format = buffer.format().sampleFormat();
    if (format == QAudioFormat::Int16) {
        const qint16 *data = buffer.constData<qint16>();
        int sampleCount = buffer.sampleCount();
        samples = QVector<qint16>(data, data + sampleCount);
        emit samplesReady(samples);
    }
}

void AudioDecoder::onFinished() {
    decodeTimer->stop();
    qDebug() << "Buffer ready!";
}
