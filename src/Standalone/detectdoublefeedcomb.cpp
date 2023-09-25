#include "mainwindow.h"

DetectDoubleFeedComb::DetectDoubleFeedComb(QWidget *parent) :
    QComboBox(parent)
{
    detectdoublefeedsettings_dialog = NULL;
    selectdetectdoublefeedsettings_dialog = NULL;
    changed = false;
    min_value = 0;
    max_value = 0;
    memset(&pt_value, 0, sizeof(INT_SETTING_VALUE));
}

void DetectDoubleFeedComb::initialize(INT_SETTING_VALUE detectdoublefeedlevel_para){
    changed = true;
    min_value = 0;
    max_value = 15.5;
    pt_value = detectdoublefeedlevel_para;
}

void DetectDoubleFeedComb::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void DetectDoubleFeedComb::update_ui(DOUBLE_FEED_DETECTION_VALUE detectdoublefeed_para)
{
    if(detectdoublefeed_para.DoubleFeedDetection.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        changed = true;
        this->show();
    }else if(detectdoublefeed_para.DoubleFeedDetection.capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        detectdoublefeed_para.DoubleFeedDetection.select = false;
        changed = true;
        this->show();
    }else if(detectdoublefeed_para.DoubleFeedDetection.capability.supportLevel == kSDISupportLevelNone){
        this->hide();
    }

    if(changed == true){
        this->clear();
        this->add_item(detectdoublefeed_para.DoubleFeedDetectionAreaMin.capability);
        this->select_item(detectdoublefeed_para.DoubleFeedDetection);
        changed = false;
    }
}

void DetectDoubleFeedComb::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void DetectDoubleFeedComb::add_item(SDICapability capability)
{
    int i = 0;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_004, Off);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_005, On);
    i++;
    if(capability.supportLevel != kSDISupportLevelNone){
        this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_010, Settings);
        this->insertSeparator(2);
    }
}

void DetectDoubleFeedComb::select_item(INT_SETTING_VALUE detectdoublefeed_para)
{
    if(detectdoublefeed_para.select){
        this->setCurrentIndex(On);
    }else {
        this->setCurrentIndex(Off);
    }
}

void DetectDoubleFeedComb::item_event(int value, DOUBLE_FEED_DETECTION_VALUE* detectdoublefeed_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == Off){
        detectdoublefeed_para->DoubleFeedDetection.select = false;
    }else if(select_item == On){
        detectdoublefeed_para->DoubleFeedDetection.select = true;
        detectdoublefeed_para->DoubleFeedDetectionLevel.select = pt_value.select;
    }else if(select_item == Settings){
        this->item_event_options(detectdoublefeed_para);
    }
    changed = true;
}

void DetectDoubleFeedComb::item_event_options(DOUBLE_FEED_DETECTION_VALUE* detectdoublefeed_para)
{
    //Initialize dialog
    if(!detectdoublefeedsettings_dialog){
        detectdoublefeedsettings_dialog = new DetectDoubleFeedSettingsDialog;
    }

    detectdoublefeedsettings_dialog->Initialize(*detectdoublefeed_para);

    if(detectdoublefeedsettings_dialog->isHidden()){
        detectdoublefeedsettings_dialog->exec();
    }else {
        detectdoublefeedsettings_dialog->activateWindow();
    }
    detectdoublefeedsettings_dialog->Finalize();

    detectdoublefeedsettings_dialog->SetValue();
    detectdoublefeedsettings_dialog->GetValue(&min_value, &max_value, &pt_value.select);

    detectdoublefeed_para->DoubleFeedDetectionLevel.select = pt_value.select;
    detectdoublefeed_para->DoubleFeedDetectionAreaMin.select = min_value;
    detectdoublefeed_para->DoubleFeedDetectionAreaLength.select = max_value;
}

