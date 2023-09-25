#include "autocroppinginscribedcheckbox.h"

AutoCroppingInscribedCheckBox::AutoCroppingInscribedCheckBox(QWidget *parent) :
    QCheckBox(parent),
    changed(false)
{

}

void AutoCroppingInscribedCheckBox::initialize(bool default_val){
    changed = true;
    current_item = default_val;
    this->setEnabled(true);
}

void AutoCroppingInscribedCheckBox::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void AutoCroppingInscribedCheckBox::update_ui(INT_SETTING_VALUE inscribed_para)
{
    /*
    if(deskew_para.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        this->show();
    }else if(deskew_para.capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        this->show();
        deskew_para.select = false;
        changed = true;
    }else if(deskew_para.capability.supportLevel == kSDISupportLevelNone){
        this->hide();lin
    }
    */

    if(changed == true){
        this->select_item(inscribed_para.select);
        changed = false;
    }
}

void AutoCroppingInscribedCheckBox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void AutoCroppingInscribedCheckBox::select_item(bool select){
    this->setChecked(select);
}

void AutoCroppingInscribedCheckBox::item_event(bool value, INT_SETTING_VALUE* inscribed_para)
{
    inscribed_para->select = value;
    changed = true;
}
