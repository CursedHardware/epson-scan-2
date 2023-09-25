#include "configurationdialog.h"
#include "configuration.h"
#include "ui_configurationdialog.h"
#include "administratorpasswordinputdialog.h"
#include "administratorpasswordinputforuksecuritydialog.h"
#include <QMessageBox>

#include "lastusedsettings.h"
#include "mainwindow.h"

#include "PathUtils.h"
#include "../Include/Config.h"

#define NEAREND 2000

ConfigurationDialog::ConfigurationDialog(Supervisor* sv, QWidget *parent) :
    QDialog(parent),
    config_ui(new Ui::ConfigurationDialog)
{
    m_sv = sv;

    config_ui->setupUi(this);
    this->setFixedSize(this->size());

    //Update Dialog
    //this->updateUI(*m_device_data);
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete config_ui;
}

void ConfigurationDialog::onValueChanged(int value)
{
    CONFIGURATION config_val;
    Configuration& config = Configuration::GetConfiguration();
    config_val = config.config_para;

    QObject* obj = sender();

    if(obj == config_ui->Units_comboBox){
        config_ui->Units_comboBox->item_event(value);
        config_val.units = config_ui->Units_comboBox->get_current_item();

    }else if(obj == config_ui->Sleep_Timer_spinBox){
        config_ui->Sleep_Timer_spinBox->item_event(value);
        config_val.sleep_timer = config_ui->Sleep_Timer_spinBox->get_current_item();

    }else if(obj == config_ui->AC_Power_comboBox){
        config_ui->AC_Power_comboBox->item_event(value);
        config_val.ac_power = config_ui->AC_Power_comboBox->get_current_item();

    }else if(obj == config_ui->Battery_Power_comboBox){
        config_ui->Battery_Power_comboBox->item_event(value);
        config_val.usb_power = config_ui->Battery_Power_comboBox->get_current_item();

    }else if(obj == config_ui->Detect_Glass_Dirt_comboBox){
        config_ui->Detect_Glass_Dirt_comboBox->item_event(value);
        config_val.sensor_glass_dirtsensitivity = config_ui->Detect_Glass_Dirt_comboBox->get_current_item();

    }else if(obj == config_ui->Paper_Protection_comboBox){
        config_ui->Paper_Protection_comboBox->item_event(value);
        config_val.adf_paper_protection = config_ui->Paper_Protection_comboBox->get_current_item();

    }else if(obj == config_ui->Double_Feed_Stop_Timing_comboBox){
        config_ui->Double_Feed_Stop_Timing_comboBox->item_event(value);
        config_val.behavior_when_doublefeed = config_ui->Double_Feed_Stop_Timing_comboBox->get_current_item();

    }else if(obj == config_ui->Power_Off_If_Disconnected_comboBox){
        config_ui->Power_Off_If_Disconnected_comboBox->item_event(value);
        config_val.non_connect_poweroff = config_ui->Power_Off_If_Disconnected_comboBox->get_current_item();

    }else if(obj == config_ui->Direct_Power_On_comboBox){
        config_ui->Direct_Power_On_comboBox->item_event(value);
        config_val.direct_power_on = config_ui->Direct_Power_On_comboBox->get_current_item();

    }else if(obj == config_ui->Count_Alert_Setting_num_spinBox){
        config_val.roller_kit_notify_counter = value / 1000;

    }else if(obj == config_ui->Count_Alert_Setting_spinBox){
        config_val.cleaning_warning_notify_counter = value / 1000;

    }
    config.SetConfiguration(config_val);
    this->updateUI(m_sv->device_data);
}

void ConfigurationDialog::onValueChangedBool(bool value)
{
    CONFIGURATION config_val;
    Configuration& config = Configuration::GetConfiguration();
    config_val = config.config_para;

    QObject* obj = sender();

    if(obj == config_ui->Last_Used_checkBox){
        config_val.last_used_settings = config_ui->Last_Used_checkBox->item_event(value);
    }else if(obj == config_ui->Transfer_data_after_comp_checkBox){
        config_val.compression = config_ui->Transfer_data_after_comp_checkBox->item_event(value);
    }else if(obj == config_ui->Open_Img_Folder_after_scan_checkBox){
        config_val.open_img_folder = config_ui->Open_Img_Folder_after_scan_checkBox->item_event(value);
    }else if(obj == config_ui->Quality_Preview_checkBox){
        config_val.quality_preview = config_ui->Quality_Preview_checkBox->item_event(value);
        if(config_val.quality_preview){
            config_val.preview_resolution = 150;
        }else {
            config_val.preview_resolution = 50;
        }
        config.SetConfiguration(config_val);
    }else if(obj == config_ui->Regular_Cleaning_Alert_checkBox){
        config_ui->Number_of_Scans_label->setEnabled(value);
        config_ui->Number_of_Scans_colon_label->setEnabled(value);
        config_ui->Number_of_Scans_num_label->setEnabled(value);
        config_ui->Maintenance_Reset_pushButton->setEnabled(value);

        config_ui->Count_Alert_Setting_label->setEnabled(value);
        config_ui->Count_Alert_Setting_colon_label->setEnabled(value);
        config_ui->Count_Alert_Setting_spinBox->setEnabled(value);

        config_val.cleaning_warning_notify = value;
    }

    config.SetConfiguration(config_val);
    this->updateUI(m_sv->device_data);
}

void ConfigurationDialog::Initialize()
{
    this->bReset = false;
    this->bCountReset = false;

    if(m_sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        config_ui->Quality_Preview_checkBox->hide();
        config_ui->Preview_label->hide();
        config_ui->Preview_colon_label->hide();
    }else {
        config_ui->Quality_Preview_checkBox->show();
        config_ui->Preview_label->show();
        config_ui->Preview_colon_label->show();
    }

    Configuration& config = Configuration::GetConfiguration();

    config_ui->Units_comboBox->initialize(config.config_para.units);
    config_ui->Last_Used_checkBox->initialize(config.config_para.last_used_settings);
    config_ui->Transfer_data_after_comp_checkBox->initialize(config.config_para.compression);
    config_ui->Open_Img_Folder_after_scan_checkBox->initialize(config.config_para.open_img_folder);
    config_ui->Quality_Preview_checkBox->initialize(config.config_para.quality_preview);
    config_ui->AC_Power_comboBox->initialize((Time)m_sv->device_data.AutoPowerOffTime.select);
    config_ui->Battery_Power_comboBox->initialize((Time)m_sv->device_data.AutoPowerOffTimeBattery.select);
    config_ui->Sleep_Timer_spinBox->initialize(m_sv->device_data.SleepTime);
    config_ui->Detect_Glass_Dirt_comboBox->initialize(config.config_para.sensor_glass_dirtsensitivity);
    config_ui->Paper_Protection_comboBox->initialize(config.config_para.adf_paper_protection);
    config_ui->Double_Feed_Stop_Timing_comboBox->initialize(config.config_para.behavior_when_doublefeed);
    config_ui->Power_Off_If_Disconnected_comboBox->initialize(config.config_para.non_connect_poweroff);
    config_ui->Direct_Power_On_comboBox->initialize(config.config_para.direct_power_on);
    config_ui->Regular_Cleaning_Alert_checkBox->setChecked(config.config_para.cleaning_warning_notify);

    //Initial update counter value
    config.config_para.cleaning_warning_counter = m_sv->device_data.ConfigureCounter.CleaningWarningCounter.select;
    config.config_para.roller_kit_counter = m_sv->device_data.ConfigureCounter.RollerKitCounter.select;
    config.config_para.roller_counter = m_sv->device_data.ConfigureCounter.RollerCounter.select;
    config.config_para.retard_roller_counter = m_sv->device_data.ConfigureCounter.RetardRollerCounter.select;

    this->updateUI(m_sv->device_data);
}

void ConfigurationDialog::updateUI(SCANPARA device_data){

    config_ui->Units_comboBox->update_ui();
    config_ui->Last_Used_checkBox->update_ui();
    config_ui->Transfer_data_after_comp_checkBox->update_ui();
    config_ui->Open_Img_Folder_after_scan_checkBox->update_ui();
    config_ui->Quality_Preview_checkBox->update_ui();
    config_ui->AC_Power_comboBox->update_ui(device_data.AutoPowerOffTime);
    config_ui->Battery_Power_comboBox->update_ui(device_data.AutoPowerOffTimeBattery);
    config_ui->Detect_Glass_Dirt_comboBox->update_ui(device_data.SensorGlassDirtSensitivity);
    config_ui->Paper_Protection_comboBox->update_ui(device_data.ADFPaperProtection);
    config_ui->Double_Feed_Stop_Timing_comboBox->update_ui(device_data.BehaviorWhenDoubleFeed);
    config_ui->Power_Off_If_Disconnected_comboBox->update_ui(device_data.NonConnectPowerOff);
    config_ui->Direct_Power_On_comboBox->update_ui(device_data.DirectPowerOn);
}

void ConfigurationDialog::UpdateInformationDisplay(SCANPARA device_data)
{
    if(device_data.SerialNumber.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Serialno_value_label->setText(device_data.Serial_Number_value.c_str());
    }else {
        config_ui->Serialno_value_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    if(device_data.FirmwareVersion.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Firmware_version_value_label->setText(device_data.Firmware_Version_value.c_str());
    }else {
        config_ui->Firmware_version_value_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }
}

void ConfigurationDialog::UpdateCounterDisplay(CONFIGURE_COUNTER_VALUE ConfigureCounter)
{
    CONFIGURATION config_val;
    Configuration& config = Configuration::GetConfiguration();
    config_val = config.config_para;

    int count_num = ConfigureCounter.ScanCounterKey.select;
    if(ConfigureCounter.ScanCounterKey.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Scanner_Glass_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Scanner_Glass_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.ADFScanCounterKey.select + ConfigureCounter.ADFDuplexScanCounterKey.select;
    if(ConfigureCounter.ADFScanCounterKey.capability.supportLevel != kSDISupportLevelNone){
        config_ui->ADF_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->ADF_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.ADFScanCounterKey.select;
    if(ConfigureCounter.ADFScanCounterKey.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Single_Sided_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Single_Sided_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.ADFDuplexScanCounterKey.select;
    if(ConfigureCounter.ADFDuplexScanCounterKey.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Double_Sided_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Double_Sided_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.ADFCarrierSheetScanCounterKey.select;
    if(ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Carrier_Sheet_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Carrier_Sheet_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.CardScanCounterKey.select;
    if(ConfigureCounter.CardScanCounterKey.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Card_Slot_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Card_Slot_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.PaperJamCounter.select;
    if(ConfigureCounter.PaperJamCounter.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Paper_Jam_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Paper_Jam_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.DoubleFeedCounter.select;
    if(ConfigureCounter.DoubleFeedCounter.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Double_Feed_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Double_Feed_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.RollerKitCounter.select;
    config_val.roller_kit_counter = count_num;
    config_ui->Number_Of_Scans_cnt_label3->setText(std::to_string(count_num).c_str());

    count_num = ConfigureCounter.RollerKitLifeLimit.select;
    if(ConfigureCounter.RollerKitLifeLimit.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Life_Cyde_cnt_label3->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Life_Cyde_cnt_label3->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.RollerCounter.select;
    config_val.roller_counter = count_num;
    config_ui->Number_Of_Scans_cnt_label->setText(std::to_string(count_num).c_str());

    count_num = ConfigureCounter.RollerLifeLimit.select;
    if(ConfigureCounter.RollerLifeLimit.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Life_Cyde_cnt_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Life_Cyde_cnt_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.RetardRollerCounter.select;
    config_val.retard_roller_counter = count_num;
    config_ui->Number_Of_Scans_cnt_label2->setText(std::to_string(count_num).c_str());

    count_num = ConfigureCounter.RetardRollerLifeLimit.select;
    if(ConfigureCounter.RetardRollerLifeLimit.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Life_Cyde_cnt_label2->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Life_Cyde_cnt_label2->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    //通知枚数設定がサポートされている場合は、ラベルではなく操作可能なスピンボックスにする
    if(ConfigureCounter.RollerKitNotifyCount.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Count_Alert_stackedWidget->setCurrentIndex(1);

        count_num = ConfigureCounter.RollerKitNotifyCount.select;
        config_val.roller_kit_notify_counter = count_num;
        config_ui->Count_Alert_Setting_num_spinBox->setMaximum(ConfigureCounter.RollerKitNotifyCount.capability.allMaxValue * 1000);
        config_ui->Count_Alert_Setting_num_spinBox->setMinimum(ConfigureCounter.RollerKitNotifyCount.capability.allMinValue * 1000);
        config_ui->Count_Alert_Setting_num_spinBox->setValue(count_num * 1000);
    }else {
        config_ui->Count_Alert_stackedWidget->setCurrentIndex(0);
    }

    count_num = ConfigureCounter.CleaningWarningNotifyCount.select;
    config_val.cleaning_warning_notify_counter = count_num;
    config_ui->Count_Alert_Setting_spinBox->setMaximum(ConfigureCounter.CleaningWarningNotifyCount.capability.allMaxValue * 1000);
    config_ui->Count_Alert_Setting_spinBox->setMinimum(ConfigureCounter.CleaningWarningNotifyCount.capability.allMinValue * 1000);
    config_ui->Count_Alert_Setting_spinBox->setValue(count_num * 1000);


    count_num = ConfigureCounter.PaperProtectionCounter.select;
    if(ConfigureCounter.PaperProtectionCounter.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Paper_Protection_cnt_num_label->setText(std::to_string(count_num).c_str());
    }else {
        config_ui->Paper_Protection_cnt_num_label->setText(TranslationString::GetString().translation_str.ES2U_MAIN_008);
    }

    count_num = ConfigureCounter.CleaningWarningCounter.select;
    if(ConfigureCounter.CleaningWarningCounter.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Number_of_Scans_num_label->setText(std::to_string(count_num).c_str());
    }

    config.SetConfiguration(config_val);
}

#include "custommessagebox.h"

void ConfigurationDialog::PushButtonAction(void)
{
    QObject* obj = sender();

    if(obj == config_ui->Other_Reset_pushButton){
        //Epson Scan 2をリセットする
        if(this->ResetMessageBox()){
            if(m_sv->Check_SupportAdministratorRestricted()){
                this->Check_AdminPassword();

                if(m_sv->device_data.admin_status == AdminLockOn){
                    return;
                }
            }

            std::string settingDir = (QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData();
#ifdef AKBMODE
            std::string strDst = ".";
            strDst = strDst + DRIVER_NAME;
            strDst = strDst + "/Settings/";
            ES_CMN_FUNCS::PATH::ES_CombinePath(settingDir, settingDir, strDst.c_str());
#else
            ES_CMN_FUNCS::PATH::ES_CombinePath(settingDir, settingDir, ".epsonscan2/Settings/");
#endif

            ES_CMN_FUNCS::PATH::ES_DeleteFolder(settingDir.c_str());
            ES_CMN_FUNCS::PATH::ES_MakeFolder(settingDir.c_str());
            this->bReset = true;
            this->reject();
        }
    }else if(obj == config_ui->Counter_Reset_pushButton){
        //ローラーKitカウンターをリセットする
        if(this->CounterResetMessageBox(TranslationString::GetString().translation_str.C_UNIT_001)){
            CONFIGURATION config_val;
            Configuration& config = Configuration::GetConfiguration();
            config_val = config.config_para;

            config_ui->Number_Of_Scans_cnt_label3->setText("0");

            config_val.roller_kit_counter = 0;
            config.SetConfiguration(config_val);
            this->bCountReset = true;
        }
    }else if(obj == config_ui->Maintenance_Reset_pushButton){
        //カウンターをリセットする
        CONFIGURATION config_val;
        Configuration& config = Configuration::GetConfiguration();
        config_val = config.config_para;

        config_ui->Number_of_Scans_num_label->setText("0");

        config_val.cleaning_warning_counter = 0;
        config.SetConfiguration(config_val);
        this->bCountReset = true;
    }else if(obj == config_ui->Reset_pushButton){
        //ローラーカウンターをリセットする
        if(this->CounterResetMessageBox(TranslationString::GetString().translation_str.C_UNIT_002)){
            CONFIGURATION config_val;
            Configuration& config = Configuration::GetConfiguration();
            config_val = config.config_para;

            config_ui->Number_Of_Scans_cnt_label->setText("0");

            config_val.roller_counter = 0;
            config.SetConfiguration(config_val);
            this->bCountReset = true;
        }
    }else if(obj == config_ui->Reset_pushButton2){
        //ローラーカウンターをリセットする
        if(this->CounterResetMessageBox(TranslationString::GetString().translation_str.C_UNIT_003)){
            CONFIGURATION config_val;
            Configuration& config = Configuration::GetConfiguration();
            config_val = config.config_para;

            config_ui->Number_Of_Scans_cnt_label2->setText("0");

            config_val.retard_roller_counter = 0;
            config.SetConfiguration(config_val);
            this->bCountReset = true;
        }
    }else if(obj == config_ui->Unlock_pushButton){
        this->Check_AdminPassword();
    }
}

void ConfigurationDialog::Check_AdminPassword()
{
    if(m_sv->Get_AdministratorRestrictedStatus()){
        AdministratorPasswordInputDialog* adminPassDialog = new AdministratorPasswordInputDialog();
        if(m_sv->device_data.DefaultPasswordType.select == kSDIDefaultPasswordTypeUnique)
        {
            delete adminPassDialog;
            adminPassDialog = (AdministratorPasswordInputDialog*)new AdministratorPasswordInputForUKSecurityDialog();
        }

        while(1){
            if(adminPassDialog->isHidden()){
                adminPassDialog->exec();
            }else {
                adminPassDialog->activateWindow();
            }

            std::string password = adminPassDialog->GetPassword().toStdString();

            if(password != ""){
                if(m_sv->Send_AdministratorRestrictedPassword(password)){
                    //パスワードが正しい場合
                    m_sv->device_data.admin_status = AdminLockOff;
                    this->DisplayItem();
                    break;
                }else {
                    //パスワードが間違いの場合
                    CustomMessageBox* msg_box = new CustomMessageBox(this);
                    msg_box->CreateMessageBox(TranslationString::GetString().translation_str.E_UI_109, TranslationString::GetString().translation_str.VERSION_003, Information);
                    if(msg_box){
                        delete msg_box;
                        msg_box = NULL;
                    }
                }
            }else {
                //Cancel
                m_sv->device_data.admin_status = AdminLockOn;
                break;
            }
        }

        if(adminPassDialog){
            delete adminPassDialog;
            adminPassDialog = NULL;
        }
    }else {
        //本体側でロックが既に解除されていた場合
        m_sv->device_data.admin_status = AdminLockOff;
        this->DisplayItem();
    }
}

bool ConfigurationDialog::Get_ResetFlag()
{
    return this->bReset;
}
bool ConfigurationDialog::Get_CounterResetFlag()
{
    return this->bCountReset;
}

bool ConfigurationDialog::ResetMessageBox(void)
{
    QMessageBox msgBox(this);
    msgBox.setText(TranslationString::GetString().translation_str.LINUX_C_UI_001);
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.ES2U_MAIN_052);
    msgBox.setButtonText(QMessageBox::No, TranslationString::GetString().translation_str.ES2U_MAIN_053);

    msgBox.setIcon(QMessageBox::Warning);

    int res = msgBox.exec();

    if (res == QMessageBox::Yes){
        return true;
    } else if (res == QMessageBox::No) {
        return false;
    }
    return false;
}

bool ConfigurationDialog::CounterResetMessageBox(QString message)
{
    QMessageBox msgBox(this);
    msgBox.setText(message);
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.ES2U_MAIN_052);
    msgBox.setButtonText(QMessageBox::No, TranslationString::GetString().translation_str.ES2U_MAIN_053);

    msgBox.setIcon(QMessageBox::Warning);

    int res = msgBox.exec();

    if (res == QMessageBox::Yes){
        return true;
    } else if (res == QMessageBox::No) {
        return false;
    }
    return false;
}

void ConfigurationDialog::DisplayItem(void)
{
    //管理者ロックcheck
    if(m_sv->Check_SupportAdministratorRestricted()){
        config_ui->Unlock_pushButton->show();
       if(m_sv->Get_AdministratorRestrictedStatus()){
           config_ui->Unlock_pushButton->setEnabled(true);
           m_sv->device_data.admin_status = AdminLockOn;
       }else {
           config_ui->Unlock_pushButton->setEnabled(false);
           m_sv->device_data.admin_status = AdminLockOff;
       }
    }else {
        config_ui->Unlock_pushButton->hide();
        m_sv->device_data.admin_status = AdminUnsupported;
    }

    //Information tab


    //General tab
    if(m_sv->device_data.TransferCompression.capability.supportLevel == kSDISupportLevelNone){
        config_ui->Transfer_data_after_comp_checkBox->hide();
    }else {
        config_ui->Transfer_data_after_comp_checkBox->show();
    }

    //Counter tab
    //ローラーキットかローラー＋分離ローラーの組み合わせ切り替え
    if(m_sv->device_data.ConfigureCounter.RollerCounter.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Counter_stackedWidget->setCurrentIndex(0);
        config_ui->tabWidget->setTabEnabled(Counter_tab, true);
    }else if(m_sv->device_data.ConfigureCounter.RollerKitCounter.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Counter_stackedWidget->setCurrentIndex(1);
        config_ui->tabWidget->setTabEnabled(Counter_tab, true);
    }else {
        //項目自体をグレーアウトする
        config_ui->tabWidget->setTabEnabled(Counter_tab, false);
        //config_ui->tabWidget->removeTab(1);
    }
    //管理者ロックが有効な場合はグレーアウト
    if(m_sv->device_data.admin_status == AdminLockOn){
        config_ui->Counter_Reset_pushButton->setEnabled(false);
        config_ui->Count_Alert_Setting_num_spinBox->setEnabled(false);
    }else {
        config_ui->Counter_Reset_pushButton->setEnabled(true);
        config_ui->Count_Alert_Setting_num_spinBox->setEnabled(true);
    }

    //管理者権限機能非表示
    if(m_sv->device_data.ConfigureCounter.RestrictFunction.select){
        config_ui->Reset_pushButton->hide();
        config_ui->Reset_pushButton2->hide();
    }else {
        config_ui->Reset_pushButton->show();
        config_ui->Reset_pushButton2->show();
    }


    //Power saving tab
    int item_num = 0;
    if(m_sv->device_data.SleepTime.capability.supportLevel == kSDISupportLevelNone){
        config_ui->Sleep_Timer_label->hide();
        config_ui->Sleep_Timer_colon_label->hide();
        config_ui->Sleep_Timer_spinBox->hide();
    }else if(m_sv->device_data.SleepTime.capability.supportLevel == kSDISupportLevelUnavailable){
        config_ui->Sleep_Timer_label->show();
        config_ui->Sleep_Timer_label->setEnabled(false);
        config_ui->Sleep_Timer_colon_label->show();
        config_ui->Sleep_Timer_colon_label->setEnabled(false);
        config_ui->Sleep_Timer_spinBox->show();
        config_ui->Sleep_Timer_spinBox->setEnabled(false);
    }else {
        config_ui->Sleep_Timer_label->show();
        config_ui->Sleep_Timer_label->setEnabled(true);
        config_ui->Sleep_Timer_colon_label->show();
        config_ui->Sleep_Timer_colon_label->setEnabled(true);
        config_ui->Sleep_Timer_spinBox->show();
        config_ui->Sleep_Timer_spinBox->setEnabled(true);
        item_num++;
    }

    int AutoPowerOffItem_num = 0;
    if(m_sv->device_data.AutoPowerOffTime.capability.supportLevel == kSDISupportLevelNone){
        config_ui->AC_Power_colon_label->hide();
        config_ui->AC_Power_label->hide();
        config_ui->AC_Power_comboBox->hide();
        //config_ui->Power_Off_Timer_groupBox->setFixedHeight(60);
    }else {
        config_ui->AC_Power_colon_label->show();
        config_ui->AC_Power_label->show();
        config_ui->AC_Power_comboBox->show();
        item_num++;
        AutoPowerOffItem_num++;
    }

    if(m_sv->device_data.AutoPowerOffTimeBattery.capability.supportLevel == kSDISupportLevelNone){
        config_ui->Battery_Power_colon_label->hide();
        config_ui->Battery_Power_label->hide();
        config_ui->Battery_Power_comboBox->hide();
        //config_ui->Power_Off_Timer_groupBox->setFixedHeight(60);
    }else {
        config_ui->Battery_Power_colon_label->show();
        config_ui->Battery_Power_label->show();
        config_ui->Battery_Power_comboBox->show();
        item_num++;
        AutoPowerOffItem_num++;
    }

    if(AutoPowerOffItem_num == 0){
        config_ui->Power_Off_Timer_groupBox->hide();
    }else {
        config_ui->Power_Off_Timer_groupBox->show();
    }

    if(m_sv->device_data.NonConnectPowerOff.capability.supportLevel == kSDISupportLevelNone){
        config_ui->Power_Off_If_Disconnected_colon_label->hide();
        config_ui->Power_Off_If_Disconnected_label->hide();
        config_ui->Power_Off_If_Disconnected_comboBox->hide();
    }else {
        config_ui->Power_Off_If_Disconnected_colon_label->show();
        config_ui->Power_Off_If_Disconnected_label->show();
        config_ui->Power_Off_If_Disconnected_comboBox->show();
        item_num++;
    }

    if(m_sv->device_data.DirectPowerOn.capability.supportLevel == kSDISupportLevelNone){
        config_ui->Direct_Power_On_colon_label->hide();
        config_ui->Direct_Power_On_label->hide();
        config_ui->Direct_Power_On_comboBox->hide();
    }else {
        config_ui->Direct_Power_On_colon_label->show();
        config_ui->Direct_Power_On_label->show();
        config_ui->Direct_Power_On_comboBox->show();
        item_num++;
    }

    //項目数が0の場合タブ自体をグレーアウトする
    if(item_num == 0){
        config_ui->tabWidget->setTabEnabled(Power_Saving_tab, false);
        //config_ui->tabWidget->removeTab(2);
    }else {
        config_ui->tabWidget->setTabEnabled(Power_Saving_tab, true);
    }

    //管理者ロックが有効な場合はグレーアウト
    if(m_sv->device_data.admin_status == AdminLockOn){
        config_ui->Sleep_Timer_spinBox->setEnabled(false);
        config_ui->AC_Power_comboBox->setEnabled(false);
        config_ui->Battery_Power_comboBox->setEnabled(false);
        config_ui->Power_Off_If_Disconnected_comboBox->setEnabled(false);
        config_ui->Direct_Power_On_comboBox->setEnabled(false);
    }else {
        config_ui->Sleep_Timer_spinBox->setEnabled(true);
        config_ui->AC_Power_comboBox->setEnabled(true);
        config_ui->Battery_Power_comboBox->setEnabled(true);
        config_ui->Power_Off_If_Disconnected_comboBox->setEnabled(true);
        config_ui->Direct_Power_On_comboBox->setEnabled(true);
    }

    //メンテナンスタブ
    int MaintenanceItem_num = 0;

    if(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Regular_Cleaning_Alert_checkBox->show();
        MaintenanceItem_num++;
        config_ui->Number_of_Scans_label->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);
        config_ui->Number_of_Scans_colon_label->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);
        config_ui->Number_of_Scans_num_label->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);
        config_ui->Maintenance_Reset_pushButton->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);

        config_ui->Count_Alert_Setting_label->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);
        config_ui->Count_Alert_Setting_colon_label->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);
        config_ui->Count_Alert_Setting_spinBox->setEnabled(m_sv->device_data.ConfigureCounter.CleaningWarningNotify.select);
    }else {
        config_ui->Regular_Cleaning_Alert_checkBox->hide();
    }
    if(MaintenanceItem_num == 0){
        config_ui->tabWidget->setTabEnabled(Maintenace_tab, false);
        //config_ui->tabWidget->removeTab(3);
    }else {
        config_ui->tabWidget->setTabEnabled(Maintenace_tab, true);
    }

    //管理者ロックが有効な場合はグレーアウト
    if(m_sv->device_data.admin_status == AdminLockOn){
        config_ui->Regular_Cleaning_Alert_checkBox->setEnabled(false);
        config_ui->Maintenance_Reset_pushButton->setEnabled(false);
        config_ui->Count_Alert_Setting_spinBox->setEnabled(false);
    }else {
        config_ui->Regular_Cleaning_Alert_checkBox->setEnabled(true);
        config_ui->Maintenance_Reset_pushButton->setEnabled(true);
        config_ui->Count_Alert_Setting_spinBox->setEnabled(true);
    }


    //Scanner Setting Tab
    int ScannerSettingItem_num = 0;
    if(m_sv->device_data.ADFPaperProtection.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Paper_Protection_colon_label->show();
        config_ui->Paper_Protection_label->show();
        ScannerSettingItem_num++;
    }else {
        config_ui->Paper_Protection_colon_label->hide();
        config_ui->Paper_Protection_label->hide();
    }
    if(m_sv->device_data.SensorGlassDirtSensitivity.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Detect_Glass_Dirt_colon_label->show();
        config_ui->Detect_Glass_Dirt_label->show();
        ScannerSettingItem_num++;
    }else {
        config_ui->Detect_Glass_Dirt_colon_label->hide();
        config_ui->Detect_Glass_Dirt_label->hide();
    }
    if(m_sv->device_data.BehaviorWhenDoubleFeed.capability.supportLevel != kSDISupportLevelNone){
        config_ui->Double_Feed_Stop_Timing_colon_label->show();
        config_ui->Double_Feed_Stop_Timing_label->show();
        ScannerSettingItem_num++;
    }else {
        config_ui->Double_Feed_Stop_Timing_colon_label->hide();
        config_ui->Double_Feed_Stop_Timing_label->hide();
    }
    //項目数が0の場合タブ自体をグレーアウトする
    if(ScannerSettingItem_num == 0){
        config_ui->tabWidget->setTabEnabled(Scanner_Setting_tab, false);
        //config_ui->tabWidget->removeTab(4);
        //this->hideTab("scanner_setting_tab");
        //config_ui->scanner_setting_tab->hide();
    }else {
        config_ui->tabWidget->setTabEnabled(Scanner_Setting_tab, true);
    }

    //管理者ロックが有効な場合はグレーアウト
    if(m_sv->device_data.admin_status == AdminLockOn){
        config_ui->Paper_Protection_comboBox->setEnabled(false);
        config_ui->Detect_Glass_Dirt_comboBox->setEnabled(false);
        config_ui->Double_Feed_Stop_Timing_comboBox->setEnabled(false);
    }else {
        config_ui->Paper_Protection_comboBox->setEnabled(true);
        config_ui->Detect_Glass_Dirt_comboBox->setEnabled(true);
        config_ui->Double_Feed_Stop_Timing_comboBox->setEnabled(true);
    }

}

void ConfigurationDialog::hideTab(const QString &tabName)
{
    // Find ribbon tab
    for (int i = 0; i < config_ui->tabWidget->count(); i++)
    {
        QString temp_tab = config_ui->tabWidget->tabText(i);
        if (temp_tab == tabName)
        {
            config_ui->tabWidget->removeTab(i);
            config_ui->general_tab->hide();
            break;
        }
    }
}

bool ConfigurationDialog::CheckOverRollerKitLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready)
{
    //LifeLimitが0の機種は非サポート
    if(ConfigureCounter.RollerKitLifeLimit.select == 0){
        return false;
    }
    if(ConfigureCounter.RollerKitCounter.select < ConfigureCounter.RollerKitLifeLimit.select){
        *AlertAlready = false;
    }
    if(!*AlertAlready){
        if(ConfigureCounter.RollerKitCounter.select >= ConfigureCounter.RollerKitLifeLimit.select){
            *AlertAlready = true;
            return true;
        }
    }
    return false;
}

bool ConfigurationDialog::CheckNearRollerKitLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready)
{
    //NearEndが0の機種は非サポート
    if(ConfigureCounter.RollerKitNearEnd.select == 0){
        return false;
    }
    if(ConfigureCounter.RollerKitCounter.select < ConfigureCounter.RollerKitNearEnd.select){
        *AlertAlready = false;
    }
    if(!*AlertAlready){
        if(ConfigureCounter.RollerKitCounter.select >= ConfigureCounter.RollerKitNearEnd.select){
            *AlertAlready = true;
            return true;
        }
    }
    return false;
}

bool ConfigurationDialog::CheckOverRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready)
{
    //LifeLimitが0の機種は非サポート
    if(ConfigureCounter.RollerLifeLimit.select == 0){
        return false;
    }
    if(ConfigureCounter.RollerCounter.select < ConfigureCounter.RollerLifeLimit.select){
        *AlertAlready = false;
    }
    if(!*AlertAlready){
        if(ConfigureCounter.RollerCounter.select >= ConfigureCounter.RollerLifeLimit.select){
            *AlertAlready = true;
            return true;
        }
    }
    return false;
}

bool ConfigurationDialog::CheckNearRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready)
{
    //NearEndが0の機種は非サポート
    if(ConfigureCounter.RollerNearEnd.select == 0){
        return false;
    }
    if(ConfigureCounter.RollerCounter.select < ConfigureCounter.RollerNearEnd.select){
        *AlertAlready = false;
    }
    if(!*AlertAlready){
        if(ConfigureCounter.RollerCounter.select >= ConfigureCounter.RollerNearEnd.select){
            *AlertAlready = true;
            return true;
        }
    }
    return false;
}

bool ConfigurationDialog::CheckOverRetardRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready)
{
    //LifeLimitが0の機種は非サポート
    if(ConfigureCounter.RetardRollerLifeLimit.select == 0){
        return false;
    }
    if(ConfigureCounter.RetardRollerCounter.select < ConfigureCounter.RetardRollerLifeLimit.select){
        *AlertAlready = false;
    }
    if(!*AlertAlready){
        if(ConfigureCounter.RetardRollerCounter.select >= ConfigureCounter.RetardRollerLifeLimit.select){
            *AlertAlready = true;
            return true;
        }
    }
    return false;
}

bool ConfigurationDialog::CheckNearRetardRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready)
{
    //NearEndが0の機種は非サポート
    if(ConfigureCounter.RetardRollerNearEnd.select == 0){
        return false;
    }
    if(ConfigureCounter.RetardRollerCounter.select < ConfigureCounter.RetardRollerNearEnd.select){
        *AlertAlready = false;
    }
    if(!*AlertAlready){
        if(ConfigureCounter.RetardRollerCounter.select >= ConfigureCounter.RetardRollerNearEnd.select){
            *AlertAlready = true;
            return true;
        }
    }
    return false;
}

bool ConfigurationDialog::CheckUnlockButton_Enabled()
{
    return config_ui->Unlock_pushButton->isEnabled();
}
