#include "mainwindow.h"

ContrastSlider::ContrastSlider(QWidget *parent) :
    QSlider(parent)
{
    changed = false;
    current_item = 0;
}

void ContrastSlider::initialize(int default_item){
    changed = true;
    current_item = default_item;
}

void ContrastSlider::update_ui(){

    if(changed == true){
        this->set_enabled(true);
        this->setValue(current_item);
        changed = false;
    }
}

void ContrastSlider::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void ContrastSlider::item_event(int value, INT_SETTING_VALUE* contrast_para)
{
    contrast_para->select = current_item = value;
    changed = true;
}
