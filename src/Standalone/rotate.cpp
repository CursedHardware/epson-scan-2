#include "mainwindow.h"

RotateComb::RotateComb(QWidget *parent) :
    QComboBox(parent)
{
    changed = false;
}

void RotateComb::initialize(){
    changed = true;
}

void RotateComb::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void RotateComb::update_ui(INT_SETTING_VALUE rotate_para){

    if(changed == true){
        this->clear();
        if(rotate_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->set_enabled(true);
            this->add_item(rotate_para.capability.list, rotate_para.capability.countOfList);
            this->select_item(rotate_para);
        }else {
            this->set_enabled(false);
        }
        changed = false;
    }
}

void RotateComb::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void RotateComb::add_item(int32_t item_list[], int32_t list_size){

    int i = 0;
    while(i < list_size){
        if(item_list[i] == OrientationNone){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_021, OrientationNone);
        }else if(item_list[i] == Orientation90){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_022, Orientation90);
        }else if(item_list[i] == Orientation180){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_023, Orientation180);
        }else if(item_list[i] == Orientation270){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_024, Orientation270);
        }else if(item_list[i] == OrientationAuto){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_025, OrientationAuto);
        }
        i++;
    }
}

void RotateComb::select_item(INT_SETTING_VALUE rotate_para){

    int list_size = rotate_para.capability.countOfList;

    int i = 0;
    while(i < list_size){
        if(rotate_para.capability.list[i] == rotate_para.select){
            this->setCurrentIndex(i);
        }
        i++;
    }
}

void RotateComb::item_event(int value, INT_SETTING_VALUE* rotate_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == OrientationNone){
        rotate_para->select = OrientationNone;
    }else if(select_item == Orientation90){
        rotate_para->select = Orientation90;
    }else if(select_item == Orientation180){
        rotate_para->select = Orientation180;
    }else if(select_item == Orientation270){
        rotate_para->select = Orientation270;
    }else if(select_item == OrientationAuto){
        rotate_para->select = OrientationAuto;
    }
    changed = true;
}

Orientation RotateComb::getCurrentItem(void)
{
    QVariant select_item = this->itemData(this->currentIndex());

    if(select_item == OrientationNone){
        return OrientationNone;
    }else if(select_item == Orientation90){
        return Orientation90;
    }else if(select_item == Orientation180){
        return Orientation180;
    }else if(select_item == Orientation270){
        return Orientation270;
    }else if(select_item == OrientationAuto){
        return OrientationAuto;
    }
    return OrientationNone;
}
