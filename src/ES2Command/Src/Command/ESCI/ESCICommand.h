////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCICommand.h
//!
//! @brif     ESCIコマンド送受信 クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommonUtility/utils/ESBuffer.h"

#include "../Base/CommandBase.h"
#include "../../Image/ESScannedImage.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CESCICommand
//!
//! @bref      ESCIコマンド送受信 クラス
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CESCICommand : public CCommandBase
{
public:
	CESCICommand();
	~CESCICommand();

	// 初期化処理
	virtual ESErrorCode Initialize();

	//
	// クラスステータス
	//--------------------------------------------
protected:
	// スキャナからのキャンセルを無視するか？
	virtual bool IsShouldIgnoreCancelFromScanner()	= 0;

	// キャプチャー機能をサポートするか？
	virtual bool IsCaptureCommandSupported()		= 0;

	// キャプチャー中か？
	virtual bool IsCaptured()						= 0;

	// キャプチャー状態をセットする
	virtual void SetCaptured( bool bCaptured )		= 0;

	// バッファサイズを取得する
	virtual ESNumber GetBufferSize()				= 0;

	// 静止中か？
	virtual bool IsDumb()							= 0;

	// ウォーミングアップ中か？
	virtual bool IsWarmingUp()						= 0;

	// 認証機能をサポートしているか？
	virtual bool IsAuthenticationSupported()		= 0;

	// 認証機能が有効状態かどうかを取得する
	virtual bool IsAuthenticationEnabled()			= 0;

	// 認証ユーザー名を取得する
	virtual ESString GetAuthUserName()				= 0;

	// 認証パスワードを取得する
	virtual ESString GetAuthPassword()				= 0;

	// プロダクト名を取得する
	virtual ESString GetProductName()				= 0;

	// エラーステータスを取得する。
	virtual ESErrorCode GetErrorStatus()		= 0;

	//
	virtual bool IsPaperEndDetectionEnabled()		= 0;

	virtual bool IsFeederEnabled()					= 0;

	// ColorMatrixを適用
	virtual ESErrorCode ApplyColorMatrix( ES_CMN_FUNCS::BUFFER::IESBuffer& cScannedData, UInt32 nDataLength )	= 0;

	// ISプロトコルにおける、拡張データ転送をサポートしているかどうかを判断する。
	bool IsUsesExtTransfer();
	// 画像転送時のドライバ側からのACK応答をすべきかどうか判断する。
	bool IsAvoidsAckWhileImageTransfer();

	// スキャナがウォームアップ状態を解除するまで待つ。
	ESErrorCode WaitWhileWarmingUp( int nTimeout_s );

	virtual bool IsShouldIgnoreCancelResponse();

	//
	// コマンド送信
	//--------------------------------------------
private:
	// スキャナにイメージ転送時のACK応答を送信する。
	ESErrorCode SendAckForImageData();


	// ESCI Command
	enum {
		CMD_NONE                    = 0x00,
		GET_IDENTITY                = 'I',
		GET_STATUS                  = 'F',
		REQ_START_SCANNING          = 'G',
		GET_HW_PROPERTY             = 'i',
		GET_EXT_STATUS              = 'f',
		GET_PUSHBUTTON_STATUS       = '!',
		SET_SCANNINGPARAMETER       = 'W',
		GET_SCANNINGPARAMETER       = 'S',
		SET_RESOLUTION              = 'R',
		SET_SCANAREA                = 'A',
		SET_COLOR                   = 'C',
		SET_DATAFORMAT              = 'D',
		SET_GAMMACORRECTION         = 'Z',
		SET_GAMMATABLE              = 'z',
		SET_COLORCORRECTIONMAT      = 'm',
		SET_THRESHOLD               = 't',
		SET_OPTIONUNIT              = 'e',
		SET_SCANNINGMODE            = 'g',
		SET_LINECOUNTER             = 'd',
		SET_FILMTYPE                = 'N',
		SET_FOCUS                   = 'p',
		GET_FOCUS                   = 'q',
		REQ_CANCEL_WARMINGUP        = 'w',
		REQ_INIT_SCANNER            = '@',
		REQ_CAPTURE_SCANNER         = '(',
		REQ_RELEASE_SCANNER         = ')',
		REQ_CAPTURE_SCANNER_AUTH    = '[',
		REQ_RELEASE_SCANNER_AUTH    = ']',
		REQ_MAINTENANCE             = '1',
		REQ_USE_DICE                = '#',
		SET_POWERSAVESHIFTTIME      = 'P',
		SET_LOGINFO                 = 0xE1,
		GET_LOGINFO                 = 0xE2,
	};
	typedef UInt8 ENUM_ESCI_COMMAND;

	// Scanner ------------------------------------------
	//               ↑
	//              ①
	//            ↑
	// Host PC ------------------------------------------
	ESErrorCode SendCommand1 ( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __in UInt32 un32ComittedReadLength );

	// Scanner ------------------------------------------
	//               ↑  ↓
	//              ①     ②
	//            ↑         ↓
	// Host PC ------------------------------------------
	ESErrorCode SendCommand2 ( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __out PESByte pOutBuffer, __in UInt32 uBufferLength );

	// Scanner ------------------------------------------
	//               ↑  ↓
	//              ①     ②
	//            ↑         ↓ ②で Ack 取得
	// Host PC ------------------------------------------
	ESErrorCode SendCommand2A ( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __out ESByte& outAck );

	// Scanner ------------------------------------------
	//               ↑  ↓      ↓
	//              ①     ②      ③
	//            ↑         ↓      ↓ 
	// Host PC ------------------------------------------
	ESErrorCode SendCommand3 ( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __out_opt UInt8* pUn8OutStatus, __out ES_CMN_FUNCS::BUFFER::IESBuffer& cOutData );

	// Scanner ------------------------------------------
	//               ↑  ↓          ↑  ↓
	//              ①     ②       ③     ④
	//            ↑         ↓   ↑         ↓ 
	// Host PC ------------------------------------------
	ESErrorCode SendCommand4 ( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __in ESByteData& bdParam, __out PESByte pOutBuffer, __in UInt32 un32BufferLen );

	// Scanner ------------------------------------------
	//               ↑  ↓          ↑  ↓
	//              ①     ②       ③     ④
	//            ↑         ↓   ↑         ↓ ④で Ack 取得
	// Host PC ------------------------------------------
	ESErrorCode SendCommand4A( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __in ESByteData& bdParam, __out ESByte& outAck );

	//
	// コマンドリクエスト
	//-------------------------------------------
protected:

#pragma pack(push, 1)
	typedef struct {
		UInt8	un8CommandLevel[2];
		UInt16	un16MaxWidth;
		UInt16	un16MaxHeight;
	} ST_ESCI_IDENTITY;   // ESC I
#pragma pack(pop)

	//
	// スキャナにIdentity（スキャナのCapability）情報を要求する。
	//
	ESErrorCode RequestIdentity( __out ST_ESCI_IDENTITY& stOutIdentity, __out ESIndexSet& arOutResolutions );
	


#pragma pack(push, 1)
	typedef struct {
		UInt8	un8MainStatus;
		UInt8	un8AdfStatus;
		UInt8	un8TpuStatus;
		UInt8	un8MainStatus2;
		UInt8	un8HolderStatus;
		UInt16	un16AdfPaperSize;
		UInt16	un16MainPaperSize;
		UInt8	un8TpuStatus2;
		UInt8	un8AdfStatus2;
		UInt8	un8Reserved[5];
	} ST_ESCI_SCANNER_STATUS;    // FS F
#pragma pack(pop)

	//
	// スキャナにステータス情報を要求する。
	//
	ESErrorCode RequestStatus( __out ST_ESCI_SCANNER_STATUS& stOutStatus );

#pragma pack(push, 1)
	typedef struct {
		UInt8	un8Status;
		UInt8	un8AdfStatus;
		UInt16	un16AdfX;
		UInt16	un16AdfY;
		UInt8	un8TpuStatus;
		UInt16	un16TpuX;
		UInt16	un16TpuY;
		UInt8	un8Status2;
		UInt16	un16X;
		UInt16	un16Y;
		UInt8	un8AdfPaperSize1;
		UInt8	un8AdfPaperSize2;
		UInt8	un8MainPaperSize1;
		UInt8	un8MainPaperSize2;
		UInt8	un8Reserved[6];
		UInt8	un8ProductName[16];
	} ST_ESCI_EXTENDED_STATUS;   // ESC f
#pragma pack(pop)

	//
	// スキャナに拡張ステータス情報を要求する。
	//
	ESErrorCode RequestExtendedStatus( __out ST_ESCI_EXTENDED_STATUS& stOutExtStatus );

#pragma pack(push, 1)
	typedef struct {
		UInt16	un16StdRes;
		UInt8	un8Sensor;
		UInt8	un8ColorSequence;
		UInt8	un8LineSpacing1;
		UInt8	un8LineSpacing2;
		UInt8	un8Reserved[8];
	} ST_ESCI_HW_PROPERTY;   // ESC i
#pragma pack(pop)

	//
	// スキャナにハードウェア情報を要求する。
	//
	ESErrorCode RequestHWProperty( __out ST_ESCI_HW_PROPERTY& stOutHWProperty, __out ESIndexSet& arOutXResolutions, __out ESIndexSet& arOutYResolutions );


#pragma pack(push, 1)
	typedef struct {
		UInt8	un8CommandLevel[2];
		UInt8	un8Reserved1[2];
		UInt32	un32BasicResolution;
		UInt32	un32MinResolution;
		UInt32	un32MaxResolution;
		UInt32	un32MaxWidthPixels;
		UInt32	un32PixelsInBasicXRes;
		UInt32	un32PixelsInBasicYRes;
		UInt32	un32AdfPixelsInBasicXRes;
		UInt32	un32AdfPixelsInBasicYRes;
		UInt32	un32TpuPixelsInBasicXRes;
		UInt32	un32TpuPixelsInBasicYRes;
		UInt8	un8MainStatus;
		UInt8	un8MainStatus2;
		char	chProductName[16];
		char	chRomVersion[4];
		UInt8	un8InBitDepth;
		UInt8	un8OutBitDepth;
		UInt32  un32Tpu2PixelsInBasicXRes;
		UInt32  un32Tpu2PixelsInBasicYRes;
		UInt8	un8MainStatus3;
		UInt8	un8Reserved2[3];
	} ST_ESCI_EXTENDED_IDENTITY;   // FS I
#pragma pack(pop)

	enum {
		kCommandLevelStandard   = 'B',
		kCommandLevelDumb       = 'D',
	};
	typedef UInt8 ENUM_ESCI_COMMAND_LEVEL;

	typedef enum {
		kColorCorrectionNone        = 0x00,
		kColorCorrecitonUserDefined = 0x01,
	} ENUM_ESCI_COLOR_CORRECTION;

	//
	// スキャナに拡張Identity情報を要求する。
	//
	ESErrorCode RequestExtendedIdentity( __out ST_ESCI_EXTENDED_IDENTITY& stOutExtIdentity );


#pragma pack(push, 1)
	typedef struct {
		UInt32  un32XResolution;
		UInt32  un32YResolution;
		UInt32  un32XOffset;
		UInt32  un32YOffset;
		UInt32  un32Width;
		UInt32  un32Height;
		UInt8   un8Color;
		UInt8   un8DataFormat;
		UInt8   un8OptionControl;
		UInt8   un8ScanningMode;
		UInt8   un8BlockLineNumber;
		UInt8   un8GammaCorrection;
		UInt8   un8Brightness;
		UInt8	un8ColorCorrection;
		UInt8	un8HalftoneMode;
		UInt8   un8Threshold;
		UInt8	un8AutoAreaSeg;
		UInt8	un8Sharpness;
		UInt8	un8Mirroring;
		UInt8   un8FilmType;
		UInt8   un8LampMode;
		UInt8   un8DoubleFeedDetection;
		UInt8	un8AutoScan;
		UInt8   un8QuietMode;
		UInt16	un16LightIntensity;
		UInt8   un8Reserved2[20];
	} ST_ESCI_SCANNING_PARAMETER;    // FS W
#pragma pack(pop)

	//
	// スキャナに設定されているスキャンパラメータ情報を要求する。
	//
	ESErrorCode RequestScanningParameter( __out ST_ESCI_SCANNING_PARAMETER& stOutScanningParameter );

	//
	// スキャナにDigitalICE機能の使用許可を要求する。
	//
	ESErrorCode RequestUseDICE();

	//
	// スキャナにスキャン動作のキャンセルを要求する。
	//
	ESErrorCode RequestCancelScanning();

	//
	// スキャナに画像転送の終了を通知する。
	// 
	ESErrorCode RequestEndTransmission();

	//
	// スキャナに排紙を要求する。
	//
	ESErrorCode RequestEjectPaper();

	//
	// スキャナに給紙を要求する。
	//
	ESErrorCode RequestLoadPaper();

	//
	// スキャナに占有開始を要求する。
	//
	ESErrorCode RequestCaptureScanner();

	//
	// スキャナに占有終了を要求する。
	//
	ESErrorCode RequestReleaseScanner();

	//
	// スキャナに占有開始を要求する。その際、ユーザー名とパスワードを合わせて送信する。
	//
	ESErrorCode RequestCaptureScannerWithAuth( __in const ESString& strAuthUserName, __in const ESString& strPassword );

	//
	// スキャナに占有終了を要求する。requestCaptureScannerWithAuth:passwordData:で占有開始を要求した場合のみ使用可能。
	//
	ESErrorCode RequestReleaseScannerWithAuth();

	//
	// スキャナに初期化を要求する。
	//
	ESErrorCode RequestInitializeScanner();

	enum {
		kOptionControlDisable                  = 0x00,
		kOptionControlEnable                   = 0x01,
		kOptionControlEnableADFDuplex          = 0x02,
		kOptionControlEnableDICEInfraRed       = 0x03,
		kOptionControlEnableDICERGB            = 0x04,
		kOptionControlEnableTPUAreaGuide       = 0x05,
		kOptionControlEnableDICELiteInfraRed   = 0x06,
	};
	typedef UInt8 ENUM_ESCI_OPTION_CONTROL;

	//
	// スキャナに取込み装置の変更を要求する。
	//
	ESErrorCode RequestSetFunctionalUnitType( __in ENUM_ESCI_OPTION_CONTROL eType );

	//
	// スキャナに色補正係数マトリックスの設定を要求する。
	//
	ESErrorCode RequestSetColorMatrix( __in const ES_COLOR_MATRIX& matrix );

	//
	// スキャナにガンマテーブルの設定を要求する。
	//
	ESErrorCode RequestSetGammaTable( __in ESGammaChannel eChannel, __in CESGammaTable& cGammaTable );

	//
	// スキャナにフォーカスの設定を要求する。
	//
	ESErrorCode RequestSetFocus( __in ESFocus eFocus );

	//
	// スキャナに現在のフォーカス設定を要求する。
	//
	ESErrorCode RequestFocus( __out ESFocus& eOutFocus );

	//
	// スキャナに現在のボタン押下状況を要求する。
	// スキャナがUSB Interruptによるボタンイベント通知に対応していない場合に使用する。
	//
	ESErrorCode RequestButtonStatus( __out UInt8& un8OutButtonStatus );

	enum {
		kPowerSaveTime15m   = 0x00,
		kPowerSaveTime30m   = 0x01,
		kPowerSaveTime45m   = 0x02,
		kPowerSaveTime60m   = 0x03,
		kPowerSaveTime120m  = 0x04,
		kPowerSaveTime180m  = 0x05,
		kPowerSaveTime240m  = 0x06,
	};
	typedef UInt8 ENUM_ESCI_POWER_SAVE_TIME;

	//
	// スキャナに自動電源オフへの移行時間の設定を要求する。
	//
	ESErrorCode RequestSetPowerOffTime( __in UInt32 un32PowerOffTime );

	//
	//  スキャナに自動電源オフへの移行時間の取得を要求する。
	// 
	ESErrorCode RequestGetPowerOffTime( __out UInt32& un32OutPowerOffTime );

#pragma pack(push, 1)
	typedef struct {
		UInt16	un16Mode;
		UInt16	un16Param1;
		UInt16	un16Param2;
		UInt16	un16Param3;
	} ST_ESCI_MAINTENANCE_PARAMETER;
#pragma pack(pop)
	
	enum {
		kMaintenanceCleaning        = 0x0001,
		kMaintenanceCalibration     = 0x0002,
		kMaintenanceStatusRequest   = 0xffff,
	};
	typedef UInt16 ENUM_ESCI_MAINTENANCE_MODE;

	//
	// スキャナにメンテナンスコマンドの設定を要求する。
	//
	ESErrorCode RequestMaintenanceWithParameter( __in ST_ESCI_MAINTENANCE_PARAMETER stParameter );

	//
	// スキャナにメンテナンスコマンドの実行ステータスを要求する。
	// 
	ESErrorCode RequestMaintenanceStatus();

	//
	// スキャナにスキャンパラメータの設定を要求する。
	//
	ESErrorCode RequestSetScanningParameters( __in ST_ESCI_SCANNING_PARAMETER eParameters );

	//
	// RequestScanningParameterにセットするパラメーターを作成する
	//
	virtual ESErrorCode CreateScanningParametersParam( __out ST_ESCI_SCANNING_PARAMETER& stOutScanningParameter ) = 0;

#pragma pack(push, 1)
	enum {
		kLogFunctionLamp1Counter        = 0x0001,
		kLogFunctionLamp2Counter        = 0x0002,
		kLogFunctionScanCounter         = 0x0005,
		kLogFunctionSimplexScanCounter  = 0x0007,
		kLogFunctionDuplexScanCounter   = 0x0008,
		kLogFunctionPickupRollerCounter = 0x000C,
		kLogFunctionPowerOffTime        = 0x0010,
	};
	typedef UInt16 ENUM_ESCI_LOG_FUNCTION;
#pragma pack(pop)

	//
	// スキャナに保存されているログ情報を要求する。スキャンカウンターの取得などで使用する。
	//
	ESErrorCode RequestReadLogForFunction( __in ENUM_ESCI_LOG_FUNCTION eFunction, __out UInt32& un32OutValue );

	//
	// スキャナに保存されているログ情報の書き換えを要求する。スキャンカウンターのリセットなどで使用する。
	//
	ESErrorCode RequestWriteLogForFunction( __in ENUM_ESCI_LOG_FUNCTION eFunction, __in UInt32 un32Value );

#pragma pack(push, 1)
	typedef struct {
		UInt8	un8Header;
		UInt8	un8Status;
		UInt32	un32ByteCounter;
		UInt32	un32BlockNumber;
		UInt32	un32LastBlockByteCounter;
	} ST_ESCI_IMAGE_INFO_BLOCK;
#pragma pack(pop)

	//
	// スキャナにスキャンの開始を要求する。
	//
	ESErrorCode RequestStartScanning( __out ST_ESCI_IMAGE_INFO_BLOCK& stImageInfoBlock );

	//
	// スキャナに1ページのスキャンを要求し、CESScannedImageに画像データを書き込む。
	//
	ESErrorCode RequestScanToImage( __out CESScannedImage*& pImage );

	//
	// スキャナから画像データを受信する。
	//
	ESErrorCode ReadImageData( ES_CMN_FUNCS::BUFFER::IESBuffer& cBufOutData, UInt32 un32BytesToRead, UInt8& un8OutErrorCode );
};

