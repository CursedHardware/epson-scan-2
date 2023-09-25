#include <QDesktopServices>
#include <QDebug>
#include <QProcess>

#include "mainwindow.h"
#include "deviceselectdialog.h"
#include "scanningdialog.h"
#include "aboutepsonscan2dialog.h"
#include "addpagedialog.h"
#include "addeditpagedialog.h"
#include "devicelist.h"
#include "devicefinder.h"
#include "PathUtils.h"

#include "common_utility.h"

#include "defaultsettings.h"
#include "textenhancesettingdialog.h"

MainWindow::MainWindow(const std::string edit_output_path, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_preview_window(nullptr),
    m_viewPreviewWindow(false)
{
    m_DoNotClose = false;

    configuration_dialog = NULL;
    m_last_used_settings_controller = nullptr;
    m_messageBox = NULL;

    m_messageBox = new CustomMessageBox(this);
    if(m_messageBox == NULL){
        exit(EXIT_SUCCESS);
    }

    sv = nullptr;

    sv = new Supervisor();
    if(sv == NULL){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
        exit(EXIT_SUCCESS);
    }
    sv->scanning_status = SAVE;
    sv->bAllReset = false;

    Configuration& config = Configuration::GetConfiguration();

    if(edit_output_path.c_str()[0] != '\0'){
        sv->edit_mode = true;
        sv->setting_file_path = edit_output_path;
    }else {
        sv->edit_mode = false;
    }
    m_last_used_settings_controller = new LastUsedSettings();
    if(m_last_used_settings_controller == NULL){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
        this->CloseWindow();
        exit(EXIT_SUCCESS);
    }
    ui->setupUi(this);

    SetupMainWindow(config);
    this->setFixedSize(this->size()); //メインウィンドウの最大化を無効に

    sv->SetUp();
#ifdef AKBMODE
    this->setWindowTitle(DRIVER_TITLE);
#ifdef HELP_URL
    if(HELP_URL == ""){
        ui->Help_toolButton->hide();
    }else {
        ui->Help_toolButton->show();
    }
#else
    ui->Help_toolButton->hide();
#endif
#endif
    //Edit Mode Select
    if(sv->edit_mode){
        ui->Scan_Button_stackedWidget->setCurrentIndex(1);
    }else {
        ui->Scan_Button_stackedWidget->setCurrentIndex(0);
    }
    //Initialize dialog
    if(!configuration_dialog){
        configuration_dialog = new ConfigurationDialog(sv, this);
        if(configuration_dialog == NULL){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
            this->CloseWindow();
            exit(EXIT_SUCCESS);
        }
    }
    bool connection_success = false;
    while(!connection_success){
        sv->Disconnecting_Scanner(); //現在接続中のデバイスをクローズ
        sv->Dispose_Scanner();

        //Initialize Device List
        ui->Device_selector_comb->initialize();

        if(m_last_used_settings_controller->CheckLastUsedDevice()){
            //前回接続機種がある場合
            if(!m_last_used_settings_controller->ReadPreferredInfo()){
                //前回接続機種の情報はあるがデバイスが見つからない場合
                if(!this->SelectDevice()){
                    break;
                }
            }
            //Common設定ファイルの読み込み
            config.SetConfiguration(m_last_used_settings_controller->ReadCommonSettingsFile());
        }else {
            //初回接続または前回接続機種情報がない場合
            if(!this->SelectDevice()){
                //接続失敗した後に×ボタンで終了する
                sv->bAllReset = true;
                break;
            }
        }
        //選択されたデバイス(DeviceList::select_device)に対して接続処理
        ui->Device_selector_comb->add_device();
        ui->Device_selector_comb->setCurrentIndex(DeviceList::select_device);
        connection_success = this->ConnectToDevice();
        if(connection_success){
            this->updateUI();

            ui->Folder_comb->select_current_path(&sv->device_data);
        }else {
            //前回接続機種に接続失敗した場合は前回接続設定ファイルを削除する
            m_last_used_settings_controller->DeletePreferredInfo();
        }
    }

    if(connection_success){
        this->ValidateOverRollerLifeLimit();
    }
}

MainWindow::~MainWindow()
{
    this->CloseWindow();
    exit(EXIT_SUCCESS);
}

void MainWindow::SetupMainWindow(Configuration config)
{
    int x, y, width, height;
    //メインウィンドウのサイズと位置の設定
    width = INIT_MAINWINDOW_WIDTH;
    config.config_para = m_last_used_settings_controller->ReadMainWindowInfo();
    if(config.config_para.WindowPositionX == INIT_POSITION_X && config.config_para.WindowPositionY == INIT_POSITION_Y){
        x = INIT_POSITION_X;
        y = INIT_POSITION_Y;
    }else {
        x = config.config_para.WindowPositionX;
        y = config.config_para.WindowPositionY;
    }
    if(config.config_para.WindowHeight != INIT_MAINWINDOW_HEIGHT){
        height = config.config_para.WindowHeight;
    }else {
        height = INIT_MAINWINDOW_HEIGHT;
    }

    this->setGeometry(x, y, width, height);

    this->setMaximumWidth(INIT_MAINWINDOW_WIDTH);
    this->setMinimumHeight(INIT_MAINWINDOW_HEIGHT);

    //フォルダを開くボタンにＱｔ標準アイコンを設定する
    ui->Folder_toolButton->setIcon(this->style()->standardIcon(QStyle::SP_DirLinkIcon));
    //ファイル名設定ボタンにＱｔ標準アイコンを設定する
    ui->FileNameSetting_toolButton->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogContentsView));

#ifdef NOUSEQRC
    std::string strHelpIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strHelpIconPath, strHelpIconPath, "Resources/Icons/btn_help.png");
    ui->Help_toolButton->setIcon(QIcon(strHelpIconPath.c_str()));

    std::string strConfIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strConfIconPath, strConfIconPath, "Resources/Icons/btn_config@2x.png");
    ui->Configuration_toolButton->setIcon(QIcon(strConfIconPath.c_str()));
#endif
#ifdef RPMBUILD
    ui->Help_toolButton->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxQuestion));
    ui->Configuration_toolButton->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));
#endif
}

void MainWindow::CloseWindow()
{
    //マーキー情報をリセットする
    MarqueeCreated(false, QRect(0,0,0,0));

    //終了時に前回接続機種ファイルを更新する
    if(m_last_used_settings_controller != nullptr){
        m_last_used_settings_controller->CreatePreferredInfo(ui->Device_selector_comb->get_cur_device());
    }
    if(sv != NULL && sv->driver){
        if(sv->bAllReset == false){
            //メインウィンドウの情報を保存する
            Configuration& config = Configuration::GetConfiguration();
            QRect MainWindowInfo = this->geometry();
            QRect MainWindowInfo_withFrame = this->frameGeometry();
            config.config_para.WindowPositionX = MainWindowInfo_withFrame.x();
            config.config_para.WindowPositionY = MainWindowInfo_withFrame.y();
            config.config_para.WindowWidth = MainWindowInfo.width();
            config.config_para.WindowHeight = MainWindowInfo.height();
            config.SetConfiguration(config.config_para);

            //終了時にCommonSettingsファイルを更新する
            m_last_used_settings_controller->CreateCommonSettingsFile(config);
            if(sv->CheckDeviceErrorStatus() != -1){
                //終了時にUser Settingsファイルを更新する
                this->isManualResolution();
                m_last_used_settings_controller->CreateSettingsFile(ui->Device_selector_comb->get_cur_device().modelID,
                                                                    sv->device_data);
            }
        }
    }
    if(m_last_used_settings_controller != nullptr){
        delete m_last_used_settings_controller;
        m_last_used_settings_controller = nullptr;
    }

    if(m_preview_window){
        delete m_preview_window;
        m_preview_window = nullptr;
    }

    if(configuration_dialog){
        delete configuration_dialog;
        configuration_dialog = NULL;
    }

    if(sv != NULL){
        sv->Disconnecting_Scanner();
        sv->Dispose_Scanner();

        sv->Terminate();

        delete sv;
        sv = nullptr;
    }

    if(m_messageBox != NULL){
        delete m_messageBox;
        m_messageBox = NULL;
    }

    ES_CMN_FUNCS::PATH::ES_DeleteFolder(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath());

    delete ui;
}

bool MainWindow::SelectDevice(void)
{
    DeviceSelectDialog* device_select_dialog = NULL;
    device_select_dialog = new DeviceSelectDialog;
    if(device_select_dialog == NULL){
        return false;
    }
    device_select_dialog->setWindowFlags(Qt:: Dialog);
    if(!device_select_dialog->Initialize(sv)){
        return false;
    }
    if(device_select_dialog->isHidden()){
        device_select_dialog->exec();
    }else {
        device_select_dialog->activateWindow();
    }
    if(device_select_dialog->terminate_flag){
        delete device_select_dialog;
        return false;
    }
    return true;
}

void MainWindow::showEvent( QShowEvent* event ) {
    if(!sv->driver){
        this->CloseWindow();
        exit(EXIT_SUCCESS);
    }
}

void MainWindow::Check2in1Difference()
{
    //2in1モードでＦＢを選択後設定を保存してノーマルモードで起動した場合
    if(!sv->device_data.Use2in1Mode.select && sv->device_data.FunctionalUnit.select == Flatbed){
        if(!common_utility.search_item_list(kSDIFunctionalUnitFlatbed, sv->device_data.FunctionalUnit.capability.list, sv->device_data.FunctionalUnit.capability.countOfList)){
            sv->device_data.FunctionalUnit.select = sv->device_data.FunctionalUnit.capability.list[0];
            sv->Set_Exclusion_Value();
        }
    }else if(!sv->device_data.Use2in1Mode.select && sv->device_data.FunctionalUnit_Auto){
        sv->device_data.FunctionalUnit.select = ADF;
        sv->Set_Exclusion_Value();
    }
}

bool MainWindow::search_item_list(int item, int32_t item_list[], int32_t list_size)
{
    int i = 0;
    while(i < list_size){
        if(item_list[i] == item){
            return true;
        }
        i++;
    }
    return false;
}

void MainWindow::Initialize(void)
{
    //FBがあるときのスキャン領域の最小値は必ず最小値が入る
    if(this->search_item_list(kSDIFunctionalUnitFlatbed, sv->device_data.FunctionalUnit.capability.list, sv->device_data.FunctionalUnit.capability.countOfList)){
        sv->device_data.ScanArea.FBWidthMin = 100;
        sv->device_data.ScanArea.FBHeightMin = 100;
    }else {
        sv->device_data.ScanArea.FBWidthMin = 0;
        sv->device_data.ScanArea.FBHeightMin = 0;
    }
    ui->Document_source_comb->initialize(sv->device_data.FunctionalUnit, sv->device_data.FunctionalUnit_Auto);
    ui->Scanning_side_comb->initialize((ScanningSide)sv->device_data.DuplexType.select);
    ui->Document_type_comb->initialize((DocumentType)sv->device_data.documentType.select);
    ui->Document_size_comb->initialize(sv->device_data.ScanArea, sv->device_data.Resolution.select, (ScannerKind)sv->device_data.ScannerKind.select, &(sv->device_data.AutoCroppingInscribed));
    ui->Image_type_comb->initialize(&(sv->device_data.documentType));
    ui->Resolution_comb->initialize(sv->device_data.Resolution);
    ui->Rotate_comb->initialize();
    ui->Deskew_checkbox->initialize();
    ui->Ad_pages_checkbox->initialize(sv->device_data.Add_Pages);
    ui->Skip_blank_pages_comb->initialize(sv->device_data);
    ui->Detect_double_feed_comb->initialize(sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel);
    ui->RemoveBack_comboBox->initialize(sv->device_data.BackgroundRemoval);
    ui->Dropout_comboBox->initialize(sv->device_data.DropoutColor);
    ui->TextEnhance_comboBox->initialize(sv->device_data.textEnhance);

    ui->Brightness_hSlider->initialize(sv->device_data.Brightness.select);
    ui->Contrast_hSlider->initialize(sv->device_data.Contrast.select);
    ui->Gamma_hSlider->initialize(sv->device_data.Gamma.select);

    ui->Threshold_hSlider->initialize(sv->device_data.Threshold.select);

    ui->Image_format_comb->initialize(sv->device_data.ImageFormatSettings);
    ui->Prefix_lineEdit->initialize(sv->device_data.Prefix);
    ui->Folder_comb->initialize(sv->device_data, m_messageBox);

    ui->Scan_Button->initialize(m_messageBox, this);
    ui->Preview_Button->initialize(m_messageBox);

    ui->Automatic_feeding_mode_comb->initialize(sv->device_data.AutomaticFeedingMode,
                                                sv->device_data.continuousAutoFeedingMode);
    ui->Background_color_comb->initialize(sv->device_data);

    ui->remove_punch_holes_comb->initialize();

    ui->edge_correction_comb->initialize(sv->device_data);

    this->InitializeImageOption(sv->device_data.ImageOption);

    ui->Configuration_toolButton->setEnabled(true);
    ui->tabWidget->setEnabled(true);
    this->SetAllLabelEnabled(true);
}

void MainWindow::SetAllLabelEnabled(bool enabled)
{
    ui->device_name_colon_label->setEnabled(enabled);

    ui->Document_source_label->setEnabled(enabled);
    ui->Doc_source_colon_label->setEnabled(enabled);

    ui->Scanning_side_label->setEnabled(enabled);
    ui->Scanning_side_colon_label->setEnabled(enabled);

    ui->Document_size_label->setEnabled(enabled);
    ui->Document_size_colon_label->setEnabled(enabled);

    ui->Image_type_label->setEnabled(enabled);
    ui->Image_type_colon_label->setEnabled(enabled);

    ui->Resolution_label->setEnabled(enabled);
    ui->Resolution_colon_label->setEnabled(enabled);
    ui->Dpi_label->setEnabled(enabled);

    ui->Rotate_label->setEnabled(enabled);
    ui->Rotate_colon_label->setEnabled(enabled);

    ui->Skip_blank_pages_label->setEnabled(enabled);
    ui->Skip_blank_pages_colon_label->setEnabled(enabled);

    ui->Detect_double_page_label->setEnabled(enabled);
    ui->Detect_double_page_colon_label->setEnabled(enabled);

    ui->Image_option_colon_label->setEnabled(enabled);

    ui->Image_format_label->setEnabled(enabled);
    ui->Image_format_colon_label->setEnabled(enabled);

    ui->File_name_label->setEnabled(enabled);
    ui->File_name_colon_label->setEnabled(enabled);
    ui->File_extension_label->setEnabled(enabled);

    ui->Folder_label->setEnabled(enabled);
    ui->Folder_colon_label->setEnabled(enabled);
    ui->Folder_toolButton->setEnabled(enabled);

    ui->Save_pushButton->setEnabled(enabled);

    ui->Help_toolButton->setEnabled(enabled);

    ui->remove_punch_holes_label->setEnabled(enabled);
    ui->remove_punch_holes_colon_label->setEnabled(enabled);

    ui->edge_correction_label->setEnabled(enabled);
    ui->edge_correction_colon_label->setEnabled(enabled);

    /*if 原稿のセット方向が有効か判定
    ui->Horizontal_doc_toolButton->setEnabled(enabled);
    ui->Vertical_doc_toolButton->setEnabled(enabled);
    ui->Horizontal_doc_toolButton->setChecked(enabled);
    }*/
}

bool MainWindow::ConnectToDevice(void)
{
    Configuration& config = Configuration::GetConfiguration();
    bool use_lastsettings = false;

    SDIDeviceInfo select_Device = ui->Device_selector_comb->get_cur_device();
    //選択されたデバイスがネットワークデバイスならばIPアドレス解決する
    if(select_Device.ipAddress[0] != '\0'){
        if(!sv->DeviceInfoResolve(&select_Device)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.LINUX_E_COMM_001, TranslationString::GetString().translation_str.VERSION_003, Warning);
            return false;
        }
    }else if(select_Device.displayName[0] == '\0'){
        //接続可能なデバイスが1台も見つからない場合
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.LINUX_E_COMM_001, TranslationString::GetString().translation_str.VERSION_003, Warning);
        return false;
    }

    DefaultSettings* default_settings_file = nullptr;
    //前回接続設定ファイルを読み込む
    if(config.config_para.last_used_settings){
        use_lastsettings = m_last_used_settings_controller->ReadSettingsFile(select_Device.modelID, &sv->device_data);
        if(!use_lastsettings){
            //デフォルト設定ファイルがある場合はそちらから設定パラメーターを取得する
            default_settings_file = new DefaultSettings();
            if(default_settings_file->CheckDefaultSettingsFile()){
                default_settings_file->ReadSettingsFile(&sv->device_data);
                config.config_para.used_default_settings = true;
            }
        }
    }else {
        //デフォルト設定ファイルがある場合はそちらから設定パラメーターを取得する
        default_settings_file = new DefaultSettings();
        if(default_settings_file->CheckDefaultSettingsFile()){
            default_settings_file->ReadSettingsFile(&sv->device_data);
            config.config_para.used_default_settings = true;
        }
    }
    DeviceConnectionDialog* device_connection_dialog = new DeviceConnectionDialog(sv, select_Device, use_lastsettings);
    if(device_connection_dialog == NULL){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
        return false;
    }

    //デフォルト設定ファイルがある場合は値をセットしてからクラスを削除
    if(default_settings_file){
        if(default_settings_file->CheckDefaultSettingsFile()){
            sv->Set_All_Value();
        }
        delete(default_settings_file);
        default_settings_file = nullptr;
    }

    if(device_connection_dialog->GetConnectionResult()){
        if(use_lastsettings){
            sv->Set_All_Value();
        }else {
            //前回設定ファイルを使用しないときにStandalone管理の変数を初期化する
            sv->device_data.AutomaticFeedingMode.select = false;
        }
        sv->Get_All_Value();
        //デバイス自体の設定を変数に反映させる
        config.config_para.ac_power = (Time)sv->device_data.AutoPowerOffTime.select;
        config.config_para.usb_power = (Time)sv->device_data.AutoPowerOffTimeBattery.select;
        config.config_para.sensor_glass_dirtsensitivity = (SensorGlassDirtSensitivity)sv->device_data.SensorGlassDirtSensitivity.select;
        config.config_para.adf_paper_protection = (ADFPaperProtection)sv->device_data.ADFPaperProtection.select;
        config.config_para.behavior_when_doublefeed = (BehaviorWhenDoubleFeed)sv->device_data.BehaviorWhenDoubleFeed.select;
        config.config_para.non_connect_poweroff = (NonConnectPowerOff)sv->device_data.NonConnectPowerOff.select;
        config.config_para.direct_power_on = (DirectPowerOn)sv->device_data.DirectPowerOn.select;
        config.config_para.cleaning_warning_notify = sv->device_data.ConfigureCounter.CleaningWarningNotify.select;
        config.config_para.sleep_timer = sv->device_data.SleepTime.select;
        this->Check2in1Difference();
        sv->Get_All_Value();

        //起動時の縦スジ警告
        //if(0 < sv->device_data.ConfigureCounter.CleaningAlertLimitCount.select){
        //
       // }
        if(!use_lastsettings){
            //FBとADFを両方サポートしている場合、デフォルト値をAutoに設定する
            if(common_utility.search_item_list(kSDIFunctionalUnitFlatbed, sv->device_data.FunctionalUnit.capability.list, sv->device_data.FunctionalUnit.capability.countOfList) &&
               common_utility.search_item_list(kSDIFunctionalUnitDocumentFeeder, sv->device_data.FunctionalUnit.capability.list, sv->device_data.FunctionalUnit.capability.countOfList)){
                sv->device_data.FunctionalUnit_Auto = true;
            }else {
                sv->device_data.FunctionalUnit_Auto = false;
            }
            //サイズ自動がない機種で前回設定を使用しない場合の初期値をA4にする
            if(sv->device_data.ScanArea.AutoSize.capability.supportLevel == kSDISupportLevelNone){
                if(QLocale::system().name() == "en_US"){
                    sv->device_data.ScanArea.FixedSize = Letter;
                }else {
                    sv->device_data.ScanArea.FixedSize = A4;
                }
            }
        }
        this->Initialize();
        return true;
    }else {
        if(sv->m_LastError == kSDIErrorUserAuthEnabled){
            //本体認証モードエラー
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_UNIT_008, TranslationString::GetString().translation_str.VERSION_003, Warning);
        }else {
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.LINUX_E_COMM_001, TranslationString::GetString().translation_str.VERSION_003, Warning);
        }
        return false;
    }
}

void MainWindow::AllItemReset(void)
{
    ui->Document_source_comb->changed = true;
    ui->Scanning_side_comb->changed = true;
    ui->Document_type_comb->changed = true;
    ui->Document_size_comb->changed = true;
    ui->Image_type_comb->changed = true;
    ui->Resolution_comb->changed = true;
    ui->Rotate_comb->changed = true;
    ui->Deskew_checkbox->changed = true;
    ui->Skip_blank_pages_comb->changed = true;
    ui->Detect_double_feed_comb->changed = true;
    ui->RemoveBack_comboBox->changed = true;
    ui->Dropout_comboBox->changed = true;
    ui->TextEnhance_comboBox->changed =true;
    ui->Automatic_feeding_mode_comb->changed = true;
    ui->Background_color_comb->changed = true;
    ui->remove_punch_holes_comb->changed = true;
    ui->edge_correction_comb->changed = true;
}

void MainWindow::closeEvent(QCloseEvent* event){
    //このダイアログで×ボタンを押下された場合アプリを終了する
    if(m_DoNotClose){
        event->ignore();
    }
}

void MainWindow::onValueChanged(int value)
{
    QObject* obj = sender();

    if(obj == ui->Device_selector_comb){
        m_DoNotClose = true; //機種切り替え中にメインウィンドウの×ボタンを無効にする
        ui->Device_selector_comb->item_event(value);
        //デバイスを追加を選択したが何も追加せずにダイアログを閉じた時
        sv->Disconnecting_Scanner(); //現在接続中のデバイスをクローズ
        sv->Dispose_Scanner();
        if(!this->ConnectToDevice()){ // 新たに選択したデバイスに接続
            //デバイスへの接続に失敗した場合Epson Scan 2を終了する
            m_DoNotClose = false;
            this->close();
        }
        this->AllItemReset();
        //機種切り替え時にプレビューウィンドウは削除する
        if(this->m_viewPreviewWindow){
            this->m_preview_window->on_Close_Button_pressed(false);
        }
        ui->Folder_comb->select_current_path(&sv->device_data);
        this->ValidateOverRollerLifeLimit();
        m_DoNotClose = false;
    }else if(obj == ui->Document_source_comb){
        FunctionalUnit old_select = ui->Document_source_comb->current_item;
        bool changed = false;
        //選択内容が前回と変わらない場合は特に処理は行わない
        if(ui->Document_source_comb->itemData(ui->Document_source_comb->currentIndex()) != old_select){
            //プレビューウィンドウが表示されているときにサイズを変更した場合 + 設定以外が選択された場合
            if(this->m_viewPreviewWindow == true && ui->Document_source_comb->itemData(ui->Document_source_comb->currentIndex()) != Unit_Settings){
                if(m_messageBox->CreateSelectableMessageBox(TranslationString::GetString().translation_str.C_UI_005, Warning)){
                    //Yes:プレビューウィンドウは削除する
                    this->m_preview_window->on_Close_Button_pressed(false);
                    //マーキー情報をリセットする
                    MarqueeCreated(false, QRect(0,0,0,0));
                    this->ChangePreviewParameter(false);
                    changed = true;
                }else {
                    //No:プレビューウィンドウはそのままに元のサイズに変更
                    ui->Document_source_comb->item_event(ui->Document_source_comb->findData(old_select), &sv->device_data);
                    changed = false;
                }
            }else {
                changed = true;
            }
        }
        if(changed){
            ui->Document_source_comb->item_event(value, &sv->device_data);
            ui->Document_size_comb->changed = true;
        }
    }else if(obj == ui->Scanning_side_comb){
        ui->Scanning_side_comb->item_event(value, &sv->device_data.DuplexType);
        ui->Document_size_comb->changed = true;
        //マーキーがある状態で両面に変更すると警告ダイアログを表示する
        if(this->m_viewPreviewWindow == true){
            if(m_preview_window->getDrawingArea()->isExistedMarquee() && sv->device_data.DuplexType.select == Double_Sided){
                if(m_messageBox->CreateSelectableMessageBox(TranslationString::GetString().translation_str.C_UI_006, Warning)){
                    //Yes:マーキーを削除する
                    m_preview_window->DeleteMarquee();
                    
                    //マーキー情報をリセットする
                    MarqueeCreated(false, QRect(0,0,0,0));
                    this->ChangePreviewParameter(false);
                }else {
                    //No:マーキーをそのままに片面に変更
                    ui->Scanning_side_comb->changed = true;
                    sv->device_data.DuplexType.select = Single_Sided;
                    this->updateUI();
                }
            }
        }
    }else if(obj == ui->Document_size_comb){
        DocumentSize old_select = ui->Document_size_comb->current_item;
        bool changed = false;
        //選択内容が前回と変わらない場合は特に処理は行わない
        if(ui->Document_size_comb->itemData(ui->Document_size_comb->currentIndex()) != old_select){
            //プレビューウィンドウが表示されているときにサイズを変更した場合
            if(this->m_viewPreviewWindow == true){
                if(ui->Document_size_comb->findData(Marquee) != 0){
                    if(m_messageBox->CreateSelectableMessageBox(TranslationString::GetString().translation_str.C_UI_005, Warning)){
                        //Yes:プレビューウィンドウは削除する
                        this->m_preview_window->on_Close_Button_pressed(false);
                        //マーキー情報をリセットする
                        MarqueeCreated(false, QRect(0,0,0,0));
                        this->ChangePreviewParameter(false);
                        changed = true;
                    }else {
                        //No:プレビューウィンドウはそのままに元のサイズに変更
                        ui->Document_size_comb->item_event(ui->Document_size_comb->findData(old_select), &sv->device_data.ScanArea, &sv->device_data.AutoCroppingInscribed);
                        changed = false;
                    }
                }
            }else {
                changed = true;
            }
        }
        if(changed){
            if(this->SelectionRestrictions(value)){
                ui->Document_size_comb->item_event(value, &sv->device_data.ScanArea, &sv->device_data.AutoCroppingInscribed);
                //選択したサイズがプラスチックカードの場合ダイアログ表示
                if(ui->Document_size_comb->current_item == PlasticCard_Landscape && !sv->device_data.DNShow_PlasticCard_dialog){
                    if(sv->device_data.ScanArea.ADFCard.select == ADFCardLandscape && sv->device_data.FunctionalUnit.select == ADF){
                        sv->device_data.DNShow_PlasticCard_dialog = m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UI_003, TranslationString::GetString().translation_str.VERSION_003, Information, true);
                    }
                //ユーザー定義サイズを追加時に解像度制限に引っかかった場合
                }if(!this->SelectionRestrictions(ui->Document_size_comb->findData(ui->Document_size_comb->current_item))){
                    ui->Document_size_comb->item_event(ui->Document_size_comb->findData(old_select), &sv->device_data.ScanArea, &sv->device_data.AutoCroppingInscribed);
                }
            }else {
                //ユーザー定義サイズを選択時に解像度制限に引っかかった場合
                ui->Document_size_comb->setCurrentIndex(ui->Document_size_comb->findData(ui->Document_size_comb->current_item));
            }
        }
    }else if(obj == ui->Image_type_comb){
        ui->Image_type_comb->item_event(value, &sv->device_data.ColorType, sv);
        this->ChangeWidget(sv->device_data.ColorType.select);
    }else if(obj == ui->Resolution_comb){
        if(this->SelectionRestrictions(value)){
            ui->Resolution_comb->item_event(value, &sv->device_data.Resolution);
            ui->Document_size_comb->changed = true;
            if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
                //接続デバイスがLFPだった場合
                ui->Document_size_comb->update_ui(sv->device_data, sv->device_data.Resolution.select, true);
            }else {
                ui->Document_size_comb->update_ui(sv->device_data, sv->device_data.Resolution.select, false);
            }
            ui->Document_size_comb->item_event(ui->Document_size_comb->currentIndex(), &sv->device_data.ScanArea, &sv->device_data.AutoCroppingInscribed);
        }else {
            //解像度を選択時にユーザー定義サイズ制限に引っかかった場合
            ui->Resolution_comb->setCurrentText(QString(QString::number(sv->device_data.Resolution.select)));
        }
        //解像度の更新に合わせてマーキー領域を再計算
        //this->UpdateMarqueeSize_resolution();
    }else if(obj == ui->Rotate_comb){
        ui->Rotate_comb->item_event(value, &sv->device_data.Orientation);
    }else if(obj == ui->Skip_blank_pages_comb){
        ui->Skip_blank_pages_comb->item_event(value, &sv->device_data);
    }else if(obj == ui->Detect_double_feed_comb){
        ui->Detect_double_feed_comb->item_event(value, &sv->device_data.DoubleFeedDetectionValue);
    }
    else if(obj == ui->RemoveBack_comboBox){
        ui->RemoveBack_comboBox->item_event(value, &sv->device_data.BackgroundRemoval);
    }else if(obj == ui->Dropout_comboBox){
        ui->Dropout_comboBox->item_event(value, &sv->device_data.DropoutColor);
    }else if(obj == ui->TextEnhance_comboBox){
        ui->TextEnhance_comboBox->item_event(value, &sv->device_data.textEnhance);
    }else if(obj == ui->Brightness_hSlider){
        ui->Brightness_hSlider->item_event(value, &sv->device_data.Brightness);
        return;
    }else if(obj == ui->Contrast_hSlider){
        ui->Contrast_hSlider->item_event(value, &sv->device_data.Contrast);
        return;
    }else if(obj == ui->Gamma_hSlider){
        ui->Gamma_hSlider->item_event(value, &sv->device_data.Gamma);
        return;
    }else if(obj == ui->Threshold_hSlider){
        ui->Threshold_hSlider->item_event(value, &sv->device_data.Threshold);
        return;
    }

    else if(obj == ui->Image_format_comb){
        ui->Image_format_comb->item_event(value, &sv->device_data.ImageFormatSettings);
        if(sv->device_data.ImageFormatSettings.ImageFormat.select == ImageFormatTIFF){
            ui->Ad_pages_checkbox->setText(TranslationString::GetString().translation_str.LINUX_MAINSETTING_001);
        }else {
            ui->Ad_pages_checkbox->setText(TranslationString::GetString().translation_str.MAINSETTING_047);
        }
    }else if(obj == ui->Folder_comb){
        ui->Folder_comb->item_event(value, &sv->device_data);
        sv->device_data.Folder = ui->Folder_comb->current_item;
        //fprintf (stderr, " selectPath : %s \n",  sv->device_data.SelectPath.c_str());
    }

    else if(obj == ui->Automatic_feeding_mode_comb){
        ui->Automatic_feeding_mode_comb->item_event(value, &sv->device_data.AutomaticFeedingMode,
                                                    &sv->device_data.use_continuousAutoFeedingMode);
    }else if(obj == ui->Background_color_comb){
        ui->Background_color_comb->item_event(value, &sv->device_data.BackgroundColor);
    }else if(obj == ui->remove_punch_holes_comb){
        ui->remove_punch_holes_comb->item_event(value, &sv->device_data.RemovePunchHoles);
    }else if(obj == ui->Document_type_comb){
        ui->Document_type_comb->item_event(value, &sv->device_data.documentType);
    }else if(obj == ui->edge_correction_comb){
        ui->edge_correction_comb->item_event(value, &sv->device_data.DeficiencyCorrection);
    }

    if (sv->device_data.ImageOption == ImageOptionTextEnhance)
    {
        ui->Gamma_label->setEnabled(false);
        ui->Gamma_colon_label->setEnabled(false);
        ui->Gamma_doubleSpinBox->setEnabled(false);
        ui->Gamma_hSlider->setEnabled(false);
    }else{
        ui->Gamma_label->setEnabled(true);
        ui->Gamma_colon_label->setEnabled(true);
        ui->Gamma_doubleSpinBox->setEnabled(true);
        ui->Gamma_hSlider->setEnabled(true);
    }
    //Update MainWindow
    this->updateUI();
}

void MainWindow::onValueChangedQstr(QString value)
{
    QObject* obj = sender();
}

void MainWindow::onValueChangedBool(bool check)
{
    QObject* obj = sender();

    if (sv->device_data.ImageOption == ImageOptionTextEnhance)
    {
        ui->Gamma_label->setEnabled(false);
        ui->Gamma_colon_label->setEnabled(false);
        ui->Gamma_doubleSpinBox->setEnabled(false);
        ui->Gamma_hSlider->setEnabled(false);
    }else{
        ui->Gamma_label->setEnabled(true);
        ui->Gamma_colon_label->setEnabled(true);
        ui->Gamma_doubleSpinBox->setEnabled(true);
        ui->Gamma_hSlider->setEnabled(true);
    }

    if(obj == ui->Deskew_checkbox){
       ui->Deskew_checkbox->item_event(check, &sv->device_data.PaperDeskew);

    }else if(obj == ui->Ad_pages_checkbox){
        ui->Ad_pages_checkbox->item_event(check, &sv->device_data.Add_Pages);

    }else if(obj == ui->None_radioButton){
        if(check){
            ui->RemoveBack_comboBox->setEnabled(false);
            sv->device_data.BackgroundRemoval.select = BackgroundRemovalNone;
            ui->Dropout_comboBox->setEnabled(false);
            sv->device_data.DropoutColor.select = DropoutColorNone;
            ui->TextEnhance_comboBox->setEnabled(false);
            ui->TextEnhance_Setting_pushButton->setEnabled(false);
            sv->device_data.textEnhance.select = TextEnhanceNone;
        }

    }else if(obj == ui->RemoveBack_radioButton){
        if(check){
            sv->device_data.DropoutColor.select = DropoutColorNone;
            sv->device_data.textEnhance.select = TextEnhanceNone;
            if(sv->device_data.ColorType.select == ColorTypeRGB24  || sv->device_data.ColorType.select == ColorTypeRGB48){
                ui->RemoveBack_comboBox->set_enabled(check);
                sv->device_data.BackgroundRemoval.select = ui->RemoveBack_comboBox->current_item;
            }else if(sv->device_data.ColorType.select == ColorTypeMono8 || sv->device_data.ColorType.select == ColorTypeMono16){
                ui->RemoveBack_comboBox->set_enabled(false);
                sv->device_data.BackgroundRemoval.select = ui->RemoveBack_comboBox->current_item = BackgroundRemovalNormal;
            }else if(sv->device_data.ColorType.select == ColorTypeMono1 && sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
                ui->RemoveBack_comboBox->set_enabled(false);
                sv->device_data.BackgroundRemoval.select = ui->RemoveBack_comboBox->current_item = BackgroundRemovalNormal;
            }
        }else {
            ui->RemoveBack_comboBox->set_enabled(check);
        }

    }else if(obj == ui->Dropout_radioButton){
        if(check){
            sv->device_data.BackgroundRemoval.select = BackgroundRemovalNone;
            sv->device_data.textEnhance.select = TextEnhanceNone;
            if(sv->device_data.ColorType.select != ColorTypeRGB24 || sv->device_data.ColorType.select != ColorTypeRGB48){
                ui->Dropout_comboBox->set_enabled(check);
                sv->device_data.DropoutColor.select = ui->Dropout_comboBox->current_item;
            }
        }else {
            ui->Dropout_comboBox->set_enabled(check);
        }
    }else if(obj == ui->TextEnhance_radioButton){
        ui->TextEnhance_comboBox->set_enabled(check);
        if(check){
            if(sv->device_data.ColorType.select == ColorTypeMono1){
                sv->device_data.textEnhance.select = TextEnhanceLevelStandard;
            }else{
               sv->device_data.textEnhance.select = ui->TextEnhance_comboBox->current_item;
            }
            sv->device_data.BackgroundRemoval.select = BackgroundRemovalNone;
            sv->device_data.DropoutColor.select = DropoutColorNone;
        }
        ui->TextEnhance_Setting_pushButton->setEnabled(check);

        ui->Gamma_label->setEnabled(false);
        ui->Gamma_colon_label->setEnabled(false);
        ui->Gamma_doubleSpinBox->setEnabled(false);
        ui->Gamma_hSlider->setEnabled(false);
    }

    this->updateUI();
}

void MainWindow::EditFinishText(void){
    QObject* obj = sender();

    if(obj == ui->Prefix_lineEdit){
        sv->device_data.Prefix = (ui->Prefix_lineEdit->set_prefix(ui->Prefix_lineEdit->text()).toLocal8Bit()).constData();
    }

    this->updateUI();
}

bool MainWindow::SelectionRestrictions(int value){
    QObject* obj = sender();

    int limit_resolution = 0;
    int limit_max_resolution = 0;
    int limit_min_resolution = 0;
    if(this->GetHeightLimitLongPaper(600) != 0){
        limit_resolution = 600;
    }
    if(sv->device_data.ImageFormatSettings.OFDEnabled.select){
        limit_max_resolution = 600;
        limit_min_resolution = 200;
    }else {
        limit_resolution = 300;
    }

    if(obj == ui->Resolution_comb){
        if(ui->Document_size_comb->current_item == USER_DEFINE){
            return UserDefineSizeRestrictions();
        }else if(ui->Document_size_comb->current_item == Auto_Detect_long){
            // - 長尺自動で解像度が600dpiより大きい場合
            if(ui->Resolution_comb->currentText().toInt() > limit_resolution){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }else if(sv->device_data.ImageFormatSettings.OFDEnabled.select && (ui->Resolution_comb->currentText().toInt() > limit_max_resolution || ui->Resolution_comb->currentText().toInt() < limit_min_resolution))
        {
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_013, TranslationString::GetString().translation_str.VERSION_003, Warning);
            return false;
        }
    }else if(obj == ui->Document_size_comb){
        if(ui->Document_size_comb->itemData(value) == USER_DEFINE){
            return UserDefineSizeRestrictions();
        }else if(ui->Document_size_comb->itemData(value) == Auto_Detect_long){
            // - 長尺自動で解像度が600dpiより大きい場合
            if(ui->Resolution_comb->currentText().toInt() > limit_resolution){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }
    }else if(obj == ui->Scan_Button){
        if(ui->Document_size_comb->itemData(value) == USER_DEFINE){
            return UserDefineSizeRestrictions();
        }
    }
    return true;
}

bool MainWindow::UserDefineSizeRestrictions(void)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;
    int current_resolution = std::atoi((ui->Resolution_comb->currentText().toLocal8Bit()).constData());
    ui->Document_size_comb->update_base_resolution(current_resolution);

    double hardware_limit_value = sv->device_data.ScanArea.MaxScanAreaHeightInLongPaper.select / 100 * current_resolution;

    //Jsonテーブルが存在すればその値を使用する
    double long_paper_limit_value_600dpi = this->GetHeightLimitLongPaper(600) / 100 * current_resolution;
    double long_paper_limit_value_300dpi = this->GetHeightLimitLongPaper(300) / 100 * current_resolution;
    double long_paper_limit_value_200dpi = this->GetHeightLimitLongPaper(200) / 100 * current_resolution;

    double current_value = 0;
    double limit_value = 0;
    if(units == unit_inchi){
        current_value = ui->Document_size_comb->calculate_pixel_size_inchi(sv->device_data.ScanArea.UserDefine_Length);
    }else if(units == unit_mm){
        current_value = ui->Document_size_comb->calculate_pixel_size(sv->device_data.ScanArea.UserDefine_Length);
    }
    limit_value = ui->Document_size_comb->calculate_pixel_size_inchi(sv->device_data.ScanArea.MaxScanAreaHeight.select / 100);

    //Jsonテーブルがないとき
    if(long_paper_limit_value_600dpi == 0){
        if(sv->device_data.ScanArea.PaperEndDetection.select){
            if(current_resolution > 300){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }else if(sv->device_data.ImageFormatSettings.OFDEnabled.select && (ui->Resolution_comb->currentText().toInt() > 600 || ui->Resolution_comb->currentText().toInt() < 200)){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_013, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }else {
                return true;
            }
        }
        if(ui->Document_size_comb->calculate_pixel_size_inchi(240) >= hardware_limit_value){
            if(units == unit_inchi){
                long_paper_limit_value_300dpi = ui->Document_size_comb->calculate_pixel_size_inchi(215);
                long_paper_limit_value_200dpi = ui->Document_size_comb->calculate_pixel_size_inchi(240);
            }else if(units == unit_mm){
                long_paper_limit_value_300dpi = ui->Document_size_comb->calculate_pixel_size(5461);
                long_paper_limit_value_200dpi = ui->Document_size_comb->calculate_pixel_size(6096);
            }
            if(limit_value < current_value
                    && current_value <= long_paper_limit_value_300dpi){
                if(current_resolution > 300){
                    m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                    return false;
                }
            }
            if(sv->device_data.ImageFormatSettings.OFDEnabled.select && (ui->Resolution_comb->currentText().toInt() > 600 || ui->Resolution_comb->currentText().toInt() < 200)){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_013, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
            if(long_paper_limit_value_300dpi < current_value
                     && current_value <= long_paper_limit_value_200dpi){
                if(current_resolution > 200){
                    m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                    return false;
                }
            }
            if(long_paper_limit_value_200dpi == current_value){
                if(current_resolution > 200){
                    m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                    return false;
                }
            }
        }else {
            if(limit_value < current_value && current_resolution > 300){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
            if(sv->device_data.ImageFormatSettings.OFDEnabled.select && (ui->Resolution_comb->currentText().toInt() > 600 || ui->Resolution_comb->currentText().toInt() < 200)){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_013, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }
    }else {
        if(sv->device_data.ScanArea.PaperEndDetection.select){
            if(current_resolution > 600){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }else if(limit_value < current_value
                && current_value <= long_paper_limit_value_600dpi){
            if(current_resolution > 600){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }else if(sv->device_data.ImageFormatSettings.OFDEnabled.select && (ui->Resolution_comb->currentText().toInt() > 600 || ui->Resolution_comb->currentText().toInt() < 200)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_013, TranslationString::GetString().translation_str.VERSION_003, Warning);
            return false;
        }
        if(long_paper_limit_value_600dpi < current_value
                && current_value <= long_paper_limit_value_300dpi){
            if(current_resolution > 300){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }
        if(long_paper_limit_value_300dpi < current_value
                 && current_value <= long_paper_limit_value_200dpi){
            if(current_resolution > 200){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }
        if(long_paper_limit_value_200dpi == current_value){
            if(current_resolution > 200){
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }
    }
    return true;
}

bool MainWindow::CheckLongPaperMode(void)
{
    if(!sv->device_data.ScanArea.PaperEndDetection.select){
        Configuration& config = Configuration::GetConfiguration();
        Units units = config.config_para.units;

        double long_paper_limit_value = 0;
        double current_value = 0;
        double limit_value = 0;
        if(units == unit_inchi){
            long_paper_limit_value = ui->Document_size_comb->calculate_pixel_size_inchi(215);
            current_value = ui->Document_size_comb->calculate_pixel_size_inchi(sv->device_data.ScanArea.UserDefine_Length);
        }else if(units == unit_mm){
            long_paper_limit_value = ui->Document_size_comb->calculate_pixel_size(5461);
            current_value = ui->Document_size_comb->calculate_pixel_size(sv->device_data.ScanArea.UserDefine_Length);
        }
        limit_value = ui->Document_size_comb->calculate_pixel_size_inchi(sv->device_data.ScanArea.MaxScanAreaHeight.select / 100.0);

        if(limit_value < current_value && current_value <= long_paper_limit_value){
            return true;
        }else if(current_value > long_paper_limit_value){
            return true;
        }
    }
    return false;
}

void MainWindow::isManualResolution(int resolution)
{
    if(resolution == 0){
        if(ui->Resolution_comb->currentText().toInt() != 0){
            //手入力の解像度のときに値を保持する
            sv->device_data.Resolution.select = ui->Resolution_comb->currentText().toInt();
            //更新した解像度からスキャン領域を再計算する
            ui->Document_size_comb->base_resolution = sv->device_data.Resolution.select;
            ui->Document_size_comb->update_scanarea(&sv->device_data.ScanArea);
        }
    }else {
        //手入力の解像度のときに値を保持する
        sv->device_data.Resolution.select = resolution;
        //更新した解像度からスキャン領域を再計算する
        ui->Document_size_comb->base_resolution = sv->device_data.Resolution.select;
        ui->Document_size_comb->update_scanarea(&sv->device_data.ScanArea);
    }
}

void MainWindow::UpdateMarqueeSize_resolution(void)
{
    //解像度の更新に合わせてマーキー領域を再計算
    if(this->m_viewPreviewWindow == true){
        ui->Document_size_comb->setMarqueeSize(m_preview_window->UpdateMarqueeSizeLabel());
        ui->Document_size_comb->setMarqueeOffset(m_preview_window->UpdateMarqueeSizeLabel());

        ui->Document_size_comb->update_scanarea(&sv->device_data.ScanArea);
    }
}

void MainWindow::updateUI(){

    this->isManualResolution();

    sv->Set_Exclusion_Value();
    sv->Get_Exclusion_Value();

    //UI update
    ui->Document_source_comb->update_ui(sv->device_data.FunctionalUnit, sv->device_data.AutomaticFeedingMode.capability.supportLevel);
    ui->Scanning_side_comb->update_ui(&sv->device_data.DuplexType);
    ui->Document_type_comb->update_ui(&sv->device_data.documentType);
    if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        //接続デバイスがLFPだった場合
        ui->Document_size_comb->update_ui(sv->device_data, sv->device_data.Resolution.select, true);
    }else {
        ui->Document_size_comb->update_ui(sv->device_data, sv->device_data.Resolution.select, false);
    }
    ui->Image_type_comb->update_ui(sv->device_data.ColorType);
    if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        //接続デバイスがLFPだった場合
        ui->Resolution_comb->update_ui(sv->device_data.Resolution, true);
    }else {
        ui->Resolution_comb->update_ui(sv->device_data.Resolution, false);
    }
    ui->Rotate_comb->update_ui(sv->device_data.Orientation);
    ui->Deskew_checkbox->update_ui(sv->device_data.PaperDeskew);
    ui->Ad_pages_checkbox->update_ui();
    ui->Skip_blank_pages_comb->update_ui(&sv->device_data.BlankPageSkipKey);
    ui->Detect_double_feed_comb->update_ui(sv->device_data.DoubleFeedDetectionValue);

    ui->RemoveBack_comboBox->update_ui(sv->device_data.BackgroundRemoval);
    ui->Dropout_comboBox->update_ui(sv->device_data.DropoutColor);
    ui->TextEnhance_comboBox->update_ui(sv->device_data.textEnhance);

    ui->Brightness_hSlider->update_ui();
    ui->Contrast_hSlider->update_ui();
    ui->Gamma_hSlider->update_ui();
    ui->Threshold_hSlider->update_ui();

    if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        //接続デバイスがLFPだった場合
        ui->Image_format_comb->update_ui(sv->device_data.ImageFormatSettings, true);
    }else {
        ui->Image_format_comb->update_ui(sv->device_data.ImageFormatSettings, false);
    }
    ui->Folder_comb->update_ui();

    ui->Automatic_feeding_mode_comb->update_ui(sv->device_data.AutomaticFeedingMode, sv->device_data.use_continuousAutoFeedingMode);
    ui->Background_color_comb->update_ui(sv->device_data.BackgroundColor);

    ui->Device_selector_comb->update_device_list();


    ui->remove_punch_holes_comb->update_ui(sv->device_data.RemovePunchHoles,ui->remove_punch_holes_label, ui->remove_punch_holes_colon_label);
    ui->edge_correction_comb->update_ui(sv->device_data.DeficiencyCorrection,ui->edge_correction_label,ui->edge_correction_colon_label);

    this->ControlImageOption();
    //排他処理
    this->ChangeWidget(sv->device_data.ColorType.select);
    this->ControlDocumentSizeAvailable();
    this->ControlDropoutAvailable(sv->device_data.ColorType.select);
    this->ControlRemoveBackGroundAvailable(sv->device_data.ColorType.select);
    this->ControlTextEnhanceAvailable(sv->device_data.ColorType.select);
    this->ChangeExtension(ui->Image_format_comb->current_item);
    this->ControlPreviewButtonAvailable(ui->Document_size_comb->current_item);
    this->ControlAutoLongPaperDropOutAvailable(ui->Document_size_comb->current_item);
    this->ControlDetDoubleFeedAvailable(ui->Document_size_comb->current_item);
    this->ControlSkipBlankPagesAvailable(ui->Document_source_comb->current_item);
    this->ControlScanningSideAvailable();
    this->ControlDocumentTypeAvailable();
    this->ControlBackgroundColorAvailable();
    this->ControlAutomaticFeedingModeAvailable();
    this->ControlGammaAvabilable();
    this->AvailableControlLFPModel();
    //this->ControlSetDocumentAvailable(ui->Document_size_comb->current_item);
    this->ControlImageType();
    //this->ControlEdgeCorrection();

    this->CheckMarqueeEnebled();
}

void MainWindow::CheckMarqueeEnebled(void)
{
    //サイズ自動または両面設定時マーキー作成不可にする
    if(this->m_viewPreviewWindow == true){
        m_preview_window->getDrawingArea()->setMarqueeEnebled(true);
        if(ui->Document_size_comb->itemData(ui->Document_size_comb->currentIndex()) == Auto_Detect ||
                ui->Document_size_comb->itemData(ui->Document_size_comb->currentIndex()) == Auto_Detect_long){
            m_preview_window->getDrawingArea()->setMarqueeEnebled(false);
        }else if(sv->device_data.DuplexType.select){
            m_preview_window->getDrawingArea()->setMarqueeEnebled(false);
        }
    }
}

#include <QStandardItemModel>

void MainWindow::ControlScanningSideAvailable(void)
{
    if(ui->Scanning_side_comb->isEnabled()){
        ui->Scanning_side_label->setEnabled(true);
        ui->Scanning_side_colon_label->setEnabled(true);
    }else {
        ui->Scanning_side_label->setEnabled(false);
        ui->Scanning_side_colon_label->setEnabled(false);
    }

    if(ui->Scanning_side_comb->isHidden()){
        ui->Scanning_side_label->hide();
        ui->Scanning_side_colon_label->hide();
    }else {
        ui->Scanning_side_label->show();
        ui->Scanning_side_colon_label->show();
    }
}

void MainWindow::ControlDocumentTypeAvailable(void)
{
    if(ui->Document_type_comb->isEnabled()){
        ui->Document_type_label->setEnabled(true);
        ui->Document_type_colon_label->setEnabled(true);
    }else {
        ui->Document_type_label->setEnabled(false);
        ui->Document_type_colon_label->setEnabled(false);
    }

    if(ui->Document_type_comb->isHidden()){
        ui->Document_type_label->hide();
        ui->Document_type_colon_label->hide();
    }else {
        ui->Document_type_label->show();
        ui->Document_type_colon_label->show();
    }
}

void MainWindow::ControlDocumentSizeAvailable(void)
{
    if(ui->Document_source_comb->current_item == Auto){
        //コンボボックスの1アイテムのみグレーアウトする
        QStandardItemModel* model = dynamic_cast< QStandardItemModel * >( ui->Document_size_comb->model() );
        QStandardItem* item = model->item(ui->Document_size_comb->findData(Auto_Detect_long));
        if(item != 0x0){
            item->setEnabled( false );
            if(ui->Document_size_comb->current_item == Auto_Detect_long){
                ui->Document_size_comb->current_item = Auto_Detect;
                ui->Document_size_comb->setCurrentIndex(ui->Document_size_comb->findData(Auto_Detect));
            }
        }
    }
}

void MainWindow::ChangeExtension(ImageFormat imgformat){
    if(imgformat == ImageFormatPNM){
        ui->File_extension_label->setText(TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_001);
    }else if(imgformat == ImageFormatJPEG){
        ui->File_extension_label->setText(TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_002);
    }else if(imgformat == ImageFormatPNG){
        ui->File_extension_label->setText(TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_003);
    }else if(imgformat == ImageFormatTIFF || imgformat == ImageFormatMultiTIFF){
        ui->File_extension_label->setText(TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_004);
    }else if(imgformat == ImageFormatColorJpegMonoRaw){ // = PDF
        ui->File_extension_label->setText(TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_005);
    }else if(imgformat == ImageFormatOFD){
        ui->File_extension_label->setText("OFD");
    }
}

void MainWindow::ChangeWidget(SDIInt type){

    if(type == ColorTypeRGB24 || type == ColorTypeMono8 || type == ColorTypeAuto || type == ColorTypeRGB48 || type == ColorTypeMono16){
        ui->image_type_stackedWidget->setCurrentIndex(1);
    }else if(type == ColorTypeMono1){
        ui->image_type_stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::CreateComboBoxToolTip(QComboBox* obj, QString message)
{
    //コンボボックスが有効であるときにツールＴＩＰを表示させる
    QString qStr;
    if(obj->isEnabled()){
        qStr = message;
    }else {
        qStr = "";
    }
    obj->setToolTip(qStr);
}

void MainWindow::CreateRadioButtonToolTip(QRadioButton* obj, QString message)
{
    //コンボボックスが有効であるときにツールＴＩＰを表示させる
    QString qStr;
    if(obj->isEnabled()){
        qStr = message;
    }else {
        qStr = "";
    }
    obj->setToolTip(qStr);
}

void MainWindow::InitializeImageOption(SDIInt select)
{
    if(select == ImageOptionNone){
        ui->None_radioButton->setChecked(true);
    }else if(select == ImageOptionDropout){
        ui->Dropout_radioButton->setChecked(true);

    }else if(select == ImageOptionBackgroundRemoval){
        ui->RemoveBack_radioButton->setChecked(true);
    }else if(select == ImageOptionTextEnhance){
        ui->TextEnhance_radioButton->setChecked(true);
    }
}

void MainWindow::ControlImageOption()
{
    if(ui->None_radioButton->isChecked()){
        sv->device_data.ImageOption = ImageOptionNone;
        ui->TextEnhance_comboBox->set_enabled(false);
        ui->TextEnhance_Setting_pushButton->setEnabled(false);
    }else if(ui->Dropout_radioButton->isChecked()){
        sv->device_data.ImageOption = ImageOptionDropout;
    }else if(ui->RemoveBack_radioButton->isChecked()){
        sv->device_data.ImageOption = ImageOptionBackgroundRemoval;
    }else if(ui->TextEnhance_radioButton->isChecked()){
        sv->device_data.ImageOption = ImageOptionTextEnhance;
    }
}

void MainWindow::ControlImageType()
{
    ui->Image_type_comb->changed = true;
    if(sv->device_data.documentType.select != Reflective &&
            (sv->device_data.ColorType.select == ColorTypeAuto || sv->device_data.ColorType.select == ColorTypeMono1) )
    {
        sv->device_data.ColorType.select = ColorTypeRGB24;
    }
    ui->Image_type_comb->update_ui(sv->device_data.ColorType);
    this->ChangeWidget(sv->device_data.ColorType.select);
    
}

void MainWindow::ControlDropoutAvailable(SDIInt type)
{
    CreateComboBoxToolTip(ui->Dropout_comboBox, TranslationString::GetString().translation_str.ADVANCEDSETTING_072);

    if(sv->device_data.documentType.select == Color_Positive_Film
            || sv->device_data.documentType.select == Color_Negative_Film
            || sv->device_data.documentType.select == Mono_Negative_Film 
            || type == ColorTypeRGB48
            || type == ColorTypeMono16 )
    {
        ui->Dropout_radioButton->setEnabled(false);
        if(ui->Dropout_radioButton->isChecked()){
            ui->None_radioButton->setChecked(true);
            ui->Dropout_comboBox->setCurrentIndex(ui->Dropout_comboBox->findData(DropoutColorRed));
            ui->Dropout_comboBox->current_item = DropoutColorRed;
        }
    }else if(type == ColorTypeRGB24 || type == ColorTypeAuto){
        ui->Dropout_radioButton->setEnabled(false);
        if(ui->Dropout_radioButton->isChecked()){
            ui->None_radioButton->setChecked(true);
            ui->Dropout_comboBox->setCurrentIndex(ui->Dropout_comboBox->findData(DropoutColorRed));
            ui->Dropout_comboBox->current_item = DropoutColorRed;
        }
    }else if(type == ColorTypeMono1 || type == ColorTypeMono8){
        ui->Dropout_radioButton->setEnabled(true);
    }

    if(ui->Dropout_comboBox->isHidden()){
        ui->Dropout_radioButton->hide();
    }else {
        ui->Dropout_radioButton->show();
    }
}

void MainWindow::ControlRemoveBackGroundAvailable(SDIInt type)
{
    CreateComboBoxToolTip(ui->RemoveBack_comboBox, TranslationString::GetString().translation_str.ADVANCEDSETTING_070);

    if(sv->device_data.documentType.select == Color_Positive_Film
            || sv->device_data.documentType.select == Color_Negative_Film
            || sv->device_data.documentType.select == Mono_Negative_Film 
            || type == ColorTypeMono16 
            || type == ColorTypeRGB48 )
    {
        ui->RemoveBack_radioButton->setEnabled(false);
        if(ui->RemoveBack_radioButton->isChecked()){
            ui->None_radioButton->setChecked(true);
            ui->RemoveBack_comboBox->setCurrentIndex(ui->RemoveBack_comboBox->findData(BackgroundRemovalNormal));
            ui->RemoveBack_comboBox->current_item = BackgroundRemovalNormal;
        }
    }else if(type == ColorTypeMono1){
        if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
            //接続デバイスがLFPだった場合=Gray
            ui->RemoveBack_radioButton->setEnabled(true);
            ui->RemoveBack_comboBox->setCurrentIndex(ui->RemoveBack_comboBox->findData(BackgroundRemovalNormal));
            if(ui->Dropout_comboBox->isEnabled() || ui->RemoveBack_radioButton->isChecked()){
                sv->device_data.BackgroundRemoval.select = ui->RemoveBack_comboBox->current_item = BackgroundRemovalNormal;
            }
            if(ui->RemoveBack_radioButton->isChecked()){
                 ui->RemoveBack_comboBox->setEnabled(false);
            }
        }else {
            ui->RemoveBack_radioButton->setEnabled(false);
            if(ui->RemoveBack_radioButton->isChecked()){
                ui->None_radioButton->setChecked(true);
                ui->RemoveBack_comboBox->setCurrentIndex(ui->RemoveBack_comboBox->findData(BackgroundRemovalNormal));
                ui->RemoveBack_comboBox->current_item = BackgroundRemovalNormal;
            }
        }
    }else if(type == ColorTypeMono8 ){
        ui->RemoveBack_radioButton->setEnabled(true);
        ui->RemoveBack_comboBox->setCurrentIndex(ui->RemoveBack_comboBox->findData(BackgroundRemovalNormal));
        if(ui->Dropout_comboBox->isEnabled() || ui->RemoveBack_radioButton->isChecked()){
            sv->device_data.BackgroundRemoval.select = ui->RemoveBack_comboBox->current_item = BackgroundRemovalNormal;
        }
        if(ui->RemoveBack_radioButton->isChecked()){
             ui->RemoveBack_comboBox->setEnabled(false);
        }
    }else if(type == ColorTypeRGB24 || type == ColorTypeAuto ){
        ui->RemoveBack_radioButton->setEnabled(true);
        if(ui->RemoveBack_radioButton->isChecked()){
            ui->RemoveBack_comboBox->setEnabled(true);
        }

    }

    if(ui->RemoveBack_comboBox->isHidden()){
        ui->RemoveBack_radioButton->hide();
    }else {
        ui->Skip_blank_pages_label->show();
        ui->RemoveBack_radioButton->show();
    }
}

void MainWindow::ControlTextEnhanceAvailable(SDIInt type)
{
    if(type == ColorTypeMono16 
        || type == ColorTypeRGB48
        || sv->device_data.documentType.select == Color_Positive_Film
        || sv->device_data.documentType.select == Color_Negative_Film
        || sv->device_data.documentType.select == Mono_Negative_Film 
    )
    {
        ui->TextEnhance_comboBox->setEnabled(false);
        if(!ui->TextEnhance_comboBox->isEnabled()){
            ui->None_radioButton->setChecked(true);
            ui->TextEnhance_radioButton->setEnabled(false);
        }
    } else {
        ui->TextEnhance_radioButton->setEnabled(true);
        if(ui->TextEnhance_radioButton->isChecked()){
            ui->TextEnhance_comboBox->setEnabled(true);
        } else {
            ui->TextEnhance_comboBox->setEnabled(false);
        }
    }

    switch (type) {
        case ColorTypeMono1:
            ui->TextEnhance_stackedWidget->setCurrentIndex(1);
            break;
        case ColorTypeMono8:
        case ColorTypeRGB24:
        case ColorTypeAuto:
        case ColorTypeRGB48:
        case ColorTypeMono16:
            ui->TextEnhance_stackedWidget->setCurrentIndex(0);
            break;
    }

    if(ui->TextEnhance_comboBox->isHidden()){
        ui->TextEnhance_Setting_pushButton->hide();
        ui->TextEnhance_radioButton->hide();
    }else {
        ui->TextEnhance_Setting_pushButton->show();
        ui->TextEnhance_radioButton->show();
    }
}

void MainWindow::ControlGammaAvabilable(void)
{
    if (sv->device_data.ImageOption == ImageOptionTextEnhance)
    {
        ui->Gamma_label->setEnabled(false);
        ui->Gamma_colon_label->setEnabled(false);
        ui->Gamma_doubleSpinBox->setEnabled(false);
        ui->Gamma_hSlider->setEnabled(false);
    }else{
        ui->Gamma_label->setEnabled(true);
        ui->Gamma_colon_label->setEnabled(true);
        ui->Gamma_doubleSpinBox->setEnabled(true);
        ui->Gamma_hSlider->setEnabled(true);
    }
}

void MainWindow::ControlRemovePunchHoleAvailable(SDIInt type)
{

}

void MainWindow::ControlPreviewButtonAvailable(SDIInt type)
{
    //長尺サイズ時プレビューボタンをグレーアウト
    if(type == Auto_Detect_long || (type == USER_DEFINE && this->CheckLongPaperMode())){
        ui->Preview_Button->setEnabled(false);
    }else {
        ui->Preview_Button->setEnabled(true);
    }

    if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        //接続デバイスがLFPだった場合
        ui->Preview_Button->hide();
    }else {
        ui->Preview_Button->show();
    }
}

void MainWindow::ControlAutoLongPaperDropOutAvailable(SDIInt type)
{
    //長尺サイズ時Dropout_radioButtonをグレーアウト

    if(type == Auto_Detect_long || (type == USER_DEFINE && this->CheckLongPaperMode())){
        ui->Dropout_radioButton->setEnabled(false);
        if(ui->Dropout_radioButton->isChecked()){
            ui->None_radioButton->setChecked(true);
            ui->Dropout_comboBox->setCurrentIndex(ui->Dropout_comboBox->findData(DropoutColorRed));
            ui->Dropout_comboBox->current_item = DropoutColorRed;
        }
    }
}

void MainWindow::ControlDetDoubleFeedAvailable(SDIInt type)
{
    //プラスティックカード時重送検知をグレーアウト
    if(type == PlasticCard || type == PlasticCard_Landscape){
        ui->Detect_double_feed_comb->item_event(0, &sv->device_data.DoubleFeedDetectionValue);
        ui->Detect_double_feed_comb->setCurrentIndex(Off);
        ui->Detect_double_feed_comb->setEnabled(false);
    //長尺サイズ時重送検知をグレーアウト
    }else if(type == Auto_Detect_long || (type == USER_DEFINE && this->CheckLongPaperMode())){
        ui->Detect_double_feed_comb->item_event(0, &sv->device_data.DoubleFeedDetectionValue);
        ui->Detect_double_feed_comb->setCurrentIndex(Off);
        ui->Detect_double_feed_comb->setEnabled(false);
    }else if(sv->device_data.ScanArea.PaperEndDetection.select){
        ui->Detect_double_feed_comb->item_event(0, &sv->device_data.DoubleFeedDetectionValue);
        ui->Detect_double_feed_comb->setCurrentIndex(Off);
        ui->Detect_double_feed_comb->setEnabled(false);
    }else {
        if(ui->Document_source_comb->current_item != Flatbed){
            ui->Detect_double_feed_comb->setEnabled(true);
        }
    }

    //ラベルの表示グレーアウトの排他条件は対になるコンボボックスの表示状況で決める
    if(ui->Detect_double_feed_comb->isEnabled()){
        ui->Detect_double_page_label->setEnabled(true);
        ui->Detect_double_page_colon_label->setEnabled(true);
    }else {
        ui->Detect_double_page_label->setEnabled(false);
        ui->Detect_double_page_colon_label->setEnabled(false);
    }

    if(ui->Detect_double_feed_comb->isHidden()){
        ui->Detect_double_page_label->hide();
        ui->Detect_double_page_colon_label->hide();
    }else {
        ui->Detect_double_page_label->show();
        ui->Detect_double_page_colon_label->show();
    }
}

void MainWindow::ControlSetDocumentAvailable(SDIInt type)
{
    if(type == Auto_Detect || type == Maximum_Size){
        ui->Horizontal_doc_toolButton->setEnabled(false);
        ui->Vertical_doc_toolButton->setEnabled(false);
        ui->Horizontal_doc_toolButton->setChecked(false);
        ui->Vertical_doc_toolButton->setChecked(false);
    }else {
        //原稿向きサポート機種か判定
        //TBD
        ui->Horizontal_doc_toolButton->setEnabled(true);
        ui->Vertical_doc_toolButton->setEnabled(true);
        //選択はデフォルトに戻る
        //TBD
        ui->Horizontal_doc_toolButton->setChecked(true);
        ui->Vertical_doc_toolButton->setChecked(false);
    }
}

void MainWindow::ControlSkipBlankPagesAvailable(SDIInt type)
{
    //取り込み装置がFBの場合、白紙ページ除去をグレーアウトする
    /*if(type == Flatbed){
        ui->Skip_blank_pages_label->setEnabled(false);
        //ui->Skip_blank_pages_comb->setEnabled(false);
        //ui->Skip_blank_pages_comb->current_item = Off;
        //ui->Skip_blank_pages_comb->changed = true;
    }else {
        ui->Skip_blank_pages_label->setEnabled(true);
        //ui->Skip_blank_pages_comb->setEnabled(true);
    }*/

    //ラベルの表示グレーアウトの排他条件は対になるコンボボックスの表示状況で決める
    if(ui->Skip_blank_pages_comb->isEnabled()){
        ui->Skip_blank_pages_label->setEnabled(true);
        ui->Skip_blank_pages_colon_label->setEnabled(true);
    }else {
        ui->Skip_blank_pages_label->setEnabled(false);
        ui->Skip_blank_pages_colon_label->setEnabled(false);
    }

    if(ui->Skip_blank_pages_comb->isHidden()){
        ui->Skip_blank_pages_label->hide();
        ui->Skip_blank_pages_colon_label->hide();
    }else {
        ui->Skip_blank_pages_label->show();
        ui->Skip_blank_pages_colon_label->show();
    }

    //LFPモデルは白紙ページ除去非サポート
    if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        ui->Skip_blank_pages_label->hide();
        ui->Skip_blank_pages_colon_label->hide();
    }
}

void MainWindow::ControlBackgroundColorAvailable()
{
    //原稿サイズが自動または自動（長尺）が選択されている場合は背景色は[黒固定]で選択不可に
    if((ui->Document_size_comb->current_item == Auto_Detect_long || ui->Document_size_comb->current_item == Auto_Detect)
            && sv->device_data.AutoCroppingInscribed.capability.supportLevel != kSDISupportLevelAvailable ){
        ui->Background_color_comb->setCurrentIndex(ui->Background_color_comb->findData(BackgroundColorBlack));
        sv->device_data.BackgroundColor.select = ui->Background_color_comb->current_item = BackgroundColorBlack;
        ui->Background_color_comb->setEnabled(false);
        goto control;
    }else {
        ui->Background_color_comb->setEnabled(true);
    }
    //取り込み装置が原稿台の場合、背景色は[白固定]で選択不可に
    if(sv->device_data.FunctionalUnit.select == Flatbed){
        ui->Background_color_comb->setCurrentIndex(ui->Background_color_comb->findData(BackgroundColorWhite));
        sv->device_data.BackgroundColor.select = ui->Background_color_comb->current_item = BackgroundColorWhite;
        ui->Background_color_comb->setEnabled(false);
        goto control;
    }else {
        ui->Background_color_comb->setEnabled(true);
    }

control:
    //ラベルの表示グレーアウトの排他条件は対になるコンボボックスの表示状況で決める
    if(ui->Background_color_comb->isEnabled()){
        ui->Background_color_label->setEnabled(true);
        ui->Background_color_colon_label->setEnabled(true);
    }else {
        ui->Background_color_label->setEnabled(false);
        ui->Background_color_colon_label->setEnabled(false);
    }

    if(ui->Background_color_comb->isHidden()){
        ui->Background_color_label->hide();
        ui->Background_color_colon_label->hide();
    }else {
        ui->Background_color_label->show();
        ui->Background_color_colon_label->show();
    }
}

void MainWindow::ControlAutomaticFeedingModeAvailable()
{
    //原稿台時はAFMをグレーアウト
    if(sv->device_data.FunctionalUnit.select == Flatbed || sv->device_data.FunctionalUnit_Auto){
        sv->device_data.AutomaticFeedingMode.select = false;
        ui->Automatic_feeding_mode_comb->changed = true;
        ui->Automatic_feeding_mode_comb->update_ui(sv->device_data.AutomaticFeedingMode, sv->device_data.use_continuousAutoFeedingMode);
        ui->Automatic_feeding_mode_comb->setEnabled(false);
    }else {
        ui->Automatic_feeding_mode_comb->setEnabled(true);
    }


    //AFM時はページ追加は無効に
    if(sv->device_data.AutomaticFeedingMode.select){
       ui->Ad_pages_checkbox->setChecked(false);
       ui->Ad_pages_checkbox->setEnabled(false);
       sv->device_data.Add_Pages = false;
    }else {
        ui->Ad_pages_checkbox->setEnabled(true);
    }

    //ラベルの表示グレーアウトの排他条件は対になるコンボボックスの表示状況で決める
    if(ui->Automatic_feeding_mode_comb->isEnabled()){
        ui->Automatic_feeding_mode_label->setEnabled(true);
        ui->Automatic_feeding_mode_colon_label->setEnabled(true);
    }else {
        ui->Automatic_feeding_mode_label->setEnabled(false);
        ui->Automatic_feeding_mode_colon_label->setEnabled(false);
    }

    if(ui->Automatic_feeding_mode_comb->isHidden()){
        ui->Automatic_feeding_mode_label->hide();
        ui->Automatic_feeding_mode_colon_label->hide();
    }else {
        ui->Automatic_feeding_mode_label->show();
        ui->Automatic_feeding_mode_colon_label->show();
    }
}

void MainWindow::AvailableControlLFPModel()
{
    if(sv->device_data.ScannerKind.select == kSDIKindMFLargeFormat){
        //LFPデバイスの場合ページ追加・編集を非表示にする
        ui->Ad_pages_checkbox->hide();
        sv->device_data.Add_Pages = false;
    }else {
        ui->Ad_pages_checkbox->show();
    }
}

bool MainWindow::CheckAutoFunctionalUnit(bool isPreview)
{
    if(this->m_viewPreviewWindow){
        if(isPreview && sv->device_data.FunctionalUnit_Auto){
            if(sv->GetPaperLoadedStatus()){
                //adf scan
                if(sv->m_Preview_FunctionalUnit == kSDIFunctionalUnitDocumentFeeder){
                    sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
                }else {
                    if(m_messageBox->CreateSelectableMessageBox(TranslationString::GetString().translation_str.C_UI_005, Warning)){
                        //Yes:プレビューウィンドウは削除する
                        this->m_preview_window->on_Close_Button_pressed(false);
                        //マーキー情報をリセットする
                        MarqueeCreated(false, QRect(0,0,0,0));
                        this->ChangePreviewParameter(false);
                        sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
                    }else {
                        //No:プレビューウィンドウはそのままにプレビュー処理を実行しない
                        return false;
                    }
                }
            }else {
                //fb scan
                if(sv->m_Preview_FunctionalUnit == kSDIFunctionalUnitFlatbed){
                    sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
                }else {
                    if(m_messageBox->CreateSelectableMessageBox(TranslationString::GetString().translation_str.C_UI_005, Warning)){
                        //Yes:プレビューウィンドウは削除する
                        this->m_preview_window->on_Close_Button_pressed(false);
                        //マーキー情報をリセットする
                        MarqueeCreated(false, QRect(0,0,0,0));
                        this->ChangePreviewParameter(false);
                        sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
                    }else {
                        //No:プレビューウィンドウはそのままにプレビュー処理を実行しない
                        return false;
                    }
                }
            }
        }else {
            sv->device_data.FunctionalUnit.select = sv->m_Preview_FunctionalUnit;
        }
    }else {
        //取り込み装置自動時
        if(sv->device_data.FunctionalUnit_Auto){
            if(sv->GetPaperLoadedStatus()){
                //adf scan
                sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
            }else {
                //fb scan
                sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
            }
        }
    }
    return true;
}

bool MainWindow::CheckBeforeScan(bool isPreview)
{
    //デバイスの姿勢が不定状態チャック
    if(sv->GetScannerPosition()){
        sv->m_LastError = kSDIErrorDeviceFormUnstable;
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_109, TranslationString::GetString().translation_str.VERSION_003, Critical);
        return false;
    }
    //スキャナの姿勢と分離レバー組み合わせチャック
    if(sv->GetUnscannebleScanParameterStatus()){
        sv->m_LastError = kSDIErrorUnscannableDeviceConfig1;
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_107, TranslationString::GetString().translation_str.VERSION_003, Critical);
        return false;
    }

    //姿勢変更しないモデルでの分離レバー組み合わせチェック
    if(sv->device_data.use_continuousAutoFeedingMode){
        if(sv->GetUnscannebleScanParameterForAFMCStatus()){
            sv->m_LastError = kSDIErrorUnscannableDeviceConfig1;
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_107, TranslationString::GetString().translation_str.VERSION_003, Critical);
            return false;
        }
     }

    //事前に紙詰まりをチェック
    if(sv->CheckDeviceErrorStatus() == kSDIErrorPaperJam){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_103, TranslationString::GetString().translation_str.VERSION_003, Critical);
        return false;
    }

    if(!sv->device_data.AutomaticFeedingMode.select){
        //事前紙なしエラーCheck
        if(sv->device_data.FunctionalUnit.select == kSDIFunctionalUnitDocumentFeeder){
            if(!sv->GetPaperLoadedStatus()){
                //紙なしエラー
                sv->m_LastError = kSDIErrorPaperEmpty;
                m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SCAN_101, TranslationString::GetString().translation_str.VERSION_003, Warning);
                return false;
            }
        }
    }

    //文字くっきり600dpiより大きいエラー
    if(sv->device_data.textEnhance.select != 0){
        if(sv->device_data.Resolution.select > 600){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_005, TranslationString::GetString().translation_str.VERSION_003, Warning);
            return false;
        }
    }

    //スキャン直前の確認処理
    // - 分離/非分離の切り替えレバーを持つ機種の確認ダイアログ表示
    // - カードレバーを持つ機種の確認ダイアログ表示

    // - 画像回転：文字向きに合わせるの解像度制限(600dpi < x)

    if(!this->SelectionRestrictions(ui->Document_size_comb->findData(ui->Document_size_comb->current_item))){
        // - 長尺スキャン時の解像度制限(215～in：200dpi ～215in：300dpi)+超える場合のエラーメッセージ
        return false;
    }
    // - ADFでサポートしない原稿サイズ
    // - 領域が広すぎる場合
    if(sv->device_data.ScanArea.AutoSize.select == kSDIAutoSizeNone && sv->device_data.ScanArea.ScanAreaWidth.select > 21000){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_001, TranslationString::GetString().translation_str.VERSION_003, Warning);
        return false;
    }
    // - 長尺自動で解像度が600dpiより大きい場合
    int limit_resolution = 0;
    if(this->GetHeightLimitLongPaper(600) != 0){
        limit_resolution = 600;
    }else {
        limit_resolution = 300;
    }
    if(sv->device_data.ScanArea.AutoSize.select == kSDIAutoSizeLongPaper && sv->device_data.Resolution.select > limit_resolution){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_011, TranslationString::GetString().translation_str.VERSION_003, Warning);
        return false;
    }
    // - OFDで解像度が200dpi未満 or 600dpiを超える場合
    if(sv->device_data.ImageFormatSettings.ImageFormat.select == ImageFormatPNM && sv->device_data.ImageFormatSettings.OFDEnabled.select && (sv->device_data.Resolution.select > 600 || sv->device_data.Resolution.select < 200) && !isPreview){
        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_013, TranslationString::GetString().translation_str.VERSION_003, Warning);
        return false;
    }

    // - 領域が狭すぎる場合
    int temp_width = ui->Document_size_comb->calculate_pixel_size(common_utility.PixelToMm(sv->device_data.ScanArea.ScanAreaWidth.select, sv->device_data.Resolution.select), 100);
    int temp_height = ui->Document_size_comb->calculate_pixel_size(common_utility.PixelToMm(sv->device_data.ScanArea.ScanAreaHeight.select, sv->device_data.Resolution.select), 100);
    if(sv->device_data.ScanArea.AutoSize.select == kSDIAutoSizeNone && sv->device_data.FunctionalUnit.select == kSDIFunctionalUnitDocumentFeeder){
        if(temp_width < sv->device_data.ScanArea.ADFWidthMin.select ||
                (temp_height < sv->device_data.ScanArea.ADFHeightMin.select && !sv->device_data.ScanArea.PaperEndDetection.select)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_PARA_003, TranslationString::GetString().translation_str.VERSION_003, Critical);
            return false;
        }
    }
    // - 長尺スキャンとなる原稿サイズが選択されてた場合のポップアップメッセージ表示
    if(!sv->device_data.DNShow_LongPaperWarning_dialog){
        if(ui->Document_size_comb->current_item == USER_DEFINE){
            if(this->CheckLongPaperMode()){
                sv->device_data.DNShow_LongPaperWarning_dialog = m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UI_005, TranslationString::GetString().translation_str.VERSION_003, Information, true);
            }
        }
        if(ui->Document_size_comb->current_item == Auto_Detect_long){
            sv->device_data.DNShow_LongPaperWarning_dialog = m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UI_005, TranslationString::GetString().translation_str.VERSION_003, Information, true);
        }
    }
    // - バッテリー状態を取得(TBD)

    // - 取り込み装置自動検知時のFB/ADF選択

    return true;
}

//解像度によって長尺時の最大読み取り高さの値を返す
qint32 MainWindow::GetHeightLimitLongPaper(int current_resolution)
{
    qint32 limit_length = 0;
    //std::cout << sv->device_data.MaxScanArea_height_table.c_str() << std::endl;

    if(sv->device_data.MaxScanArea_height_table != ""){
        QJsonDocument d = QJsonDocument::fromJson(sv->device_data.MaxScanArea_height_table.c_str());
        QJsonArray array = d.array();
        foreach(QJsonValue value, array){
            QJsonObject obj = value.toObject();
            QJsonObject res_array = obj.value("Resolution").toObject();
            qint32 resolution = res_array.value("int").toInt();
            if(resolution >= current_resolution){
                QJsonObject len_array = obj.value("Length").toObject();
                limit_length = len_array.value("int").toInt();
                break;
            }
        }
    }else {
        limit_length = 0;
    }
    return limit_length;
}

void MainWindow::on_Scan_Button_pressed()
{
    //フォーカスを外す
    ui->Scan_Button->setEnabled(false);
    ui->Scan_Button->setEnabled(true);
    ui->Resolution_comb->setEnabled(false);
    ui->Resolution_comb->setEnabled(true);
    ui->Prefix_lineEdit->setEnabled(false);
    ui->Prefix_lineEdit->setEnabled(true);

    this->ValidateOverRollerLifeLimit();

    sv->device_data.PagesTobeScanned.select = 0;

    this->isManualResolution();

    //解像度の更新に合わせてマーキー領域を再計算
    this->UpdateMarqueeSize_resolution();

    this->CheckAutoFunctionalUnit(false);
    if(this->CheckBeforeScan(false)){
        ui->Prefix_lineEdit->get_prefix(&sv->device_data.Prefix);

        ui->Scan_Button->Button_pressed(sv, ui->Ad_pages_checkbox->isChecked());
    }
    if(sv->scanning_status == ERROR){
        //スキャン中に通信エラーが発生した場合は、Epson Scan 2を終了する
        this->close();
    }

    //Update Roller counter
    sv->Get_Counter_Value();
}

void MainWindow::MarqueeCreated(bool exist, QRect MarqueeSize)
{
    if(exist){
        //原稿サイズのリスト更新
        ui->Document_size_comb->ExistMarquee();
        //読み取り領域の更新
        //qDebug() << "MarqueeSize" << MarqueeSize;
        ui->Document_size_comb->setMarqueeSize(MarqueeSize);
        ui->Document_size_comb->setMarqueeOffset(MarqueeSize);
        sv->device_data.ScanArea.ScanAreaOffsetX.select = (ui->Document_size_comb->previewOffsetXAsInch(
                                                               this->m_viewPreviewWindow,
                                                               (GuidePosition)sv->device_data.GuidePosition.select,
                                                               m_preview_window->getPreviewImage().width(), (FunctionalUnit)sv->device_data.FunctionalUnit.select)) * sv->device_data.Resolution.select;
        sv->device_data.ScanArea.ScanAreaOffsetX.select += ui->Document_size_comb->getMarqueeOffset().x();
        sv->device_data.ScanArea.ScanAreaOffsetY.select = ui->Document_size_comb->getMarqueeOffset().y();
    }else {
        //原稿サイズのリストを元に戻す
        ui->Document_size_comb->changed = true;
        ui->Document_size_comb->setItemData(1, 0, Auto_Detect);
        ui->Document_size_comb->setCurrentIndex(1);
        this->updateUI();
        sv->device_data.ScanArea.ScanAreaOffsetX.select = 0;
        sv->device_data.ScanArea.ScanAreaOffsetY.select = 0;
    }
}


void MainWindow::on_Preview_Button_pressed()
{
    //フォーカスを外す
    ui->Preview_Button->setEnabled(false);
    ui->Preview_Button->setEnabled(true);
    ui->Resolution_comb->setEnabled(false);
    ui->Resolution_comb->setEnabled(true);
    ui->Prefix_lineEdit->setEnabled(false);
    ui->Prefix_lineEdit->setEnabled(true);

    this->ValidateOverRollerLifeLimit();

    //this->ChangePreviewParameter(true);

    if(this->CheckAutoFunctionalUnit(true)){
        this->ChangePreviewParameter(true);
        sv->m_Preview_FunctionalUnit = (FunctionalUnit)sv->device_data.FunctionalUnit.select;
        if(this->CheckBeforeScan(true)){
            ui->Preview_Button->Button_pressed(sv);
        }else {
            sv->device_data.ImageFormatSettings.ImageFormat.select = sv->m_PreserveImageFormat;
            return;
        }
        if(sv->scanning_status == ERROR){
            //スキャン中に通信エラーが発生した場合は、Epson Scan 2を終了する
            this->close();
        }

        sv->device_data.ImageFormatSettings.ImageFormat.select = sv->m_PreserveImageFormat;
        if(sv->scanning_status != CANCEL){
            if(m_preview_window == nullptr){
                m_preview_window = new PreviewWindow(sv);
            }
            if(this->m_viewPreviewWindow == false){
                m_preview_window->initialize(this);
            }
            this->ChangePreviewParameter(false);

            if(m_preview_window->DrawImage()){
                if(this->m_viewPreviewWindow == false){
                    m_preview_window->getDrawingArea()->SetMarqueeCreatedCallBack(
                                                    std::bind(
                                                    &MainWindow::MarqueeCreated,
                                                    this,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2));
                }
                this->m_viewPreviewWindow = true;
                this->CheckMarqueeEnebled();
            }else {
                return;
            }
        }else {
            this->ChangePreviewParameter(false);
        }

        //Update Roller counter
        sv->Get_Counter_Value();
    }

}

void MainWindow::ChangePreviewParameter(bool start)
{
    if(start){
        //プレビュー時の設定を適用
        MarqueeCreated(false, QRect(0,0,0,0));
        Configuration::GetConfiguration().config_para.preview_immediately_resolution = Configuration::GetConfiguration().config_para.preview_resolution;
        this->isManualResolution(Configuration::GetConfiguration().config_para.preview_resolution);
        //int res = Configuration::GetConfiguration().config_para.preview_resolution;
        sv->device_data.PagesTobeScanned.select = 1;
        sv->device_data.Orientation.select = OrientationNone;
        sv->m_PreserveImageFormat = (ImageFormat)sv->device_data.ImageFormatSettings.ImageFormat.select;
        sv->device_data.ImageFormatSettings.ImageFormat.select = kSDIImageFormatPNM;
    }else {
        //プレビュー用になっていた設定をもとに戻す
        this->isManualResolution();
        sv->device_data.PagesTobeScanned.select = 0;
        sv->device_data.Orientation.select = (SDIInt)ui->Rotate_comb->getCurrentItem();
    }
}

void MainWindow::onSave_Button_pressed()
{
    QObject* obj = sender();

    if(obj == ui->Save_pushButton){
        m_last_used_settings_controller->CreateSettingsDict(sv->setting_file_path, sv->device_data);
    }
}

void MainWindow::onConfiguration_Button_pressed()
{
    sv->Get_Counter_Value();

    configuration_dialog->Initialize();
    configuration_dialog->UpdateCounterDisplay(sv->device_data.ConfigureCounter);
    configuration_dialog->UpdateInformationDisplay(sv->device_data);
    configuration_dialog->DisplayItem();

    if(configuration_dialog->isHidden()){
        configuration_dialog->exec();
    }else {
        configuration_dialog->activateWindow();
    }

    ui->Document_size_comb->item_event(-1, &sv->device_data.ScanArea, &sv->device_data.AutoCroppingInscribed);

    //configuration_dialog->SetAllValue(sv);
    CONFIGURATION config_val;
    Configuration& config = Configuration::GetConfiguration();
    config_val = config.config_para;


    //設定確定時の管理者ロック確認
    if(sv->Check_SupportAdministratorRestricted()){
        if(!configuration_dialog->CheckUnlockButton_Enabled()){
            configuration_dialog->Check_AdminPassword();
        }

        if(sv->device_data.admin_status == AdminLockOff){
            //デバイス自体の設定をデバイス自体に保存する
            sv->device_data.AutoPowerOffTime.select = config_val.ac_power;
            sv->device_data.AutoPowerOffTimeBattery.select = config_val.usb_power;
            sv->device_data.SensorGlassDirtSensitivity.select = config.config_para.sensor_glass_dirtsensitivity;
            sv->device_data.ADFPaperProtection.select = config.config_para.adf_paper_protection;
            sv->device_data.BehaviorWhenDoubleFeed.select = config.config_para.behavior_when_doublefeed;
            sv->device_data.NonConnectPowerOff.select = config.config_para.non_connect_poweroff;
            sv->device_data.DirectPowerOn.select = config.config_para.direct_power_on;

            sv->device_data.ConfigureCounter.RollerKitCounter.select = config_val.roller_kit_counter;
            sv->device_data.ConfigureCounter.CleaningWarningCounter.select = config_val.cleaning_warning_counter;
        }else {
            //ロック解除できない場合は設定をもとに戻す
            config_val.ac_power = (Time)sv->device_data.AutoPowerOffTime.select;
            config_val.usb_power = (Time)sv->device_data.AutoPowerOffTimeBattery.select;
            config.config_para.sensor_glass_dirtsensitivity = (SensorGlassDirtSensitivity)sv->device_data.SensorGlassDirtSensitivity.select;
            config.config_para.adf_paper_protection = (ADFPaperProtection)sv->device_data.ADFPaperProtection.select;
            config.config_para.behavior_when_doublefeed = (BehaviorWhenDoubleFeed)sv->device_data.BehaviorWhenDoubleFeed.select;
            config.config_para.non_connect_poweroff = (NonConnectPowerOff)sv->device_data.NonConnectPowerOff.select;
            config.config_para.direct_power_on = (DirectPowerOn)sv->device_data.DirectPowerOn.select;

            config_val.roller_kit_counter = sv->device_data.ConfigureCounter.RollerKitCounter.select;
            config_val.cleaning_warning_counter = sv->device_data.ConfigureCounter.CleaningWarningCounter.select;
        }
    }else {
        //デバイス自体の設定をデバイス自体に保存する
        sv->device_data.AutoPowerOffTime.select = config_val.ac_power;
        sv->device_data.AutoPowerOffTimeBattery.select = config_val.usb_power;
        sv->device_data.SensorGlassDirtSensitivity.select = config.config_para.sensor_glass_dirtsensitivity;
        sv->device_data.ADFPaperProtection.select = config.config_para.adf_paper_protection;
        sv->device_data.BehaviorWhenDoubleFeed.select = config.config_para.behavior_when_doublefeed;
        sv->device_data.NonConnectPowerOff.select = config.config_para.non_connect_poweroff;
        sv->device_data.DirectPowerOn.select = config.config_para.direct_power_on;

        sv->device_data.ConfigureCounter.RollerKitCounter.select = config_val.roller_kit_counter;
        sv->device_data.ConfigureCounter.CleaningWarningCounter.select = config_val.cleaning_warning_counter;
    }

    sv->device_data.TransferCompression.select = config_val.compression;

    if(this->m_viewPreviewWindow == true){
        m_preview_window->UpdateMarqueeSizeLabel();
    }

    sv->bAllReset = configuration_dialog->Get_ResetFlag();

    if(configuration_dialog->Get_CounterResetFlag()){
        sv->Reset_RollerCouunter_Value();
    }
    sv->Set_RollerCouunter_Value();

    if(sv->bAllReset){
        //TBD:警告ダイアログの次回から表示しないフラグをオールリセット
        //環境設定のリセット時、Epson Scan 2を終了する
        this->close();
    }

    this->updateUI();
}

#include "savedestfolder.h"

void MainWindow::onFolder_Button_pressed()
{
    QObject* obj = sender();

    if(obj == ui->Folder_toolButton){
        //保存先のディレクトリが存在しているか確認する
        const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFile(sv->device_data.SelectPath);
        if (result) {
            //ボタンを押すたびにすでに開いているディレクトリでも何度も追加で開く
            QUrl url = QUrl::fromLocalFile( sv->device_data.SelectPath.c_str() );
            QDesktopServices::openUrl(url);
        }else {
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SAVE_004, TranslationString::GetString().translation_str.VERSION_003, Warning);
        }
    }
}

void MainWindow::onFileNameSetting_Button_pressed()
{
    QObject* obj = sender();

    if(obj == ui->FileNameSetting_toolButton){
        FileNameSettingDialog* filenamesetting_dialog = new FileNameSettingDialog;
        if(filenamesetting_dialog == NULL){
            return;
        }
        filenamesetting_dialog->Initialize(sv->device_data);
        if(filenamesetting_dialog->isHidden()){
            filenamesetting_dialog->exec();
        }else {
            filenamesetting_dialog->activateWindow();
        }

        sv->device_data.FileName_Counter = filenamesetting_dialog->getFileName_Counter();
        sv->device_data.FileName_OverWrite = filenamesetting_dialog->getFileName_OverWrite();

        delete filenamesetting_dialog;
        filenamesetting_dialog = NULL;
    }
}

void MainWindow::onAbout_Epson_Scan2_pressed()
{
    AboutEpsonScan2Dialog* about_dialog = new AboutEpsonScan2Dialog;
    if(about_dialog == NULL){
        return;
    }
    about_dialog->Initialize();
    if(about_dialog->isHidden()){
        about_dialog->exec();
    }else {
        about_dialog->activateWindow();
    }
}

void MainWindow::onHelp_Button_pressed()
{
    QString link;
    //日本環境以外は英語マニュアルを表示する
#ifdef HELP_URL
    link = HELP_URL;
#else
    if(QLocale::system().name() == "ja_JP"){
        link = "http://download.ebz.epson.net/man_j/linux/epsonscan2_j.html";
    }else {
        link = "http://download.ebz.epson.net/man/linux/epsonscan2_e.html";
    }
#endif
    QDesktopServices::openUrl(QUrl(link));
}


void MainWindow::ValidateOverRollerLifeLimit()
{
    //ローラーキットカウンターサポート
    if(sv->device_data.ConfigureCounter.RollerKitCounter.capability.supportLevel != kSDISupportLevelNone){
        if(configuration_dialog->CheckOverRollerKitLifeLimit(sv->device_data.ConfigureCounter, &sv->device_data.OverDurationAlert)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UNIT_001, TranslationString::GetString().translation_str.VERSION_003, Information);
            sv->device_data.NearDurationAlert = true;
        }else if(configuration_dialog->CheckNearRollerKitLifeLimit(sv->device_data.ConfigureCounter, &sv->device_data.NearDurationAlert)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UNIT_009, TranslationString::GetString().translation_str.VERSION_003, Information);
        }
    }else if(sv->device_data.ConfigureCounter.RollerCounter.capability.supportLevel != kSDISupportLevelNone){
    //ローラーサポート
        if(configuration_dialog->CheckOverRollerLifeLimit(sv->device_data.ConfigureCounter, &sv->device_data.OverDurationAlert)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UNIT_001, TranslationString::GetString().translation_str.VERSION_003, Information);
            sv->device_data.NearDurationAlert = true;
        }else if(configuration_dialog->CheckNearRollerLifeLimit(sv->device_data.ConfigureCounter, &sv->device_data.NearDurationAlert)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UNIT_009, TranslationString::GetString().translation_str.VERSION_003, Information);
        }
        if(configuration_dialog->CheckOverRetardRollerLifeLimit(sv->device_data.ConfigureCounter, &sv->device_data.OverDurationAlert)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UNIT_001, TranslationString::GetString().translation_str.VERSION_003, Information);
            sv->device_data.NearDurationAlert = true;
        }else if(configuration_dialog->CheckNearRetardRollerLifeLimit(sv->device_data.ConfigureCounter, &sv->device_data.NearDurationAlert)){
            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_UNIT_009, TranslationString::GetString().translation_str.VERSION_003, Information);
        }
    }
}

void MainWindow::onTextEnhanceSetting_Button_pressed()
{
    TextEnhanceSettingDialog* textenhancesetting_dialog = new TextEnhanceSettingDialog(sv->device_data);
    if(textenhancesetting_dialog == NULL){
        return;
    }
    if(textenhancesetting_dialog->isHidden()){
        textenhancesetting_dialog->exec();
    }else {
        textenhancesetting_dialog->activateWindow();
    }
    textenhancesetting_dialog->SetValue();
    textenhancesetting_dialog->GetValue(&sv->device_data);

    delete textenhancesetting_dialog;
    textenhancesetting_dialog = NULL;
}
