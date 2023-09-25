#include "mainwindow.h"

TextEnhanceCombobox::TextEnhanceCombobox(QWidget *parent) :
    QComboBox(parent),
    changed(false),
    current_item(TextEnhanceNone)
{

}

void TextEnhanceCombobox::initialize(INT_SETTING_VALUE textenhance_para)
{
    changed = true;
    current_item = (TextEnhance)textenhance_para.select;
}

void TextEnhanceCombobox::update_ui(INT_SETTING_VALUE textenhance_para)
{
    this->set_visible(textenhance_para.capability.supportLevel);

    if(changed == true){
        this->clear();
        if(textenhance_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->add_item(textenhance_para.capability.list, textenhance_para.capability.countOfList);
            this->select_item();
        }else {
            this->add_item(textenhance_para.capability.allList, textenhance_para.capability.countOfAllList);
            this->set_enabled(false);
        }
        changed = false;
    }
}

void TextEnhanceCombobox::set_visible(SDISupportLevel level)
{
    if(level == kSDISupportLevelAvailable){
        this->show();
    }else if(level == kSDISupportLevelUnavailable){
        this->show();
    }else if(level == kSDISupportLevelNone){
        this->hide();
    }
}

void TextEnhanceCombobox::set_enabled(bool enabled)
{
    if(enabled == true){
        this->setEnabled(true);
        if(current_item == TextEnhanceNone){
            current_item = TextEnhanceNormal;
            changed = true;
        }
    }else {
        this->setEnabled(false);
        current_item = TextEnhanceNone;
        this->setCurrentIndex(this->findData(TextEnhanceNormal));
    }
}

void TextEnhanceCombobox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;
    int item_num = 0;
    while(i < list_size){
        if(item_list[i] == TextEnhanceNormal){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ADVANCEDSETTING_008, TextEnhanceNormal);
            item_num++;
        }else if(item_list[i] == TextEnhanceHigh){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ADVANCEDSETTING_009, TextEnhanceHigh);
            item_num++;
        }
        i++;
    }
}

void TextEnhanceCombobox::select_item()
{
    int item = this->findData(current_item);
    if(item != -1){
        this->setCurrentIndex(item);
    }else {
        this->setCurrentIndex(0);
    }
}

void TextEnhanceCombobox::item_event(int value, INT_SETTING_VALUE* textenhance_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == TextEnhanceNormal){
        textenhance_para->select = current_item = TextEnhanceNormal;
    }else if(select_item == TextEnhanceHigh){
        textenhance_para->select = current_item = TextEnhanceHigh;
    }
    changed = true;
}
