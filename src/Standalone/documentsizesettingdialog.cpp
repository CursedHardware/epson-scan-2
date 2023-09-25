#include "documentsizesettingdialog.h"
#include "ui_documentsizesettingdialog.h"

DocumentSizeSettingDialog::DocumentSizeSettingDialog(INT_SETTING_VALUE* inscribed_para, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DocumentSizeSettingDialog)
{
    m_inscribed_para = inscribed_para;
    if(inscribed_para->select == 0)
    {
        m_inscribed_para->select = false;
    } else {
        m_inscribed_para->select = true;
    }

    ui->setupUi(this);
}

DocumentSizeSettingDialog::~DocumentSizeSettingDialog()
{
    delete ui;
}

void DocumentSizeSettingDialog::onValueChangedBool(bool value)
{
    QObject* obj = sender();

    if(obj == ui->Black_edge_inscribed_checkBox){
        // m_inscribed_para->select = ui->Black_edge_inscribed_checkBox->isChecked();
        if(ui->Black_edge_inscribed_checkBox->isChecked())
        {
            m_inscribed_para->select = 1;
        } else {
            m_inscribed_para->select = 0;
        }
    }

    //this->updateUI();
}

/*
void DocumentSizeSettingDialog::SetValue(){
    m_inscribed_para = ui->Black_edge_inscribed_checkBox->isChecked();
}
*/
/*
void DocumentSizeSettingDialog::GetValue(SCANPARA* device_data){

    if(m_inscribed_para == true)
    {
        device_data->AutoCroppingInscribed.select = 0;
    } else {
        device_data->AutoCroppingInscribed.select = 1;
    }
}
*/

void DocumentSizeSettingDialog::Initialize(INT_SETTING_VALUE* document_size_setting)
{
    //m_inscribed_para->select = *(document_size_setting.select);
    if(document_size_setting->select == 0)
    {
        ui->Black_edge_inscribed_checkBox->setChecked(false);
    }else {
        ui->Black_edge_inscribed_checkBox->setChecked(true);
    }
    ui->Black_edge_inscribed_checkBox->show();
    if(document_size_setting->capability.supportLevel == kSDISupportLevelAvailable)
    {
        ui->Black_edge_inscribed_checkBox->show();
    }else{
        ui->Black_edge_inscribed_checkBox->hide();
    }

}
