#include "mainwindow.h"

TransferDataAfterCompCheckBox::TransferDataAfterCompCheckBox(QWidget *parent) :
    QCheckBox(parent),
    changed(false),
    current_item(false)
{
}

void TransferDataAfterCompCheckBox::initialize(bool default_item){
    changed = true;
    current_item = default_item;
}

void TransferDataAfterCompCheckBox::update_ui()
{
    if(changed == true){
        this->set_enabled(true);
        this->select_item((bool)current_item);
        changed = false;
    }
}

void TransferDataAfterCompCheckBox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void TransferDataAfterCompCheckBox::select_item(bool select){
    this->setChecked(select);
}

bool TransferDataAfterCompCheckBox::item_event(bool value)
{
    current_item = value;
    changed = true;
    return value;
}
