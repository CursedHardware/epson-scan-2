#include "mainwindow.h"

GammaSlider::GammaSlider(QWidget *parent) :
    QSlider(parent)
{
    changed = false;
    current_item = 0;
}

void GammaSlider::initialize(double default_item){
    changed = true;
    current_item = default_item;
}

void GammaSlider::update_ui(){

    if(changed == true){
        this->set_enabled(true);
        this->setValue(current_item);
        changed = false;
    }
}

void GammaSlider::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void GammaSlider::item_event(double value, INT_SETTING_VALUE* gamma_para)
{
    gamma_para->select = current_item = value;
    changed = true;
}

void GammaSlider::setGammaSlider(double value){
    this->setValue(value * 10);
}
