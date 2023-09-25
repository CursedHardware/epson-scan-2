#include "verticaldoctoolbutton.h"

VerticalDoctoolButton::VerticalDoctoolButton(QWidget *parent) :
    QToolButton(parent)
{

}

void VerticalDoctoolButton::button_event(bool checked)
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
