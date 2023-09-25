#include "textenhancesettingdialog.h"
#include "ui_textenhancesettingdialog.h"
#include "mainwindow.h"

TextEnhanceSettingDialog::TextEnhanceSettingDialog(SCANPARA device_data, QWidget *parent) :
    QDialog(parent),
    text_enhance_ui(new Ui::TextEnhanceSettingDialog)
{
    text_enhance_ui->setupUi(this);
    this->setFixedSize(this->size());

    m_textEnhanceSensitivity = device_data.textEnhanceSensitivity.select;
    m_thresholdAdjustment = device_data.thresholdAdjustment.select;
    m_textEnhanceNoiseReductionLevel = device_data.textEnhanceNoiseReductionLevel.select;
    if(device_data.textEnhancePaperCreaseReduction.select == 0){
        m_textEnhancePaperCreaseReduction = false;
    }else {
        m_textEnhancePaperCreaseReduction = true;
    }

    //Update Dialog
    this->updateUI();
}

TextEnhanceSettingDialog::~TextEnhanceSettingDialog()
{
    delete text_enhance_ui;
}

void TextEnhanceSettingDialog::SetValue(){
    m_textEnhanceSensitivity = text_enhance_ui->textEnhanceSensitivity_hSlider->value();
    m_thresholdAdjustment = text_enhance_ui->thresholdAdjustment_hSlider->value();
    m_textEnhanceNoiseReductionLevel = text_enhance_ui->textEnhanceNoiseReductionLevel_hSlider->value();
    m_textEnhancePaperCreaseReduction = text_enhance_ui->textEnhancePaperCreaseReduction_checkBox->isChecked();
}

void TextEnhanceSettingDialog::GetValue(SCANPARA* device_data){

    device_data->textEnhanceSensitivity.select = m_textEnhanceSensitivity;
    device_data->thresholdAdjustment.select = m_thresholdAdjustment;
    device_data->textEnhanceNoiseReductionLevel.select = m_textEnhanceNoiseReductionLevel;
    device_data->textEnhancePaperCreaseReduction.select = m_textEnhancePaperCreaseReduction;
}

void TextEnhanceSettingDialog::onValueChanged(int value)
{

    QObject* obj = sender();

    if(obj == text_enhance_ui->textEnhanceNoiseReductionLevel_hSlider){
        text_enhance_ui->textEnhanceNoiseReductionLevel_hSlider->setValue(value);
    }else if(obj == text_enhance_ui->textEnhanceSensitivity_hSlider){
        text_enhance_ui->textEnhanceSensitivity_hSlider->setValue(value);
    }else if(obj == text_enhance_ui->thresholdAdjustment_hSlider){
        text_enhance_ui->thresholdAdjustment_hSlider->setValue(value);
    }

    //this->updateUI();
}

void TextEnhanceSettingDialog::updateUI(){
    text_enhance_ui->textEnhanceSensitivity_hSlider->setValue(m_textEnhanceSensitivity);
    text_enhance_ui->thresholdAdjustment_hSlider->setValue(m_thresholdAdjustment);
    text_enhance_ui->textEnhanceNoiseReductionLevel_hSlider->setValue(m_textEnhanceNoiseReductionLevel);
    text_enhance_ui->textEnhancePaperCreaseReduction_checkBox->setChecked(m_textEnhancePaperCreaseReduction);
}
