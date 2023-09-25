#include "mainwindow.h"

NonConnectPowerOffCombobox::NonConnectPowerOffCombobox(QWidget *parent) :
    QComboBox(parent)
{
    this->initialize(NonConnectPowerOff_Off);
}

void NonConnectPowerOffCombobox::initialize(NonConnectPowerOff default_item){
    changed = true;
    enabled = true;
    current_item = default_item;
}

void NonConnectPowerOffCombobox::update_ui(INT_SETTING_VALUE non_connect_power_off_para){

    if(non_connect_power_off_para.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        this->show();
        changed = true;
    }else if(non_connect_power_off_para.capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        this->show();
        current_item = NonConnectPowerOff_Off;
        changed = true;
    }else if(non_connect_power_off_para.capability.supportLevel == kSDISupportLevelNone){
        this->hide();
        current_item = NonConnectPowerOff_Off;
    }
    if(changed == true){
        this->clear();
        this->add_item(non_connect_power_off_para.capability.list, non_connect_power_off_para.capability.countOfAllList);
        this->select_item();
        changed = false;
    }
}

void NonConnectPowerOffCombobox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void NonConnectPowerOffCombobox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;
    int item_num = 0;
    while(i < list_size){
        if(item_list[i] == NonConnectPowerOff_Off){
            this->insertItem(item_num, TranslationString::GetString().translation_str.CONFIG_SCAN_009, NonConnectPowerOff_Off);
            item_num++;
        }else if(item_list[i] == NonConnectPowerOff_On){
            this->insertItem(item_num, TranslationString::GetString().translation_str.CONFIG_SCAN_008, NonConnectPowerOff_On);
            item_num++;
        }
        i++;
    }
}

void NonConnectPowerOffCombobox::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void NonConnectPowerOffCombobox::item_event(int value)
{
    QVariant select_item = this->itemData(value);

    if(select_item == NonConnectPowerOff_Off){
        current_item = NonConnectPowerOff_Off;
    }else if(select_item == NonConnectPowerOff_On){
        current_item = NonConnectPowerOff_On;
    }
    changed = true;
}

NonConnectPowerOff NonConnectPowerOffCombobox::get_current_item(void)
{
    return current_item;
}
