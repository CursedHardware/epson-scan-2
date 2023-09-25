#include "documentsourcesettingsdialog.h"
#include "ui_documentsourcesettingsdialog.h"

DocumentSourceSettingsDialog::DocumentSourceSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DocumentSourceSettingsDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
}

DocumentSourceSettingsDialog::~DocumentSourceSettingsDialog()
{
    delete ui;
}

void DocumentSourceSettingsDialog::Initialize(INT_SETTING_VALUE afm_para)
{
    ui->Afm_Timeout_comboBox->initialize(afm_para);
}

void DocumentSourceSettingsDialog::onValueChanged(int value)
{

    QObject* obj = sender();

    if(obj == ui->Afm_Timeout_comboBox){
        ui->Afm_Timeout_comboBox->item_event(value);
    }
    ui->Afm_Timeout_comboBox->update_ui();
}

int DocumentSourceSettingsDialog::GetCurrentValue()
{
    return ui->Afm_Timeout_comboBox->current_item;
}
