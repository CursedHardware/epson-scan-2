#include "mainwindow.h"

AddpagesCheckbox::AddpagesCheckbox(QWidget *parent) :
    QCheckBox(parent)
{
    changed = false;
    current_item = false;
}

void AddpagesCheckbox::initialize(bool default_val){
    changed = true;
    current_item = default_val;
    this->setEnabled(true);
}

void AddpagesCheckbox::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void AddpagesCheckbox::update_ui(){

    if(changed == true){
        this->select_item((bool)current_item);
        changed = false;
    }
}

void AddpagesCheckbox::select_item(bool select){
    this->setChecked(select);
}

void AddpagesCheckbox::item_event(bool value, bool* add_pages)
{
    *add_pages = current_item = value;
    changed = true;
}

bool AddpagesCheckbox::get_current_item()
{
    return current_item;
}
