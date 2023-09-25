#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QSlider>
#include "supervisor.h"

class DoubleSlider : public QSlider
{
public:
    DoubleSlider(QWidget *parent = 0);

    int current_item;

    void initialize(int default_item);

    void update_ui();

    void item_event(int value, INT_SETTING_VALUE* slider_para);

public slots:
    void setSlider(double value);

private:
    bool changed;

    void set_enabled(bool enabled);

};

#endif // DOUBLESLIDER_H
