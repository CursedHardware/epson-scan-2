#include "mainwindow.h"

DeskewCheckbox::DeskewCheckbox(QWidget *parent) :
    QCheckBox(parent),
    changed(false)
{

}

void DeskewCheckbox::initialize(){
    changed = true;
}

void DeskewCheckbox::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void DeskewCheckbox::update_ui(INT_SETTING_VALUE deskew_para)
{
    if(deskew_para.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        this->show();
    }else if(deskew_para.capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        this->show();
        deskew_para.select = false;
        changed = true;
    }else if(deskew_para.capability.supportLevel == kSDISupportLevelNone){
        this->hide();
    }

    if(changed == true){
        this->select_item(deskew_para.select);
        changed = false;
    }
}

void DeskewCheckbox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void DeskewCheckbox::select_item(bool select){
    this->setChecked(select);
}

void DeskewCheckbox::item_event(bool value, INT_SETTING_VALUE* deskew_para)
{
    deskew_para->select = value;
    changed = true;
}
