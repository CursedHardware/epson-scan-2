#ifndef IMAGEEDITSCENE_H
#define IMAGEEDITSCENE_H

#include <QObject>
#include <QWidget>
#include <QGraphicsScene>

class ImageEditDialog;

class ImageEditScene : public QGraphicsScene
{
public:
    ImageEditScene(QObject *parent = 0);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hyouji();

    QPointF m_scenePos;
    //QGraphicsSceneMouseEvent *m_sceneMouseEvent;
    //QMouseEvent *m_MouseEvent;
    QPointF getScenePos();

private:
    QPointF scenePos;
};

#endif // IMAGEEDITSCENE_H
