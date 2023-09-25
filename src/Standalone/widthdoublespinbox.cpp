#include "mainwindow.h"

WidthDoubleSpinBox::WidthDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    current_value_inchi(0),
    current_value_mm(0),
    base_value(0)
{
}

void WidthDoubleSpinBox::initialize(SCAN_AREA_VALUE default_value)
{
    this->SetSpinBoxParam(default_value);

    if(default_value.UserDefine_Width != 0){
        this->setValue(default_value.UserDefine_Width);
        this->CreateValueToBase();
    }else {
        base_value = 200;
        this->CreateBaseToValue();
    }
    if(this->isUnit_inchi()){
        this->setValue(CreateDisplayValue(current_value_inchi));
    }else {
        this->setValue(CreateDisplayValue(current_value_mm));
    }
}

void WidthDoubleSpinBox::Finalize(void)
{
    this->CreateValueToBase();
    this->CreateBaseToValue();
}

void WidthDoubleSpinBox::SetSpinBoxParam(SCAN_AREA_VALUE default_value)
{
    double max_width_size = default_value.MaxScanAreaWidth.select;
    double min_width_size;
    if(default_value.FBWidthMin == 0){
        min_width_size = default_value.ADFWidthMin.select;
    }else {
        min_width_size = default_value.FBWidthMin;
    }

    if(this->isUnit_inchi()){
        this->setDecimals(2);

        max_width_size = max_width_size / 100;
        min_width_size = min_width_size / 100;

        this->setMaximum(max_width_size);
        this->setMinimum(min_width_size);
    }else {
        this->setDecimals(1);

        max_width_size = max_width_size * 25.4 / 100;
        min_width_size = min_width_size * 25.4 / 100;

        this->setMaximum(max_width_size);
        this->setMinimum(min_width_size);
    }
}

void WidthDoubleSpinBox::CreateBaseToValue(void)
{
    current_value_inchi = base_value / 100;
    current_value_mm = base_value * 25.4 / 100;
}

void WidthDoubleSpinBox::CreateValueToBase(void)
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

double WidthDoubleSpinBox::CreateDisplayValue(double value)
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

void WidthDoubleSpinBox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

bool WidthDoubleSpinBox::isUnit_inchi(void)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(units == unit_inchi){
        return true;
    }else if(units == unit_mm){
        return false;
    }
}
