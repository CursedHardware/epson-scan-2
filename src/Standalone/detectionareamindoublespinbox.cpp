#include "mainwindow.h"

DetectionAreaMinDoubleSpinBox::DetectionAreaMinDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    base_value(0),
    current_value_inchi(0),
    current_value_mm(0)
{
    this->CreateBaseToValue();
}

void DetectionAreaMinDoubleSpinBox::initialize(SDIInt default_value)
{
    base_value = default_value;
    this->CreateBaseToValue();

    this->SetSpinBoxParam();

    if(this->isUnit_inchi()){
        this->setValue(CreateDisplayValue(current_value_inchi));
    }else {
        this->setValue(CreateDisplayValue(current_value_mm));
    }
}

void DetectionAreaMinDoubleSpinBox::Finalize(void)
{
    this->CreateValueToBase();
    this->CreateBaseToValue();
}

void DetectionAreaMinDoubleSpinBox::SetSpinBoxParam(void)
{
    if(this->isUnit_inchi()){
        this->setDecimals(1);
        this->setSingleStep(0.1);

        this->setMaximum(14.5);
        this->setMinimum(0.0);
    }else {
        this->setDecimals(0);
        this->setSingleStep(1);

        this->setMaximum(367);
        this->setMinimum(0);
    }
}

void DetectionAreaMinDoubleSpinBox::CreateBaseToValue(void)
{
    current_value_inchi = base_value / 100;
    current_value_mm = base_value * 25.4 / 100;
}

void DetectionAreaMinDoubleSpinBox::CreateValueToBase(void)
{
    if(this->isUnit_inchi()){
        if(CreateDisplayValue(current_value_inchi) != CreateDisplayValue(this->value())){
            current_value_inchi = this->value();
            base_value =  (current_value_inchi * 100) + 0.005;
        }
    }else { //mm
        if(CreateDisplayValue(current_value_mm) != CreateDisplayValue(this->value())){
            current_value_mm = this->value();
            base_value =  (current_value_mm * 100 / 25.4) + 0.005;
        }
    }
}

double DetectionAreaMinDoubleSpinBox::CreateDisplayValue(double value)
{
    int magnification;
    int temp;
    if(this->isUnit_inchi()){
        magnification = 100;
        temp = value * magnification;
    }else {
        magnification = 10;
        temp = value * magnification;
    }
    return (double)temp / (double)magnification;
}

void DetectionAreaMinDoubleSpinBox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

bool DetectionAreaMinDoubleSpinBox::isUnit_inchi(void)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(units == unit_inchi){
        return true;
    }else if(units == unit_mm){
        return false;
    }
}

void DetectionAreaMinDoubleSpinBox::setMaxValue(double value)
{
    double dif = 0;
    if(this->isUnit_inchi()){
        dif = 1.0;
    }else { //mm
        dif = 26;
    }
    if(value - dif < 0){
        this->setMaximum(0);
    }else {
        this->setMaximum(value - dif);
    }
}
