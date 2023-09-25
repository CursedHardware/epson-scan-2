#ifndef VERTICALDOCTOOLBUTTON_H
#define VERTICALDOCTOOLBUTTON_H

#include <QToolButton>
#include "supervisor.h"

class VerticalDoctoolButton : public QToolButton
{
    Q_OBJECT

public:
    VerticalDoctoolButton(QWidget *parent = 0);

public slots:
    void button_event(bool checked);
};

#endif // VERTICALDOCTOOLBUTTON_H
