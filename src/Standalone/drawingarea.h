#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QRubberBand>
#include <QMouseEvent>
#include <QLabel>

#include <QPainter>
#include <functional>

#include "myrubberband.h"

typedef enum {
    TopLeft = 0,
    TopRight,
    DownLeft,
    DownRight
}MarqueeArea;

typedef enum {
    TopLeftPos = 0,
    TopRightPos,
    DownLeftPos,
    DownRightPos,
    TopEdge,
    LeftEdge,
    DownEdge,
    RightEdge
}MarqueeEdge;


typedef std::function<void(bool isExists, QRect MarqueeSize)> MarqueeCreatedCallBack;
typedef std::function<void(bool isExists)> MarqueeCreatedCallBack_forDelButton;


class DrawingArea : public QGraphicsView
{

public:
    DrawingArea(QWidget *parent = 0);
    ~DrawingArea();

    void SetMarqueeSizeLabel(QLabel *width_label, QLabel *height_label);
    void UpdateMarqueeSizeLabel(int width_size, int height_size);
    QRect GetMarqueeSize();


    void SetMarqueeCreatedCallBack(MarqueeCreatedCallBack callback)
    {
        m_mqrqueeCreatedCallBack = callback;
    }

    void SetMarqueeCreatedForDelButtonCallBack(MarqueeCreatedCallBack_forDelButton callback)
    {
        m_mqrqueeCreatedForDelButtonCallBack = callback;
    }

    void setMarqueeEnebled(bool Enabled);
    bool isExistedMarquee(void);
    void DeleteMarquee(void);
    void setMarqueeExist(bool existed);

    double MagnificationConversion(double input_num, bool toOriginal);
    double m_adjustment_value;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

private:
    MarqueeCreatedCallBack m_mqrqueeCreatedCallBack;
    MarqueeCreatedCallBack_forDelButton m_mqrqueeCreatedForDelButtonCallBack;

    MyRubberBand* m_rubberBand_;

    QPainter *m_painter;

    QPoint m_Before_Pos;
    QPoint m_Diff_Pos;
    QPoint m_MousePos;

    bool m_MousePressed;
    bool m_MouseEdgePressed;

    void OptimizeArea(void);

    bool CheckMouseOnMarquee(QPoint current_pos);
    QPoint MarqueeRangeLimit(QPoint current_pos);
    QPoint CalculateDiff(QPoint A_pos, QPoint B_pos, bool mode);
    bool CheckMouseOnEdge(QPoint current_pos);
    void CheckMarqueeArea(QPoint current_pos);
    void CheckMoveMarquee();

    MarqueeArea m_marqueeArea;
    MarqueeEdge m_marqueeEdge;
    QPoint m_side_position;

    QLabel *m_Width_label;
    QLabel *m_Height_label;

    bool m_MarqueeEnebled;

};

#endif // DRAWINGAREA_H
