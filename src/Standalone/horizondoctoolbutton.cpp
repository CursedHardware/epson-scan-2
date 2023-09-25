#include "horizondoctoolbutton.h"

HorizonDoctoolButton::HorizonDoctoolButton(QWidget *parent) :
    QToolButton(parent)
{

}

void HorizonDoctoolButton::button_event(bool checked)
{
    QObject* obj = sender();

    if(obj == this){
        if(!checked){
            this->setChecked(true);
        }
    }else {
        if(checked){
            this->setChecked(false);
        }
    }
}
