////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2Command.h
//!
//! @brif     ESCI2コマンド送受信 クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommonUtility/utils/ESBuffer.h"
#include "Command/Base/CommandBase.h"
#include "ESCI2Defs.h"

#include "ESCI2DataConstructor.h"
#include "ESCI2DataEnumerator.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CESCI2Command
//!
//! @bref      ESCI2コマンド送受信 クラス
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CESCI2Command : public CCommandBase
{
public:
	CESCI2Command();
	virtual ~CESCI2Command();

	// 初期化処理
	virtual ESErrorCode Initialize();

public:
	// 数値IDを、ESCI2の4文字コードに変換する
	static ESStringA FCCSTR( const UInt32 un32CmdId );
	// ESCI2の4文字コードを、数値IDに変換する
	static UInt32 FourCharCode( const ESStringA strCode );

	//
	// クラスステータス
	//--------------------------------------------
protected:
	// 認証機能をサポートしているか？
	virtual bool IsAuthenticationSupported()		= 0;

	// 認証ユーザー名を取得する
	virtual ESString GetAuthUserName()				= 0;

	// 認証機能が有効状態かどうかを取得する
	virtual bool IsAuthenticationEnabled()			= 0;

	// 認証パスワードデーターを取得する
	virtual ESString GetAuthPassword()				= 0;

	// プロダクト名を取得する
	virtual ESString GetProductName()				= 0;

	// 給紙タイプを取得する
	virtual ESFunctionalUnitType GetFunctionalUnitType() = 0;

	// デバイスの最大バッファサイズ取得
	virtual ESNumber GetDeviceMaxBufferSize()		= 0;

	// スキャンするエリアをピクセル単位で取得する
	virtual ST_ES_RECT_UN32 GetScanAreaInPixel()	= 0;

	// 両面同時読み取り機能が有効か?
	virtual bool IsDuplexEnabled() = 0;

	virtual bool IsAdminLockSupported()				= 0;

	virtual bool IsAdminLockEnabled()				= 0;
	virtual bool IsAdminLockEnabledWithUnlockTest(bool bUnlockTest) = 0;


	virtual ESString GetAdminLockPassword()			= 0;

	virtual bool	RequestingUnlockAdminLock()		= 0;

	virtual ESErrorCode SetAdminLockPassword(ESString strPassword) = 0;

	// データー受信に失敗した時にリトライを実行するか？
	bool IsRetry();

	bool IsShouldSyncFunctionalUnit();

	bool IsShouldKeepControlMode();
	
	bool IsShouldZeroClearEdgeFillForFB();

	bool IsShouldIgnoreDateAndTime();

	bool IsShouldSendStatAfterADFScan();

	ESErrorCode SyncFunctionalUnit();

    virtual bool IsFeedTypeADF() = 0;
    virtual bool IsAdfLoadSupported() = 0 ;
    virtual bool IsAutoCroppingEnabled() = 0;
	virtual bool IsAutoCroppingInscribedEnabled() = 0;
    virtual bool IsSkewCorrectionEnabled() =0;
    virtual ESNumber GetBackGroundRemoval() = 0;
    
	//
	// コマンド送信
	//--------------------------------------------
private:
	// スキャナに要求を送信する。
	ESErrorCode SendRequest ( __in ESCI2RequestCode eRequestCode, __in_opt ESByteData* pParamData );

	// スキャナからの応答を受信する。
	ESErrorCode ReceiveReply( __inout ESCI2RequestCode& eRequestCode, __out_opt ESDictionaryA* pDicOutHeaderInfo, __out_opt ES_CMN_FUNCS::BUFFER::CESHeapBuffer* pOutDataBuffer );

	// スキャナに対し、要求送信・応答受信の一連のシーケンスを実行する。スキャナがウォームアップ中やBusy状態であった場合は、解除されるまでリトライする。
	ESErrorCode RunSequence ( __in ESCI2RequestCode eRequestCode, __in_opt ESByteData* pParamBlock, __out_opt ESDictionaryA* pDicOutHeaderInfo, __out_opt ES_CMN_FUNCS::BUFFER::CESHeapBuffer* pOutDataBuffer );

	//
	// コマンドリクエスト
	//-------------------------------------------
protected:
	// スキャナにControl要求（スキャナの状態変更を伴うコマンド送信開始通知）をする。
	ESErrorCode RequestScannerControl();
	// スキャナにInquiry要求（スキャナの状態変更を伴わないコマンド送信開始通知）をする。
	ESErrorCode RequestScannerInquiry();
	// スキャナにMaintenance要求（スキャナのメンテナンスコマンド送信開始通知）をする。
	ESErrorCode RequestScannerMaintenance();
	// スキャナに機械的制御を要求する。（例：クリーニング要求など）
	ESErrorCode RequestFinalize();
	// スキャナに機械的制御を要求する。（例：クリーニング要求など）
	ESErrorCode RequestMechanicalControlWithParameter( ESByteData& parameter );

	enum {
		kModeNone         = 0,
		kModeControl      = (1 << 0),
		kModeInquiry      = (1 << 1),
		kModeMaintenance  = (1 << 2),
	};
	typedef UInt8 ESCI2Mode;

	// 現在のモードを取得する
	ESCI2Mode GetMode();
	// モードを設定する
	ESErrorCode SetMode( ESCI2Mode eMode );

	// スキャナに送信するスキャンパラメータを、内部表現である辞書形式からESC/I-2データのバイト列に変換する。
	ESByteData ParameterDataWithDict( ESDictionaryA& dicParameters, UInt32 un32MaxSize, ESStringArrayA& arOutProcessedKeys );

	// スキャナに送信するメンテナンスパラメータを、内部表現である辞書形式からESC/I-2データのバイト列に変換する。
	ESByteData MaintenanceParameterDataWithDict( ESDictionaryA& dicParameters, UInt32 un32MaxSize, ESStringArrayA& arOutProcessedKeys );

	// 指定モードでRunSequenceを実行し、データーをPaseする。
	ESErrorCode RequestRunSequence( ESCI2RequestCode eRequestCode, ESCI2Mode eMode, ESByteData* pParamBlock, const ESDictionaryA& dicRules, ESDictionaryA& dicOutResult );

	// 動作完了まで待つ
	ESErrorCode WaitUntilDone();

protected:

	// スキャナにスキャン開始要求をする。
	ESErrorCode StartScanning();

	// 
	ESErrorCode RequestImage( ESDictionaryA& dicHeaderInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cOutBuffer );
	// 
	ESErrorCode RequestCancel();
    ESErrorCode RequestAdministratorLock(bool bModeOn, const ESString &password);
	ESErrorCode RequestAfmMode( bool bModeOn );
	ESErrorCode RequestPanelToPushScanReady( bool bPushScanReady );
	ESErrorCode RequestJobMode( UInt32 un32JobMode );

	// スキャナにスキャナの情報（'INFO'）を要求し、dicOutInfo に格納する。
	ESErrorCode GetInfo( __out ESDictionaryA& dicOutInfo );

	// スキャナにスキャナのメンテナンス情報（FS Z + 'INFO'）を要求し、dicOutMaintenanceInfoに格納する。
	ESErrorCode GetMaintenanceInfo( __out ESDictionaryA& dicOutMaintenanceInfo );

	ESErrorCode GetMaintenanceStatus( __out ESDictionaryA& dicOutStatus );

	ESErrorCode GetExtInfo( __out ESDictionaryA& dicOutExtInfo );

	// スキャナにスキャナのCapability（'CAPA'）を要求し、dicOutCapabilityに格納する。
	ESErrorCode GetCapabilities( __out ESDictionaryA& dicOutCapability );

	// スキャナにスキャナのメンテナンスCapability（FS Z + 'CAPA'）を要求し、dicOutMaintenanceCapabilityに格納する。
	ESErrorCode GetMaintenanceCapabilities( __out ESDictionaryA& dicOutMaintenanceCapability );

	// スキャナステータス（'STAT'）を要求し、dicOutStatusに格納する。
	ESErrorCode GetStatus( __out ESDictionaryA& dicOutStatus );

	// スキャナに現在のスキャン設定（'RESA'）を要求する。
	ESErrorCode GetResults( __out ESDictionaryA& dicOutResults, __in_opt ESStringArrayA* pStrarKeys );

	// スキャナに現在のメンテナンス設定（FS Z + 'RESA'）を要求する。
	ESErrorCode GetMaintenanceResults( __out ESDictionaryA& dicOutResults, __in_opt ESStringArrayA* pStrarKeys );

	// スキャナにスキャン設定を送信する。スキャナ側のバッファ上限を超えないように分割して送信される。
	ESErrorCode SendParameters( __in ESDictionaryA dicParameters );

	// スキャナにメンテナンス設定を送信する。スキャナ側のバッファ上限を超えないように分割して送信される。
	ESErrorCode SendMaintenanceParameters( __in ESDictionaryA dicParameters );

    const ESDictionaryA& HeaderPaseRule();
    const ESDictionaryA& CapabilitiesPaseRule();
    const ESDictionaryA& GetMaintenanceCapabilitiesPaseRule();
    const ESDictionaryA& GetInfoPaseRule();
    const ESDictionaryA& GetMaintenanceInfoPaseRule();
    const ESDictionaryA& GetStatusPaseRule();
    const ESDictionaryA& GetResultsPaseRule();
    const ESDictionaryA& GetMaintenanceResultsPaseRule();

protected:
	//
	// CESCI2DataEnumeratorクラスのデーターソースDelegate
	//--------------------------------------------
	class CESCI2DataEnumeratorDataSource : public IESCI2DataEnumeratorDataSource
	{
	public:
		CESCI2DataEnumeratorDataSource();
		~CESCI2DataEnumeratorDataSource();

		void Init(CESCI2Command* pParent );
		virtual UInt32 ReadFromEnumerator( CESCI2DataEnumerator* pSender, PESByte pBuffer, UInt32 un32Length );

	protected:
		CESCI2Command* m_pParent;
	};

protected:
	bool								m_bIsRetry;
	ESCI2Mode							m_eESCI2Mode;
	CESCI2DataEnumeratorDataSource		m_cEnumeratorDataSource;

	ESDictionaryA						m_dicMaintenanceResults;

	bool								m_bIsAllowAuthError;
	
	bool								m_bIsTransferring;

    ESDictionaryA                       m_dicHeaderPaseRule;
    ESDictionaryA                       m_dicCapabilitiesPaseRule;
    ESDictionaryA                       m_dicMaintenanceCapabilitiesPaseRule;
    ESDictionaryA                       m_dicInfoPaseRule;
    ESDictionaryA                       m_dicMaintenanceInfoPaseRule;
    ESDictionaryA                       m_dicStatusPaseRule;
    ESDictionaryA                       m_dicResultsPaseRule;
    ESDictionaryA                       m_dicMaintenanceResultsPaseRule;
};
