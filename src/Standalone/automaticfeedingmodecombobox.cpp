#include "mainwindow.h"

AutomaticFeedingModeCombobox::AutomaticFeedingModeCombobox(QWidget *parent) :
    QComboBox(parent)
{
    changed = true;
    current_item = false;
}

void AutomaticFeedingModeCombobox::initialize(INT_SETTING_VALUE afm_para, INT_SETTING_VALUE cafm_para)
{
    current_item = (bool)afm_para.select;
    supported_cafm = cafm_para.select;
}

void AutomaticFeedingModeCombobox::update_ui(INT_SETTING_VALUE afm_para, bool use_continuousAutoFeedingMode)
{
    if(changed == true){
        this->clear();
        if(afm_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->add_item();
            this->select_item(afm_para, use_continuousAutoFeedingMode);
            this->set_enabled(true);
            this->show();
        }else if(afm_para.capability.supportLevel == kSDISupportLevelUnavailable){
            this->add_item();
            this->select_item(afm_para, use_continuousAutoFeedingMode);
            this->set_enabled(false);
            this->show();
        }else if(afm_para.capability.supportLevel == kSDISupportLevelNone){
            this->select_item(afm_para, use_continuousAutoFeedingMode);
            this->hide();
        }
        changed = false;
    }
}

void AutomaticFeedingModeCombobox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void AutomaticFeedingModeCombobox::add_item()
{
    int i = 0;
    this->insertItem(i, TranslationString::GetString().translation_str.AUTOFEEDMODE_005, AFM_Off);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.AUTOFEEDMODE_006, AFM_On);
    i++;
    //連続AFMサポート時
    if(supported_cafm){
        this->insertItem(i, TranslationString::GetString().translation_str.AUTOFEEDMODE_007, AFMC_On);
        i++;
    }
}

void AutomaticFeedingModeCombobox::select_item(INT_SETTING_VALUE afm_para, bool use_continuousAutoFeedingMode)
{
    if(use_continuousAutoFeedingMode){
        this->setCurrentIndex(AFMC_On);
    }else if(afm_para.select){
        this->setCurrentIndex(AFM_On);
    }else {
        this->setCurrentIndex(AFM_Off);
    }
}

void AutomaticFeedingModeCombobox::item_event(int value, INT_SETTING_VALUE* afm_para, bool* use_continuousAutoFeedingMode)
{
    QVariant select_item = this->itemData(value);

    if(select_item == AFM_Off){
        afm_para->select = current_item = false;
        *use_continuousAutoFeedingMode = false;
    }else if(select_item == AFM_On){
        afm_para->select = current_item = true;
        *use_continuousAutoFeedingMode = false;
    }else if(select_item == AFMC_On){
        afm_para->select = current_item = true;
        *use_continuousAutoFeedingMode = true;
    }
    changed = true;
}
