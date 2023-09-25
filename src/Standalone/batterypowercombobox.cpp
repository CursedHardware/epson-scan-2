#include "mainwindow.h"

BatteryPowerComboBox::BatteryPowerComboBox(QWidget *parent) :
    QComboBox(parent)
{
    this->initialize(t_Off);
}

void BatteryPowerComboBox::initialize(Time default_item){
    changed = true;
    enabled = true;
    current_item = default_item;
}

void BatteryPowerComboBox::update_ui(INT_SETTING_VALUE power_of_time_para){

    if(changed == true){
        this->clear();
        this->set_enabled(enabled);
        this->add_item(power_of_time_para.capability.list, power_of_time_para.capability.countOfAllList);
        this->select_item();
        changed = false;
    }
}

void BatteryPowerComboBox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void BatteryPowerComboBox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;
    int item_num = 0;
    while(i < list_size){
        if(item_list[i] == t_Off){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ES2U_MAIN_025, t_Off);
            item_num++;
        }else if(item_list[i] == t_3){
            this->insertItem(item_num, "3", t_3);
            item_num++;
        }else if(item_list[i] == t_5){
            this->insertItem(item_num, "5", t_5);
            item_num++;
        }else if(item_list[i] == t_10){
            this->insertItem(item_num, "10", t_10);
            item_num++;
        }else if(item_list[i] == t_15){
            this->insertItem(item_num, "15", t_15);
            item_num++;
        }else if(item_list[i] == t_30){
            this->insertItem(item_num, "30", t_30);
            item_num++;
        }else if(item_list[i] == t_60){
            this->insertItem(item_num, "60", t_60);
            item_num++;
        }else if(item_list[i] == t_120){
            this->insertItem(item_num, "120", t_120);
            item_num++;
        }else if(item_list[i] == t_240){
            this->insertItem(item_num, "240", t_240);
            item_num++;
        }else if(item_list[i] == t_480){
            this->insertItem(item_num, "480", t_480);
            item_num++;
        }else if(item_list[i] == t_720){
            this->insertItem(item_num, "720", t_720);
            item_num++;
        }
        i++;
    }
}

void BatteryPowerComboBox::select_item(){

    this->setCurrentIndex(this->findData(current_item));
}

void BatteryPowerComboBox::item_event(int value)
{
    QVariant select_item = this->itemData(value);

    if(select_item == t_Off){
        current_item = t_Off;
    }else if(select_item == t_3){
        current_item = t_3;
    }else if(select_item == t_5){
        current_item = t_5;
    }else if(select_item == t_10){
        current_item = t_10;
    }else if(select_item == t_15){
        current_item = t_15;
    }else if(select_item == t_30){
        current_item = t_30;
    }else if(select_item == t_60){
        current_item = t_60;
    }else if(select_item == t_120){
        current_item = t_120;
    }else if(select_item == t_240){
        current_item = t_240;
    }else if(select_item == t_480){
        current_item = t_480;
    }else if(select_item == t_720){
        current_item = t_720;
    }
    changed = true;
}

Time BatteryPowerComboBox::get_current_item(void)
{
    return current_item;
}
