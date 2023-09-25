#include "documentsizedialog.h"
#include "ui_documentsizedialog.h"
#include "configuration.h"
#include <QMessageBox>

DocumentSizeDialog::DocumentSizeDialog(QWidget *parent) :
    QDialog(parent),
    d_size_ui(new Ui::DocumentSizeDialog),
    width(0),
    length(0),
    det_paper_length(false)
{
    d_size_ui->setupUi(this);
    this->setFixedSize(this->size());
}

DocumentSizeDialog::~DocumentSizeDialog()
{
    delete d_size_ui;
}

void DocumentSizeDialog::Initialize(SCAN_AREA_VALUE document_size_para, ScannerKind kind)
{
    this->select_units();
    det_paper_length = document_size_para.PaperEndDetectionMemory;
    d_size_ui->Det_Paper_checkBox->setChecked(det_paper_length);
    if(kind != KindSFDocument){
        d_size_ui->Det_Paper_checkBox->hide();
    }else if(document_size_para.PaperEndDetection.capability.supportLevel == kSDISupportLevelAvailable){
        d_size_ui->Det_Paper_checkBox->show();
        d_size_ui->Det_Paper_checkBox->setEnabled(true);
    }else if(document_size_para.PaperEndDetection.capability.supportLevel == kSDISupportLevelUnavailable){
        d_size_ui->Det_Paper_checkBox->show();
        d_size_ui->Det_Paper_checkBox->setEnabled(false);
    }else if(document_size_para.PaperEndDetection.capability.supportLevel == kSDISupportLevelNone){
        d_size_ui->Det_Paper_checkBox->hide();
    }
    d_size_ui->Width_doubleSpinBox->initialize(document_size_para);
    d_size_ui->Length_doubleSpinBox->initialize(document_size_para);

    this->ChangeDetPaperCheck(det_paper_length);
}

void DocumentSizeDialog::Finalize()
{
    d_size_ui->Width_doubleSpinBox->Finalize();
    d_size_ui->Length_doubleSpinBox->Finalize();
}

void DocumentSizeDialog::SetSize()
{
    d_size_ui->Width_doubleSpinBox->CreateBaseToValue();
    if(d_size_ui->Width_doubleSpinBox->isUnit_inchi()){
        width = d_size_ui->Width_doubleSpinBox->CreateDisplayValue(d_size_ui->Width_doubleSpinBox->current_value_inchi);
    } else {
        width = d_size_ui->Width_doubleSpinBox->CreateDisplayValue(d_size_ui->Width_doubleSpinBox->current_value_mm);
    }

    d_size_ui->Length_doubleSpinBox->CreateBaseToValue();
    if(d_size_ui->Width_doubleSpinBox->isUnit_inchi()){
        length = d_size_ui->Length_doubleSpinBox->CreateDisplayValue(d_size_ui->Length_doubleSpinBox->current_value_inchi);
    } else {
        length = d_size_ui->Length_doubleSpinBox->CreateDisplayValue(d_size_ui->Length_doubleSpinBox->current_value_mm);
    }

    det_paper_length = d_size_ui->Det_Paper_checkBox->current_item;
}

void DocumentSizeDialog::GetSize(double *user_width, double *user_length, bool *user_det_paper_len){

    *user_det_paper_len = det_paper_length;

    *user_width = width;
    *user_length = length;
}

void DocumentSizeDialog::onValueChangedBool(bool value)
{
    QObject* obj = sender();

    if(obj == d_size_ui->Det_Paper_checkBox){
        d_size_ui->Det_Paper_checkBox->item_event(value);
        this->ChangeDetPaperCheck(value);

        d_size_ui->Det_Paper_checkBox->update_ui();
    }
}

void DocumentSizeDialog::ChangeDetPaperCheck(bool checked)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(checked == true){
        d_size_ui->Length_stackedWidget->setCurrentIndex(1);
        if(units == unit_inchi){
            d_size_ui->inchi_stackedWidget->setCurrentIndex(1);
        }else {
            d_size_ui->mm_stackedWidget->setCurrentIndex(1);
        }
    }else {
        d_size_ui->Length_stackedWidget->setCurrentIndex(0);
        if(units == unit_inchi){
            d_size_ui->inchi_stackedWidget->setCurrentIndex(0);
        }else {
            d_size_ui->mm_stackedWidget->setCurrentIndex(0);
        }
    }
}

void DocumentSizeDialog::select_units(void)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(units == unit_inchi){
        d_size_ui->units_stackedWidget->setCurrentIndex(0);
    }else if(units == unit_mm){
        d_size_ui->units_stackedWidget->setCurrentIndex(1);
    }
}
