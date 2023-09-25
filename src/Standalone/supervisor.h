#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "../Include/Controller/EpsonScan2.h"
#include <stdio.h>
#include <dlfcn.h>
#include <iostream>
#include <boost/format.hpp>
#include <string.h>

#include "capitem.h"

#define INIT_POSITION_X 50
#define INIT_POSITION_Y 50
#define INIT_MAINWINDOW_WIDTH 488
#define INIT_MAINWINDOW_HEIGHT 670

#define DEVICE_SEARCH_TIME 1 //search time : 1 sec

typedef enum {CONTINUE, SAVE, CANCEL, ERROR, DISCARD, EDIT}ScanningStatus;

typedef enum {
    IMMEDIATE,
    AFTER_EJECT
} DoubleFeedMode;


typedef struct {
    bool AllPage;
    SDIInt page;
} PDF_IMGFMT_OPTION;

typedef struct {
    SDIInt select;
    SDICapability capability;
} INT_SETTING_VALUE;

typedef struct {
    INT_SETTING_VALUE ScanAreaWidth;
    INT_SETTING_VALUE ScanAreaHeight;
    INT_SETTING_VALUE ScanAreaOffsetX;
    INT_SETTING_VALUE ScanAreaOffsetY;
    INT_SETTING_VALUE MaxScanAreaWidth;
    INT_SETTING_VALUE MaxScanAreaHeight;
    INT_SETTING_VALUE MaxScanAreaHeightInLongPaper;
    INT_SETTING_VALUE ADFHeightMin;
    INT_SETTING_VALUE ADFDuplexHeightMin;
    INT_SETTING_VALUE ADFWidthMin;
    INT_SETTING_VALUE AutoSize;
    INT_SETTING_VALUE PaperEndDetection;
    bool PaperEndDetectionMemory;
    DocumentSize FixedSize;
    double UserDefine_Width;
    double UserDefine_Length;
    double FBWidthMin;
    double FBHeightMin;
    INT_SETTING_VALUE ADFCard;

} SCAN_AREA_VALUE;

typedef struct {
    INT_SETTING_VALUE ImageFormat;
    INT_SETTING_VALUE MultiTiffEnabled;
    INT_SETTING_VALUE OFDEnabled;
    INT_SETTING_VALUE TiffCompression;
    INT_SETTING_VALUE JpegQuality;
    SDIInt JpegQuality_forJPEG;
    SDIInt JpegQuality_forPDF;
    SDIInt DefaultJpegQuality_forPDF;
    INT_SETTING_VALUE JpegProgressive;
    PDF_IMGFMT_OPTION PDFImgFormatOption;
} IMAGE_FORMAT_SETTINGS_VALUE;

typedef struct {
    INT_SETTING_VALUE ScanCounterKey; //Scanner Glass
    INT_SETTING_VALUE ADFScanCounterKey; //Single-Sided
    INT_SETTING_VALUE ADFDuplexScanCounterKey; //Double-Sided
    INT_SETTING_VALUE ADFCarrierSheetScanCounterKey; // Carrier Sheet
    INT_SETTING_VALUE CardScanCounterKey; // Card Slot
    INT_SETTING_VALUE PaperJamCounter; // Paper Jam
    INT_SETTING_VALUE DoubleFeedCounter; //Double Feed
    //ローラーキットカウンター
    INT_SETTING_VALUE RollerKitCounter; //Roller Assembly kit - Number of Scans
    INT_SETTING_VALUE RollerKitLifeLimit; //Roller Assembly Kit - Life Cyde
    INT_SETTING_VALUE RollerKitNearEnd;
    //ローラーカウンター
    INT_SETTING_VALUE RollerCounter; //Roller Assembly - Number of Scans
    INT_SETTING_VALUE RollerLifeLimit;
    INT_SETTING_VALUE RollerNearEnd;
    //分離ローラーカウンター
    INT_SETTING_VALUE RetardRollerCounter;
    INT_SETTING_VALUE RetardRollerLifeLimit;
    INT_SETTING_VALUE RetardRollerNearEnd;

    INT_SETTING_VALUE RestrictFunction;

    INT_SETTING_VALUE CleaningAlertLimitCount;

    INT_SETTING_VALUE CleaningWarningNotifyCount;
    INT_SETTING_VALUE RollerKitNotifyCount;
    INT_SETTING_VALUE RetardRollerNotifyCount;
    INT_SETTING_VALUE SeparationPadNotifyCount;
    INT_SETTING_VALUE PickupRollerNotifyCount;
    INT_SETTING_VALUE CleaningWarningCounter;
    INT_SETTING_VALUE CleaningWarningNotify;
    INT_SETTING_VALUE PaperProtectionCounter;
} CONFIGURE_COUNTER_VALUE;

typedef struct {
    INT_SETTING_VALUE DoubleFeedDetection;
    INT_SETTING_VALUE DoubleFeedDetectionLevel;
    INT_SETTING_VALUE DoubleFeedDetectionAreaMin;
    INT_SETTING_VALUE DoubleFeedDetectionAreaLength;
} DOUBLE_FEED_DETECTION_VALUE;

struct imageEditInfo {
    std::string file_path;
    int angle;
};

typedef struct {

    INT_SETTING_VALUE FunctionalUnit;
    INT_SETTING_VALUE GuidePosition;
    INT_SETTING_VALUE ColorType;
    INT_SETTING_VALUE AutoColorPixelType;
    INT_SETTING_VALUE AutoColorPixelLevel;
    INT_SETTING_VALUE Resolution;
    SCAN_AREA_VALUE   ScanArea;
    INT_SETTING_VALUE DropoutColor;
    INT_SETTING_VALUE Orientation;
    INT_SETTING_VALUE Gamma;
    IMAGE_FORMAT_SETTINGS_VALUE ImageFormatSettings;
    INT_SETTING_VALUE Threshold;
    INT_SETTING_VALUE Brightness;
    INT_SETTING_VALUE Contrast;
    INT_SETTING_VALUE DuplexType;
    INT_SETTING_VALUE PaperLoaded;
    INT_SETTING_VALUE PagesTobeScanned;
    DOUBLE_FEED_DETECTION_VALUE DoubleFeedDetectionValue;
    INT_SETTING_VALUE BlankPageSkipKey;
    INT_SETTING_VALUE BlankPageSkipLevelKey;
    INT_SETTING_VALUE PaperDeskew;
    INT_SETTING_VALUE BackgroundRemoval;
    INT_SETTING_VALUE TransferCompression;
    INT_SETTING_VALUE SleepTime;
    INT_SETTING_VALUE AutoPowerOffTime;
    INT_SETTING_VALUE AutoPowerOffTimeBattery;
    CONFIGURE_COUNTER_VALUE ConfigureCounter;

    INT_SETTING_VALUE SensorGlassDirtSensitivity;
    INT_SETTING_VALUE ADFPaperProtection;
    INT_SETTING_VALUE BehaviorWhenDoubleFeed;
    INT_SETTING_VALUE DirectPowerOn;
    INT_SETTING_VALUE NonConnectPowerOff;

    INT_SETTING_VALUE Use2in1Mode;

    INT_SETTING_VALUE AdfDuplexType;
    INT_SETTING_VALUE ScannerKind;

    INT_SETTING_VALUE FirmwareVersion;
    std::string Firmware_Version_value;
    INT_SETTING_VALUE SerialNumber;
    std::string Serial_Number_value;

    INT_SETTING_VALUE textEnhance;
    INT_SETTING_VALUE textEnhanceNoiseReductionLevel;
    INT_SETTING_VALUE textEnhanceSensitivity;
    INT_SETTING_VALUE textEnhancePaperCreaseReduction;
    INT_SETTING_VALUE thresholdAdjustment;

    INT_SETTING_VALUE filmAreaGuide;
    INT_SETTING_VALUE documentType;
    INT_SETTING_VALUE focusPosition;

    bool FunctionalUnit_Auto;
    bool Add_Pages;
    std::string Prefix;
    bool FileName_OverWrite;
    bool FileName_Counter;
    SDIInt Folder;
    std::string UserDefinePath;
    std::string SelectPath;
    SDIInt ImageOption;

    std::string MaxScanArea_height_table;

    //Do not show flags for dialog
    bool DNShow_PlasticCard_dialog;
    bool DNShow_LongPaperWarning_dialog;

    bool OverDurationAlert;
    bool NearDurationAlert;

    INT_SETTING_VALUE BackgroundColor;

    INT_SETTING_VALUE AutomaticFeedingMode;

    INT_SETTING_VALUE AFMTimeout;

    INT_SETTING_VALUE continuousAutoFeedingMode;

    AdministratorStatus admin_status;

    INT_SETTING_VALUE DefaultPasswordType;

    bool use_continuousAutoFeedingMode;

    INT_SETTING_VALUE RemovePunchHoles;
    
    INT_SETTING_VALUE DeficiencyCorrection;

    INT_SETTING_VALUE ADFSpecialErrorMessageForPassport;

    INT_SETTING_VALUE AutoCroppingInscribed;
} SCANPARA;

typedef void (*ProgessCallBack)(int, int);

class Supervisor {

public:
    Supervisor();

    void* es2lib_;
    SDIDeviceFinder_CreatePtr		   SDIDeviceFinder_CreatePtr_;
    SDIDeviceFinder_DisposePtr		   SDIDeviceFinder_DisposePtr_;
    SDIDeviceFinder_StartDiscoveryPtr  SDIDeviceFinder_StartDiscoveryPtr_;
    SDIDeviceFinder_StopDiscoveryPtr   SDIDeviceFinder_StopDiscoveryPtr_;
    SDIDeviceFinder_GetDevicesPtr      SDIDeviceFinder_GetDevicesPtr_;

    SDIScannerDriver_CreatePtr 			SDIScannerDriver_CreatePtr_;
    SDIScannerDriver_OpenPtr 			SDIScannerDriver_OpenPtr_;
    SDIScannerDriver_ClosePtr 			SDIScannerDriver_ClosePtr_;
    SDIScannerDriver_DisposePtr 		SDIScannerDriver_DisposePtr_;
    SDIScannerDriver_SetValuePtr   		SDIScannerDriver_SetValuePtr_;
    SDIScannerDriver_GetValuePtr        SDIScannerDriver_GetValuePtr_;
    SDIScannerDriver_GetCapabilityPtr   SDIScannerDriver_GetCapability_;
    SDIScannerDriver_DoScanJobPtr     	SDIScannerDriver_DoScanJobPtr_;
    SDIScannerDriver_CheckNextTransferEventPtr SDIScannerDriver_CheckNextTransferEventPtr_;
    SDIScannerDriver_GetNextTransferEventPtr SDIScannerDriver_GetNextTransferEventPtr_;
    SDIImage_CreatePtr					SDIImage_CreatePtr_;
    SDIImage_GetPathPtr					SDIImage_GetPathPtr_;
    SDIImage_DisposePtr 				SDIImage_DisposePtr_;

    SDIImage_GetWidthPtr				SDIImage_GetWidthPtr_;
    SDIImage_GetHeightPtr				SDIImage_GetHeightPtr_;
    SDIImage_GetSamplesPerPixelPtr		SDIImage_GetSamplesPerPixelPtr_;
    SDIImage_GetBitsPerSamplePtr		SDIImage_GetBitsPerSamplePtr_;

    SDIMultipage_StartPtr               SDIMultipage_StartPtr_;
    SDIMultipage_AddPtr                 SDIMultipage_AddPtr_;
    SDIMultipage_FinPtr                 SDIMultipage_FinPtr_;

    SDIDeviceInfo_ResolvePtr            SDIDeviceInfo_ResolvePtr_;

    SDIImage_CheckImageIsBlankPtr       SDIImage_CheckImageIsBlankPtr_;

    SDIScannerDriver_CheckCautionStatusPtr  SDIScannerDriver_CheckCautionStatusPtr_;

    SDIScannerDriver_UnlockAdministratorLockPtr SDIScannerDriver_UnlockAdministratorLockPtr_;

    SDIScannerDriver_LockAdministratorLockPtr SDIScannerDriver_LockAdministratorLockPtr_;

    SCANPARA device_data;
    SDIScannerDriver* driver;

    std::string Filer;

    void SetUp();
    void Terminate();

    SDIError Connecting_Scanner(SDIDeviceInfo devInfo, InterruptEventCallBackProc AlertFunc);
    void Disconnecting_Scanner(void);
    void Dispose_Scanner(void);
    void Cancel_Scanning(void);
    void Continue_Scanning(void);

    bool Get_All_Value(void);
    bool Get_Counter_Value(void);
    bool Set_All_Value(void);
    bool Reset_RollerCouunter_Value(void);
    bool Set_RollerCouunter_Value(void);
    bool Get_Exclusion_Value(void);
    bool Set_Exclusion_Value(void);

    bool GetPaperLoadedStatus(void);
    bool GetUnscannebleScanParameterStatus(void);
    bool GetUnscannebleScanParameterForAFMCStatus(void);
    bool GetScannerPosition(void);
    bool Scan_Start(SDIOperationType operation_type);
    bool Scan_Terminate();
    bool CheckNextTransferEvent(bool wait);
    bool Scanning(void);
    bool CheckScanningError(SDIError outError);
    SDIError CheckDeviceErrorStatus();
    void Scan_Complete(uint32_t imageCount);
    void Get_Image();
    void Save_Path(void);
    bool CheckImageIsBlank();
    //void Save_Image(uint32_t imageCount);


    bool Save_Image2(ProgessCallBack progressCallBack = nullptr);
    void Dispose_Ptr(void);

    bool Check_SupportAdministratorRestricted(void);
    bool Get_AdministratorRestrictedStatus(void);
    bool Send_AdministratorRestrictedPassword(std::string password);

    bool SANEManuNetfinder();
    bool DeviceInfoResolve(SDIDeviceInfo* devInfo);

    SDITransferEventType outEventType;
    SDIError m_LastError;

    ScanningStatus scanning_status;
    bool bAllReset;
    bool edit_mode;
    std::string setting_file_path;

    bool m_DoneBlanckPage; // 1枚以上白紙ページを除去したか？
    bool m_ExistsPage; // 画像が１枚以上存在するか？

    FunctionalUnit m_Preview_FunctionalUnit; //プレビュー時の取り込み装置を記憶する

    ImageFormat m_PreserveImageFormat;

    std::list<imageEditInfo> m_ImageEdit_InfoList = {};

private:
    SDIImage* outImageData;
    std::string output_path;
};

#endif // SUPERVISOR_H
