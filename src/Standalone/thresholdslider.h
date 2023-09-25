#ifndef THRESHOLDSLIDER_H
#define THRESHOLDSLIDER_H

#include <QSlider>
#include "supervisor.h"

class ThresholdSlider : public QSlider
{
public:
    ThresholdSlider(QWidget *parent = 0);

    int current_item;

    void initialize(int default_item);

    void update_ui();

    void item_event(int value, INT_SETTING_VALUE* threshold_para);

private:
    bool changed;

    void set_enabled(bool enabled);

};

#endif // THRESHOLDSLIDER_H
