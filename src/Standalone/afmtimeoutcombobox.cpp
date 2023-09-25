#include "afmtimeoutcombobox.h"
#include "mainwindow.h"

AfmTimeoutCombobox::AfmTimeoutCombobox(QWidget *parent) :
    QComboBox(parent),
    current_item(Timeout_none)
{
}

void AfmTimeoutCombobox::initialize(INT_SETTING_VALUE afm_para)
{
    current_item = (AutomaticModeTimeout)afm_para.select;
    this->update_ui();
}

void AfmTimeoutCombobox::update_ui()
{
    this->clear();
    this->add_item();
    this->select_item();
}

void AfmTimeoutCombobox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void AfmTimeoutCombobox::add_item(){

    int i = 0;

    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_019, Timeout_none);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_023, Timeout_10);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_024, Timeout_30);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_020, Timeout_60);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_021, Timeout_180);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_022, Timeout_300);
    i++;
}

void AfmTimeoutCombobox::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void AfmTimeoutCombobox::item_event(int value)
{
    QVariant select_item = this->itemData(value);

    if(select_item == Timeout_none){
        current_item = Timeout_none;
    }else if(select_item == Timeout_10){
        current_item = Timeout_10;
    }else if(select_item == Timeout_30){
        current_item = Timeout_30;
    }else if(select_item == Timeout_60){
        current_item = Timeout_60;
    }else if(select_item == Timeout_180){
        current_item = Timeout_180;
    }else if(select_item == Timeout_300){
        current_item = Timeout_300;
    }
}
