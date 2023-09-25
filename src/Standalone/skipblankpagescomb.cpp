#include "mainwindow.h"

SkipblankpagesComb::SkipblankpagesComb(QWidget *parent) :
    QComboBox(parent)
{
    skipblankpagessettings_dialog = NULL;
    changed = false;
    current_level = 0;
    current_item = false;
}

void SkipblankpagesComb::initialize(SCANPARA skipblankpageslv_para){
    changed = true;
    current_level = skipblankpageslv_para.BlankPageSkipLevelKey.select;
    current_item = skipblankpageslv_para.BlankPageSkipKey.select;
}

void SkipblankpagesComb::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void SkipblankpagesComb::update_ui(INT_SETTING_VALUE* skipblankpages_para)
{
    if(skipblankpages_para->capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        this->show();
    }else if(skipblankpages_para->capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        this->show();
        skipblankpages_para->select =current_item = Off;
        changed = true;
    }else if(skipblankpages_para->capability.supportLevel == kSDISupportLevelNone){
        this->hide();
    }

    if(changed == true){
        this->clear();
        this->add_item();
        this->select_item();
        changed = false;
    }
}

void SkipblankpagesComb::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void SkipblankpagesComb::add_item(){

    int i = 0;
    this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_031, Off);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_059, On);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.ADVANCEDSETTING_060, Settings);
    this->insertSeparator(2);
}

void SkipblankpagesComb::select_item(void){

    if(current_item){
        this->setCurrentIndex(On);
    }else {
        this->setCurrentIndex(Off);
    }
}

void SkipblankpagesComb::item_event(int value, SCANPARA* skipblankpageslv_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == Off){
        skipblankpageslv_para->BlankPageSkipKey.select = current_item = false;
    }else if(select_item == On){
        skipblankpageslv_para->BlankPageSkipKey.select = current_item = true;
    }else if(select_item == Settings){
        this->item_event_options();
    }
    skipblankpageslv_para->BlankPageSkipLevelKey.select = current_level;

    changed = true;
}

void SkipblankpagesComb::item_event_options(void){

    //Initialize dialog
    if(!skipblankpagessettings_dialog){
        skipblankpagessettings_dialog = new SkipBlankPagesSettingsDialog(current_level);
        if(skipblankpagessettings_dialog == NULL){
            return;
        }
    }
    if(skipblankpagessettings_dialog->isHidden()){
        skipblankpagessettings_dialog->exec();
    }else {
        skipblankpagessettings_dialog->activateWindow();
    }

    skipblankpagessettings_dialog->SetValue();
    skipblankpagessettings_dialog->GetValue(&current_level);

    //this->setCurrentIndex(this->findData(On));
}
