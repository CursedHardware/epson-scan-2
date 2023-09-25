#pragma once
#ifndef _cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

typedef struct SDIScannerDriver SDIScannerDriver;
typedef struct SDIDeviceFinder  SDIDeviceFinter;
typedef struct SDIImage		    SDIImage;
typedef int32_t SDIInt;
typedef const char SDIChar;

////////////////////////////////////////////////////////////////////////////
#define MAX_MODEL_ID 20
#define MAX_IP_ADDR  64
#define MAX_DISPLAYNAME 50
typedef struct {
	SDIInt version;			     	  	   // out  version of this struct definition
	char modelID[MAX_MODEL_ID];   	   // out ES00xx..
	SDIInt  productID;   				   // out usb pid
	char ipAddress[MAX_IP_ADDR];  	   // out 192..
	char displayName[MAX_DISPLAYNAME]; // displayname
}SDIDeviceInfo;

////////////////////////////////////////////////////////////////////////////

typedef enum
{
  kSDITrasnferEventTypeImage,
  kSDITransferEventTypeComplete,
  kSDITransferEventTypeCancel,
  kSDITransferEventTypeStartContinuousScanInAFM,
  kSDITransferEventTypeStopContinuousScanInAFM
} SDITransferEventType;

////////////////////////////////////////////////////////////////////////////

typedef enum  {
	kSDISupportLevelNone,		 // this feature is not supported
	kSDISupportLevelUnavailable, // this model support this feature, but currently unavailable
	kSDISupportLevelAvailable   //  this feature is available 
}SDISupportLevel;

typedef enum  {
	kSDICapabilitTypeList,
    kSDICapabilitTypeRange
}SDICapabilityType;

#define SDI_CAPABILITY_LISTMAX 20
typedef struct {
	SDIInt version; // version of this struct definition
    SDISupportLevel supportLevel;
    SDICapabilityType capabilityType;	
    
	SDIInt minValue; // this field is only available for "Range" capability 
	SDIInt maxValue; //  this field is only available for "Range" capability 
 
	SDIInt allMinValue;
	SDIInt allMaxValue;

    SDIInt  list[SDI_CAPABILITY_LISTMAX]; // this field is only available for "List" capability
	SDIInt  countOfList;

	SDIInt  allList[SDI_CAPABILITY_LISTMAX]; // all value list supprted by scanner
	SDIInt  countOfAllList;
}SDICapability;

////////////////////////////////////////////////////////////////////////////

typedef enum : SDIInt  { //SDKでコマンドラインから使用されることを考慮し(エラーコード=終了コードとし)0～255の間の値とする
	// Common errors
	kSDIErrorNone					= 0,		//!< エラーなし。
	kSDIErrorUnknownError			= 1,		//!< 不明なエラー。
	kSDIErrorInvalidParam			= 2,		//!< パラメータが不正。
	kSDIErrorFileNotFound			= 3,		//!< 必要なファイルが見つからない

	// Device errors
	kSDIErrorDeviceNotOpened		= 10,		//!< デバイスがオープンされていない。
	kSDIErrorDeviceNotClosed		= 11,		//!< デバイスがクローズされていない。
	kSDIErrorDeviceInUse			= 12,		//!< デバイスが使用中である。(スキャナがコピーで使用中)
	kSDIErrorDeviceInBusy			= 13,		//!< デバイスがビジーである。
	kSDIErrorPaperEmpty				= 14,		//!< 紙が乗っていない。
	kSDIErrorPaperJam				= 15,		//!< 紙ジャムが発生した。
	kSDIErrorPaperDoubleFeed		= 16,		//!< 重送検知が発生した。
	kSDIErrorCoverOpen				= 17,		//!< カバーがオープンしている。
	kSDIErrorTrayClose				= 19,		//!< トレーがクローズしている。
	kSDIErrorCRLock					= 20,		//!< キャリッジがロックしている。
	kSDIErrorLamp					= 21,		//!< ランプエラー。
	kSDIErrorLampTime				= 22,		//!< ランプタイムエラー。
	kSDIErrorAuthFailure			= 23,		//!< 認証エラー
	kSDIErrorNoPermission			= 24,		//!< スキャンを行う権限が無い。
	kSDIErrorLowBattery				= 25,		//!< バッテリーが低下している。
	kSDIErrorDataSend				= 26,		//!< データの送信に失敗
	kSDIErrorDataReceive			= 27,		//!< データの受信に失敗
	kSDIErrorInvalidResponse		= 29,		//!< スキャナからの応答が無効
	kSDIErrorDeviceOpen				= 30,		//!< デバイスが開けない
	kSDIErrorDeviceIncorrect		= 31,		//!< ユーザーが選択した機種と違う機種が接続されている
	kSDIErrorConnectionTimeout		= 32,		//!< 一定時間通信がないため、デバイスが接続を切断した。
	kSDIErrorDeviceDisconnected		= 33,		//!< デバイス側の操作で接続を切断した。
	kSDIErrorDeviceNotFound		    = 34,		//!< デバイスが見つからない。
	kSDIErrorDeviceFatalError		= 35,		//!< デバイスで致命的なエラーが発生した。
	kSDIErrorServerError			= 36,		//!< スキャンサーバーにエラーが発生した。
	kSDIErrorCRSheetSensorError    	= 37,		//!< Carrier sheet sensor error
	kSDIErrorPaperProtect          	= 38,		//!< Paper Protection
	kSDIErrorGapSensorError			= 39, 	//!< Gap Sensor error
	kSDIErrorDeviceFormUnstable		= 40,		//!< デバイスの姿勢が不定状態
	kSDIErrorDeviceFormChangedInterruptedly = 41, //!< デバイスの姿勢がスキャン中に変更された 
	kSDIErrorSepLeverChangedInterruptedly = 42,//!< 　原稿分離レバーがスキャン中に変更された
	kSDIErrorUnscannableDeviceConfig1 = 43,	// !< スキャンできないデバイス設定
	kSDIErrorPaperDoubleFeed2		= 45,		//!< 重送検知が発生した。
	kSDIErrorETSensorError			= 46,		//!< ET Sensor error
	kSDIErrorPESensorError			= 47,       //!< PE Sensor Error

	// File write errors
	kSDIErrorDiskFull				= 100,		//!< ディスクがいっぱい
	kSDIErrorWriteFile				= 102,		//!< ファイル書き込みに失敗
	kSDIErrorWriteTempFile			= 103,		//!< 一時ファイルの書き込みに失敗
	kSDIErrorFileNameConflict		= 104,		//!< ファイル名が衝突
	kSDIErrorCreateWorkDir			= 105,		//!< 作業フォルダが作れない

	// Memory errors
	kSDIErrorNoMemory				= 151,		//!< メモリが足りない

	// check before scan
	kSDIErrorLongPaperUnsupporetedResolution	= 207,		//!< 長尺スキャンを行うには解像度が高すぎる
	kSDIErrorScanAreaTooLarge					= 209,		//!< 領域が広すぎる
	kSDIErrorScanAreaTooSmall					= 210,		//!< 領域が狭すぎる
	kSDIErrorDocumentSeparationLever			= 211,		//!< 分離レバーエラー
	kSDIErrorCardLever							= 212,		//!< カードレバーエラー
	kSDIErrorRotateUnsupportedResolution		= 213,		//!< 「回転：文字向きに合わせる」を行うには解像度が高すぎる

	// JSON errors
	kSDIErrorJsonParse				= 251,	//!< JSONパース失敗
	kSDIErrorValueType				= 254,	//!< 型の不整合

	kSDIErrorModuleNotFound			= 253,	//!< 必要なモジュールが見つからない

	kSDIErrorUserAuthEnabled		= 255,
	kSDIErrorPaperRemoveError      = 326,
    kSDIErrorSkewDetectError       = 327,

} SDIError;

typedef enum : SDIInt 
{
 	kSDIInterruptEventTypePushScan,
	kSDIInterruptEventTypeDisconnect,
	kSDIInterruptEventTypeGlassDirty,
	kSDIInterruptEventTypeCleaningRequired
} SDIInterruptEventType;

typedef enum : SDIInt 
{
	kSDIValueTypeInt,   // SDIInt
	kSDIValueTypeString // pointer to SDIChar
} SDIValueType;

typedef enum : SDIInt 
{
  kSDIOperationTypeNew,
  kSDIOperationTypeCancel,
  kSDIOperationTypeStartAFM,
  kSDIOperationTypeStopAFM,
  kSDIOperationTypeStartAFMC,
  kSDIOperationTypeStopAFMC
} SDIOperationType;


////////////////////////////////////////////////////////////////////////////

typedef void (*DeviceFinderCallBackProc)(SDIDeviceFinder* finder, const SDIDeviceInfo* info, void* userdata);
typedef void (*InterruptEventCallBackProc)(SDIScannerDriver* driver, SDIInterruptEventType event , void* userdata);

////////////////////////////////////////////////////////////////////////////

typedef SDIError (*SDIDeviceFinder_CreatePtr)(SDIDeviceFinter** outFinder);
typedef SDIError (*SDIDeviceFinder_StartDiscoveryPtr)(SDIDeviceFinder* finder,DeviceFinderCallBackProc callBack,  void* param);
typedef SDIError (*SDIDeviceFinder_StopDiscoveryPtr)(SDIDeviceFinder* finder);
typedef SDIError (*SDIDeviceFinder_SetTimeoutPtr)(SDIDeviceFinder* finder, SDIInt seconds);

typedef SDIError (*SDIDeviceFinder_GetDevicesPtr)(SDIDeviceFinder* finder, SDIDeviceInfo** outDevices, SDIInt* outCount);
typedef SDIError (*SDIDeviceFinder_FreeDevicesPtr)(SDIDeviceInfo* devices);
typedef SDIError (*SDIDeviceFinder_DisposePtr)(SDIDeviceFinder* finder);
typedef SDIError (*SDIScannerDriver_CreatePtr)(SDIScannerDriver** driver, 
											   const SDIDeviceInfo* deviceInfo,
									   		   InterruptEventCallBackProc callBack,
									   		   void *userdata);
typedef SDIError (*SDIScannerDriver_OpenPtr)(SDIScannerDriver* driver);
typedef SDIError (*SDIScannerDriver_ResetPtr)(SDIScannerDriver* driver);
typedef SDIError (*SDIScannerDriver_GetAllKeysPtr)(SDIScannerDriver* driver, SDIChar **outKeys, SDIInt* outCount);
typedef SDIError (*SDIScannerDriver_SetValuePtr)(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value, SDIInt size);
typedef SDIError (*SDIScannerDriver_GetValuePtr)(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value, SDIInt size);
typedef SDIError (*SDIScannerDriver_GetDefaultPtr)(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value);
typedef SDIError (*SDIScannerDriver_GetCapabilityPtr)(SDIScannerDriver* driver, SDIChar* key, SDICapability* outCapability);
typedef SDIError (*SDIScannerDriver_DoScanJobPtr)(SDIScannerDriver* driver, 
								   				 SDIOperationType operationType);

typedef bool (*SDIScannerDriver_CheckNextTransferEventPtr)(SDIScannerDriver* driver ,bool waitUntilReceived);

typedef SDIError (*SDIScannerDriver_GetNextTransferEventPtr)(SDIScannerDriver* driver,
											  			 SDITransferEventType* outType,
													     SDIImage* outImageData,
											   		     SDIError* outError);

typedef SDIError (*SDIScannerDriver_ClosePtr)(SDIScannerDriver* driver);
typedef SDIError (*SDIScannerDriver_DisposePtr)(SDIScannerDriver* driver);
typedef SDIError (*SDIImage_CreatePtr)(SDIImage** image);
typedef SDIInt (*SDIImage_GetWidthPtr)(SDIImage* image);
typedef SDIInt (*SDIImage_GetHeightPtr)(SDIImage* image);
typedef SDIInt (*SDIImage_GetSamplesPerPixelPtr)(SDIImage* image);
typedef SDIInt (*SDIImage_GetBitsPerSamplePtr)(SDIImage* image);
typedef SDIInt (*SDIImage_GetPathPtr)(SDIImage* image, SDIChar* buf, SDIInt bufSize);
typedef SDIInt (*SDIImage_DisposePtr)(SDIImage* image);


typedef bool (*SDIDeviceInfo_ResolvePtr)(SDIDeviceInfo* resolveDevice);

typedef void (*SDIScannerDriver_CheckCautionStatusPtr)(SDIScannerDriver* driver);

typedef SDIError (*SDIScannerDriver_UnlockAdministratorLockPtr)(SDIScannerDriver* driver);

typedef SDIError (*SDIScannerDriver_LockAdministratorLockPtr)(SDIScannerDriver* driver);

//////////////////////////////  Keys  /////////////////////////////

/*
 *  specify functional unit to scan 
 *  availability : r/w
 */
static SDIChar* kSDIFunctionalUnitKey = "FunctionalUnit"; 
typedef enum : SDIInt 
{
  kSDIFunctionalUnitFlatbed 	   = 0,
  kSDIFunctionalUnitDocumentFeeder = 1,
  kSDIFunctionalUnitCarrierSheet   = 2
}SDIFunctionalUnit;


/*
 *  returns guide potion
 *  availability : r
 */
static  SDIChar* kSDIGuidePositionKey = "GuidePosition";
typedef enum : SDIInt 
{
  kSDIGuidePositionLeft   = 0,
  kSDIGuidePositionCenter = 1,
  kSDIGuidePositionCenterManual = 2,
  kSDIGuidePositionRight  = 3
}SDIGuidePosition;


/*
 *  returns colorType
 *  availability : r
 */
static  SDIChar* kSDIColorTypeKey  = "ColorType";
typedef enum : SDIInt
{	
		kSDIColorTypeAuto  = -1,
        kSDIColorTypeRGB24 = 0,
        kSDIColorTypeMono8 = 1,
        kSDIColorTypeMono1 = 2,
        kSDIColorTypeRGB48 = 3,
        kSDIColorTypeMono16 = 4
}SDIColorType;

/*  specify resolution to scan image
 *  availability : r/w
 *  value type : SDIInt 
 */
static SDIChar* kSDIResolutionKey = "Resolution"; 

/*
 * specify width of scanned area as p1ixel
 *  availability : r/w
 * value type : SDIInt
 */
static SDIChar* kSDIScanAreaWidthKey = "ScanAreaWidth";

/*
 * specify height of scanned area as pixek
  *  availability : r/w
 * value type : SDIInt
 */
static SDIChar* kSDIScanAreaHeightKey = "ScanAreaHeight";


/*
 * specify offset of scan area as pixel
 *  availability : r/w
 * value type : SDIInt
 */
static SDIChar* kSDIScanAreaOffsetXKey = "ScanAreaOffsetX";

/* 
 * specify offset of scan area as pixel
 * value type : SDIInt
 *  availability : r/w
 */
static  SDIChar* kSDIScanAreaOffsetYKey = "ScanAreaOffsetY";

/* 
 *  returns maximum scan area as inch ( x100)
 *  value type : SDIInt
  *  availability : r
 */
static  SDIChar* kSDIMaxScanAreaWidthKey = "ScanAreaMaxWidth";

/*
 *  returns maximum scan area as inch ( x100)
 *  value type : SDIInt
 *  availability : r
 */
static SDIChar* kSDIMaxScanAreaHeightKey = "ScanAreaMaxHeight";

/*  
 *  returns maximu height in longpaper as inch (x100)
 *  value type : SDIInt
 *  availability : r
 */ 
static SDIChar* kSDIMaxScanAreaHeightInLongPaperKey = "ScanAreaMaxHeightInLongPaper";


/*
*  returns miximu height in long paper as table (json)
*  valut type : SDIChar*
*/
static SDIChar* kSDIMaxScanAreaHeightInLongPaperResolutionTableKey = "ScanAreaMaxHeightInLongPaperResolutionTable";

/*
 * specify dropout color
 * value type : SDIInt
 * availability : r/w
 */
static  SDIChar* kSDIDropoutColorKey = "DropoutColor";
typedef enum : SDIInt
{
	kSDIDropoutColorNone  = 0,
	kSDIDropoutColorRed   = 1,
	kSDIDropoutColorGreen = 2,
	kSDIDropoutColorBlue  = 3
} SDIDropoutColor;

/*
 * specify orientation
 * value type : SDIInt
 * availability : r/w
 */
static SDIChar* kSDIOrientationKey = "Orientation";
typedef enum : SDIInt
{
	kSDIOrientationNone = 0,
	kSDIOrientation90   = 1,
	kSDIOrientation180  = 2,
	kSDIOrientation270  = 3,
	kSDIOrientationAuto = 4
} SDIOrientation;


/* 
 * specify gamma ( x10)
 * value type : SDIInt
 * availability : r/w
 */
static SDIChar* kSDIGammaKey = "Gamma";

/*
 *  specify imageformat
 *  value type : SDIInt 
 *  availability : r/w
 */
static SDIChar* kSDIImageFormatKey = "ImageFormat";
typedef enum : SDIInt
{
  kSDIImageFormatRaw,
  kSDIImageFormatJPEG,
  kSDIImageFormatTIFF,
  kSDIImageFormatPDF,
  kSDIImageFormatPNG,
  kSDIImageFormatPNM,
  kSDIImageFormatColorJpegMonoPNM,
  kSDIImageFormatBMP,
  kSDIImageFormatOFD
}SDIImageFormat;

/*
 *  specify multi-tiff format
 *  value type : SDIInt 
 *  availability : r/w
 */
static SDIChar* kSDIMultiTiffEnabledKey = "MultiTiffEnabled";

/*
 *  returns ofd supported 
 *  value type : SDIInt
 *  availability : r/w
 */
static SDIChar* kSDIOFDSupportedKey = "OFDSupported";

/*
 * specify tiff compression-scheme
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDITiffCompressionKey = "MultiTiffCompression";
typedef enum : SDIInt
{
	kSDITiffCompressionNone,
	kSDITiffCompressionCCITFAX4
} SDITiffCompression;

/*
 * specify threshold
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIThresholdKey = "Threshold";

/*
 * specify brightness
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIBrightnessKey = "Brightness";

/*
 * specify contrast
 * value type : SDIInt 
 * availability : r/w 
 */
static SDIChar* kSDIContrastKey = "Contrast";

/*
 * specify jpeg quality
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIJpegQuality = "JpegQuality";


/**
 *  specify jpeg progressive
 * value type : SDIInt : 0,1
 * availability : r/w
 */
static SDIChar* kSDIJpegProgressiveKey = "jpegProgressive";

/*
 * specify duplex type
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIDuplexTypeKey = "DuplexType";

/*
 * returns paper loaded status
 * value type : SDIInt 
 * availability : r
 */
static SDIChar* kSDIPaperLoadedKey = "PaperLoaded";

/*
 * specify pages to be scanned
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIPagesTobeScannedKey = "PagesTobeScanned";

/* 
 * specify double feed detection
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIDoubleFeedDetectionKey = "DoubleFeedDetection";


/*
*   specify minumu value of double feed detection area
*   value type : SDIInt (inch * 100)
*   availablity ; r/w
*/
static SDIChar* kSDIDoubleFeedDetectionAreaMinKey = "DoubleFeedDetectionAreaMin";

/*
*   specify maximum value of double feed detection area
*   value type : SDIInt (inch * 100)
*   availablity ; r/w
*/
static SDIChar* kSDIDoubleFeedDetectionAreaLengthKey = "DoubleFeedDetectionAreaLength";


/* 
 * specify double feed detection level
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIDoubleFeedDetectionLevelKey = "DoubleFeedDetectionLevel";
typedef enum : SDIInt 
{
  kSDIDoubleFeedDetectionLow  = 0,
  kSDIDoubleFeedDetectionHigh = 1,
  kSDIDoubleFeedDetectionVeryLow  = 2,
} SDIDoubleFeedDetectionLevel;


/*
 * specify blankpage 
 *  value type : SDIInt : 0,1
 *  availability : r/w
*/
static SDIChar* kSDIBlankPageSkipKey	= "BlankPageSkip";

/*
 * specify blank page skip leval
 * value type : SDIInt : 0-30 
 * availability : r/w
 */
static SDIChar* kSDIBlankPageSkipLevelKey = "BlankPageSkipLevel";

/*  
 *  specify papaer deskew 
 *  value type : SDIInt : 0,1
 *  availability : r/w
 */
static SDIChar* kSDIPaperDeskewKey = "PaperDeskew";

/* 
 *  specify background removal
 *  value type : SDIInt 
 *  availability : r/w
 */
static SDIChar* kSDIBackgroundRemovalKey = "BackgroundRemoval";
typedef enum : SDIInt 
{
   kSDIBackgroundRemovalNone = 0,
   kSDIBackgroundRemovalNormal = 1,
   kSDIBackgroundRemovalHigh = 2
}SDIBackgroundRemoval; 


/*
 * specify auto size
 * value type : SDIInt
 * availability : r/w
 */
static SDIChar* kSDIAutoSizeKey = "AutoSize";
typedef enum : SDIInt 
{
	kSDIAutoSizeNone = 0,
	kSDIAutoSizeStandard = 1,
	kSDIAutoSizeLongPaper = 2
}SDIAutoSizeType;


/*
 * specify transfer compression 
 * value type : SDIInt
 * availability : r/w
 */
static SDIChar* kSDITransferCompressionKey = "TransferCompression";
typedef enum : SDIInt 
{
	kSDITransferCompressionRAW = 0,
	kSDITransferCompressionJPEG = 1,
}SDITransferCompression;

/*
 * specify sleep time
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDISleepTimeKey = "SleepTime";

/*
 *  specify poweroff time
 *  value type : SDIInt 
 *  availability : r/w 
 */
 static SDIChar* kSDIAutoPowerOffTimeKey = "AutoPowerOffTime";

 /*
  * specify poweroff time when battery is connected
  * value type : SDIInt 
  * availability : r/w 
  */
 static SDIChar* kSDIAutoPowerOffTimeBatteryKey = "AutoPowerOffTimeBattery";

/*
 * returns scan counter 
 * value type: SDIInt 
 * availability: r
 */
static SDIChar* kSDIScanCounterKey = "ScanCounterKey";

/*
 * returns adf scan counter 
 * value type : SDIInt 
 * availability : r
 */
static SDIChar* kSDIADFScanCounterKey = "ADFScanCounterKey";

/*
 * returns adf scan counter 
 * value type : SDIInt 
 * availability : r
 */
static SDIChar* kSDIADFDuplexCounterKey = "ADFDuplexScanCounterKey";

/*
 * returns carrier sheet adf scan counter 
 * value type : SDIInt 
 * availability : r
 */
static SDIChar* kSDIADFCarrierSheetCounterKey = "ADFCarrierSheetScanCounterKey";

/*
 * returns cardscan counter 
 * value type : SDIInt 
 * availability : r
 */ 
static SDIChar* kSDICardScanCounterKey = "CardScanCounterKey";

/*
 * returns paperjam counter
 * value type : SDIInt
 * availability : r
 */
static SDIChar* kSDIPaperJamCounterKey = "PaperJamCounter";

/*
 * returns double feed counter
 * value type : SDIInt
 * availability : r
 */
static SDIChar* kSDIDoubleFeedCounterKey = "DoubleFeedCounter";

/*
 * returns pickuproller counter
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIPickupRollerCounterKey = "PickupRollerCounter";

/**
 * specify paper end detection 
 *  value type : SDIInt
 *  availability : rw
 */
static SDIChar* kSDIPaperEndDetectionKey = "PaperEndDetection";

/*
 *   returns adf height min (inch)
 */
static SDIChar* kSDIADFHeightMinKey = "ADFHeightMin";

/*
 *   returns duplex adf height min (inch)
 */
static SDIChar* kSDIADFDuplexHeightMinKey = "ADFDuplexHeightMin";

/*
 *   returns duplex adf height min (inch)
 */
static SDIChar* kSDIADFWidthMinKey = "ADFWidthMin";

/**
 *   returns 2in1 mode is enabled
 */
static SDIChar* kSDI2in1ModeKey = "2in1Mode";


/**
 * Scanner Position 
 */
static SDIChar* kSDIScannerPositionKey = "ScannerPositon";
 typedef enum : SDIInt 
{
	kSDIScannerPositionFlat = 0,
	kSDIScannerPositionTilt = 1,
	kSDIScannerPositionInvalid = 2
}SDIScannerPosition;

/*
   スキャン可能なデバイス姿勢かどうか（BOOL
*/
static SDIChar* kSDIUnscannebleScanParameterStatusAllKey = "UnscannebleScanParameter";

/*
   スキャン可能なデバイス姿勢かどうか（BOOL
*/
static SDIChar* kSDIUnscannebleScanParameterForAFMCStatusAllKey = "UnscannebleScanParameterForAFMC";

/**
 * background color 
 */
static SDIChar* kSDIBackgroundColorKey = "BackgroundColor";
 typedef enum : SDIInt 
{
	kSDIBackgroundColorWhite = 0,
	kSDIBackgroundColorBlack = 1,
	kSDIBackgroundColorGray = 2,
	kSDIBackgroundColorNone = 3
}SDIBackgroundColor;


static SDIChar* kSDIAutoColorPixelTypeKey = "AutoColorPixelType";
typedef enum : SDIInt 
{
  kSDIAutoColorPixelTypeMono,
  kSDIAutoColorPixelTypeGray,
  kSDIAutoColorPixelTypeAuto
}SDIAutoColorPixelType;

static SDIChar* kSDIAutoColorPixelLevelKey = "AutoColorPixelLevel";


/*
   AFM
*/
static SDIChar* kSDIAFMModeKey = "AFMMode";


/*
 * returns rooller kit counter
 * value type : SDIInt 
 * availability : r/w
 */
static SDIChar* kSDIRollerKitCounterKey = "RollerKitCounter";

/**
 *  Get Roller Kit life limit 
 */
static SDIChar* kSDIRollerKitLifeLimitKey = "RollerKitLifeLimit";

/**
 *  Get Roller Kit nearend
 */
static SDIChar* kSDIRollerKitNearEndKey = "RollerKitNearEnd";

/**
 *  Get roller counter 
 */
static SDIChar* kSDIRollerCounterKey = "RollerCounter";


/**
 *  Get Roller life limit 
 */
static SDIChar* kSDIRollerLifeLimitKey = "RollerLifeLimit";

/**
 *  Get Roller nearend
 */
static SDIChar* kSDIRollerNearEndKey = "RollerNearEnd";

/**
 *  Get retard roller counter 
 */
static SDIChar* kSDIRetardRollerCounterKey = "RetardRollerCounter";


/**
 *  Get Retard roller life limit 
 */
static SDIChar* kSDIRetardRollerLifeLimitKey = "RetardRollerLifeLimit";

/**
 *  Get Retard Roller nearend
 */
static SDIChar* kSDIRetardRollerNearEndKey = "RetardRollerNearEnd";

/**
 *  Get utilty restrict function enabled
 */
static SDIChar* kSDIRestrictFunctionKey = "RestrictFunction";


/**
 *  Specify AFM timeout 
 */
static SDIChar* kSDIAFMTimeoutKey = "AFMTimeout";


/**
 *  return error code
 */
static SDIChar* kSDIErrorStatusKey = "ErrorStatus";


/*
  Counts of cleaning message count
*/
static SDIChar* kSDICleaningAlertLimitCountKey = "CleaningAlertLimitCount";


/**
 *  specify document type
 */ 
static SDIChar*  kSDIDocumentTypeKey = "DocumentType";
typedef enum : SDIInt 
{
	kSDIDocumentTypeReflective = 0,
	kSDIDocumentTypePositiveFilm = 1,
	kSDIDocumentTypeMonoNegativeFilm = 2,
	kSDIDocumentTypeColorNegativeFilm = 3
}SDIDocumentType;

/**
 *  specify film area guide (bool)
 */ 
static SDIChar* kSDIFilmAreaGuideKey = "FilmAreaGuide";


/*
  Specify Focus Position
*/
static SDIChar* kSDIFocusPositionKey = "FocusPostion";

static SDIChar*  kSDIADFCardKey = "ESADFCard";
typedef enum: uint32_t
{
	kSDIADFCardNotSupport = 0,
	kSDIADFCardSupport,
	kSDIADFCardPortrait,
	kSDIADFCardLandscape,
}SDIADFCard;


static SDIChar*   kSDIDirectPowerOnKey					= "directPowerOn";
typedef enum: uint32_t
{
	kSDIDirectPowerOn_Off = 0,
	kSDIDirectPowerOn_On
}SDIDirectPowerOn;

static SDIChar*   kSDINonConnectPowerOffKey				= "nonConnectPowerOff";	
typedef enum: uint32_t
{
	kSDINonConnectPowerOff_Off = 0,
	kSDINonConnectPowerOff_On
}SDINonConnectPowerOff;

static SDIChar*   kSDIADFPaperProtectionKey				= "adfPaperProtection";	
typedef enum: uint32_t
{
	kSDIADFPaperProtection_Off		= 0,
	kSDIADFPaperProtection_Low		= 1,
	kSDIADFPaperProtection_Normal	= 2,
	kSDIADFPaperProtection_High		= 3,
}SDIADFPaperProtection;

static SDIChar*   kSDIBehaviorWhenDoubleFeedKey			= "behaviorWhenDoubleFeed";
typedef enum: uint32_t
{
	kSDIBehaviorWhenDoubleFeed_Immediately = 0,
	kSDIBehaviorWhenDoubleFeed_StopAfterEjecting
}SDIBehaviorWhenDoubleFeed;

static SDIChar*   kSDICleaningWarningNotifyCountKey		= "cleaningWarningNotifyCount";	


static SDIChar*   kSDIRollerKitNotifyCountKey			= "rollerKitNotifyCount";	

static SDIChar*   kSDIRetardRollerNotifyCountKey		= "retardRollerNotifyCount";

static SDIChar*   kSDISeparationPadNotifyCountKey		= "separationPadNotifyCount";

static SDIChar*   kSDIPickupRollerNotifyCountKey		= "pickupRollerNotifyCount";

static SDIChar*   kSDICleaningWarningCounterKey			= "cleaningWarningCounter";	

static SDIChar*   kSDICleaningWarningNotifyKey		= "cleaningWarningNotify";	
typedef enum: uint32_t
{
	kSDICleaningWarningNotify_Off = 0,
	kSDICleaningWarningNotify_On
}SDICleaningWarningNotiy;


static SDIChar* kSDISensorGlassDirtSensitivity = "sensorGlassDirtySensitivity";

typedef enum: uint32_t
{
	kSDISensorGlassDirtSensitivityOff	= 0,
	kSDISensorGlassDirtSensitivityLow	= 1,
	kSDISensorGlassDirtSensitivityNormal	= 2,
}SDISensorGlassDirtSensitivity;


static SDIChar* kSDIPaperProtectionCounterKey = "paperProtectionCounter";


static SDIChar* KSDIScannerKindKey = "scannerKind";
typedef enum: uint32_t
{
	kSDIKindSFDocument = 11,
	kSDIKindSFPhoto = 12,
	kSDIKindMFBusiness = 21,
	kSDIKindMFConsumer = 22,
	kSDIKindMFLargeFormat = 23,
}SDIScannerKind;


static SDIChar* kSDIADFDuplexTypeKey = "adfDuplexType";
typedef enum: uint32_t
{
	kSDIADFDuplexType1Pass    = 1,
    kSDIADFDuplexType2Pass    = 2,
}SDIDuplexType;

static SDIChar* kSDIFirmwareVersionKey = "firmwareVersion";

static SDIChar* kSDISerialNumberKey = "serialNumber";

static SDIChar* kSDITextEnhanceKey = "textEnhance"; 
typedef enum: uint32_t
{
	kSDITextEnhanceLevelNone	    = 0,
	kSDITextEnhanceLevelStandard    = 1,
    kSDITextEnhanceLevelHigh 	    = 2
}SDITextEnhanceLevel;

static SDIChar* kSDITextEnhanceNoiseReductionLevelKey = "textEnhanceNoiseReductionLevel";

static SDIChar* kSDITextEnhanceSensitivityKey = "textEnhanceSensitivity";

static SDIChar* kSDITextEnhancePaperCreaseReductionKey = "textEnhancePaperCreaseReduction";

static SDIChar* kSDITextEnhanceThresholdAdjustmentKey = "thresholdAdjustment";

static SDIChar* kSDIDefaultPasswordTypeKey				= "defaultPasswordType";
typedef enum: uint32_t 
{
	kSDIDefaultPasswordTypeSerial	= 0,	// 製品シリアル番号
	kSDIDefaultPasswordTypeUnique	= 1,	// ユニークな番号（製品本体に添付）
}SDIDefaultPasswordType;

static SDIChar* kSDIAdminLockKey						= "adminLock";
static SDIChar* kSDIAdminLockEnabledKey				= "adminLockEnabled";		
static SDIChar* kSDIAdminLockPasswordKey				= "adminLockPassword";//String
	
static SDIChar* kSDIContinuousAutoFeedingModeKey		= "continuousAutoFeedingMode";

/*
  Remove punch hole
*/
static SDIChar* kSDIRemovePunchholeKey = "RemovePunchHole";
typedef enum : SDIInt
{
	kSDIRemovePunchHoleNone 	     =  0,   
	kSDIRemovePunchHoleAll       =  1,   ///< 上下左右
	kSDIRemovePunchHoleTopBottom =  2,   ///< 上下
	kSDIRemovePunchHoleLeftRight =  3,   ///< 左右
}SDIRemovePunchhole;



/*
  Inscribed Auto crop
*/
static SDIChar* kSDIAutoCroppingInscribedKey = "AutoCroppingInscribed";



static SDIChar* kSDIDeficiencyCorrectionKey = "DeficienryCorrection";


static SDIChar* kSDIADFSpecialErrorMessageForPassportKey = "ADFSpecialErrorMessageForPassport";


static SDIChar* kSDIDeviceErrorCodeKey = "DeviceErrorCode";




////////////////////////////  DeviceFinder API  ////////////////////////////
/*
 *   Create new deviceFinder  
 *   outFinder: newly created deviceFinder pointer 
 */
SDIError SDIDeviceFinder_Create(SDIDeviceFinter** outFinder);

/*
 *  Discover devices ..
 *  callback : callback functions notify found devices
 *  param    : freely used field, this is a pointer which pass to Devicefindercallbackproc  
 */
SDIError SDIDeviceFinder_StartDiscovery(SDIDeviceFinder* finder,DeviceFinderCallBackProc callBack,  void* userdata);

/*  
 *  Stop devoce discovery
 */
SDIError SDIDeviceFinder_StopDiscovery(SDIDeviceFinder* finder);

/*
 *  seconds: timeout duration
 */
SDIError SDIDeviceFinder_SetTimeout(SDIDeviceFinder* finder, SDIInt seconds);

/* 
 *  outDevices : pointer to array of found devices
 *  outCount   : number of found devices
 *
 *  The array will be disposed when DeviceFinder has disposed, client could not dispose array 
 */
SDIError SDIDeviceFinder_GetDevices(SDIDeviceFinder* finder, SDIDeviceInfo** outDevices, SDIInt* outCount);

/*
 * dispose finder
 */
SDIError SDIDeviceFinder_Dispose(SDIDeviceFinder* finder);

////////////////////////////  Resolve API //////////////////////////

/*
  Resolve API
*/
bool SDIDeviceInfo_Resolve(SDIDeviceInfo* resolveDevice);


////////////////////////////  Scan API  ////////////////////////////

/*
 * Creates new ScannerDriver
 *
 * driver : newly created scannerDriver pointer 
 */
SDIError SDIScannerDriver_Create(SDIScannerDriver** driver, 
		 						 const SDIDeviceInfo* deviceInfo,
							   	 InterruptEventCallBackProc callBack,
							  	 void *userdata);

/*
 *  Open device 
 * 
 *  deviceInfo : information of target device
 *  callback   : event handler callback
 *  param      : freely used field, pass to callback argument
 */
SDIError SDIScannerDriver_Open(SDIScannerDriver* driver);


/*
 * Reset all parameters to default 
 */
SDIError SDIScannerDriver_Reset(SDIScannerDriver* driver);

/*
 *  outKeys: pointer to array of all supported keys. This will be deallocated by driver when driver disposed.
 *  outCount : pointer to count of array  
 */
SDIError SDIScannerDriver_GetAllKeys(SDIScannerDriver* driver, SDIChar **outKeys, SDIInt* outCount);

/*
 *  Set parameter
 *
 *  key : key of parameter to be set
 *  type: value type of key client code assumed this is used for validation
 *  void* : value
 *  size  : size of value / kSDIValueTypeInt : sizeof(SDIInt) , kSDIValueTypeString : size of string buffer
 */ 
SDIError SDIScannerDriver_SetValue(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value, SDIInt size);

/*
 *  Get parameter
 *  
 *  key : key of parameter to be gotten
 *  type : value type of key client code assumed this is used for validation
 *  void* : value ( will be written by library)
 *  size  : size of value / kSDIValueTypeInt : sizeof(SDIInt) , kSDIValueTypeString : size of string buffer
 *
 *  when you trys to get "kSDIValueTypeString", you have to allocates enough size of buffer (depending on key specification) and pass it.
 *  Library will copy the string to your buffer. 
 */
SDIError SDIScannerDriver_GetValue(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value, SDIInt size);


/*
 *  Get default
 *  
 *  key : key of parameter to be gotten
 *  type : value type of key client code assumed this is used for validation
 *  void* : value
 */
SDIError SDIScannerDriver_GetDefault(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value);

/*  
 * Get Capabiluty
 * 
 * key : key of parameter to be gotton capability 
 * outCapability : out capability updated by driver 
 */
SDIError SDIScannerDriver_GetCapability(SDIScannerDriver* driver, SDIChar* key, SDICapability* outCapability);



/*
*  CheckCautionStatus 
*/
void  SDIScannerDriver_CheckCautionStatus(SDIScannerDriver* driver);



SDIError  SDIScannerDriver_UnlockAdministratorLock(SDIScannerDriver* driver);

SDIError  SDIScannerDriver_LockAdministratorLock(SDIScannerDriver* driver);

/*
 *  Start scan job
 *
 *  ====== This is a scan sequence ==========================
 *
 *  client request scan operation by this method and receive event by "Getnexttransferevent"
 *  Client must report this producer, until all scanning finished
 *
 *   DoScanJob -> GetNextTransferEvent -> DoScanJob (Cancel)
 *   Doscanjob -> Getnexttransferevent -> Getnexttransferevent  
 *  ============================================
 *
 *   operationType : type of operation
 *   				 NewScan  : start Scan
 *   				 Cancel   : cancel 
 *					 
 *	 nextTransferevent : this is available when operationtype = NextTransferEvent
 *   					 if operationtype != Nexttransferevent this argument ignored
 *
 * 	outError : errroCode this is available when 
 * 						operationtype = Nexttransferevent && eventType == error 
 *												
 */
SDIError SDIScannerDrive_DoScanJob(SDIScannerDriver* driver, 
								   SDIOperationType operationType);


/*
 *  CheckNext Event
 *  
 *  When event has been arraived, this function returns true
 *
 *
 */
bool  SDIScannerDriver_CheckNextTransferEvent(SDIScannerDriver* driver, bool waitUntilReceived);

/*
 *
 *  outType  : Type of next event (eg. receive image, error, complete)  
 *  outImageData : pointer to the received image, this is just available when outType == image
 *  			   client should set empty image data here
 *  outError     : pointer to the received error, this is just available when outType == error , interrupt 
 *
 */
SDIError SDIScannerDriver_GetNextTransferEvent(SDIScannerDriver* driver,
											   SDITransferEventType* outType,
											   SDIImage* outImageData,
											   SDIError* outError
											  );




/*
 * close driver
 */
SDIError SDIScannerDriver_Close(SDIScannerDriver* driver);


/* 
 * dispose scanner driver
 */
SDIError SDIScannerDriver_Dispose(SDIScannerDriver* driver);

////////////////////////////  ImageData API  ////////////////////////////

/*
 *  Create empty image data
 */
SDIError SDIImage_Create(SDIImage** image);

/*
 * Get the width of image
 */
SDIInt SDIImage_GetWidth(SDIImage* image);

/*
 * Get the height of image
 */
SDIInt SDIImage_GetHeight(SDIImage* image);

/*
 * Get the samples per pixel of image
 */
SDIInt SDIImage_GetSamplesPerPixel(SDIImage* image);

/*
 *  Get the bits per sample of image
*/
SDIInt SDIImage_GetBitsPerSample(SDIImage* image);

/*
 *  Get the path of image
 *	
 *	buf: buffer to be coppied path string
 *  bufsize : size of client allocated buffer
 */
SDIInt SDIImage_GetPath(SDIImage* image, int8_t * buf, SDIInt bufSize);


typedef bool (*SDIImage_CheckImageIsBlankPtr)(SDIImage* image);
/**
 *  Check is image is blank
 */
SDIInt SDIImage_CheckImageIsBlank(SDIImage* image);

/*
 *   dispose image data
 */
SDIInt SDIImage_Dispose(SDIImage* image);


////////////////////////////  Multipage API  ////////////////////////////
typedef bool (*SDIMultipage_StartPtr)(SDIScannerDriver* driver, const char* destPath, SDIImageFormat format);
typedef bool (*SDIMultipage_AddPtr)(SDIScannerDriver* driver, const char* sourcePath);
typedef bool (*SDIMultipage_FinPtr)(SDIScannerDriver* driver);
/*
	Create multi page file

	destPath : destination of multi page file path
	sourcePath : input image path. File format must be (uniquecount)_(width)_(height)_(samplesPerPixel)_(bitsPerSample).(extension)
*/
bool SDIMultipage_Start(SDIScannerDriver* driver, const char* destPath, SDIImageFormat format);

bool SDIMultipage_Add(SDIScannerDriver* driver, const char* sourcePath);

bool SDIMultipage_Fin(SDIScannerDriver* driver);


#ifndef _cplusplus
}
#endif


