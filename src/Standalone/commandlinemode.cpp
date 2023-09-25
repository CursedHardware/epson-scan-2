#include "commandlinemode.h"
#include "devicelist.h"
#include "PathUtils.h"
#include "lastusedsettings.h"
#include "../Include/Config.h"
#ifndef NO_QTMODE
#include <QStandardPaths>
#endif
#include "savedestfolder.h"

#include <signal.h>

//Option list
#define OPTION_HELP "--help"
#define OPTION_HELP_S "-h"
#define OPTION_LIST "--list"
#define OPTION_LIST_S "-l"
#define OPTION_SET_IP "--set-ip"
#define OPTION_SET_IP_S "-i"
#define OPTION_SCAN "--scan"
#define OPTION_SCAN_S "-s"
#define OPTION_CREATE "--create"
#define OPTION_CREATE_S "-c"
#define OPTION_EDIT "--edit"
#define OPTION_EDIT_S "-e"

#define OPTION_GET_STATUS "--get-status"
#define OPTION_GET_STATUS_S "-gs"

static bool gs_cancel_do;

CommandLineMode::CommandLineMode()
{
    this->m_DeviceStatus = kSDIErrorNone;
    gs_cancel_do = false;
    signal(SIGINT, this->cancel_sig_handler); // Ctrl+C (cancel) シグナルハンドルを登録
}

bool CommandLineMode::SelectMode(int option_num, const char** option)
{
    int i = 1;
    while(i < option_num){
        if(strcmp(option[i], OPTION_HELP) == 0 || strcmp(option[i], OPTION_HELP_S) == 0){
            this->ShowHelp();
            return true;

        }else if(strcmp(option[i], OPTION_LIST) == 0 || strcmp(option[i], OPTION_LIST_S) == 0){
            this->ListDevices();
            return true;

        }else if(strcmp(option[i], OPTION_SET_IP) == 0 || strcmp(option[i], OPTION_SET_IP_S) == 0){
            if((option_num-1) > i){
                if(strlen(option[i+1]) != 0 && option[i+1][0] != '-'){
                    this->SetIPAddress(option[i+1]);
                    i++;
                }else {
                    std::cout << "--set-ip option format invalid." << std::endl;
                }
            }else {
                std::cout << "--set-ip -> There are not enough options." << std::endl;
            }
            return true;
        }else if(strcmp(option[i], OPTION_SCAN) == 0 || strcmp(option[i], OPTION_SCAN_S) == 0){
            this->ValidateScanOption(option_num, option);
            return true;

        }else if(strcmp(option[i], OPTION_CREATE) == 0 || strcmp(option[i], OPTION_CREATE_S) == 0){
            this->CreateDefaultSettingFile(NULL, "./DefaultSettings.SF2");
            return true;

        }else if(strcmp(option[i], OPTION_EDIT) == 0 || strcmp(option[i], OPTION_EDIT_S) == 0){
            return false;

        }else if(strcmp(option[i], OPTION_GET_STATUS) == 0 || strcmp(option[i], OPTION_GET_STATUS_S) == 0){
            this->ValidateGetStatusOption(option_num, option);
            return true;

        }else {
            std::cout << "ERROR : Option format invalid." << std::endl;
            return false;
        }
    }

    return false;
}

bool CommandLineMode::SelectEditMode(int option_num, const char** option)
{
    int i = 1;
    if(i < option_num){
        if(strcmp(option[i], OPTION_EDIT) == 0 || strcmp(option[i], OPTION_EDIT_S) == 0){
            return this->ValidateEditOption(option_num, option);
        }else {
            std::cout << "ERROR : Option format invalid." << std::endl;
            return false;
        }
    }

    return false;
}

bool CommandLineMode::ValidateScanOption(int option_num, const char** option)
{
    const char* option2;
    const char* option3;
    ESString extension;

    if(option_num == 3){
        //option1 = option[1]; // --scan
        option2 = option[2]; // SF2 File
        //オプションの次の引数も'-'が付いている場合エラー
        if(option2[0] == '-'){
            std::cout << "ERROR : Option format invalid. -> Unnecessary [-]" << std::endl;
            return false;
        }
        //オプション数が3つにも関わらず3つ目が設定ファイル名でないとき(SF2ファイルでない)エラー
        if(!ES_CMN_FUNCS::PATH::ES_GetExtension(extension, option2)){
            std::cout << "ERROR : Option format invalid. -> The third option is not a SF2 file" << std::endl;
            return false;
        }
        if(strcmp(extension.c_str(), "SF2") != 0 && strcmp(extension.c_str(), "sf2") != 0){
            std::cout << "ERROR : Option format invalid. -> Configuration file extension is wrong (correct : sf2 or SF2)" << std::endl;
            return false;
        }
        //機種指定なし
        this->ScanStart(NULL, option2);

    }else if(option_num == 4){
        //option1 = option[1]; // --scan
        option2 = option[2]; // Target Device (Device ID or IP Address)
        option3 = option[3]; // SF2 File

        //オプションの次の引数も'-'が付いている場合エラー
        if(option2[0] == '-' || option3[0] == '-'){
            std::cout << "ERROR : Option format invalid. -> Unnecessary [-]" << std::endl;
            return false;
        }
        //オプション数が4つにも関わらず3つ目が設定ファイル名の場合エラー
        if(!ES_CMN_FUNCS::PATH::ES_GetExtension(extension, option3)){
            std::cout << "ERROR : Option format invalid. -> The third option is incorrect" << std::endl;
            return false;
        }
        //オプション数が4つにも関わらず4つ目が設定ファイル名でないとき(SF2ファイルでない)エラー
        if(!ES_CMN_FUNCS::PATH::ES_GetExtension(extension, option3)){
            std::cout << "ERROR : Option format invalid. -> The fourth option is not a SF2 file" << std::endl;
            return false;
        }
        if(strcmp(extension.c_str(), "SF2") != 0 && strcmp(extension.c_str(), "sf2") != 0){
            std::cout << "ERROR : Option format invalid. -> Configuration file extension is wrong (correct : sf2 or SF2)" << std::endl;
            return false;
        }
        this->ScanStart(option2, option3);

    }else {
        std::cout << "ERROR : Option format invalid." << std::endl;
        return false;
    }

    return true;
}

bool CommandLineMode::ValidateGetStatusOption(int option_num, const char** option)
{
    const char* option2;

    if(option_num == 2){
        //option1 = option[1]; // --get-status
        //機種指定なし
        this->GetStatus(NULL);

    }else if(option_num == 3){
        //option1 = option[1]; // --scan
        option2 = option[2]; // Target Device (Device ID or IP Address)

        //オプションの次の引数も'-'が付いている場合エラー
        if(option2[0] == '-'){
            std::cout << "ERROR : Option format invalid. -> Unnecessary [-]" << std::endl;
            return false;
        }
        this->GetStatus(option2);

    }else {
        std::cout << "ERROR : Option format invalid." << std::endl;
        return false;
    }

    return true;
}

void CommandLineMode::ShowHelp()
{
    //コマンドライン関連は翻訳対象外 (すべて英語で表記
#ifdef AKBMODE
    std::cout << " === Scan Tool Command line Mode Help===" << std::endl;
    std::cout << "Usage: " << DRIVER_NAME << " [OPTION]...\n" << std::endl;
    std::cout << "Start image acquisition on a scanner device.\n" << std::endl;
    std::cout << "Parameters are separated by a blank from single-character options (e.g." << std::endl;
    std::cout << "-h, --help : display this help message and exit" << std::endl;
    std::cout << "-l, --list : show available scanner devices" << std::endl;
    std::cout << "-i, --set-ip : set manual ip address\n"
                 "e.g.) $ " << DRIVER_NAME << " --set-ip 192.168.xx.x\n" << std::endl;
    std::cout << "-s, --scan [device ID or IP address](optional) [setting file]\n"
                 " : start scan with user setting file.\n"
                 "e.g.) $ " << DRIVER_NAME << " --scan DS-XX0 UserSettingFile.SF2\n" << std::endl;
    std::cout << "-e, --edit [full path of setting file] : create a user setting file\n"
                 "e.g.) $ " << DRIVER_NAME << " --edit /home/user1/UserSettingFile.SF2\n" << std::endl;
#else
    std::cout << " === Epson Scan 2 Command line Mode Help===" << std::endl;
    std::cout << "Usage: epsonscan2 [OPTION]...\n" << std::endl;
    std::cout << "Start image acquisition on a scanner device.\n" << std::endl;
    std::cout << "Parameters are separated by a blank from single-character options (e.g." << std::endl;
    std::cout << "-h, --help : display this help message and exit" << std::endl;
    std::cout << "-l, --list : show available scanner devices" << std::endl;
    std::cout << "-i, --set-ip : set manual ip address\n"
                 "e.g.) $ epsonscan2 --set-ip 192.168.xx.x\n" << std::endl;
    std::cout << "-s, --scan [device ID or IP address](optional) [setting file]\n"
                 " : start scan with user setting file.\n"
                 "e.g.1) $ epsonscan2 --scan DS-XX0 UserSettingFile.SF2\n"
                 "e.g.2) $ epsonscan2 --scan DS-XX0:001:004 UserSettingFile.SF2\n" << std::endl;
    std::cout << "-c, --create : create a default user setting file in current directry" << std::endl;
    std::cout << "-e, --edit [full path of setting file] : create a user setting file\n"
                 "e.g.) $ epsonscan2 --edit /home/user1/UserSettingFile.SF2\n" << std::endl;
    std::cout << "-gs, --get-status : Get device status.\n" << std::endl;
#endif
}

void CommandLineMode::ListDevices()
{
    DeviceList* finder = new DeviceList();
    if(finder == NULL){
        return;
    }
    finder->show_list();

    delete finder;
    finder = NULL;
}

void CommandLineMode::GetStatus(const char* target_device)
{
    Supervisor* sv = new Supervisor;
    if(sv == NULL){
        std::cout << "ERROR : An unexpected error occurred. Epson Scan 2 will close." << std::endl;
        return;
    }
    SDIDeviceInfo devInfo;

    sv->SetUp();

    DeviceList* finder = new DeviceList();
    if(finder == NULL){
        delete sv;
        sv = NULL;
        std::cout << "ERROR : An unexpected error occurred. Epson Scan 2 will close." << std::endl;
        return;
    }
    devInfo = finder->list(target_device);

    //デバイスがみつからないとき
    if(devInfo.displayName[0] == '\0'){
        sv->m_LastError = kSDIErrorDeviceNotFound;
        goto memfree;
    }
    sv->Connecting_Scanner(devInfo, nullptr);
    if(gs_cancel_do){
        goto memfree;
    }

    sv->m_LastError = sv->CheckDeviceErrorStatus();

    //紙なしステータスはADFのみサポートしている機種のときだけ返す
    if(sv->device_data.FunctionalUnit.capability.countOfList == 1 && this->search_item_list(kSDIFunctionalUnitDocumentFeeder, sv->device_data.FunctionalUnit.capability.list, sv->device_data.FunctionalUnit.capability.countOfList)){
        if(sv->m_LastError == 0){ // ステータスが成功で返ってきた場合、用紙なしエラーチェックをする
            if(!sv->GetPaperLoadedStatus()){
                sv->m_LastError = kSDIErrorPaperEmpty;
            }
        }
    }

memfree:

    ErrorMessage(sv->m_LastError);

    delete finder;
    finder = NULL;

    sv->Disconnecting_Scanner();
    sv->Dispose_Scanner();

    delete sv;
    sv = NULL;
}

void CommandLineMode::SetIPAddress(const char* ip_address)
{
    if(strlen(ip_address) > 64){
        std::cout << "ERROR : IP address length too long" << std::endl;
        return;
    }
    std::cout << "Set IP :" << ip_address << std::endl;

#ifdef NO_QTMODE
    std::string netDst = std::getenv("HOME");
#else
    std::string netDst((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
#endif

#ifdef AKBMODE
    std::string dir_name = ".";
    dir_name = dir_name + DRIVER_NAME;
    dir_name = dir_name + "/";
    ES_CMN_FUNCS::PATH::ES_CombinePath(netDst, netDst, dir_name.c_str());
#else
    ES_CMN_FUNCS::PATH::ES_CombinePath(netDst, netDst,".epsonscan2/");
#endif
    if(!(bool)ES_CMN_FUNCS::PATH::ES_IsExistFile(netDst)){
        ES_CMN_FUNCS::PATH::ES_MakeFolder(netDst);
    }

    ES_CMN_FUNCS::PATH::ES_CombinePath(netDst, netDst, "Network");
#ifdef AKBMODE
    std::string file_name = "/";
    file_name = file_name + DRIVER_NAME;
    file_name = file_name + ".conf";
    std::string network_settings_file = netDst + file_name.c_str();
#else
    std::string network_settings_file = netDst + "/epsonscan2.conf";
#endif

    //Networkフォルダがなければ作成する
    if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(netDst) == FALSE)
    {
        ES_CMN_FUNCS::PATH::ES_MakeFolder(netDst);
    }

    if(ES_CMN_FUNCS::PATH::ES_IsExistFile(network_settings_file) == FALSE)
    {
        std::ofstream out_netSettingFile(network_settings_file.c_str(), std::ios_base::out | std::ios_base::app );
        //Add [Network] section
        out_netSettingFile << "[Network]" << std::endl;
        out_netSettingFile << ip_address << std::endl;
    }else {
        std::ofstream out_netSettingFile(network_settings_file.c_str(), std::ios_base::out | std::ios_base::app );

        out_netSettingFile << ip_address << std::endl;
    }
}

void CommandLineMode::cancel_sig_handler(int sig){
    gs_cancel_do = true;
    std::cout << " CANCEL!" << std::endl;
}

void CommandLineMode::ScanStart(const char* target_device, const char* settings_file_path)
{
    Supervisor* sv = new Supervisor;
    if(sv == NULL){
        std::cout << "ERROR : An unexpected error occurred. Epson Scan 2 will close." << std::endl;
        return;
    }
    SDIDeviceInfo devInfo;

    sv->SetUp();

    DeviceList* finder = new DeviceList();
    if(finder == NULL){
        delete sv;
        sv = NULL;
        std::cout << "ERROR : An unexpected error occurred. Epson Scan 2 will close." << std::endl;
        return;
    }
    devInfo = finder->list(target_device);

    //デバイスがみつからないとき
    if(devInfo.displayName[0] == '\0'){
        sv->m_LastError = kSDIErrorDeviceNotFound;
        goto memfree;
    }
    sv->Connecting_Scanner(devInfo, nullptr);
    if(gs_cancel_do){
        goto memfree;
    }

    sv->Get_Exclusion_Value();

    //設定ファイルの読み込み
    {
        LastUsedSettings* last_used_settings_controller = new LastUsedSettings();
        if(!last_used_settings_controller){
            goto memfree;
        }
        if(!last_used_settings_controller->ReadCmdSettingsFile(settings_file_path, &sv->device_data)){
            std::cout << "ERROR : SF2 File is not found..." << std::endl;

            delete last_used_settings_controller;
            last_used_settings_controller = NULL;
            goto memfree;
        }
        delete last_used_settings_controller;
        last_used_settings_controller = NULL;
    }

#ifndef NO_QTMODE
    if(sv->device_data.Folder == Document){
        sv->device_data.SelectPath = (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toLocal8Bit()).constData();
    }else if(sv->device_data.Folder == Picture){
        sv->device_data.SelectPath = (QStandardPaths::writableLocation(QStandardPaths::PicturesLocation).toLocal8Bit()).constData();
    }else if(sv->device_data.Folder == Desktop){
        sv->device_data.SelectPath = (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).toLocal8Bit()).constData();
    }else
#endif
        if(sv->device_data.Folder == USER_DEFINE){
        sv->device_data.SelectPath = sv->device_data.UserDefinePath;
    }

    //取り込み装置自動時の判別処理
    if(sv->device_data.FunctionalUnit_Auto){
        if(sv->GetPaperLoadedStatus()){
            sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
        }else {
            sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
        }
    }

    //定型サイズが指定されている場合にスキャン領域をアップデートする
    if(sv->device_data.ScanArea.FixedSize != Manual){
        this->UpdateScanArea(sv->device_data.ScanArea, sv->device_data.Resolution.select);
    }else {
        sv->device_data.ScanArea.ScanAreaWidth.select = sv->device_data.ScanArea.ScanAreaWidth.select / 100 * sv->device_data.Resolution.select;
        sv->device_data.ScanArea.ScanAreaHeight.select = sv->device_data.ScanArea.ScanAreaHeight.select / 100 * sv->device_data.Resolution.select;
    }

    //scanning roop
    do{
        sv->scanning_status = SAVE;
        if(sv->Scan_Start(kSDIOperationTypeNew)){
            while(sv->scanning_status == SAVE){
                if(gs_cancel_do){
                    std::cout << "CANCEL DO!" << std::endl;
                    sv->scanning_status = CANCEL;
                }
                if(sv->scanning_status == CANCEL){
                    sv->Cancel_Scanning();
                    sv->outEventType = kSDITransferEventTypeCancel;
                    break;
                }else {
                    if(sv->CheckNextTransferEvent(false)){
                        if(sv->Scanning()){
                            if(sv->outEventType == kSDITrasnferEventTypeImage){
                                sv->Get_Image();
                                sv->Save_Path();
                            }
                        }else {//scanning error
                            sv->Get_Image();
                            break;
                        }
                    }else if(sv->outEventType == kSDITransferEventTypeComplete){
                        break;
                    }
                }
            }
        }

        //スキャン後のページ追加フラグ処理
        if(sv->device_data.Add_Pages){
            do{
                //選択肢をコマンドラインに表示
                while(1){
                    if(gs_cancel_do){
                        goto memfree;
                    }
                    std::cout << "Scanning complete." << std::endl;
                    std::cout << "" << std::endl;
                    std::cout << "Input [1:Save] to save your scanned images." << std::endl;
                    std::cout << "Input [2:Cancel] to discard the scanned images." << std::endl;
                    std::cout << "Input [3:Add] to scan additional images." << std::endl;

                    char input[2];
                    if(!fgets(input, 2, stdin)){ // 入力：
                        goto memfree;
                    }

                    char *ln = strchr(input, '\n'); /* 改行文字を検索 */

                    if (ln != NULL) { /* 改行が読み取られていたかどうか */
                       *ln = '\0';    /* 改行文字を終端文字に置き換える */
                    }
                    else {            /* 入力ストリーム上に文字が残ってる場合 */
                       while (1) {    /* 改行文字が読み取られるまで空読みする */
                          int c = getchar();
                          if (c == '\n' || c == EOF) break;
                       }
                    }

                    if(strncmp(input, "1", 1) == 0){
                        sv->scanning_status = SAVE;
                        break;
                    }else if(strncmp(input, "2", 1) == 0){
                        sv->scanning_status = CANCEL;
                        break;
                    }else if(strncmp(input, "3", 1) == 0){
                        sv->scanning_status = CONTINUE;
                        break;
                    }
                }

                if(sv->scanning_status == SAVE){
                    sv->outEventType = kSDITransferEventTypeComplete;
                }
                //事前紙なしエラーCheck
                if(sv->device_data.FunctionalUnit.select == kSDIFunctionalUnitDocumentFeeder &&
                        sv->scanning_status == CONTINUE){
                    if(!sv->GetPaperLoadedStatus()){
                        //紙なしエラー
                        sv->m_LastError = kSDIErrorPaperEmpty;
                    }else {
                        sv->m_LastError = kSDIErrorNone;
                    }
                }else {
                    sv->m_LastError = kSDIErrorNone;
                }
            }while(sv->m_LastError == kSDIErrorPaperEmpty);
        }


        if(sv->m_LastError == kSDIErrorNone){
            if(sv->scanning_status == SAVE){
                if(sv->Save_Image2()){
                    //保存先ディレクトリを表示するフラグがたっているか？
                    /*Configuration& config = Configuration::GetConfiguration();
                    if(config.config_para.open_img_folder){
                        SaveDestFolder* folder = new SaveDestFolder();
                        if(folder){
                            if(!folder->open_folder(sv)){
                                std::cout << "ERROR : Unable to save scanned files." << std::endl;
                            }
                            delete folder;
                            folder = NULL;
                        }
                    }else {*/
                        //保存先のディレクトリが存在しているか確認する
                        const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFile(sv->device_data.SelectPath);
                        if (!result) {
                            std::cout << "ERROR : Unable to save scanned files." << std::endl;
                        }
                    //}
                }else {
                    if(sv->m_LastError == kSDIErrorWriteFile){
                        //書き込み失敗エラー
                        std::cout << "ERROR : Unable to save scanned files." << std::endl;
                    }else {
                        //すべて白紙ページエラー
                        std::cout << "ERROR : All scanned pages were blank." << std::endl;
                    }
                }
            }
        }
    }while(sv->scanning_status == CONTINUE); //スキャン継続フラグがたっている場合次のスキャンに移行する

memfree:

    ErrorMessage(sv->m_LastError);

    delete finder;
    finder = NULL;

    sv->Disconnecting_Scanner();
    sv->Dispose_Scanner();

    delete sv;
    sv = NULL;
}

void CommandLineMode::CreateDefaultSettingFile(const char* target_device, const char* settings_file_path)
{
    Supervisor* sv = new Supervisor;
    if(sv == NULL){
        std::cout << "ERROR : An unexpected error occurred. Epson Scan 2 will close." << std::endl;
        return;
    }
    SDIDeviceInfo devInfo;

    sv->SetUp();

    DeviceList* finder = new DeviceList();
    if(finder == NULL){
        delete sv;
        sv = NULL;
        std::cout << "ERROR : An unexpected error occurred. Epson Scan 2 will close." << std::endl;
        return;
    }
    devInfo = finder->list(target_device);

    //デバイスがみつからないとき
    if(devInfo.displayName[0] == '\0'){
        sv->m_LastError = kSDIErrorDeviceNotFound;
        goto memfree;
    }
    sv->Connecting_Scanner(devInfo, nullptr);
    if(gs_cancel_do){
        goto memfree;
    }

    sv->Get_Exclusion_Value();

    //setting default value
    sv->device_data.ImageFormatSettings.ImageFormat.select = 6;
    sv->device_data.ImageFormatSettings.PDFImgFormatOption.AllPage = 1;
    sv->device_data.ImageFormatSettings.PDFImgFormatOption.page = 1;
    sv->device_data.Prefix = "img";
    sv->device_data.Gamma.select = 22;
    sv->device_data.Folder = 101;
    sv->device_data.UserDefinePath = "./";

    //設定ファイル
    {
        LastUsedSettings* last_used_settings_controller = new LastUsedSettings();
        if(!last_used_settings_controller){
            goto memfree;
        }
        last_used_settings_controller->CreateSettingsDict(settings_file_path, sv->device_data);
        delete last_used_settings_controller;
        last_used_settings_controller = NULL;
    }

memfree:

    ErrorMessage(sv->m_LastError);

    delete finder;
    finder = NULL;

    sv->Disconnecting_Scanner();
    sv->Dispose_Scanner();

    delete sv;
    sv = NULL;
}

void CommandLineMode::ErrorMessage(SDIError outError)
{
    std::string error_message;

    this->m_DeviceStatus = outError;

    //message TBD
    if(outError != kSDIErrorNone){
        if(outError == kSDIErrorPaperEmpty){
            //原稿なしエラー
            error_message = "ERROR : Load the originals in the ADF.";
        }else if(outError == kSDIErrorPaperJam){
            //紙ジャム＆重送エラー時の復帰ダイアログ
            error_message = "ERROR : Double feed detected. See the documentation for instructions on removing your originals.";
        }else if(outError == kSDIErrorPaperProtect){
            //原稿保護エラー時の復帰ダイアログ
            error_message = "ERROR : A paper jam has occurred. See the documentation for instructions on removing your originals.";
        }else  if(outError == kSDIErrorDeviceInBusy){
            error_message = "kSDIErrorDeviceInBusy";
        }else  if(outError == kSDIErrorCoverOpen){
            //ADFカバーオープンエラー
            error_message = "ERROR : ADF or ADF cover is open. Close it and reload the originals.";
        }else  if(outError == kSDIErrorTrayClose){
            //給紙トレークローズエラー
            error_message = "ERROR : The input tray is closed. Open the input tray.";
        }else if(outError == kSDIErrorDataSend){
            //データ送信エラー
            error_message = "ERROR : Unable to send data. Check the connection to the scanner and try again.";
        }else if(outError == kSDIErrorDataReceive){
            //データ受信エラー
            error_message = "ERROR : Unable to receive data. Check the connection to the scanner and try again.";
        }else if(outError == kSDIErrorDeviceInUse){
            //スキャナーが使用中または使用できないエラー
            error_message = "ERROR : The scanner is in use or unavailable. Please wait.";
        }else  if(outError == kSDIErrorNoMemory){
            //メモリ不足エラー
            error_message = "ERROR : Not enough memory for Epson Scan 2. Close other applications and try again.";
        }else  if(outError == kSDIErrorWriteFile){
            //ファイルの書き込みエラー
            error_message = "ERROR : Unable to save scanned files.";
        }else if(outError == kSDIErrorUnknownError){
            //予期せぬエラー・不明なエラー
            error_message = "ERROR : An unexpected error occurred. Epson Scan 2 will close.";
        }else if(outError == kSDIErrorDeviceNotFound){
            //
            error_message = "ERROR : Device is not found...";
        }else if(outError == kSDIErrorUserAuthEnabled){
            //本体認証モードエラー
            error_message = "ERROR : Scanning from Epson Scan 2 is not allowed because the authentication function for the scanner is enabled.";
        }else if(outError == kSDIErrorPaperRemoveError){
            //
            error_message = "ERROR : Remove the original.";
        }else if(outError == kSDIErrorSkewDetectError){
            //
            error_message = "ERROR : The original was fed skewed or the original size could not be detected. Load the original again.";
        }else {
            //予期せぬエラー・不明なエラー
            error_message = "ERROR : An unexpected error occurred. Epson Scan 2 will close.";
        }
    }else {
        return;
    }

    std::cout << error_message << std::endl;
}

bool CommandLineMode::ValidateEditOption(int option_num, const char** option)
{
    const char* option1;
    const char* option2;
    ESString extension;

    if(option_num == 3){
        option1 = option[1]; // --edit or -e
        option2 = option[2]; // SF2 File
        //オプションの次の引数も'-'が付いている場合エラー
        if(option2[0] == '-'){
            std::cout << "ERROR : Option format invalid. -> Unnecessary [-]" << std::endl;
            return false;
        }
        //オプション数が3つにも関わらず3つ目が設定ファイル名でないとき(SF2ファイルでない)エラー
        if(!ES_CMN_FUNCS::PATH::ES_GetExtension(extension, option2)){
            std::cout << "ERROR : Option format invalid. -> The third option is not a SF2 file" << std::endl;
            return false;
        }
        if(strcmp(extension.c_str(), "SF2") != 0 && strcmp(extension.c_str(), "sf2") != 0){
            std::cout << "ERROR : Option format invalid. -> Configuration file extension is wrong (correct : sf2 or SF2)" << std::endl;
            return false;
        }
        ESString out_path;
        ES_CMN_FUNCS::PATH::ES_GetFolderPath(out_path, option2);
        if(!ES_CMN_FUNCS::PATH::ES_IsExistFolder(out_path)){
            std::cout << "ERROR : Target directory is not found." << std::endl;
            return false;
        }

    }else {
        std::cout << "ERROR : Option format invalid." << std::endl;
        return false;
    }

    return true;
}

void CommandLineMode::UpdateScanArea(SCAN_AREA_VALUE& scan_area_parameter, int resolution)
{
    if(scan_area_parameter.FixedSize == A3){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(297, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(420, resolution);
    }else if(scan_area_parameter.FixedSize == A4){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(210, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(297, resolution);
    }else if(scan_area_parameter.FixedSize == A4_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(297, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(210, resolution);
    }else if(scan_area_parameter.FixedSize == A5){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(148, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(210, resolution);
    }else if(scan_area_parameter.FixedSize == A5_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(210, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(148, resolution);
    }else if(scan_area_parameter.FixedSize == A6){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(105, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(148, resolution);
    }else if(scan_area_parameter.FixedSize == A6_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(148, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(105, resolution);
    }else if(scan_area_parameter.FixedSize == A8){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(52, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(74, resolution);
    }else if(scan_area_parameter.FixedSize == A8_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(74, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(52, resolution);
    }else if(scan_area_parameter.FixedSize == B4_JIS){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(257, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(364, resolution);
    }else if(scan_area_parameter.FixedSize == B4_JIS_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(364, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(257, resolution);
    }else if(scan_area_parameter.FixedSize == B5_JIS){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(182, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(257, resolution);
    }else if(scan_area_parameter.FixedSize == B5_JIS_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(257, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(182, resolution);
    }else if(scan_area_parameter.FixedSize == s8K){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(270, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(390, resolution);
    }else if(scan_area_parameter.FixedSize == s16K){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(195, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(270, resolution);
    }else if(scan_area_parameter.FixedSize == s16K_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(270, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(195, resolution);
    }else if(scan_area_parameter.FixedSize == Letter){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(216, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(279, resolution);
    }else if(scan_area_parameter.FixedSize == Letter_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(279, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(216, resolution);
    }else if(scan_area_parameter.FixedSize == Legal){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(216, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(356, resolution);
    }else if(scan_area_parameter.FixedSize == Legal_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(356, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(216, resolution);
    }else if(scan_area_parameter.FixedSize == Postcard){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(100, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(148, resolution);
    }else if(scan_area_parameter.FixedSize == Postcard_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(148, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(100, resolution);
    }else if(scan_area_parameter.FixedSize == PlasticCard){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(85.6, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(54, resolution);
    }else if(scan_area_parameter.FixedSize == PlasticCard_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(85.6, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(54, resolution);
    }else if(scan_area_parameter.FixedSize == Tabloid){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(279, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(432, resolution);
    }else if(scan_area_parameter.FixedSize == Tabloid_Landscape){
        scan_area_parameter.ScanAreaWidth.select = calculate_pixel_size(432, resolution);
        scan_area_parameter.ScanAreaHeight.select = calculate_pixel_size(279, resolution);
    }else if(scan_area_parameter.FixedSize == Maximum_Size){
        scan_area_parameter.ScanAreaWidth.select = scan_area_parameter.MaxScanAreaWidth.select / 100 * resolution;
        scan_area_parameter.ScanAreaHeight.select = scan_area_parameter.MaxScanAreaHeight.select / 100 * resolution;
    }
}

double CommandLineMode::calculate_pixel_size(double value, int res)
{
    //mm -> pixel
    double mpp = 25.4 / res;
    return value / mpp;
}

int CommandLineMode::IsGetStatus()
{
    return this->m_DeviceStatus;
}

bool CommandLineMode::search_item_list(int item, int32_t item_list[], int32_t list_size)
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
