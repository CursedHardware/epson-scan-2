////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2Scanner.h
//!
//! @brif     ESCI/2コマンド用スキャナクラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ESCI2Scanner.h"


//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESCI2Scanner::CESCI2Scanner()
{
	PROPERTY_R ( kESVersion,                            CESCI2Scanner, ESString,     GetVersion                                                                         ); // r    ESString
	PROPERTY_R ( kESProductName,                        CESCI2Scanner, ESString,     GetProductName                                                                     ); // r    ESString
	PROPERTY_RW( kESDisableJobContinue,                 CESCI2Scanner, bool,         IsDisableJobContinue,                  bool,     SetDisableJobContinue             ); // rw   bool
	PROPERTY_RW( kESDisableKeepingCapture,              CESCI2Scanner, bool,         IsDisableKeepingCapture,               bool,     SetDisableKeepingCapture          ); // rw   bool
	PROPERTY_R ( kESBatteryStatus,                      CESCI2Scanner, ESNumber,     GetBatteryStatus                                                                   ); // r    ESNumber (ESBatteryStatus)
	PROPERTY_R ( kESErrorCode,                    		CESCI2Scanner, ESNumber,     GetErrorCode                                                               		); // r    ESNumber
	PROPERTY_R ( kESSensorGlassStatus,                  CESCI2Scanner, ESNumber,     GetSensorGlassStatus                                                               ); // r    ESNumber
	PROPERTY_R ( kESScannerPositionStatus,              CESCI2Scanner, ESNumber,     GetScannerPositionStatus                                                           ); // r    ESNumber
	PROPERTY_RW( kESSensorGlassDirtSensitivity,         CESCI2Scanner, ESNumber,     GetSensorGlassDirtSensitivity,         ESNumber, SetSensorGlassDirtSensitivity     ); // rw   ESNumber
	PROPERTY_RW( kESInterruptionEnabled,                CESCI2Scanner, bool,         IsInterruptionEnabled,                 bool,     SetInterruptionEnabled            ); // rw   bool
	PROPERTY_R ( kESLengthPadding,                      CESCI2Scanner, bool,         IsLengthPaddingSupported                                                           ); // r    bool
	PROPERTY_R ( kESAutoFeedingMode,                    CESCI2Scanner, bool,         IsAfmEnabled                                                                       ); // r    bool
	PROPERTY_R ( kESContinuousAutoFeedingMode,          CESCI2Scanner, bool,         IsContinuousAutoFeedingModeSupported                                               ); // r    bool
	PROPERTY_RW( kESAutoFeedingModeTimeout,             CESCI2Scanner, ESNumber,     GetAutoFeedingModeTimeout,             ESNumber, SetAutoFeedingModeTimeout         ); // rw   ESNumber (minutes)
	PROPERTY_R ( kESSerialNumber,                       CESCI2Scanner, ESString,     GetSerialNumber                                                                    ); // r    ESString
	PROPERTY_R ( kESMaxImagePixels,                     CESCI2Scanner, ST_ES_SIZE_F, GetMaxImagePixels                                                                  ); // r    ST_ES_SIZE_F
	PROPERTY_R ( kESMaxScanSizeInLongLength,            CESCI2Scanner, ST_ES_SIZE_F, GetMaxScanSizeInLongLength                                                         ); // r    ST_ES_SIZE_F
	PROPERTY_R ( kESMaxLongLengthTable,                 CESCI2Scanner, ESDicArray,   GetMaxLongLengthTable                                                              ); // r    ESDicArray
	PROPERTY_R ( kESMinScanSize,                        CESCI2Scanner, ST_ES_SIZE_F, GetMinScanSize                                                                     ); // r    ST_ES_SIZE_F
	PROPERTY_RW( kESImageFormat,                        CESCI2Scanner, ESNumber,     GetImageFormat,                        ESNumber, SetImageFormat                    ); // rw   ESNumber (ESImageFormat)
	PROPERTY_RW( kESColorMatrixMode,                    CESCI2Scanner, ESNumber,     GetColorMatrixMode,                    ESNumber, SetColorMatrixMode                ); // rw   ESNumber (ESColorMatrixMode)
	PROPERTY_RW( kESLaminatedPaperMode,                 CESCI2Scanner, ESNumber,     GetLaminatedPaperMode,                 ESNumber, SetLaminatedPaperMode             ); // rw   ESNumber (ESColorMatrixMode)
	PROPERTY_RW( kESSkewCorrection,                     CESCI2Scanner, bool,         IsSkewCorrectionEnabled,               bool,     SetSkewCorrectionEnabled          ); // rw   bool
	PROPERTY_RW( kESJPEGQuality,                        CESCI2Scanner, ESNumber,     GetJPEGQuality,                        ESNumber, SetJPEGQuality                    ); // rw   ESNumber
	PROPERTY_RW( kESPaperEndDetection,                  CESCI2Scanner, bool,         IsPaperEndDetectionEnabled,            bool,     SetPaperEndDetectionEnabled       ); // rw   bool
	PROPERTY_R ( kESCarrierSheetDetection,              CESCI2Scanner, bool,         IsCarrierSheetDetectionSupported                                                   ); // r    bool
	PROPERTY_RW( kESOverScan,                           CESCI2Scanner, bool,         IsOverScanEnabled,                     bool,     SetOverScanEnabled                ); // rw   bool
	PROPERTY_RW( kESOverScanForCropping,                CESCI2Scanner, bool,         IsOverScanForCroppingEnabled,          bool,     SetOverScanForCroppingEnabled    	); // rw   bool
	PROPERTY_RW( kESPassportCarrierSheetScan,           CESCI2Scanner, bool,         IsPassportCarrierSheetSupported,       bool,     SetPassportCarrierSheetEnabled    ); // rw   bool
	PROPERTY_R ( kESMinDoubleFeedDetectionRangeLength,  CESCI2Scanner, ESFloat,      GetMinDoubleFeedDetectionRangeLength                                               ); // r    ESFloat
	PROPERTY_RW( kESDoubleFeedDetectionRangeOffset,     CESCI2Scanner, ESFloat,      GetDoubleFeedDetectionRangeOffset,     ESFloat, SetDoubleFeedDetectionRangeOffset  ); // rw   ESFloat
	PROPERTY_RW( kESDoubleFeedDetectionRangeLength,     CESCI2Scanner, ESFloat,      GetDoubleFeedDetectionRangeLength,     ESFloat, SetDoubleFeedDetectionRangeLength  ); // rw   ESFloat
	PROPERTY_RW( kESLenghtDoubleFeedDetection,          CESCI2Scanner, bool,         IsLengthDoubleFeedDetectionEnabled,    bool,  SetLengthDoubleFeedDetectionEnabled  ); // rw   bool
	PROPERTY_RW( kESLengthDoubleFeedDetectionLength,    CESCI2Scanner, ESFloat,      GetLengthDoubleFeedDetectionLength,    ESFloat, SetLengthDoubleFeedDetectionLength ); // rw   ESFloat
	PROPERTY_RW( kESImageDoubleFeedDetection,           CESCI2Scanner, bool,         IsImageDoubleFeedDetectionEnabled,     bool,   SetImageDoubleFeedDetectionEnabled  ); // rw   bool
	PROPERTY_RW( kESImagePaperProtection,               CESCI2Scanner, bool,         IsImagePaperProtectionEnabled,         bool,     SetImagePaperProtectionEnabled    ); // rw   bool
	PROPERTY_RW( kESFilmType,                     		CESCI2Scanner, ESNumber,     GetFilmType,                    		ESNumber, SetFilmType    	                ); // rw   ESNumber (ENUM_ES_FILM_TYPE)
	PROPERTY_R ( kESDetectedDocumentSize,               CESCI2Scanner, ST_ES_SIZE_F, GetDetectedDocumentSize                                                            ); // r    ST_ES_SIZE_F
	PROPERTY_RW( kESAutoCropping,                       CESCI2Scanner, bool,         IsAutoCroppingEnabled,                 bool,     SetAutoCroppingEnabled            ); // rw   bool
	PROPERTY_RW( kESAutoCroppingInscribed,              CESCI2Scanner, bool,         IsAutoCroppingInscribedEnabled,        bool,     SetAutoCroppingInscribedEnabled   ); // rw   bool
	PROPERTY_R ( kESCroppableResolutions,               CESCI2Scanner, ESAny,        GetCroppableResolutions                                                            ); // r    ESIndexSet or ST_ES_RANGE
	PROPERTY_RW( kESCroppingSize,                       CESCI2Scanner, ESFloat,      GetCroppingSize,                       ESFloat,  SetCroppingSize                   ); // rw   ESFloat
	PROPERTY_RW( kESEdgeFillColor,                      CESCI2Scanner, ESNumber,     GetEdgeFillColor,                      ESNumber, SetEdgeFillColor                  ); // rw   ESNumber (ESEdgeFillColor)
	PROPERTY_RW( kESEdgeFillWidthRight,                 CESCI2Scanner, ESFloat,      GetEdgeFillWidthRight,                 ESFloat,  SetEdgeFillWidthRight             ); // rw   ESFloat
	PROPERTY_RW( kESEdgeFillWidthTop,                   CESCI2Scanner, ESFloat,      GetEdgeFillWidthTop,                   ESFloat,  SetEdgeFillWidthTop               ); // rw   ESFloat
	PROPERTY_RW( kESEdgeFillWidthLeft,                  CESCI2Scanner, ESFloat,      GetEdgeFillWidthLeft,                  ESFloat,  SetEdgeFillWidthLeft              ); // rw   ESFloat
	PROPERTY_RW( kESEdgeFillWidthBottom,                CESCI2Scanner, ESFloat,      GetEdgeFillWidthBottom,                ESFloat,  SetEdgeFillWidthBottom            ); // rw   ESFloat
	PROPERTY_RW( kESPowerSaveTime,                      CESCI2Scanner, ESNumber,     GetPowerSaveTime,                      ESNumber, SetPowerSaveTime                  ); // rw   ESNumber
	PROPERTY_RW( kESPowerOffTime,                       CESCI2Scanner, ESNumber,     GetPowerOffTime,                       ESNumber, SetPowerOffTime                   ); // rw   ESNumber
	PROPERTY_RW( kESPowerOffTime2nd,                    CESCI2Scanner, ESNumber,     GetPowerOffTime2nd,                    ESNumber, SetPowerOffTime2nd                ); // rw   ESNumber
	PROPERTY_R ( kESMaxFocus,                 		    CESCI2Scanner, ESFloat,      GetMaxFocus                                                               			); // r    ESFloat
	PROPERTY_R ( kESMinFocus,                     		CESCI2Scanner, ESFloat,      GetMinFocus                                                              		    ); // r    ESFloat
	PROPERTY_RW( kESFocus,                        		CESCI2Scanner, ESFloat,      GetFocus,                      		ESFloat,  SetFocus                        	); // rw   ESFloat
	
	PROPERTY_R ( kESFirstPCConnectionDate,              CESCI2Scanner, ESString,     GetFirstPCConnectionDate                                                           ); // r    ESString
	PROPERTY_RW( kESSimplexScanCounter,                 CESCI2Scanner, ESNumber,     GetSimplexScanCounter,                 ESNumber, SetSimplexScanCounter             ); // rw   ESNumber
	PROPERTY_RW( kESDuplexScanCounter,                  CESCI2Scanner, ESNumber,     GetDuplexScanCounter,                  ESNumber, SetDuplexScanCounter              ); // rw   ESNumber
	PROPERTY_RW( kESDocumentFeederLamp1Counter,         CESCI2Scanner, ESNumber,     GetDocumentFeederLamp1Counter,         ESNumber, SetDocumentFeederLamp1Counter     ); // rw   ESNumber
	PROPERTY_RW( kESDocumentFeederLamp2Counter,         CESCI2Scanner, ESNumber,     GetDocumentFeederLamp2Counter,         ESNumber, SetDocumentFeederLamp2Counter     ); // rw   ESNumber
	PROPERTY_RW( kESPaperJamCounter,                    CESCI2Scanner, ESNumber,     GetPaperJamCounter,                    ESNumber, SetPaperJamCounter                ); // rw   ESNumber
	PROPERTY_RW( kESUltraSonicDoubleFeedCounter,        CESCI2Scanner, ESNumber,     GetUltraSonicDoubleFeedCounter,        ESNumber, SetUltraSonicDoubleFeedCounter    ); // rw   ESNumber
	PROPERTY_RW( kESLengthDoubleFeedCounter,            CESCI2Scanner, ESNumber,     GetLengthDoubleFeedCounter,            ESNumber, SetLengthDoubleFeedCounter        ); // rw   ESNumber
	PROPERTY_RW( kESImageDoubleFeedCounter,             CESCI2Scanner, ESNumber,     GetImageDoubleFeedCounter,             ESNumber, SetImageDoubleFeedCounter         ); // rw   ESNumber
	PROPERTY_RW( kESPaperProtectionCounter,             CESCI2Scanner, ESNumber,     GetPaperProtectionCounter,             ESNumber, SetPaperProtectionCounter         ); // rw   ESNumber
	PROPERTY_RW( kESRollerKitCounter,                   CESCI2Scanner, ESNumber,     GetRollerKitCounter,                   ESNumber, SetRollerKitCounter               ); // rw   ESNumber
	PROPERTY_RW( kESRetardRollerCounter,                CESCI2Scanner, ESNumber,     GetRetardRollerCounter,                ESNumber, SetRetardRollerCounter            ); // rw   ESNumber
	PROPERTY_RW( kESPickupRollerCounter,                CESCI2Scanner, ESNumber,     GetPickupRollerCounter,                ESNumber, SetPickupRollerCounter            ); // rw   ESNumber
	PROPERTY_RW( kESSeparationPadCounter,               CESCI2Scanner, ESNumber,     GetSeparationPadCounter,               ESNumber, SetSeparationPadCounter           ); // rw   ESNumber
	PROPERTY_RW( kESSpecialDocumentCounter,             CESCI2Scanner, ESNumber,     GetSpecialDocumentCounter,             ESNumber, SetSpecialDocumentCounter         ); // rw   ESNumber
	PROPERTY_RW( kESPassportCarrierSheetCounter,        CESCI2Scanner, ESNumber,     GetPassportCarrierSheetCounter,        ESNumber, SetPassportCarrierSheetCounter    ); // rw   ESNumber
	PROPERTY_RW( kESScanCounter,                        CESCI2Scanner, ESNumber,     GetScanCounter,                        ESNumber, SetScanCounter                    ); // rw   ESNumber
	PROPERTY_RW( kESSimplexCardScanCounter,             CESCI2Scanner, ESNumber,     GetSimplexCardScanCounter,             ESNumber, SetSimplexCardScanCounter         ); // rw   ESNumber
	PROPERTY_RW( kESDuplexCardScanCounter,              CESCI2Scanner, ESNumber,     GetDuplexCardScanCounter,              ESNumber, SetDuplexCardScanCounter          ); // rw   ESNumber
	PROPERTY_RW( kESFlatbedLamp1Counter,                CESCI2Scanner, ESNumber,     GetFlatbedLamp1Counter,                ESNumber, SetFlatbedLamp1Counter            ); // rw   ESNumber
	PROPERTY_RW( kESFlatbedLamp2Counter,                CESCI2Scanner, ESNumber,     GetFlatbedLamp2Counter,                ESNumber, SetFlatbedLamp2Counter            ); // rw   ESNumber
	PROPERTY_R ( kESDocumentSeparation,                 CESCI2Scanner, ESNumber,     GetDocumentSeparation                                                              ); // r    bool
	PROPERTY_R ( kESCardScanning,                       CESCI2Scanner, bool,         IsCardScanningEnabled                                                              ); // r    bool
	PROPERTY_RW( kESDocumentTopCorrectionFront,         CESCI2Scanner, ESFloat,      GetDocumentTopCorrectionFront,         ESFloat,  SetDocumentTopCorrectionFront     ); // rw   ESFloat
	PROPERTY_RW( kESDocumentTopCorrectionBack,          CESCI2Scanner, ESFloat,      GetDocumentTopCorrectionBack,          ESFloat,  SetDocumentTopCorrectionBack      ); // rw   ESFloat
	PROPERTY_RW( kESDocumentDriveCorrectionFront,       CESCI2Scanner, ESFloat,      GetDocumentDriveCorrectionFront,       ESFloat,  SetDocumentDriveCorrectionFront   ); // rw   ESFloat
	PROPERTY_RW( kESDocumentDriveCorrectionBack,        CESCI2Scanner, ESFloat,      GetDocumentDriveCorrectionBack,        ESFloat,  SetDocumentDriveCorrectionBack    ); // rw   ESFloat
	PROPERTY_RW( kESLightIntensityBack,                 CESCI2Scanner, ESNumber,     GetLightIntensityBack,                 ESNumber, SetLightIntensityBack             ); // rw   ESNumber
	PROPERTY_R ( kESGuidePosition,                      CESCI2Scanner, ESNumber,     GetGuidePosition                                                                   ); // r    ESNumber (ESGuidePosition)
	PROPERTY_R ( kESGuidelessADF,                       CESCI2Scanner, bool,         IsGuidelessADF                                                                     ); // r    bool
	PROPERTY_R ( kESDocumentFeederBGLevelFront,         CESCI2Scanner, ESDictionary, GetDocumentFeederBGLevelFront                                                      ); // r    ESDictionary
	PROPERTY_R ( kESDocumentFeederBGLevelBack,          CESCI2Scanner, ESDictionary, GetDocumentFeederBGLevelBack                                                       ); // r    ESDictionary
	PROPERTY_R ( kESPickupRollerLifeLimit,              CESCI2Scanner, ESNumber,     GetPickupRollerLifeLimit                                                           ); // r    ESNumber
	PROPERTY_R ( kESSeparationPadLifeLimit,             CESCI2Scanner, ESNumber,     GetSeparationPadLifeLimit                                                          ); // r    ESNumber
	PROPERTY_R ( kESRollerKitLifeLimit,                 CESCI2Scanner, ESNumber,     GetRollerKitLifeLimit                                                              ); // r    ESNumber
	PROPERTY_R ( kESRetardRollerLifeLimit,              CESCI2Scanner, ESNumber,     GetRetardRollerLifeLimit                                                           ); // r    ESNumber
	PROPERTY_R ( kESPickupRollerNearend,                CESCI2Scanner, ESNumber,     GetPickupRollerNearend                                                             ); // r    ESNumber
	PROPERTY_R ( kESSeparationPadNearend,               CESCI2Scanner, ESNumber,     GetSeparationPadNearend                                                            ); // r    ESNumber
	PROPERTY_R ( kESRollerKitNearend,                   CESCI2Scanner, ESNumber,     GetRollerKitNearend                                                                ); // r    ESNumber
	PROPERTY_R ( kESRetardRollerNearend,                CESCI2Scanner, ESNumber,     GetRetardRollerNearend                                                             ); // r    ESNumber
	PROPERTY_R ( kESWarningStatus,                      CESCI2Scanner, ESNumber,     GetWarningStatus                                                                   ); // r    ESNumber (ENUM_ES_WARNING_STATUS)
	PROPERTY_RW( kESScanningMode,                 		CESCI2Scanner, ESNumber,     GetScanningMode,                		ESNumber, SetScanningMode                 	); // rw   ESNumber (ENUM_ES_SCANNING_MODE)
	PROPERTY_R ( kESUnavailableScanParameterStatus,     CESCI2Scanner, ESIndexArray, GetUnavailableScanParameterStatus                                                  ); // r    ESIndexArray (ENUM_ES_UNAVAILABLE_SCAN_PARAMETER_STATUS)
	PROPERTY_RW( kESBGColor,                            CESCI2Scanner, ESNumber,     GetBGColor,                            ESNumber, SetBGColor                        ); // rw   ESNumber (ENUM_ES_BG_COLOR)
	PROPERTY_RW( kESColorCounterType,         		    CESCI2Scanner, ESNumber,     GetColorCounterType,             		ESNumber, SetColorCounterType             	); // rw   ESNumber (ENUM_ES_COLORCOUNTER_TYPE)
	PROPERTY_RW( kESDirectPowerOn,               		CESCI2Scanner, ESNumber,     GetDirectPowerOn,                		ESNumber, SetDirectPowerOn                  ); // rw   ESNumber
	PROPERTY_RW( kESNonConnectPowerOff,           		CESCI2Scanner, ESNumber,     GetNonConnectPowerOff,          		ESNumber, SetNonConnectPowerOff          	); // rw   ESNumber
	PROPERTY_RW( kESADFPaperProtection,           		CESCI2Scanner, ESNumber,     GetADFPaperProtection,           		ESNumber, SetADFPaperProtection             ); // rw   ESNumber
	PROPERTY_RW( kESBehaviorWhenDoubleFeed,       		CESCI2Scanner, ESNumber,     GetBehaviorWhenDoubleFeed,      		ESNumber, SetBehaviorWhenDoubleFeed         ); // rw   ESNumber
	PROPERTY_RW( kESRollerKitNotifyCount,         		CESCI2Scanner, ESNumber,     GetRollerKitNotifyCount,         		ESNumber, SetRollerKitNotifyCount           ); // rw   ESNumber
	PROPERTY_RW( kESRetardRollerNotifyCount,      		CESCI2Scanner, ESNumber,     GetRetardRollerNotifyCount,    	    ESNumber, SetRetardRollerNotifyCount        ); // rw   ESNumber
	PROPERTY_RW( kESSeparationPadNotifyCount,     		CESCI2Scanner, ESNumber,     GetSeparationPadNotifyCount,    	    ESNumber, SetSeparationPadNotifyCount       ); // rw   ESNumber
	PROPERTY_RW( kESPickupRollerNotifyCount,      		CESCI2Scanner, ESNumber,     GetPickupRollerNotifyCount,      		ESNumber, SetPickupRollerNotifyCount        ); // rw   ESNumber
	PROPERTY_RW( kESCleaningWarningCounter,       		CESCI2Scanner, ESNumber,     GetCleaningWarningCounter,       		ESNumber, SetCleaningWarningCounter         ); // rw   ESNumber
	PROPERTY_RW( kESCleaningWarningNotify,        		CESCI2Scanner, ESNumber,     GetCleaningWarningNotify,        		ESNumber, SetCleaningWarningNotify          ); // rw   ESNumber
	PROPERTY_RW( kESCleaningWarningNotifyCount,   		CESCI2Scanner, ESNumber,     GetCleaningWarningNotifyCount,   		ESNumber, SetCleaningWarningNotifyCount     ); // rw   ESNumber

    PROPERTY_RW( kESClientApplication,                 CESCI2Scanner, ESNumber,     GetClientApplication,                 ESNumber, SetClientApplication                ); // rw   ESNumber
    PROPERTY_RW( kESBackgroundRemoval,            CESCI2Scanner, ESNumber,     GetBackGroundRemoval,            ESNumber, SetBackGroundRemoval            ); // rw   ESNumber
    PROPERTY_RW( kESBrightness,                   CESCI2Scanner, ESNumber,     GetBrightness,                   ESNumber, SetBrightness                   ); // rw   ESNumber
    PROPERTY_RW( kESContrast,                     CESCI2Scanner, ESNumber,     GetContrast,                     ESNumber, SetContrast                     ); // rw   ESNumber
    PROPERTY_RW( kESGammaScale,                   CESCI2Scanner, ESNumber,     GetGammaScale,                   ESNumber, SetGammaScale                   ); // rw   ESNumber
    PROPERTY_RW( kESSharpnessFilter,              CESCI2Scanner, ESNumber,     GetSharpnessLevel,               ESNumber, SetSharpnessLevel               ); // rw   ESNumber
    PROPERTY_R ( kESADFLoadSupported,             CESCI2Scanner, bool,    IsAdfLoadSupported); // r    bool

	PROPERTY_RW( kESDetectColorTypeEnabled,       CESCI2Scanner, bool,         IsDetectColorTypeEnabled,        bool,     SetDetectColorTypeEnabled       ); // rw   bool
	PROPERTY_RW( kESDetectBlankPageEnabled,       CESCI2Scanner, bool,         IsDetectBlankPageEnabled,        bool,     SetDetectBlankPageEnabled       ); // rw   bool
	PROPERTY_RW( kESDetectBlankPageLevel,         CESCI2Scanner, ESNumber,     GetDetectBlankPageLevel,         ESNumber, SetDetectBlankPageLevel         ); // rw   ESNumber
	PROPERTY_RW( kESSkipImageEnhancement,         CESCI2Scanner, bool,         IsSkipImageEnabled,              bool,     SetSkipImageEnabled             ); // rw   bool

	PROPERTY_R ( kESAdminLock,                    CESCI2Scanner, bool,         IsAdminLockSupported                                                       ); // r    bool
	PROPERTY_R ( kESAdminLockEnabled,             CESCI2Scanner, bool,         IsAdminLockEnabled                                                         ); // r    bool
	PROPERTY_RW( kESAdminLockPassword,            CESCI2Scanner, ESString,     GetAdminLockPassword,            ESString, SetAdminLockPassword            ); // rw   ESString
	PROPERTY_R ( kESDefaultPasswordType,      CESCI2Scanner, ESNumber,     GetDefaultPasswordType                                                     	  ); // r    bool

	
this->GetAllKeys();}
	
//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESCI2Scanner::~CESCI2Scanner()
{
}

//===============================================================
//!
//! 初期化処理
//!
//===============================================================
ESErrorCode CESCI2Scanner::Initialize()
{
	return CESCI2Accessor::Initialize();
}

//===============================================================
//!
//! DeviceInterfaceをセットします
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetDeviceInterface(IInterface* pDeviceInterface )
{
	return CESCI2Accessor::SetDeviceInterface( pDeviceInterface );
}

//===============================================================
//!
//! DeviceInterfaceを取得します
//!
//===============================================================
IInterface* CESCI2Scanner::GetDeviceInterface()
{
	return CESCI2Accessor::GetDeviceInterface();
}

//===============================================================
//!
//! スキャン時のプロセス処理を行うDelegateを設定する
//!
//===============================================================
void CESCI2Scanner::SetDelegate( IESScannerDelegate* pDelegate )
{
	CESCI2Accessor::SetDelegate( pDelegate, this );
}
void CESCI2Scanner::SetDelegate( IESScannerDelegate* pDelegate, IESScanner* pScanner )
{
	CESCI2Accessor::SetDelegate( pDelegate, pScanner );
}

//===============================================================
//!
//! スキャン時のプロセス処理を行うDelegateを取得する
//!
//===============================================================
IESScannerDelegate* CESCI2Scanner::GetDelegate()
{
	return CESCI2Accessor::GetDelegate();
}

//===============================================================
//!
//! 物理インターフェースのOpenおよびスキャナから情報を取得し、スキャナの使用を開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::Open()
{
	return CESCI2Accessor::Open();
}

//===============================================================
//!
//! 物理インターフェースをCloseし、スキャナの使用を終了する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::Close()
{
	return CESCI2Accessor::Close();
}

//===============================================================
//!
//! スキャナがオープンしているか？
//!
//===============================================================
bool CESCI2Scanner::IsOpened() const 
{
	return CESCI2Accessor::IsOpened();
}

//===============================================================
//!
//! 現在のスキャン設定でスキャンを開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::Scan()
{
	return CESCI2Accessor::Scan();
}

//===============================================================
//!
//! スキャン処理をキャンセルする。
//!
//===============================================================
ESErrorCode CESCI2Scanner::Cancel()
{
	return CESCI2Accessor::Cancel();
}

//===============================================================
//!
//! Abort
//!
//===============================================================
ESErrorCode CESCI2Scanner::Abort()
{
	return CESCI2Accessor::Abort();
}

//===============================================================
//!
//! スキャン中か？
//!
//===============================================================
bool CESCI2Scanner::IsScanning() const
{
	return CESCI2Accessor::IsScanning();
}

//===============================================================
//!
//! スキャン中フラグのセット
//!
//===============================================================
void CESCI2Scanner::SetScanning( bool bScanning )
{
	CESCI2Accessor::SetScanning( bScanning );
}

//===============================================================
//!
//! キャンセル中フラグのセット
//!
//===============================================================
void CESCI2Scanner::SetCancelled( bool bCanncelled )
{
	CESCI2Accessor::SetCancelled( bCanncelled );
}

//===============================================================
//!
//! スキャナセンサーのクリーニング動作を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::DoCleaning()
{
	return CESCI2Accessor::DoCleaning();
}

//===============================================================
//!
//! スキャナセンサーのキャリブレーションを要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::DoCalibration()
{
	return CESCI2Accessor::DoCalibration();
}

//===============================================================
//!
//! 設定をリセットする
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::Reset()
{
	return CESCI2Accessor::Reset();
}

//===============================================================
//!
//! 中断中かどうかを回答する
//!
//! @return 中断中かどうか。
//!
//===============================================================
bool CESCI2Scanner::IsInterrupted() const
{
	return CESCI2Accessor::IsInterrupted();
}

//===============================================================
//!
//! AFM中かどうかを回答する
//!
//! @return AFM中かどうか。
//!
//===============================================================
bool CESCI2Scanner::IsAfmEnabled() const
{
	return CESCI2Accessor::IsAfmEnabled();
}

//===============================================================
//!
//! Auto Feeding Mode タイムアウトチェック開始
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Scanner::ScheduleAutoFeedingModeTimeout()
{
	return CESCI2Accessor::ScheduleAutoFeedingModeTimeout();
}

//===============================================================
//!
//! スキャンジョブの開始
//!
//===============================================================
ESErrorCode CESCI2Scanner::StartJobInMode( ESJobMode eJobMode )
{
	return CESCI2Accessor::StartJobInMode(eJobMode);
}

//===============================================================
//!
//! スキャンジョブの終了
//!
//===============================================================
ESErrorCode CESCI2Scanner::StopJobInMode( ESJobMode eJobMode )
{
	return CESCI2Accessor::StopJobInMode(eJobMode);
}

//===============================================================
//!
//!
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Scanner::DoAutoFocus(ESFloat* pfOutFocus)
{
	return CESCI2Accessor::DoAutoFocus(pfOutFocus);
}


//===============================================================
//!
//! スキャナーパネルのプッシュスキャン可能状態を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetPanelToPushScanReady( BOOL bPushScanReady )
{
	return CESCI2Accessor::SetPanelToPushScanReady(bPushScanReady);
}

ESErrorCode CESCI2Scanner::UnlockAdministratorLock()
{
	return CESCI2Accessor::UnlockAdministratorLock();
}
ESErrorCode CESCI2Scanner::LockAdministratorLock()
{
	return CESCI2Accessor::LockAdministratorLock();
}


BOOL CESCI2Scanner::IsScannableDeviceConfig()
{
	return CESCI2Accessor::IsScannableDeviceConfig();
}