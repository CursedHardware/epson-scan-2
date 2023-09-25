#include "skipblankpagessettingsdialog.h"
#include "ui_skipblankpagessettingsdialog.h"
#include "mainwindow.h"

SkipBlankPagesSettingsDialog::SkipBlankPagesSettingsDialog(int default_lv, QWidget *parent) :
    QDialog(parent),
    det_skip_ui(new Ui::SkipBlankPagesSettingsDialog)
{
    detection_level = 0;
    det_skip_ui->setupUi(this);
    this->setFixedSize(this->size());

    det_skip_ui->Det_Lv_horizontalSlider->setValue(default_lv);
    det_skip_ui->Description_textBrowser->setText(TranslationString::GetString().translation_str.ADVANCEDSETTING_061);

    //Update Dialog
    this->updateUI();
}

SkipBlankPagesSettingsDialog::~SkipBlankPagesSettingsDialog()
{
    delete det_skip_ui;
}

void SkipBlankPagesSettingsDialog::SetValue(){
    detection_level = det_skip_ui->Det_Lv_horizontalSlider->current_value;
}

void SkipBlankPagesSettingsDialog::GetValue(int *level){

    *level = detection_level;
}

void SkipBlankPagesSettingsDialog::onValueChanged(int value)
{

    QObject* obj = sender();

    if(obj == det_skip_ui->Det_Lv_horizontalSlider){
        det_skip_ui->Det_Lv_horizontalSlider->item_event(value);
    }

    this->updateUI();
}

void SkipBlankPagesSettingsDialog::updateUI(){

    det_skip_ui->Det_Lv_horizontalSlider->update_ui();
}
