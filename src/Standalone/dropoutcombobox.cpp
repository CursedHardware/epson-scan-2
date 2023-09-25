#include "mainwindow.h"

DropoutCombobox::DropoutCombobox(QWidget *parent) :
    QComboBox(parent),
    changed(false),
    current_item(DropoutColorNone)
{

}

void DropoutCombobox::initialize(INT_SETTING_VALUE dropout_para)
{
    changed = true;
    current_item = (DropoutColor)dropout_para.select;
}

void DropoutCombobox::update_ui(INT_SETTING_VALUE dropout_para)
{
    if(changed == true){
        this->clear();
        if(dropout_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->add_item(dropout_para.capability.list, dropout_para.capability.countOfList);
            this->select_item();
            this->show();
        }else if(dropout_para.capability.supportLevel == kSDISupportLevelUnavailable){
            this->add_item(dropout_para.capability.allList, dropout_para.capability.countOfAllList);
            this->set_enabled(false);
            this->show();
        }else if(dropout_para.capability.supportLevel == kSDISupportLevelNone){
            this->hide();
        }
        changed = false;
    }
}

void DropoutCombobox::set_enabled(bool enabled)
{
    if(enabled == true){
        this->setEnabled(true);
        if(current_item == DropoutColorNone){
            current_item = DropoutColorRed;
            changed = true;
        }
    }else {
        this->setEnabled(false);
        current_item = DropoutColorNone;
        this->setCurrentIndex(this->findData(DropoutColorRed));
    }
}

void DropoutCombobox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;
    int item_num = 0;
    while(i < list_size){
        if(item_list[i] == DropoutColorRed){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ADVANCEDSETTING_011, DropoutColorRed);
            item_num++;
        }else if(item_list[i] == DropoutColorBlue){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ADVANCEDSETTING_013, DropoutColorBlue);
            item_num++;
        }else if(item_list[i] == DropoutColorGreen){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ADVANCEDSETTING_012, DropoutColorGreen);
            item_num++;
        }
        i++;
    }
}

void DropoutCombobox::select_item()
{
    int item = this->findData(current_item);
    if(item != -1){
        this->setCurrentIndex(item);
    }else {
        this->setCurrentIndex(0);
    }
}

void DropoutCombobox::item_event(int value, INT_SETTING_VALUE* dropout_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == DropoutColorRed){
        dropout_para->select = current_item = DropoutColorRed;
    }else if(select_item == DropoutColorBlue){
        dropout_para->select = current_item = DropoutColorBlue;
    }else if(select_item == DropoutColorGreen){
        dropout_para->select = current_item = DropoutColorGreen;
    }
    changed = true;
}
