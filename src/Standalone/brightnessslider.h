#ifndef BRIGHTNESSSLIDER_H
#define BRIGHTNESSSLIDER_H

#include <QSlider>
#include "supervisor.h"

class BrightnessSlider : public QSlider
{
public:
    BrightnessSlider(QWidget *parent = 0);

    int current_item;

    void initialize(int default_item);

    void update_ui();

    void item_event(int value, INT_SETTING_VALUE* brightness_para);

private:
    bool changed;

    void set_enabled(bool enabled);

};

#endif // BRIGHTNESSSLIDER_H
