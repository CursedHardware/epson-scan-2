#include "mainwindow.h"

BrightnessSlider::BrightnessSlider(QWidget *parent) :
    QSlider(parent),
    changed(false),
    current_item(0)
{
}

void BrightnessSlider::initialize(int default_item){
    changed = true;
    current_item = default_item;
}

void BrightnessSlider::update_ui(){

    if(changed == true){
        this->set_enabled(true);
        this->setValue(current_item);
        changed = false;
    }
}

void BrightnessSlider::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void BrightnessSlider::item_event(int value, INT_SETTING_VALUE* brightness_para)
{
    brightness_para->select = current_item = value;
    changed = true;
}
