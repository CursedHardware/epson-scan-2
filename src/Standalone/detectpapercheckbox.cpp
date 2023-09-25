#include "mainwindow.h"

DetectPaperCheckBox::DetectPaperCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    this->initialize(false);
}

void DetectPaperCheckBox::initialize(bool default_item)
{
    changed = true;
    enabled = true;
    current_item = default_item;
}

void DetectPaperCheckBox::update_ui(){

    if(changed == true){
        this->set_enabled(enabled);
        this->select_item((bool)current_item);
        changed = false;
    }
}

void DetectPaperCheckBox::set_enabled(bool enabled)
{
    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void DetectPaperCheckBox::select_item(bool select){
    this->setChecked(select);
}

void DetectPaperCheckBox::item_event(bool value)
{

    current_item = value;
    changed = true;
}

