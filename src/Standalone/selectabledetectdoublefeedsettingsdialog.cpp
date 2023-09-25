#include "selectabledetectdoublefeedsettingsdialog.h"
#include "ui_selectabledetectdoublefeedsettingsdialog.h"
#include "configuration.h"
#include "mainwindow.h"
#include <QMessageBox>

SelectableDetectDoubleFeedSettingsDialog::SelectableDetectDoubleFeedSettingsDialog(QWidget *parent) :
    QDialog(parent),
    det_double_feed_ui(new Ui::SelectableDetectDoubleFeedSettingsDialog),
    m_ultrasonic_sensor(false),
    detection_area_min(0),
    detection_area_max(0),
    paper_thickness(DoubleFeedDetectionLow),
    m_scanned_image(false),
    m_paper_protection(false)
{
    memset(&DoubleFeedDetectionLevel_item, 0, sizeof(INT_SETTING_VALUE));
    det_double_feed_ui->setupUi(this);
    this->setFixedSize(this->size());
}

SelectableDetectDoubleFeedSettingsDialog::~SelectableDetectDoubleFeedSettingsDialog()
{
    delete det_double_feed_ui;
}

void SelectableDetectDoubleFeedSettingsDialog::Initialize(INT_SETTING_VALUE DoubleFeedDetectionLevel)
{
    this->select_units();
    //det_double_feed_ui->Min_doubleSpinBox->initialize();
    //det_double_feed_ui->Max_doubleSpinBox->initialize();
    det_double_feed_ui->Paper_Thickness_comboBox->initialize(DoubleFeedDetectionLow);
    DoubleFeedDetectionLevel_item = DoubleFeedDetectionLevel;

    this->updateUI();
}

void SelectableDetectDoubleFeedSettingsDialog::Finalize()
{
    det_double_feed_ui->Min_doubleSpinBox->Finalize();
    det_double_feed_ui->Max_doubleSpinBox->Finalize();
}

void SelectableDetectDoubleFeedSettingsDialog::SetValue()
{
    det_double_feed_ui->Min_doubleSpinBox->CreateBaseToValue();
    if(det_double_feed_ui->Min_doubleSpinBox->isUnit_inchi()){
        detection_area_min = det_double_feed_ui->Min_doubleSpinBox->CreateDisplayValue(det_double_feed_ui->Min_doubleSpinBox->current_value_inchi);
    } else {
        detection_area_min = det_double_feed_ui->Min_doubleSpinBox->CreateDisplayValue(det_double_feed_ui->Min_doubleSpinBox->current_value_mm);
    }

    det_double_feed_ui->Max_doubleSpinBox->CreateBaseToValue();
    if(det_double_feed_ui->Max_doubleSpinBox->isUnit_inchi()){
        detection_area_max = det_double_feed_ui->Max_doubleSpinBox->CreateDisplayValue(det_double_feed_ui->Max_doubleSpinBox->current_value_inchi);
    } else {
        detection_area_max = det_double_feed_ui->Max_doubleSpinBox->CreateDisplayValue(det_double_feed_ui->Max_doubleSpinBox->current_value_mm);
    }

    paper_thickness = det_double_feed_ui->Paper_Thickness_comboBox->current_item;
}

void SelectableDetectDoubleFeedSettingsDialog::GetValue(double *min, double *max, SDIInt *pt){

    //m_ultrasonic_sensor
    *min = detection_area_min;
    *max = detection_area_max;
    *pt = paper_thickness;
    //m_scanned_image
    //m_paper_protection
}

void SelectableDetectDoubleFeedSettingsDialog::onValueChanged(int value)
{

    QObject* obj = sender();

    if(obj == det_double_feed_ui->Paper_Thickness_comboBox){
        det_double_feed_ui->Paper_Thickness_comboBox->item_event(value);
    }
    this->updateUI();
}

void SelectableDetectDoubleFeedSettingsDialog::updateUI()
{
    det_double_feed_ui->Paper_Thickness_comboBox->update_ui(DoubleFeedDetectionLevel_item);
}

void SelectableDetectDoubleFeedSettingsDialog::select_units(void)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(units == unit_inchi){
        det_double_feed_ui->Det_Area_stackedWidget->setCurrentIndex(1);
    }else if(units == unit_mm){
        det_double_feed_ui->Det_Area_stackedWidget->setCurrentIndex(0);
    }
}

void SelectableDetectDoubleFeedSettingsDialog::WarningMessageBox(void)
{
    QMessageBox msgBox(this);
    msgBox.setText("When enabled, feeding may pause before outputting the\n"
                      "originals and scanning may be slower, depending on the\n"
                      "document type.\n"); // no ID -> TBD
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.VERSION_003);

    msgBox.setIcon(QMessageBox::Warning);

    msgBox.exec();
}

void SelectableDetectDoubleFeedSettingsDialog::onValueChangedBool(bool check)
{
    QObject* obj = sender();

    if(obj == det_double_feed_ui->Ultrasonic_sensor_checkBox){
        m_ultrasonic_sensor = check;
        det_double_feed_ui->Paper_Thickness_label->setEnabled(check);
        det_double_feed_ui->Paper_Thickness_comboBox->setEnabled(check);
        det_double_feed_ui->Det_Area_stackedWidget->setEnabled(check);
        det_double_feed_ui->Detection_Area_label->setEnabled(check);
        det_double_feed_ui->Min_doubleSpinBox->setEnabled(check);
        det_double_feed_ui->Max_doubleSpinBox->setEnabled(check);
    }else if(obj == det_double_feed_ui->Scanned_image_checkBox){
        m_scanned_image = check;
        if(check){
            this->WarningMessageBox();
        }
    }else if(obj == det_double_feed_ui->Paper_Protection_checkBox){
        m_paper_protection = check;
    }
}
