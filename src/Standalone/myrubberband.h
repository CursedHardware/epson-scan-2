#ifndef MYRUBBERBAND_H
#define MYRUBBERBAND_H

#include <QRubberBand>
#include <QPaintEvent>
#include <QTimer>
#include <QObject>
#include <QApplication>
#include <QDebug>

class MyRubberBand : public QRubberBand
{
    Q_OBJECT

public:
    explicit MyRubberBand(Shape s, QWidget * p = 0);
    ~MyRubberBand();

    QPoint m_startPos_;
    QPoint m_endPos_;

    bool m_existMarquee;

    QTimer* m_timer;

    int m_animation_flag;
    bool m_onMouse;

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void Update();
};

#endif // MYRUBBERBAND_H
