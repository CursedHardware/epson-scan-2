////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCIScanner.cpp
//!
//! @brif     ESCIコマンド用スキャナクラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ESCIScanner.h"

//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESCIScanner::CESCIScanner()
{
	PROPERTY_R ( kESVersion,                       CESCIScanner, ESString,     GetVersion                                                                      ); // r    ESString
	PROPERTY_R ( kESProductName,                   CESCIScanner, ESString,     GetProductName                                                                  ); // r    ESString
	PROPERTY_RW( kESDisableJobContinue,            CESCIScanner, bool,         IsDisableJobContinue,            bool,         SetDisableJobContinue            ); // rw   bool
	PROPERTY_R ( kESWarmingUp,                     CESCIScanner, bool,         IsWarmingUp                                                                     ); // r    bool
	PROPERTY_R ( kESButtonStatus,                  CESCIScanner, ESNumber,     GetButtonStatus                                                                 ); // r    ESNumber
	PROPERTY_RW( kESLampMode,                      CESCIScanner, ESNumber,     GetLampMode,                     ESNumber,     SetLampMode                      ); // rw   ESNumber (ESLampMode)
	PROPERTY_RW( kESPowerOffTime,                  CESCIScanner, ESNumber,     GetPowerOffTime,                 ESNumber,     SetPowerOffTime                  ); // rw   ESNumber(min)
	PROPERTY_RW( kESHalftone,                      CESCIScanner, ESNumber,     GetHalftones,                    ESNumber,     SetHalftones                     ); // rw   ESNumber (ESHalftone)
	PROPERTY_R ( kESMaxFocus,                      CESCIScanner, ESFloat,      GetMaxFocus                                                                     ); // r    ESFloat
	PROPERTY_R ( kESMinFocus,                      CESCIScanner, ESFloat,      GetMinFocus                                                                     ); // r    ESFloat
	PROPERTY_RW( kESFocus,                         CESCIScanner, ESFloat,      GetFocus,                        ESFloat,      SetFocus                         ); // rw   ESFloat
	PROPERTY_RW( kESDigitalICE,                    CESCIScanner, ESNumber,     GetDigitalICE,                   ESNumber,     SetDigitalICE                    ); // rw   ESNumber (ESDigitalICE)
	PROPERTY_RW( kESFilmType,                      CESCIScanner, ESNumber,     GetFilmType,                     ESNumber,     SetFilmType                      ); // rw   ESNumber (ESFilmType)
	PROPERTY_R ( kESDetectedDocumentSize,          CESCIScanner, ST_ES_SIZE_F, GetDetectedDocumentSize                                                         ); // r    ST_ES_SIZE_F
	PROPERTY_RW( kESSimplexScanCounter,            CESCIScanner, ESNumber,     GetSimplexScanCounter,           ESNumber,     SetSimplexScanCounter            ); // rw   ESNumber
	PROPERTY_RW( kESDuplexScanCounter,             CESCIScanner, ESNumber,     GetDuplexScanCounter,            ESNumber,     SetDuplexScanCounter             ); // rw   ESNumber
	PROPERTY_RW( kESLamp1Counter,                  CESCIScanner, ESNumber,     GetLamp1Counter,                 ESNumber,     SetLamp1Counter                  ); // rw   ESNumber
	PROPERTY_RW( kESLamp2Counter,                  CESCIScanner, ESNumber,     GetLamp2Counter,                 ESNumber,     SetLamp2Counter                  ); // rw   ESNumber
	PROPERTY_RW( kESPickupRollerCounter,           CESCIScanner, ESNumber,     GetPickupRollerCounter,          ESNumber,     SetPickupRollerCounter           ); // rw   ESNumber
	PROPERTY_RW( kESScanCounter,                   CESCIScanner, ESNumber,     GetScanCounter,                  ESNumber,     SetScanCounter                   ); // rw   ESNumber
	PROPERTY_RW( kESPaperEndDetection,             CESCIScanner, bool,         IsPaperEndDetectionEnabled,      bool,         SetPaperEndDetectionEnabled      ); // rw   bool
	PROPERTY_RW( kESCaptureCommandSupported,       CESCIScanner, bool,         IsCaptureCommandSupported,       bool,         SetCaptureCommandSupported       ); // rw   bool
	PROPERTY_RW( kESShouldIgnoreCancelFromScanner, CESCIScanner, bool,         IsShouldIgnoreCancelFromScanner, bool,         SetShouldIgnoreCancelFromScanner ); // rw   bool
	PROPERTY_R ( kESGuidePosition,                 CESCIScanner, ESNumber,     GetGuidePosition                                                                ); // r    ESNumber (ESGuidePosition)
	PROPERTY_RW( kESGuidePositionFB,               CESCIScanner, ESNumber,     GetGuidePositionFB,              ESNumber,     SetGuidePositionFB               ); // rw   ESNumber (ESGuidePosition)
	PROPERTY_RW( kESGuidePositionADF,              CESCIScanner, ESNumber,     GetGuidePositionADF,             ESNumber,     SetGuidePositionADF              ); // rw   ESNumber (ESGuidePosition)
	PROPERTY_RW( kESMaxScanSizeInLongLength,       CESCIScanner, ST_ES_SIZE_F, GetMaxScanSizeInLongLength,      ST_ES_SIZE_F, SetMaxScanSizeInLongLength       ); // rw   ST_ES_SIZE_F
	PROPERTY_RW( kESOverScan,                      CESCIScanner, bool,         IsOverScanEnabled,               bool,         SetOverScanEnabled               ); // rw   bool
	PROPERTY_RW( kESScanningMode,                  CESCIScanner, ESNumber,     GetScanningMode,                 ESNumber,     SetScanningMode                  ); // rw   ESNumber (ESScanningMode)
	PROPERTY_RW( kESLightIntensityFB,              CESCIScanner, ESNumber,     GetLightIntensityFB,             ESNumber,     SetLightIntensityFB              ); // rw   ESNumber

	this->GetAllKeys();
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESCIScanner::~CESCIScanner()
{
}

//===============================================================
//!
//! 初期化処理
//!
//===============================================================
ESErrorCode CESCIScanner::Initialize()
{
	return CESCIAccessor::Initialize();
}

//===============================================================
//!
//! DeviceInterfaceをセットします
//!
//===============================================================
ESErrorCode CESCIScanner::SetDeviceInterface(IInterface* pDeviceInterface )
{
	return CESCIAccessor::SetDeviceInterface( pDeviceInterface );
}

//===============================================================
//!
//! DeviceInterfaceを取得します
//!
//===============================================================
IInterface* CESCIScanner::GetDeviceInterface()
{
	return CESCIAccessor::GetDeviceInterface();
}

//===============================================================
//!
//! スキャン時のプロセス処理を行うDelegateを設定する
//!
//===============================================================
void CESCIScanner::SetDelegate( IESScannerDelegate* pDelegate )
{
	CESCIAccessor::SetDelegate( pDelegate, this );
}
void CESCIScanner::SetDelegate( IESScannerDelegate* pDelegate, IESScanner* pScanner )
{
	CESCIAccessor::SetDelegate( pDelegate, pScanner );
}

//===============================================================
//!
//! スキャン時のプロセス処理を行うDelegateを取得する
//!
//===============================================================
IESScannerDelegate* CESCIScanner::GetDelegate()
{
	return CESCIAccessor::GetDelegate();
}

//===============================================================
//!
//! 物理デバイスのOpenおよびスキャナから情報を取得し、スキャナの使用とボタン監視を開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::Open()
{
	return CESCIAccessor::Open();
}

//===============================================================
//!
//! 物理デバイスをCloseし、スキャナの使用とボタン監視を終了する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::Close()
{
	return CESCIAccessor::Close();
}

//===============================================================
//!
//! スキャナがオープンしているか？
//!
//===============================================================
bool CESCIScanner::IsOpened() const
{
	return CESCIAccessor::IsOpened();
}

//===============================================================
//!
//! 現在のスキャン設定でスキャンを開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::Scan()
{
	return CESCIAccessor::Scan();
}

//===============================================================
//!
//! スキャン処理をキャンセルする。
//!
//===============================================================
ESErrorCode CESCIScanner::Cancel()
{
	return CESCIAccessor::Cancel();
}

//===============================================================
//!
//! 
//!
//===============================================================
ESErrorCode CESCIScanner::Abort()
{
	return kESErrorNoError;
}


//===============================================================
//!
//! スキャン中か？
//!
//===============================================================
bool CESCIScanner::IsScanning() const
{
	return CESCIAccessor::IsScanning();
}

//===============================================================
//!
//! スキャン中フラグのセット
//!
//===============================================================
void CESCIScanner::SetScanning( bool bScanning )
{
	CESCIAccessor::SetScanning( bScanning );
}

//===============================================================
//!
//! キャンセル中フラグのセット
//!
//===============================================================
void CESCIScanner::SetCancelled( bool bCanncelled )
{
	CESCIAccessor::SetCancelled( bCanncelled );
}

//===============================================================
//!
//! スキャナセンサーのクリーニング動作を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::DoCleaning()
{
	return CESCIAccessor::DoCleaning();
}

//===============================================================
//!
//! スキャナセンサーのキャリブレーションを要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::DoCalibration()
{
	return CESCIAccessor::DoCalibration();
}

//===============================================================
//!
//! 設定をリセットする
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::Reset()
{
	return CESCIAccessor::Reset();
}


//===============================================================
//!
//! 中断中かどうかを回答する
//!
//! @return 中断中かどうか。
//!
//===============================================================
bool CESCIScanner::IsInterrupted() const
{
	return CESCIAccessor::IsInterrupted();
}

//===============================================================
//!
//! AFM中かどうかを回答する
//!
//! @return AFM中かどうか。
//!
//===============================================================
bool CESCIScanner::IsAfmEnabled() const
{
	return false;
}

//===============================================================
//!
//! Auto Feeding Mode タイムアウトチェック開始
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::ScheduleAutoFeedingModeTimeout()
{
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャンジョブの開始
//!
//===============================================================
ESErrorCode CESCIScanner::StartJobInMode( ESJobMode eJobMode )
{
	return CESCIAccessor::StartJobInMode( eJobMode );
}

//===============================================================
//!
//! スキャンジョブの終了
//!
//===============================================================
ESErrorCode CESCIScanner::StopJobInMode( ESJobMode eJobMode )
{
	return CESCIAccessor::StopJobInMode( eJobMode );
}

//===============================================================
//!
//! オートフォーカスを要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIScanner::DoAutoFocus( ESFloat* pfOutFocus )
{
	return CESCIAccessor::DoAutoFocus( pfOutFocus );
}

//===============================================================
//!
//! スキャナーパネルのプッシュスキャン可能状態を設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetPanelToPushScanReady( BOOL bPushScanReady )
{
	return kESErrorNoError;
}

ESErrorCode CESCIScanner::UnlockAdministratorLock()
{
	return kESErrorNoError;
}
ESErrorCode CESCIScanner::LockAdministratorLock()
{
	return kESErrorNoError;
}
