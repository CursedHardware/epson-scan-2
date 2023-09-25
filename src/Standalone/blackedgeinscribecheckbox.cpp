#include "blackedgeinscribecheckbox.h"

BlackEdgeInscribeCheckBox::BlackEdgeInscribeCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    changed = false;
    current_item = false;
}

void BlackEdgeInscribeCheckBox::initialize(bool default_val){
    changed = true;
    current_item = default_val;
    this->setEnabled(true);
}

void BlackEdgeInscribeCheckBox::focusOutEvent(QFocusEvent* event){
    //event->accept();
}

void BlackEdgeInscribeCheckBox::update_ui(){

    if(changed == true){
        this->select_item((bool)current_item);
        changed = false;
    }
}

void BlackEdgeInscribeCheckBox::select_item(bool select){
    this->setChecked(select);
}

void BlackEdgeInscribeCheckBox::item_event(bool value)
{
    current_item = value;
    changed = true;
}

bool BlackEdgeInscribeCheckBox::get_current_item()
{
    return current_item;
}
