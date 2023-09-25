#include "mainwindow.h"

LengthDoubleSpinBox::LengthDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    current_value_inchi(0),
    current_value_mm(0),
    base_value(0)
{
}

void LengthDoubleSpinBox::initialize(SCAN_AREA_VALUE default_value)
{
    this->SetSpinBoxParam(default_value);

    if(default_value.UserDefine_Length != 0){
        this->setValue(default_value.UserDefine_Length);
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

void LengthDoubleSpinBox::Finalize(void)
{
    this->CreateValueToBase();
    this->CreateBaseToValue();
}

void LengthDoubleSpinBox::SetSpinBoxParam(SCAN_AREA_VALUE default_value)
{
    double max_height_size = default_value.MaxScanAreaHeight.select;
    double max_long_height_size = default_value.MaxScanAreaHeightInLongPaper.select;
    double min_height_size;
    if(default_value.FBHeightMin == 0){
        min_height_size = default_value.ADFHeightMin.select;
    }else {
        min_height_size = default_value.FBHeightMin;
    }
    //double min_duplex_height_size = default_value.ADFDuplexHeightMin;

    if(this->isUnit_inchi()){
        this->setDecimals(2);

        if(max_long_height_size == 0){
            max_height_size = max_height_size / 100;
            this->setMaximum(max_height_size);
        }else {
            max_long_height_size = max_long_height_size / 100;
            this->setMaximum(max_long_height_size);
        }

        min_height_size = min_height_size / 100;
        //min_duplex_height_size = min_duplex_height_size / 100;

        this->setMinimum(min_height_size);
    }else { //mm
        this->setDecimals(1);

        if(max_long_height_size == 0){
            max_height_size = max_height_size * 25.4 / 100;
            this->setMaximum(max_height_size);
        }else {
            max_long_height_size = max_long_height_size * 25.4 / 100;
            this->setMaximum(max_long_height_size);
        }

        min_height_size = min_height_size * 25.4 / 100;
        //min_duplex_height_size = min_duplex_height_size * 25.4 / 100;

        this->setMinimum(min_height_size);
    }
}

void LengthDoubleSpinBox::CreateBaseToValue(void)
{
    current_value_inchi = base_value / 100;
    current_value_mm = base_value * 25.4 / 100;
}

void LengthDoubleSpinBox::CreateValueToBase(void)
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

double LengthDoubleSpinBox::CreateDisplayValue(double value)
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


void LengthDoubleSpinBox::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

bool LengthDoubleSpinBox::isUnit_inchi(void)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(units == unit_inchi){
        return true;
    }else if(units == unit_mm){
        return false;
    }
}
