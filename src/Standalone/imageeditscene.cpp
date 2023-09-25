#include "imageeditscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

ImageEditScene::ImageEditScene(QObject *parent)
{
    //・qDebug() << "constract";
}

void ImageEditScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "mousePressEvent_" << event->scenePos();
}

void ImageEditScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    scenePos = event->scenePos();
    //qDebug() << "mousePressEvent_" << event->scenePos();
}

QPointF ImageEditScene::getScenePos()
{
   return scenePos;
}
