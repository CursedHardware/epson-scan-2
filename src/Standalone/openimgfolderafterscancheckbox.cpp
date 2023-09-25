#include "mainwindow.h"

OpenImgFolderAfterScanCheckBox::OpenImgFolderAfterScanCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    this->initialize(true);
}

void OpenImgFolderAfterScanCheckBox::initialize(bool default_item){
    changed = true;
    enabled = true;
    current_item = default_item;
}

void OpenImgFolderAfterScanCheckBox::update_ui(){

    if(changed == true){
        this->set_enabled(enabled);
        this->select_item((bool)current_item);
        changed = false;
    }
}

void OpenImgFolderAfterScanCheckBox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void OpenImgFolderAfterScanCheckBox::select_item(bool select){
    this->setChecked(select);
}

bool OpenImgFolderAfterScanCheckBox::item_event(bool value)
{
    current_item = value;
    changed = true;
    return value;
}
