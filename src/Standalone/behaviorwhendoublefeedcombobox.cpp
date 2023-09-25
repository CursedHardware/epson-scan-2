#include "mainwindow.h"

BehaviorWhenDoubleFeedCombobox::BehaviorWhenDoubleFeedCombobox(QWidget *parent) :
    QComboBox(parent)
{
    this->initialize(BehaviorWhenDoubleFeed_Immediately);
}

void BehaviorWhenDoubleFeedCombobox::initialize(BehaviorWhenDoubleFeed default_item){
    changed = true;
    enabled = true;
    current_item = default_item;
}

void BehaviorWhenDoubleFeedCombobox::update_ui(INT_SETTING_VALUE behavior_when_double_feed_para){

    if(behavior_when_double_feed_para.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        this->show();
        changed = true;
    }else if(behavior_when_double_feed_para.capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        this->show();
        current_item = BehaviorWhenDoubleFeed_Immediately;
        changed = true;
    }else if(behavior_when_double_feed_para.capability.supportLevel == kSDISupportLevelNone){
        this->hide();
        current_item = BehaviorWhenDoubleFeed_Immediately;
    }
    if(changed == true){
        this->clear();
        this->add_item(behavior_when_double_feed_para.capability.list, behavior_when_double_feed_para.capability.countOfAllList);
        this->select_item();
        changed = false;
    }
}

void BehaviorWhenDoubleFeedCombobox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void BehaviorWhenDoubleFeedCombobox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;
    int item_num = 0;
    while(i < list_size){
        if(item_list[i] == BehaviorWhenDoubleFeed_Immediately){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ES2U_SCANNER_010, BehaviorWhenDoubleFeed_Immediately);
            item_num++;
        }else if(item_list[i] == BehaviorWhenDoubleFeed_StopAfterEjecting){
            this->insertItem(item_num, TranslationString::GetString().translation_str.ES2U_SCANNER_011, BehaviorWhenDoubleFeed_StopAfterEjecting);
            item_num++;
        }
        i++;
    }
}

void BehaviorWhenDoubleFeedCombobox::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void BehaviorWhenDoubleFeedCombobox::item_event(int value)
{
    QVariant select_item = this->itemData(value);

    if(select_item == BehaviorWhenDoubleFeed_Immediately){
        current_item = BehaviorWhenDoubleFeed_Immediately;
    }else if(select_item == BehaviorWhenDoubleFeed_StopAfterEjecting){
        current_item = BehaviorWhenDoubleFeed_StopAfterEjecting;
    }
    changed = true;
}

BehaviorWhenDoubleFeed BehaviorWhenDoubleFeedCombobox::get_current_item(void)
{
    return current_item;
}
