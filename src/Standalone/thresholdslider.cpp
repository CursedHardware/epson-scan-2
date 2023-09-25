#include "mainwindow.h"

ThresholdSlider::ThresholdSlider(QWidget *parent) :
    QSlider(parent),
    changed(false),
    current_item(0)
{
}

void ThresholdSlider::initialize(int default_item){
    changed = true;
    current_item = default_item;
}

void ThresholdSlider::update_ui(){

    if(changed == true){
        this->set_enabled(true);
        this->setValue(current_item);
        changed = false;
    }
}

void ThresholdSlider::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void ThresholdSlider::item_event(int value, INT_SETTING_VALUE* threshold_para)
{
    threshold_para->select = current_item = value;
    changed = true;
}
