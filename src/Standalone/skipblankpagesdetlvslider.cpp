#include "mainwindow.h"


SkipBlankPagesDetLvSlider::SkipBlankPagesDetLvSlider(QWidget *parent) :
    QSlider(parent)
{
    this->initialize(10);
}

void SkipBlankPagesDetLvSlider::initialize(int default_item){
    changed = true;
    enabled = true;
    current_value = default_item;
}

void SkipBlankPagesDetLvSlider::update_ui(){

    if(changed == true){
        this->set_enabled(enabled);
        changed = false;
    }
}

void SkipBlankPagesDetLvSlider::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void SkipBlankPagesDetLvSlider::item_event(int value)
{
    current_value = value;
    changed = true;
}
