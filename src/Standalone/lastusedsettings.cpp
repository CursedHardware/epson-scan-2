#include <fstream>
#define BOOST_NO_CXX11_RVALUE_REFERENCES 1
#include <boost/any.hpp>
#include <utils/PathUtils.h>

#ifndef NO_QTMODE
#include <QStandardPaths>
#endif

#include "devicelist.h"
#include "lastusedsettings.h"
#include "../Include/Config.h"


LastUsedSettings::LastUsedSettings()
{
#ifdef AKBMODE
    std::string strDst = ".";
    strDst = strDst + DRIVER_NAME;

    std::string dir1((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, (strDst + "/Connection").c_str());
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1,"PreferredInfo.dat");
    this->PreferredInfoFilePath = dir1.c_str();

    std::string dir3((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir3, dir3, (strDst + "/Settings/").c_str());
    this->SettingsFilePath = dir3.c_str();
#else

#ifdef NO_QTMODE
    std::string dir1 = std::getenv("HOME");
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, ".epsonscan2/Connection");
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, "PreferredInfo.dat");
    this->PreferredInfoFilePath = dir1.c_str();

    std::string dir3 = std::getenv("HOME");
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir3, dir3,".epsonscan2/Settings/");
    this->SettingsFilePath = dir3.c_str();
#else
   std::string dir1((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
   ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, ".epsonscan2/Connection");
   ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, "PreferredInfo.dat");
   this->PreferredInfoFilePath = dir1.c_str();

   std::string dir3((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
   ES_CMN_FUNCS::PATH::ES_CombinePath(dir3, dir3,".epsonscan2/Settings/");
   this->SettingsFilePath = dir3.c_str();
#endif
#endif
}


//前回接続デバイス関連処理
bool LastUsedSettings::CheckLastUsedDevice()
{
#ifdef NO_QTMODE
    std::string dir1 = std::getenv("HOME");
#else
    std::string dir1((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
#endif
#ifdef AKBMODE
    std::string strDst = ".";
    strDst = strDst + DRIVER_NAME;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, (strDst + "/").c_str());
#else
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1,".epsonscan2/");
#endif
    if(!(bool)ES_CMN_FUNCS::PATH::ES_IsExistFile(dir1)){
        ES_CMN_FUNCS::PATH::ES_MakeFolder(dir1);
    }

    std::string folder;
    ES_CMN_FUNCS::PATH::ES_GetFolderPath(folder, this->PreferredInfoFilePath);
    if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(folder) == FALSE)
    {
        ES_CMN_FUNCS::PATH::ES_MakeFolder(folder);
        return false;
    }

    const std::string connect_path(this->PreferredInfoFilePath.c_str());
    return (bool)ES_CMN_FUNCS::PATH::ES_IsExistFile(connect_path);
}

bool LastUsedSettings::ReadPreferredInfo()
{
  ESDictionary out;

  try{

      if(!this->CheckLastUsedDevice()){
          return false;
      }

      ES_CMN_FUNCS::JSON::JSONFiletoDictionary(this->PreferredInfoFilePath.c_str(), out);

      ESString connection = boost::any_cast<ESString>(out["Connection"]);
      ESString display_name = boost::any_cast<ESString>(out["ESDisplayName"]);

      //std::cout << ES_STRING("Connection : ") << connection << std::endl;
      //std::cout << ES_STRING("ModelID : ") << model_id << std::endl;

      //Connection : USB or Network > IP(exp "192.168.xxx.x")

      if(connection == "USB"){
          if(!this->SelectUSBDevice(display_name)){
              //not found
              return false;
          }
      }else {
          //Network device
          if(!this->SelectNetworkDevice(connection)){
              //not found
              return false;
          }
      }
  }

  catch (...) {
      DeleteFile(this->PreferredInfoFilePath.c_str());
  }
  return true;
}

bool LastUsedSettings::SelectUSBDevice(std::string display_name)
{
    //デバイスリストに同じモデルＩＤがあるか調べる
    auto device = DeviceList::device_list.begin();
    int i = 0;
    while(i < (signed)DeviceList::device_list.size()){
        if(device->displayName == display_name){
            DeviceList::select_device = i;
            return true;
        }
        ++device;
        i++;
    }
    //not found
    return false;
}

bool LastUsedSettings::SelectNetworkDevice(std::string ip)
{
    //デバイスリストに同じIPがあるか調べる
    auto device = DeviceList::device_list.begin();
    int i = 0;
    while(i < (signed)DeviceList::device_list.size()){
        if(device->ipAddress == ip){
            DeviceList::select_device = i;
            return true;
        }
        ++device;
        i++;
    }
    //手動追加ネットワークＩＰアドレス検索
    device = DeviceList::manu_network_device_list.begin();
    i = 0;//DeviceList::device_list.size();
    while(i < (signed)DeviceList::manu_network_device_list.size()){
        if(strncmp(ip.c_str(), device->ipAddress, strlen(ip.c_str())) == 0){
            DeviceList::select_device = i + DeviceList::device_list.size();
            return true;
        }
        ++device;
        i++;
    }
    //not found
    return false;
}


void LastUsedSettings::CreatePreferredInfo(SDIDeviceInfo dev_info)
{
    if(this->CheckLastUsedDevice()){
        //元の前回接続機種ファイルを削除
        try {
            DeleteFile(this->PreferredInfoFilePath.c_str());
        }
        catch (...) {
            throw;
        }
    }

    try{
        ESDictionary dict;

        //dict["ESModelID"] = (ESString)dev_info.modelID;
        if(dev_info.ipAddress[0] == '\0'){
            dict["ESDisplayName"] = (ESString)dev_info.displayName;
            dict["Connection"] = (ESString)"USB";
        }else {
            dict["ESDisplayName"] = (ESString)"Network Scanner";
            dict["Connection"] = (ESString)dev_info.ipAddress;
        }

        ES_CMN_FUNCS::JSON::DictionaryToJSONFile(dict, this->PreferredInfoFilePath.c_str());
    }
    catch (...) {
        DeleteFile(this->PreferredInfoFilePath.c_str());
    }
}

void LastUsedSettings::DeletePreferredInfo(void)
{
    //元の前回接続機種ファイルを削除
    try {
        DeleteFile(this->PreferredInfoFilePath.c_str());
    }
    catch (...) {
        throw;
    }
}


//CommonSettingsファイル関連
bool LastUsedSettings::CheckCommonSettingsFile()
{
#ifdef NO_QTMODE
    std::string dir1 = std::getenv("HOME");
#else
    std::string dir1((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
#endif
#ifdef AKBMODE
    std::string strDst = ".";
    strDst = strDst + DRIVER_NAME;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, (strDst + "/").c_str());
#else
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1,".epsonscan2/");
#endif
    if(!(bool)ES_CMN_FUNCS::PATH::ES_IsExistFile(dir1)){
        ES_CMN_FUNCS::PATH::ES_MakeFolder(dir1);
    }


    if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(this->SettingsFilePath) == FALSE)
    {
        ES_CMN_FUNCS::PATH::ES_MakeFolder(this->SettingsFilePath);
    }

    std::string common_settings_file = this->SettingsFilePath.c_str();
    ES_CMN_FUNCS::PATH::ES_CombinePath(common_settings_file, common_settings_file, "CommonSettings.SF2");
    return ES_CMN_FUNCS::PATH::ES_IsExistFile(common_settings_file);
}

CONFIGURATION LastUsedSettings::ReadCommonSettingsFile()
{
    CONFIGURATION config_val;
    memset(&config_val, 0, sizeof(CONFIGURATION));

    try{
        if(this->CheckCommonSettingsFile()){
            std::string common_settings_file = this->SettingsFilePath.c_str();
            common_settings_file = common_settings_file + "CommonSettings.SF2";
            ESDictionary out;
            ES_CMN_FUNCS::JSON::JSONFiletoDictionary(common_settings_file, out);

            config_val.units = (Units)boost::any_cast<ESNumber>(out["ESUnit"]);
            config_val.last_used_settings = boost::any_cast<ESNumber>(out["ESLastUsedSettings"]);
            config_val.compression = boost::any_cast<ESNumber>(out["ESCompression"]);
            config_val.open_img_folder = boost::any_cast<ESNumber>(out["ESOpenFolder"]);
            config_val.quality_preview = boost::any_cast<ESNumber>(out["ESQualityPreview"]);
            if(config_val.quality_preview){
                config_val.preview_resolution = 150;
            }else {
                config_val.preview_resolution = 50;
            }
            //config_val.sleep_timer = boost::any_cast<ESNumber>(out["ESSleepTimer"]);
            //config_val.ac_power = (Time)boost::any_cast<ESNumber>(out["ESACPower"]);
            //config_val.usb_power = (Time)boost::any_cast<ESNumber>(out["ESUSBPower"]);
        }else {
            //default value
            config_val.units = (Units)unit_inchi;
            config_val.last_used_settings = true;
            config_val.compression = true;
            config_val.open_img_folder = true;
            config_val.quality_preview = false;
            config_val.preview_resolution = 50;
            //config_val.sleep_timer = 240;
            //config_val.ac_power = t_Off;
            //config_val.usb_power = t_Off;
        }
    }
    catch (...) {
        DeleteFile((this->SettingsFilePath + "CommonSettings.SF2").c_str());
    }
    return config_val;
}

CONFIGURATION LastUsedSettings::ReadMainWindowInfo()
{
    Configuration& config = Configuration::GetConfiguration();
    if(this->CheckCommonSettingsFile()){
        try{
            std::string common_settings_file = this->SettingsFilePath.c_str();
            common_settings_file = common_settings_file + "CommonSettings.SF2";
            ESDictionary out;
            ES_CMN_FUNCS::JSON::JSONFiletoDictionary(common_settings_file, out);

            config.config_para.WindowPositionX = boost::any_cast<ESNumber>(out["WindowPositionX"]);
            config.config_para.WindowPositionY = boost::any_cast<ESNumber>(out["WindowPositionY"]);
            config.config_para.WindowWidth = boost::any_cast<ESNumber>(out["WindowWidth"]);
            config.config_para.WindowHeight = boost::any_cast<ESNumber>(out["WindowHeight"]);
        }
        catch (...) {
            DeleteFile((this->SettingsFilePath + "CommonSettings.SF2").c_str());
        }
    }
    return config.config_para;
}

void LastUsedSettings::CreateCommonSettingsFile(Configuration& config)
{
    std::string common_settings_file = this->SettingsFilePath.c_str();
    common_settings_file = common_settings_file + "CommonSettings.SF2";

    if(this->CheckCommonSettingsFile()){
        this->DeleteCommonSettingsFile();
    }

    try{
        ESDictionary dict;

        dict["ESUnit"] = (ESNumber)config.config_para.units;
        dict["ESLastUsedSettings"] = (ESNumber)config.config_para.last_used_settings;
        dict["ESCompression"] = (ESNumber)config.config_para.compression;
        dict["ESOpenFolder"] = (ESNumber)config.config_para.open_img_folder;
        dict["ESQualityPreview"] = (ESNumber)config.config_para.quality_preview;
        //dict["ESSleepTimer"] = (ESNumber)config.config_para.sleep_timer;
        //dict["ESACPower"] = (ESNumber)config.config_para.ac_power;
        //dict["ESUSBPower"] = (ESNumber)config.config_para.usb_power;

        dict["WindowPositionX"] = (ESNumber)config.config_para.WindowPositionX;
        dict["WindowPositionY"] = (ESNumber)config.config_para.WindowPositionY;
        dict["WindowWidth"] = (ESNumber)config.config_para.WindowWidth;
        dict["WindowHeight"] = (ESNumber)config.config_para.WindowHeight;

        ES_CMN_FUNCS::JSON::DictionaryToJSONFile(dict, common_settings_file);
    }
    catch (...) {
        DeleteFile((this->SettingsFilePath + "CommonSettings.SF2").c_str());
    }
}

void LastUsedSettings::DeleteCommonSettingsFile()
{
    std::string common_settings_file = this->SettingsFilePath.c_str();
    common_settings_file = common_settings_file + "CommonSettings.SF2";
    //元のCommonSettingsファイルを削除
    try {
        DeleteFile(common_settings_file.c_str());
    }
    catch (...) {
        throw;
    }
}


//前回設定の保存関連
bool LastUsedSettings::CreateSettingsFile(std::string model_id, const SCANPARA device_data)
{
    std::string model_id_folder = this->SettingsFilePath.c_str();
    model_id_folder = model_id_folder + model_id;
    std::string user_settings_file = model_id_folder + "/UserSettings.SF2";

#ifdef NO_QTMODE
    std::string dir1 = std::getenv("HOME");
#else
    std::string dir1((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
#endif
#ifdef AKBMODE
    std::string strDst = "/.";
    strDst = strDst + DRIVER_NAME;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1, (strDst + "/").c_str());
#else
    ES_CMN_FUNCS::PATH::ES_CombinePath(dir1, dir1,"/.epsonscan2/");
#endif
    //.epsonscan2フォルダがなければ作成する
    if(!(bool)ES_CMN_FUNCS::PATH::ES_IsExistFile(dir1)){
        ES_CMN_FUNCS::PATH::ES_MakeFolder(dir1);
    }

    //Settingsフォルダがなければ作成する
    if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(this->SettingsFilePath) == FALSE)
    {
        ES_CMN_FUNCS::PATH::ES_MakeFolder(this->SettingsFilePath);
    }

    //機種IDのディレクトリがなければ作成する
    bool result = (bool)ES_CMN_FUNCS::PATH::ES_IsExistFile(model_id_folder);
    if ( !result ) {
        result = ES_CMN_FUNCS::PATH::ES_MakeFolder(model_id_folder);
        if ( !result ) {
            //failed create folder
            return false;
        }
    }
    //機種IDのディレクトリの中に設定ファイルがなければ作成する
    result =ES_CMN_FUNCS::PATH::ES_IsExistFile(user_settings_file);
    if ( !result ) {
       std::ofstream(user_settings_file.c_str());
       std::fstream SettingFile(user_settings_file.c_str());
       if (SettingFile.fail())
       {
           return false;
       }
    }
    this->CreateSettingsDict(user_settings_file, device_data);

    return true;
}

void LastUsedSettings::CreateSettingsDict(std::string user_settngs_file, const SCANPARA device_data)
{
    ESDictionary dict;
    ESDictionary preset;
    ESDictionary devnum;

    dict["FunctionalUnit"] =                (ESNumber)device_data.FunctionalUnit.select;
    dict["FunctionalUnit_Auto"] =           (ESNumber)device_data.FunctionalUnit_Auto;
    dict["ColorType"] =                     (ESNumber)device_data.ColorType.select;
    dict["AutoColorPixelType"] =            (ESNumber)device_data.AutoColorPixelType.select;
    dict["AutoColorPixelLevel"] =           (ESNumber)device_data.AutoColorPixelLevel.select;
    dict["Resolution"] =                    (ESNumber)device_data.Resolution.select;
    dict["ImageFormat"] =                   (ESNumber)device_data.ImageFormatSettings.ImageFormat.select;
    dict["Gamma"] =                         (ESNumber)device_data.Gamma.select;
    dict["ScanAreaWidth"] =                 (ESNumber)device_data.ScanArea.ScanAreaWidth.select;
    dict["ScanAreaHeight"] =                (ESNumber)device_data.ScanArea.ScanAreaHeight.select;
    dict["ScanAreaOffsetX"] =               (ESNumber)device_data.ScanArea.ScanAreaOffsetX.select;
    dict["ScanAreaOffsetY"] =               (ESNumber)device_data.ScanArea.ScanAreaOffsetY.select;
    //dict["MaxScanAreaWidth"] =              (ESNumber)device_data.ScanArea.MaxScanAreaWidth.select;
    //dict["MaxScanAreaHeight"] =             (ESNumber)device_data.ScanArea.MaxScanAreaHeight.select;
    //dict["MaxScanAreaHeightInLongPaper"] =  (ESNumber)device_data.ScanArea.MaxScanAreaHeightInLongPaper.select;
    dict["FixedDocumentSize"] =             (ESNumber)device_data.ScanArea.FixedSize;
    int temp_num = device_data.ScanArea.UserDefine_Width * 100;
    dict["UserScanAreaWidth"] =             (ESNumber)temp_num;
    temp_num = device_data.ScanArea.UserDefine_Length * 100;
    dict["UserScanAreaHeight"] =            (ESNumber)temp_num;
    //dict["ADFHeightMin"] =                  (ESNumber)device_data.ScanArea.ADFHeightMin.select;
    //dict["ADFDuplexHeightMin"] =            (ESNumber)device_data.ScanArea.ADFDuplexHeightMin.select;
    //dict["ADFWidthMin"] =                   (ESNumber)device_data.ScanArea.ADFWidthMin.select;
    dict["DropoutColor"] =                  (ESNumber)device_data.DropoutColor.select;
    dict["Orientation"] =                   (ESNumber)device_data.Orientation.select;
    dict["MultiTiffEnabled"] =              (ESNumber)device_data.ImageFormatSettings.MultiTiffEnabled.select;
    dict["OFDEnabled"] =              (ESNumber)device_data.ImageFormatSettings.OFDEnabled.select;
    dict["MultiTiffCompression"] =          (ESNumber)device_data.ImageFormatSettings.TiffCompression.select;
    dict["Contrast"] =                      (ESNumber)device_data.Contrast.select;
    dict["Brightness"] =                    (ESNumber)device_data.Brightness.select;
    dict["Threshold"] =                     (ESNumber)device_data.Threshold.select;
    dict["JpegQuality"] =                   (ESNumber)device_data.ImageFormatSettings.JpegQuality.select;
    dict["JpegQualityForJpeg"] =            (ESNumber)device_data.ImageFormatSettings.JpegQuality_forJPEG;
    dict["JpegQualityForPdf"] =            (ESNumber)device_data.ImageFormatSettings.JpegQuality_forPDF;
    dict["jpegProgressive"] =               (ESNumber)device_data.ImageFormatSettings.JpegProgressive.select;
    dict["DuplexType"] =                    (ESNumber)device_data.DuplexType.select;
    dict["PagesTobeScanned"] =              (ESNumber)device_data.PagesTobeScanned.select;
    dict["DoubleFeedDetectionLevel"] =      (ESNumber)device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select;
    dict["DoubleFeedDetection"] =           (ESNumber)device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select;
    dict["DoubleFeedDetectionAreaMin"] =    (ESNumber)device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select;
    dict["DoubleFeedDetectionAreaLength"] = (ESNumber)device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select;
    dict["BlankPageSkip"] =                 (ESNumber)device_data.BlankPageSkipKey.select;
    dict["BlankPageSkipLevel"] =            (ESNumber)device_data.BlankPageSkipLevelKey.select;
    dict["BackgroundRemoval"] =             (ESNumber)device_data.BackgroundRemoval.select;
    dict["PaperDeskew"] =                   (ESNumber)device_data.PaperDeskew.select;
    dict["AutoSize"] =                      (ESNumber)device_data.ScanArea.AutoSize.select;
    dict["PaperEndDetection"] =             (ESNumber)device_data.ScanArea.PaperEndDetection.select;
    dict["PaperEndDetectionMemory"] =       (ESNumber)device_data.ScanArea.PaperEndDetectionMemory;
    dict["TransferCompression"] =           (ESNumber)device_data.TransferCompression.select;

    dict["AddPages"] =                      (ESNumber)device_data.Add_Pages;
    dict["FileNamePrefix"] =                (ESString)device_data.Prefix;
    dict["FileNameOverWrite"] =             (ESNumber)device_data.FileName_OverWrite;
    dict["FileNameCounter"] =               (ESNumber)device_data.FileName_Counter;
    dict["PDFAllPages"] =                   (ESNumber)device_data.ImageFormatSettings.PDFImgFormatOption.AllPage;
    dict["PDFSelectPage"] =                 (ESNumber)device_data.ImageFormatSettings.PDFImgFormatOption.page;
    dict["UserDefinePath"] =                (ESString)device_data.UserDefinePath;
    dict["Folder"] =                        (ESNumber)device_data.Folder;

    dict["ImageOption"] =                   (ESNumber)device_data.ImageOption;

    dict["DNShow_PlasticCard_dialog"] =     (ESNumber)device_data.DNShow_PlasticCard_dialog;
    dict["DNShow_LongPaperWarning_dialog"] =     (ESNumber)device_data.DNShow_LongPaperWarning_dialog;

    dict["OverDurationAlert"] =             (ESNumber)device_data.OverDurationAlert;
    dict["NearDurationAlert"] =             (ESNumber)device_data.NearDurationAlert;

    dict["AFMMode"] =                       (ESNumber)device_data.AutomaticFeedingMode.select;
    dict["AFMTimeout"] =                    (ESNumber)device_data.AFMTimeout.select;
    dict["BackgroundColor"] =               (ESNumber)device_data.BackgroundColor.select;

    dict["textEnhance"] =                       (ESNumber)device_data.textEnhance.select;
    dict["textEnhanceNoiseReductionLevel"] =    (ESNumber)device_data.textEnhanceNoiseReductionLevel.select;
    dict["textEnhanceSensitivity"] =            (ESNumber)device_data.textEnhanceSensitivity.select;
    dict["textEnhancePaperCreaseReduction"] =   (ESNumber)device_data.textEnhancePaperCreaseReduction.select;
    dict["thresholdAdjustment"] =               (ESNumber)device_data.thresholdAdjustment.select;

    dict["RemovePunchHole"] =               (ESNumber)device_data.RemovePunchHoles.select;
    dict["DeficiencyCorrection"] =          (ESNumber)device_data.DeficiencyCorrection.select;

    dict["documentType"] =                  (ESNumber)device_data.documentType.select;
    dict["focusPosition"] =                              (ESNumber)device_data.focusPosition.select;

    dict["AutoCroppingInscribed"] =                   (ESNumber)device_data.AutoCroppingInscribed.select;

    //dict["SensorGlassDirtSensitivity"] =    (ESNumber)device_data.SensorGlassDirtSensitivity.select;
    //dict["ADFPaperProtection"] =            (ESNumber)device_data.ADFPaperProtection.select;
    //dict["BehaviorWhenDoubleFeed"] =        (ESNumber)device_data.BehaviorWhenDoubleFeed.select;
    //dict["DirectPowerOn"] =                 (ESNumber)device_data.DirectPowerOn.select;
    //dict["NonConnectPowerOff"] =            (ESNumber)device_data.NonConnectPowerOff.select;

    dict["continuousAutoFeedingMode"] =       (ESNumber)device_data.use_continuousAutoFeedingMode;

    devnum["0"] = dict;
    preset["Preset"] = devnum;

    ES_CMN_FUNCS::JSON::DictionaryToJSONFile(preset, user_settngs_file);
}

bool LastUsedSettings::ReadSettingsFile(std::string model_id, SCANPARA* device_data)
{
    std::string user_settings_file = this->SettingsFilePath.c_str();
    user_settings_file = user_settings_file + model_id + "/UserSettings.SF2";

    //機種IDのディレクトリの中に設定ファイルがあるか
    const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFolder(user_settings_file);
    if ( !result ) {
        // initialize some values
        device_data->ImageFormatSettings.OFDEnabled.select = 0;
        return false;
    }
    this->SetDeviceData(user_settings_file, device_data);

    return true;
}

bool LastUsedSettings::ReadCmdSettingsFile(std::string settings_file_path, SCANPARA* device_data)
{
    //機種IDのディレクトリの中に設定ファイルがあるか
    const bool result =  ES_CMN_FUNCS::PATH::ES_IsExistFolder(settings_file_path);
    if ( !result )  {
        return false;
    }
    this->SetDeviceData(settings_file_path, device_data);

    return true;
}

#include "ESAnyCastUtils.h"

void LastUsedSettings::SetDeviceData(std::string settings_file_path, SCANPARA* device_data)
{
    try{
        ESDictionary out;
        ES_CMN_FUNCS::JSON::JSONFiletoDictionary(settings_file_path, out);

        ESDictionary preset = SAFE_ANY_VALUE_FROM_DICT(out,"Preset",ESDictionary, ESDictionary());//boost::any_cast<ESDictionary>(out["Preset"]);
        ESDictionary devnum = SAFE_ANY_VALUE_FROM_DICT(preset,"0",ESDictionary, ESDictionary());//boost::any_cast<ESDictionary>(preset["0"]);

        device_data->FunctionalUnit.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"FunctionalUnit",ESNumber,ADF);//boost::any_cast<ESNumber>(devnum["FunctionalUnit"]);
        device_data->FunctionalUnit_Auto = SAFE_ANY_VALUE_FROM_DICT(devnum,"FunctionalUnit_Auto",ESNumber,0);//boost::any_cast<ESNumber>(devnum["FunctionalUnit_Auto"]);
        device_data->ColorType.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ColorType",ESNumber,ColorTypeRGB24);//boost::any_cast<ESNumber>(devnum["ColorType"]);
        device_data->AutoColorPixelType.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AutoColorPixelType",ESNumber,AutoColorPixelTypeMono);
        device_data->AutoColorPixelLevel.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AutoColorPixelLevel",ESNumber,0);
        device_data->Resolution.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"Resolution",ESNumber,200);//boost::any_cast<ESNumber>(devnum["Resolution"]);
        device_data->ImageFormatSettings.ImageFormat.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ImageFormat",ESNumber,ImageFormatColorJpegMonoRaw);//boost::any_cast<ESNumber>(devnum["ImageFormat"]);
        device_data->Gamma.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"Gamma",ESNumber,22);//boost::any_cast<ESNumber>(devnum["Gamma"]);
        device_data->ScanArea.ScanAreaWidth.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ScanAreaWidth",ESNumber,0);//boost::any_cast<ESNumber>(devnum["ScanAreaWidth"]);
        device_data->ScanArea.ScanAreaHeight.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ScanAreaHeight",ESNumber,0);//boost::any_cast<ESNumber>(devnum["ScanAreaHeight"]);
        device_data->ScanArea.ScanAreaOffsetX.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ScanAreaOffsetX",ESNumber,0);//boost::any_cast<ESNumber>(devnum["ScanAreaOffsetX"]);
        device_data->ScanArea.ScanAreaOffsetY.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ScanAreaOffsetY",ESNumber,0);//boost::any_cast<ESNumber>(devnum["ScanAreaOffsetY"]);
        //device_data->ScanArea.MaxScanAreaWidth.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"MaxScanAreaWidth",ESNumber,0);//boost::any_cast<ESNumber>(devnum["MaxScanAreaWidth"]);
        //device_data->ScanArea.MaxScanAreaHeight.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"MaxScanAreaHeight",ESNumber,0);//boost::any_cast<ESNumber>(devnum["MaxScanAreaHeight"]);
        //device_data->ScanArea.MaxScanAreaHeightInLongPaper.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"MaxScanAreaHeightInLongPaper",ESNumber,0);//boost::any_cast<ESNumber>(devnum["MaxScanAreaHeightInLongPaper"]);
        device_data->ScanArea.FixedSize = (DocumentSize)SAFE_ANY_VALUE_FROM_DICT(devnum,"FixedDocumentSize",ESNumber,Auto_Detect);//(DocumentSize)boost::any_cast<ESNumber>(devnum["FixedDocumentSize"]);
        device_data->ScanArea.UserDefine_Width = SAFE_ANY_VALUE_FROM_DICT(devnum,"UserScanAreaWidth",ESNumber,0);//(boost::any_cast<ESNumber>(devnum["UserScanAreaWidth"]));
        device_data->ScanArea.UserDefine_Width /= 100;
        device_data->ScanArea.UserDefine_Length = SAFE_ANY_VALUE_FROM_DICT(devnum,"UserScanAreaHeight",ESNumber,0);//(boost::any_cast<ESNumber>(devnum["UserScanAreaHeight"]));
        device_data->ScanArea.UserDefine_Length /= 100;
        //device_data->ScanArea.ADFHeightMin.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ADFHeightMin",ESNumber,0);//(DocumentSize)boost::any_cast<ESNumber>(devnum["ADFHeightMin"]);
        //device_data->ScanArea.ADFDuplexHeightMin.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ADFDuplexHeightMin",ESNumber,0);//(DocumentSize)boost::any_cast<ESNumber>(devnum["ADFDuplexHeightMin"]);
        //device_data->ScanArea.ADFWidthMin.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ADFWidthMin",ESNumber,0);//(DocumentSize)boost::any_cast<ESNumber>(devnum["ADFWidthMin"]);
        device_data->DropoutColor.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DropoutColor",ESNumber,DropoutColorNone);//boost::any_cast<ESNumber>(devnum["DropoutColor"]);
        device_data->Orientation.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"Orientation",ESNumber,OrientationNone);//boost::any_cast<ESNumber>(devnum["Orientation"]);
        device_data->ImageFormatSettings.MultiTiffEnabled.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"MultiTiffEnabled",ESNumber,0);//boost::any_cast<ESNumber>(devnum["MultiTiffEnabled"]);
        device_data->ImageFormatSettings.OFDEnabled.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"OFDEnabled",ESNumber,0);//boost::any_cast<ESNumber>(devnum["OFDEnabled"]);
        device_data->ImageFormatSettings.TiffCompression.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"MultiTiffCompression",ESNumber,TiffCompressionNone);//boost::any_cast<ESNumber>(devnum["MultiTiffCompression"]);
        device_data->Contrast.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"Contrast",ESNumber,0);//boost::any_cast<ESNumber>(devnum["Contrast"]);
        device_data->Brightness.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"Brightness",ESNumber,0);//boost::any_cast<ESNumber>(devnum["Brightness"]);
        device_data->Threshold.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"Threshold",ESNumber,110);//boost::any_cast<ESNumber>(devnum["Threshold"]);
        device_data->ImageFormatSettings.JpegQuality.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"JpegQuality",ESNumber,85);//boost::any_cast<ESNumber>(devnum["JpegQuality"]);
        device_data->ImageFormatSettings.JpegQuality_forJPEG = SAFE_ANY_VALUE_FROM_DICT(devnum,"JpegQualityForJpeg",ESNumber,85);
        device_data->ImageFormatSettings.JpegQuality_forPDF = SAFE_ANY_VALUE_FROM_DICT(devnum,"JpegQualityForPdf",ESNumber,37);
        device_data->ImageFormatSettings.JpegProgressive.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"jpegProgressive",ESNumber,false);//boost::any_cast<ESNumber>(devnum["jpegProgressive"]);
        device_data->DuplexType.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DuplexType",ESNumber,Double_Sided);//boost::any_cast<ESNumber>(devnum["DuplexType"]);
        device_data->PagesTobeScanned.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"PagesTobeScanned",ESNumber,0);//boost::any_cast<ESNumber>(devnum["PagesTobeScanned"]);
        device_data->DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DoubleFeedDetectionLevel",ESNumber,DoubleFeedDetectionLow);//boost::any_cast<ESNumber>(devnum["DoubleFeedDetectionLevel"]);
        device_data->DoubleFeedDetectionValue.DoubleFeedDetection.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DoubleFeedDetection",ESNumber,true);//boost::any_cast<ESNumber>(devnum["DoubleFeedDetection"]);
        device_data->DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DoubleFeedDetectionAreaMin",ESNumber,0);//boost::any_cast<ESNumber>(devnum["DoubleFeedDetectionAreaMin"]);
        device_data->DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DoubleFeedDetectionAreaLength",ESNumber,0);//boost::any_cast<ESNumber>(devnum["DoubleFeedDetectionAreaLength"]);
        device_data->BlankPageSkipKey.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"BlankPageSkip",ESNumber,false);//boost::any_cast<ESNumber>(devnum["BlankPageSkip"]);
        device_data->BlankPageSkipLevelKey.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"BlankPageSkipLevel",ESNumber,10);//boost::any_cast<ESNumber>(devnum["BlankPageSkipLevel"]);
        device_data->BackgroundRemoval.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"BackgroundRemoval",ESNumber,BackgroundRemovalNone);//boost::any_cast<ESNumber>(devnum["BackgroundRemoval"]);
        device_data->PaperDeskew.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"PaperDeskew",ESNumber,true);//boost::any_cast<ESNumber>(devnum["PaperDeskew"]);
        device_data->ScanArea.AutoSize.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AutoSize",ESNumber,true);//boost::any_cast<ESNumber>(devnum["AutoSize"]);
        device_data->ScanArea.PaperEndDetection.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"PaperEndDetection",ESNumber,false);//boost::any_cast<ESNumber>(devnum["PaperEndDetection"]);
        device_data->ScanArea.PaperEndDetectionMemory = SAFE_ANY_VALUE_FROM_DICT(devnum,"PaperEndDetectionMemory",ESNumber,false);//boost::any_cast<ESNumber>(devnum["PaperEndDetectionMemory"]);
        device_data->TransferCompression.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"TransferCompression",ESNumber,true);//boost::any_cast<ESNumber>(devnum["TransferCompression"]);

        device_data->Add_Pages = SAFE_ANY_VALUE_FROM_DICT(devnum,"AddPages",ESNumber,false);//boost::any_cast<ESNumber>(devnum["AddPages"]);
        device_data->Prefix = SAFE_ANY_VALUE_FROM_DICT(devnum,"FileNamePrefix",ESString,"img");//boost::any_cast<ESString>(devnum["FileNamePrefix"]);
        device_data->FileName_OverWrite = SAFE_ANY_VALUE_FROM_DICT(devnum,"FileNameOverWrite",ESNumber,false);
        device_data->FileName_Counter = SAFE_ANY_VALUE_FROM_DICT(devnum,"FileNameCounter",ESNumber,false);

        device_data->ImageFormatSettings.PDFImgFormatOption.AllPage = SAFE_ANY_VALUE_FROM_DICT(devnum,"PDFAllPages",ESNumber,true);//boost::any_cast<ESNumber>(devnum["PDFAllPages"]);
        if(device_data->ImageFormatSettings.PDFImgFormatOption.AllPage){
            device_data->ImageFormatSettings.PDFImgFormatOption.page = SAFE_ANY_VALUE_FROM_DICT(devnum,"PDFSelectPage",ESNumber,0);//boost::any_cast<ESNumber>(devnum["PDFSelectPage"]);
        }else {
            device_data->ImageFormatSettings.PDFImgFormatOption.page = SAFE_ANY_VALUE_FROM_DICT(devnum,"PDFSelectPage",ESNumber,1);
        }

    #ifdef NO_QTMODE
        device_data->UserDefinePath = SAFE_ANY_VALUE_FROM_DICT(devnum,"UserDefinePath",ESString, std::getenv("HOME"));
        device_data->Folder = SAFE_ANY_VALUE_FROM_DICT(devnum,"Folder",ESNumber,USER_DEFINE);
    #else
        device_data->UserDefinePath = SAFE_ANY_VALUE_FROM_DICT(devnum,"UserDefinePath",ESString,(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toLocal8Bit()).constData());//boost::any_cast<ESString>(devnum["UserDefinePath"]);
        device_data->Folder = SAFE_ANY_VALUE_FROM_DICT(devnum,"Folder",ESNumber,Document);//boost::any_cast<ESNumber>(devnum["Folder"]);
    #endif

        device_data->ImageOption = SAFE_ANY_VALUE_FROM_DICT(devnum,"ImageOption",ESNumber,ImageOptionNone);//boost::any_cast<ESNumber>(devnum["ImageOption"]);

        device_data->DNShow_PlasticCard_dialog = SAFE_ANY_VALUE_FROM_DICT(devnum,"DNShow_PlasticCard_dialog",ESNumber,false);//boost::any_cast<ESNumber>(devnum["DNShow_PlasticCard_dialog"]);
        device_data->DNShow_LongPaperWarning_dialog = SAFE_ANY_VALUE_FROM_DICT(devnum,"DNShow_LongPaperWarning_dialog",ESNumber,false);//boost::any_cast<ESNumber>(devnum["DNShow_LongPaperWarning_dialog"]);

        device_data->OverDurationAlert = SAFE_ANY_VALUE_FROM_DICT(devnum,"OverDurationAlert",ESNumber,false);//boost::any_cast<ESNumber>(devnum["OverDurationAlert"]);
        device_data->NearDurationAlert = SAFE_ANY_VALUE_FROM_DICT(devnum,"NearDurationAlert",ESNumber,false);//boost::any_cast<ESNumber>(devnum["NearDurationAlert"]);

        device_data->AutomaticFeedingMode.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AFMMode",ESNumber,false);
        device_data->AFMTimeout.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AFMTimeout",ESNumber,180);
        device_data->BackgroundColor.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"BackgroundColor",ESNumber,BackgroundColorBlack);

        device_data->textEnhance.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"textEnhance",ESNumber,TextEnhanceLevelNone);
        device_data->textEnhanceNoiseReductionLevel.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"textEnhanceNoiseReductionLevel",ESNumber,1);
        device_data->textEnhanceSensitivity.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"textEnhanceSensitivity",ESNumber,0);
        device_data->textEnhancePaperCreaseReduction.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"textEnhancePaperCreaseReduction",ESNumber,false);
        device_data->thresholdAdjustment.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"thresholdAdjustment",ESNumber,0);

        device_data->use_continuousAutoFeedingMode = SAFE_ANY_VALUE_FROM_DICT(devnum,"continuousAutoFeedingMode",ESNumber,false);


        device_data->RemovePunchHoles.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"RemovePunchHole",ESNumber,RemovePunchHoleNone);
        device_data->documentType.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"documentType",ESNumber,Reflective);
        device_data->focusPosition.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"focusPosition",ESNumber,0);
        device_data->DeficiencyCorrection.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DeficiencyCorrection",ESNumber,0);

        device_data->AutoCroppingInscribed.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AutoCroppingInscribed",ESNumber,true);

        //device_data->SensorGlassDirtSensitivity.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"SensorGlassDirtSensitivity",ESNumber,SensorGlassDirtSensitivityOff);
        //device_data->ADFPaperProtection.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ADFPaperProtection",ESNumber,ADFPaperProtection_Off);
        //device_data->BehaviorWhenDoubleFeed.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"BehaviorWhenDoubleFeed",ESNumber,BehaviorWhenDoubleFeed_Immediately);
        //device_data->DirectPowerOn.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DirectPowerOn",ESNumber,DirectPowerOn_Off);
        //device_data->NonConnectPowerOff.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"NonConnectPowerOff",ESNumber,NonConnectPowerOff_Off);
    }
    catch (...) {
        DeleteFile(settings_file_path.c_str());
    }
}
