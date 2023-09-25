#include "focuspositionsettingdialog.h"
#include "ui_focuspositionsettingdialog.h"

FocusPositionSettingDialog::FocusPositionSettingDialog(SCANPARA* device_data, QWidget *parent) :
    QDialog(parent),
    focus_position_ui(new Ui::FocusPositionSettingDialog)
{
    focus_position_ui->setupUi(this);
    this->setFixedSize(this->size());

    m_focusPositionLevel = device_data->focusPosition.select;

    focus_position_ui->FocusPosition_slider->initialize(m_focusPositionLevel);

    //Update Dialog
    this->updateUI();
}

FocusPositionSettingDialog::~FocusPositionSettingDialog()
{
    delete focus_position_ui;
}

void FocusPositionSettingDialog::updateUI()
{
    focus_position_ui->FocusPosition_slider->update_ui();
}

void FocusPositionSettingDialog::onValueChanged(int value)
{
    this->updateUI();
}

void FocusPositionSettingDialog::SetValue()
{
    m_focusPositionLevel = focus_position_ui->FocusPosition_slider->value();
}

void FocusPositionSettingDialog::GetValue(SCANPARA* device_data)
{
    device_data->focusPosition.select = m_focusPositionLevel;
}

void FocusPositionSettingDialog::PushButtonAction(void)
{
    QObject* obj = sender();

    if(obj == focus_position_ui->OK_pushButton){
        m_focusPositionLevel = focus_position_ui->FocusPosition_slider->value();
        this->reject();
    }else if(obj == focus_position_ui->Cancel_pushButton){
        this->reject();
    }
}
