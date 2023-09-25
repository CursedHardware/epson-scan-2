#ifndef HORIZONDOCTOOLBUTTON_H
#define HORIZONDOCTOOLBUTTON_H

#include <QToolButton>
#include "supervisor.h"

class HorizonDoctoolButton : public QToolButton
{
    Q_OBJECT

public:
    HorizonDoctoolButton(QWidget *parent = 0);

public slots:
    void button_event(bool checked);
};

#endif // HORIZONDOCTOOLBUTTON_H
