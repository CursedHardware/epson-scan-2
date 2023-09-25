#include "defaultsettings.h"

#include <fstream>
#define BOOST_NO_CXX11_RVALUE_REFERENCES 1
#include <boost/any.hpp>
#include <utils/PathUtils.h>

#if !(defined(NO_QTMODE) || defined(BUILDSANE))
#include <QStandardPaths>
#endif

#include "devicelist.h"
#include "../Include/Config.h"

DefaultSettings::DefaultSettings()
{
#ifdef AKBMODE
   std::string strDst = ".";
   strDst = strDst + DRIVER_NAME;

   std::string dir3((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
   ES_CMN_FUNCS::PATH::ES_CombinePath(dir3, dir3, (strDst + "/").c_str());
   this->SettingsFilePath = dir3.c_str();
#else

#if defined(NO_QTMODE) || defined(BUILDSANE)

   std::string dir3 = std::getenv("HOME");
   ES_CMN_FUNCS::PATH::ES_CombinePath(dir3, dir3,".epsonscan2/");
   this->SettingsFilePath = dir3.c_str();
#else

   std::string dir3((QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toLocal8Bit()).constData());
   ES_CMN_FUNCS::PATH::ES_CombinePath(dir3, dir3,".epsonscan2/");
   this->SettingsFilePath = dir3.c_str();
#endif
#endif
}

//DefaultSettingsファイル関連
bool DefaultSettings::CheckDefaultSettingsFile()
{
#if defined(NO_QTMODE) || defined(BUILDSANE)
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

    std::string default_settings_file = this->SettingsFilePath.c_str();
    ES_CMN_FUNCS::PATH::ES_CombinePath(default_settings_file, default_settings_file, "DefaultSettings.SF2");
    return ES_CMN_FUNCS::PATH::ES_IsExistFile(default_settings_file);
}

void DefaultSettings::DeleteDefaultSettingsFile()
{
    std::string default_settings_file = this->SettingsFilePath.c_str();
    default_settings_file = default_settings_file + "DefaultSettings.SF2";
    //元のDefaultSettingsファイルを削除
    try {
        DeleteFile(default_settings_file.c_str());
    }
    catch (...) {
        throw;
    }
}

bool DefaultSettings::ReadSettingsFile(SCANPARA* device_data)
{
    std::string user_settings_file = this->SettingsFilePath.c_str();
    user_settings_file = user_settings_file + "/DefaultSettings.SF2";

    //設定ファイルがあるか
    const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFolder(user_settings_file);
    if ( !result ) {
        return false;
    }
    this->SetDeviceData(user_settings_file, device_data);

    return true;
}

#include "ESAnyCastUtils.h"

void DefaultSettings::SetDeviceData(std::string settings_file_path, SCANPARA* device_data)
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
        device_data->ImageFormatSettings.TiffCompression.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"MultiTiffCompression",ESNumber,TiffCompressionNone);//boost::any_cast<ESNumber>(devnum["MultiTiffCompression"]);
        device_data->ImageFormatSettings.OFDEnabled.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"OFDEnabled",ESNumber,0);//boost::any_cast<ESNumber>(devnum["OFDEnabled"]);
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
        device_data->AutoCroppingInscribed.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"AutoCroppingInscribed",ESNumber,true);

    #if defined(NO_QTMODE) || defined(BUILDSANE)
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

        //device_data->SensorGlassDirtSensitivity.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"SensorGlassDirtSensitivity",ESNumber,SensorGlassDirtSensitivityOff);
        //device_data->ADFPaperProtection.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"ADFPaperProtection",ESNumber,ADFPaperProtection_Off);
        //device_data->BehaviorWhenDoubleFeed.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"BehaviorWhenDoubleFeed",ESNumber,BehaviorWhenDoubleFeed_Immediately);
        //device_data->DirectPowerOn.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"DirectPowerOn",ESNumber,DirectPowerOn_Off);
        //device_data->NonConnectPowerOff.select = SAFE_ANY_VALUE_FROM_DICT(devnum,"NonConnectPowerOff",ESNumber,NonConnectPowerOff_Off);
    }
    catch (...) {
        std::cout << "A problem was found in the contents of the DefaultSettings.SF2 file." << std::endl;
        //DeleteFile(settings_file_path.c_str());
        throw;
    }
}
