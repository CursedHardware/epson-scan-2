#include "mainwindow.h"

SleepTimerSpinBox::SleepTimerSpinBox(QWidget *parent) :
    QSpinBox(parent),
    changed(false),
    enabled(false),
    current_value(0)
{
}

void SleepTimerSpinBox::initialize(INT_SETTING_VALUE sleep_time_para)
{
    this->setMaximum(sleep_time_para.capability.allMaxValue);
    this->setMinimum(sleep_time_para.capability.allMinValue);

    this->setValue(sleep_time_para.select);

    changed = true;
    enabled = true;
    current_value = sleep_time_para.select;

}

void SleepTimerSpinBox::update_ui(){

    if(changed == true){
        this->set_enabled(enabled);
        changed = false;
    }
}

void SleepTimerSpinBox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void SleepTimerSpinBox::item_event(int value)
{
    current_value = value;
    changed = true;
}

int SleepTimerSpinBox::get_current_item(void)
{
    return current_value;
}

