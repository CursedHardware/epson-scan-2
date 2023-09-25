#include "mainwindow.h"
#include "removepunchholes.h"

RemovePunchHoles::RemovePunchHoles(QWidget *parent) :
    QComboBox(parent)
{
    changed = false;
}

void RemovePunchHoles::initialize(){
    changed = true;
}


void RemovePunchHoles::update_ui(INT_SETTING_VALUE remove_punchhole_para, QLabel* label1, QLabel* label2){

    label1->setText(TranslationString::GetString().translation_str.ADVANCEDSETTING_050);

    if (remove_punchhole_para.capability.supportLevel != kSDISupportLevelNone)
    {
        this->setVisible(true);
        label1->setVisible(true);
        label2->setVisible(true);
    }else
    {
        this->setVisible(false);
        label1->setVisible(false);
        label2->setVisible(false);
        return;
    }

    if(changed == true){
        this->clear();
        if(remove_punchhole_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->setEnabled(true);
            label1->setVisible(true);
            label2->setVisible(true);

            this->add_item(remove_punchhole_para.capability.list, remove_punchhole_para.capability.countOfList);
            this->select_item(remove_punchhole_para);
        }else {
            this->setEnabled(false);
            label1->setVisible(false);
            label2->setVisible(false);
        }
        changed = false;
    }
}

void RemovePunchHoles::add_item(int32_t item_list[], int32_t list_size){

    int i = 0;
    while(i < list_size){
        if(item_list[i] == RemovePunchHoleNone){
            this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_051, RemovePunchHoleNone);
        }else if(item_list[i] == RemovePunchHoleLeftRight){
            this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_053, RemovePunchHoleLeftRight);
        }else if(item_list[i] == RemovePunchHoleTopBottom){
            this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_054, RemovePunchHoleTopBottom);
        }else if(item_list[i] == RemovePunchHoleAll && list_size == 2){
            this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_052, RemovePunchHoleAll);
        }else if(item_list[i] == RemovePunchHoleAll){
            this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_055, RemovePunchHoleAll);
        }
        i++;
    }
}

void RemovePunchHoles::select_item(INT_SETTING_VALUE remove_punchhole_para){

    int list_size = remove_punchhole_para.capability.countOfList;

    int i = 0;
    while(i < list_size){
        if(remove_punchhole_para.capability.list[i] == remove_punchhole_para.select){
            this->setCurrentIndex(i);
        }
        i++;
    }
}

void RemovePunchHoles::item_event(int value, INT_SETTING_VALUE* remove_punchhole_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == RemovePunchHoleNone){
        remove_punchhole_para->select = RemovePunchHoleNone;
    }else if(select_item == RemovePunchHoleLeftRight){
        remove_punchhole_para->select = RemovePunchHoleLeftRight;
    }else if(select_item == RemovePunchHoleTopBottom){
        remove_punchhole_para->select = RemovePunchHoleTopBottom;
    }else if(select_item == RemovePunchHoleAll){
        remove_punchhole_para->select = RemovePunchHoleAll;
    }
    changed = true;
}
