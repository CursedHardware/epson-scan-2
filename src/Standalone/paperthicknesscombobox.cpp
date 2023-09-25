#include "mainwindow.h"

PaperThicknessComboBox::PaperThicknessComboBox(QWidget *parent) :
    QComboBox(parent)
{
    this->initialize(DoubleFeedDetectionLow);
}

void PaperThicknessComboBox::initialize(DoubleFeedDetection default_item){
    changed = true;
    enabled = false;
    current_item = default_item;
}

void PaperThicknessComboBox::update_ui(INT_SETTING_VALUE DoubleFeedDetectionLevel){

    if(changed == true){
        this->clear();
        this->add_item(DoubleFeedDetectionLevel.capability.list, DoubleFeedDetectionLevel.capability.countOfList);
        this->select_item();
        changed = false;
    }
}

void PaperThicknessComboBox::set_enabled(bool enabled)
{
    this->enabled = enabled;
    this->setEnabled(enabled);
}

void PaperThicknessComboBox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;

    while(i < list_size){
        if(item_list[i] == kSDIDoubleFeedDetectionLow){
            this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_014, DoubleFeedDetectionLow);
        }else if(item_list[i] == kSDIDoubleFeedDetectionHigh){
            this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_013, DoubleFeedDetectionHigh);
        }
        i++;
    }
}

void PaperThicknessComboBox::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void PaperThicknessComboBox::item_event(int value)
{
    QVariant select_item = this->itemData(value);

    if(select_item == DoubleFeedDetectionLow){
        current_item = DoubleFeedDetectionLow;
    }else if(select_item == DoubleFeedDetectionHigh){
        current_item = DoubleFeedDetectionHigh;
    }
    changed = true;
}
