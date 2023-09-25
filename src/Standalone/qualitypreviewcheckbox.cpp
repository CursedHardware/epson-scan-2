#include "mainwindow.h"

QualityPreviewCheckbox::QualityPreviewCheckbox(QWidget *parent) :
    QCheckBox(parent)
{
    this->initialize(true);
}

void QualityPreviewCheckbox::initialize(bool default_item){
    changed = true;
    enabled = true;
    current_item = default_item;
}

void QualityPreviewCheckbox::update_ui(){

    if(changed == true){
        this->set_enabled(enabled);
        this->select_item((bool)current_item);
        changed = false;
    }
}

void QualityPreviewCheckbox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void QualityPreviewCheckbox::select_item(bool select){
    this->setChecked(select);
}

bool QualityPreviewCheckbox::item_event(bool value)
{
    current_item = value;
    changed = true;
    return value;
}
