#ifndef SEEKSLIDER_H
#define SEEKSLIDER_H

#include <QSlider>
#include <QMouseEvent>

class SeekSlider : public QSlider {
    Q_OBJECT

public:
    explicit SeekSlider(QWidget* parent = nullptr) : QSlider(parent) {}

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (orientation() == Qt::Horizontal) {
            double pos = static_cast<double>(event->position().x()) / width();
            setValue(static_cast<int>(pos * maximum()));
            emit sliderMoved(value());
        }
        QSlider::mousePressEvent(event);
    }
};

#endif // SEEKSLIDER_H
