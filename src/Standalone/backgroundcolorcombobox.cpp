#include "mainwindow.h"

BackgroundColorCombobox::BackgroundColorCombobox(QWidget *parent) :
    QComboBox(parent)
{
    changed = false;
    current_item = 0;
}

void BackgroundColorCombobox::initialize(SCANPARA backgroundcolor_para){
    changed = true;
    current_item = backgroundcolor_para.BackgroundColor.select;
}

void BackgroundColorCombobox::update_ui(INT_SETTING_VALUE backgroundcolor_para)
{
    if(backgroundcolor_para.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        this->show();
        changed = true;
    }else if(backgroundcolor_para.capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        this->show();
        current_item = BackgroundColorNone;
        changed = true;
    }else if(backgroundcolor_para.capability.supportLevel == kSDISupportLevelNone){
        this->hide();
    }

    if(changed == true){
        this->clear();
        this->add_item();
        this->select_item();
        changed = false;
    }
}

void BackgroundColorCombobox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void BackgroundColorCombobox::add_item(){

    int i = 0;
    this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_077, BackgroundColorBlack);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_078, BackgroundColorWhite);
    i++;
}

void BackgroundColorCombobox::select_item(){

    int item = this->findData(current_item);
    if(item != -1){
        this->setCurrentIndex(item);
    }else {
        this->setCurrentIndex(0);
    }
}

void BackgroundColorCombobox::item_event(int value, INT_SETTING_VALUE* backgroundcolor_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == BackgroundColorBlack){
        backgroundcolor_para->select = current_item = BackgroundColorBlack;
    }else if(select_item == BackgroundColorWhite){
        backgroundcolor_para->select = current_item = BackgroundColorWhite;
    }
    changed = true;
}
