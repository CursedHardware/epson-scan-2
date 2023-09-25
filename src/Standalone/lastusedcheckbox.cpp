#include "mainwindow.h"

LastUsedCheckBox::LastUsedCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    this->initialize(true);
}

void LastUsedCheckBox::initialize(bool default_item){
    changed = true;
    enabled = true;
    current_item = default_item;
}

void LastUsedCheckBox::update_ui(){

    if(changed == true){
        this->set_enabled(enabled);
        this->select_item((bool)current_item);
        changed = false;
    }
}

void LastUsedCheckBox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void LastUsedCheckBox::select_item(bool select){
    this->setChecked(select);
}

bool LastUsedCheckBox::item_event(bool value)
{
    current_item = value;
    changed = true;

    return value;
}
