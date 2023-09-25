#include "myrubberband.h"

#include <QPainter>
#include <QStyleOptionFocusRect>

#define UPDATE_INTERVAL 150

MyRubberBand::MyRubberBand(QRubberBand::Shape s, QWidget *p) :
    QRubberBand(s,p),
    m_timer(NULL),
    m_existMarquee(false),
    m_animation_flag(0),
    m_onMouse(false)
{
    //定期実行QTimerの準備
    m_timer = new QTimer(this);
    if(m_timer == NULL){
        return;
    }
    m_timer->setInterval(UPDATE_INTERVAL);
    m_timer->setSingleShot(false);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(Update()));
}

MyRubberBand::~MyRubberBand(){
    if(m_timer){
        if(m_timer->isActive()){
            m_timer->stop();
        }

        delete m_timer;
        m_timer = NULL;
    }
}

void MyRubberBand::paintEvent(QPaintEvent *event)
{
    if(m_existMarquee == false){
        QRubberBand::paintEvent(event);
    }
    QPainter p(this);
    p.setBrush(QBrush(Qt::NoBrush));

    QPen pen = p.pen();
    if(m_animation_flag == 0){
        pen.setStyle(Qt::DashLine   );
    }else if(m_animation_flag == 1){
        pen.setStyle(Qt::DashDotLine );
    }else if(m_animation_flag == 2){
        pen.setStyle(Qt::DashDotDotLine );
    }
    pen.setBrush(Qt::blue);
    pen.setWidth(2);
    p.setPen(pen);

    int width, height;
    width = this->width();
    height = this->height();

    p.drawLine(QPoint(0, 0), QPoint(width, 0));
    p.drawLine(QPoint(0, 0), QPoint(0, height));
    p.drawLine(QPoint(width, 0), QPoint(width, height));
    p.drawLine(QPoint(0, height), QPoint(width, height));

    //if(size().width() >10 && size().height() >10)
    //{
      //  p.drawText(20,20,QString("%1,%2").arg(size().width()).arg(size().height()));
    //}
}

void MyRubberBand::Update()
{
    m_animation_flag++;
    if(m_animation_flag == 3){
        m_animation_flag = 0;
    }
    this->repaint();
}
