#include "doubleslider.h"

DoubleSlider::DoubleSlider(QWidget *parent) :
    QSlider(parent),
    changed(false),
    current_item(0)
{

}

void DoubleSlider::initialize(int default_item){
    changed = true;
    current_item = default_item;
}

void DoubleSlider::update_ui(){

    if(changed == true){
        this->set_enabled(true);
        this->setValue(current_item);
        changed = false;
    }
}

void DoubleSlider::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void DoubleSlider::item_event(int value, INT_SETTING_VALUE* slider_para)
{
    slider_para->select = current_item = value;
    changed = true;
}

void DoubleSlider::setSlider(double value)
{
    this->setValue(value * 10);
}
