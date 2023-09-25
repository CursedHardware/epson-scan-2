////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScanner.h
//!
//! @brif     スキャナーコマンド用ベースクラスの定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <pthread.h>
#include "Include/ES2Command/ES2CommandAPI.h"
#include "Interface/IInterface.h"
#include "Utils/ESAccessor.h"

class IESScannerDelegate;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CESScanner
//!
//! @bref      スキャナーコマンド用ベースクラス
//!
//! @par      更新履歴
//! @par        - 新規作成       2014/09/
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CESScanner : public IESScanner
{
public:
	static CESScanner* CreateInstance(ESCommandType eCommandType);

public:
	// コンストラクタ
	CESScanner();
	// デストラクタ
	virtual ~CESScanner();

	//==============================================================================================
	//
	// 初期化・終了処理
	//
	//==============================================================================================

	//
	// インスタンスの破壊
	//
	virtual void DestroyInstance() override;

	//
	// 初期化処理
	//
	virtual ESErrorCode Initialize();

	//
	// [ 派生クラスで実装する ]
	// スキャン時のプロセス処理を行うDelegateを設定する
	//
	// virtual ESErrorCode SetDelegate( IESScannerDelegate* pDelegate ) override;

	// CESScanner派生用 CESScannerThreadからセットする
	virtual void SetDelegate( IESScannerDelegate* pDelegate, IESScanner* pScanner ) = 0;

	//
	// スキャン時のプロセス処理を行うDelegateを取得する
	//
	virtual IESScannerDelegate* GetDelegate() = 0;

	//
	//  DeviceInterfaceをセットします
	//
	virtual ESErrorCode SetDeviceInterface(IInterface* pDeviceInterface ) = 0;

	//
	//  DeviceInterfaceを取得します
	//
	virtual IInterface* GetDeviceInterface() = 0;

	//
	// 接続設定を行う
	//
	virtual ESErrorCode SetConnection( ES_JSON_CPTR pszJSON ) override;

	//==============================================================================================
	//
	// オープン・クローズ
	//
	//==============================================================================================

	//
	// [ 派生クラスで実装する ]
	// スキャナとの通信を開始する。
	//
	//virtual ESErrorCode Open() override;

	//
	// [ 派生クラスで実装する ]
	// スキャナとの通信を終了する
	//
	//virtual ESErrorCode Close() override;

	//
	// [ 派生クラスで実装する ]
	// スキャナがオープンしているか？
	//
	//virtual bool IsOpened() override;

	//==============================================================================================
	//
	// スキャン操作
	//
	//==============================================================================================

	//
	// [ 派生クラスで実装する ]
	// 現在のスキャン設定でスキャンを開始する。
	//
	//virtual ESErrorCode Scan() override;

	//
	// 別スレッドでスキャンを開始する。
	//
	virtual ESErrorCode ScanInBackground() override;

	//
	// [ 派生クラスで実装する ]
	// スキャン処理をキャンセルする。
	//
	//virtual ESErrorCode Cancel() override;

	//
	// [ 派生クラスで実装する ]
	// スキャン中か？
	//
	//virtual bool IsScanning() override;

	// スキャン中フラグのセット
	virtual void SetScanning( bool bScanning ) = 0;
	// キャンセル中フラグのセット
	virtual void SetCancelled( bool bCanncelled ) = 0;

	//==============================================================================================
	//
	// メンテナンス操作
	//
	//==============================================================================================

	//
	// [ 派生クラスで実装する ]
	// スキャナセンサーのクリーニング動作を要求する。
	//
	//virtual ESErrorCode DoCleaning() override;

	//
	// [ 派生クラスで実装する ]
	// スキャナセンサーのキャリブレーションを要求する。
	//
	//virtual ESErrorCode DoCalibration() override;

	//==============================================================================================
	//
	// AutoFeedingMode操作
	//
	//==============================================================================================

	//
	// [ 派生クラスで実装する ]
	// 中断中か？
	//
	//virtual bool IsInterrupted() = 0;

	//
	// [ 派生クラスで実装する ]
	// AFM中か？
	//
	//virtual bool IsAfmEnabled() = 0;

	//
	// [ 派生クラスで実装する ]
	// Auto Feeding Mode タイムアウトチェック開始
	//
	//virtual ESErrorCode ScheduleAutoFeedingModeTimeout() = 0;

	//==============================================================================================
	//
	// ジョブリクエスト
	//
	//==============================================================================================

	//
	// [ 派生クラスで実装する ]
	// スキャンジョブの開始
	//
	//virtual ESErrorCode StartJobInMode( ESJobMode eJobMode ) override;

	//
	// [ 派生クラスで実装する ]
	// スキャンジョブの終了
	//
	//virtual ESErrorCode StopJobInMode( ESJobMode eJobMode ) override;

	//==============================================================================================
	//
	// その他
	//
	//==============================================================================================

	//
	// オートフォーカスを要求する。
	//
	virtual ESErrorCode DoAutoFocus( ESFloat* pfOutFocus ) override;

	//
	// [ 派生クラスで実装する ]
	// スキャナーパネルのプッシュスキャン可能状態を設定する
	//
	//virtual ESErrorCode SetPanelToPushScanReady( BOOL bPushScanReady ) override;

	virtual BOOL IsScannableDeviceConfig() override;

	//==============================================================================================
	//
	// スキャナ情報の取得
	//
	//==============================================================================================

	//
	// [ 派生クラスで実装する ]
	// 設定をリセットする
	//
	//virtual ESErrorCode Reset() override;

	//
	// すべての設定可能なキーをJSONで取得する
	//
	virtual ESErrorCode GetAllKeys( IESResultString* pstrResult ) override;

	//
	// 指定したキーのデフォルト値をJSONで取得する
	//
	virtual ESErrorCode GetDefaultValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult ) override;

	//
	// 指定したキーにセット可能なすべての値をJSONで取得する
	//
	virtual ESErrorCode GetAllValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult ) override;

	//
	// 指定したキーに現在セット可能なすべての値をJSONで取得する
	//
	virtual ESErrorCode GetAvailableValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult ) override;

	//
	//  全キーに対する、セット可能なすべての値を取得する
	//
	virtual ESErrorCode GetAllValues( IESResultString* pstrResult ) override;

	//
	//  全キーに対する、セット可能なすべての値を取得する
	//
	virtual ESErrorCode GetAllAvailableValues( IESResultString* pstrResult ) override;

	//
	// 指定したキーの設定値をJSONで取得する
	//
	virtual ESErrorCode GetValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult ) override;

	//
	// 指定したキーの値をJSONで設定する
	//
	virtual ESErrorCode SetValueForKey( ES_CHAR_CPTR pszKey, ES_JSON_CPTR pszJSON ) override;

	//
	// JSON形式で値をセットする
	//
	virtual ESErrorCode SetValuesWithJSON( ES_JSON_CPTR pszJSON ) override;


protected:
	//
	// すべての設定可能なキーを取得する
	//
	virtual const ESStringArray& GetAllKeys();

	//
	// 指定したキーの能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )を取得する
	//
	virtual ESErrorCode GetCapabilityForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult );

	//
	// 指定したキーの指定した能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )を取得する
	//
	ESErrorCode GetTargetCapabilityForKey( ES_CHAR_CPTR pszKey, ES_CHAR_CPTR pszTarget, ESDictionary& dicResult );

	//
	// 指定したキーのデフォルト値を取得する
	//
	virtual ESErrorCode GetDefaultValueForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult );

	//
	// 指定したキーにセット可能なすべての値を取得する
	//
	virtual ESErrorCode GetAllValuesForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult );

	//
	// 指定したキーに現在セット可能なすべての値を取得する
	//
	virtual ESErrorCode GetAvailableValuesForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult );

	//
	//  全キーに対する、セット可能なすべての値を取得する
	//
	virtual ESDictionary GetAllValuesDictionary();

	//
	//  全キーに対する、セット可能なすべての値を取得する
	//
	virtual ESDictionary GetAvailableDictionary();

	//
	// 指定したキーの設定値を取得する
	//
	virtual ESErrorCode GetValueForKey( ES_CHAR_CPTR pszKey, ESAny& anyValue );

	//
	// 指定したキーの値を設定する
	//
	virtual ESErrorCode SetValueForKey( ES_CHAR_CPTR pszKey, ESAny anyValue );

	// 優先するキーを取得
	virtual ESStringArray GetPriorKeys();

	//
	// Dictionary形式で値をセットする
	//
	virtual ESErrorCode SetValuesForKeysWithDictionary( ESDictionary& dicKeysValues );

	// 登録されていないキーの値の取得処理
	virtual ESErrorCode GetValueForUndefinedKey( ES_CHAR_CPTR pszKey, ESAny& anyValue );
	// 登録されていないキーのセット処理
	virtual ESErrorCode SetValueForUndefinedKey( ES_CHAR_CPTR pszKey, ESAny anyValue );


	//
	// 指定したキーの指定した能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )をJSONで取得する
	//
	ESErrorCode GetTargetCapabilityForKey( ES_CHAR_CPTR pszKey, ES_CHAR_CPTR pszTarget, IESResultString* pstrResult );

	//
	// Capabilityの取得関数
	//-------------------------------------------------
protected:
	virtual void GetVersionCapability				( ESDictionary& dicResult ) = 0;
	virtual void GetWorkFolderCapability			( ESDictionary& dicResult );
	virtual void GetErrorStatusCapability			( ESDictionary& dicResult );
	virtual void GetAuthenticationCapability		( ESDictionary& dicResult );
	virtual void GetAuthenticationEnabledCapability	( ESDictionary& dicResult );
	virtual void GetAuthUserNameCapability			( ESDictionary& dicResult );
	virtual void GetAuthPasswordCapability			( ESDictionary& dicResult );
	virtual void GetProductNameCapability			( ESDictionary& dicResult );
	virtual void GetFunctionalUnitTypeCapability	( ESDictionary& dicResult );
	virtual void GetXResolutionCapability			( ESDictionary& dicResult );
	virtual void GetYResolutionCapability			( ESDictionary& dicResult );
	virtual void GetOpticalResolutionCapability		( ESDictionary& dicResult );
	virtual void GetMaxScanSizeCapability			( ESDictionary& dicResult );
	virtual void GetScanAreaCapability				( ESDictionary& dicResult );
	virtual void GetScanAreaInPixelCapability		( ESDictionary& dicResult );
	virtual void GetScanSizeCapability				( ESDictionary& dicResult );
	virtual void GetScanSizeInPixelCapability		( ESDictionary& dicResult );
	virtual void GetXOffsetMarginCapability			( ESDictionary& dicResult );
	virtual void GetYOffsetMarginCapability			( ESDictionary& dicResult );
	virtual void GetColorFormatCapability			( ESDictionary& dicResult );
	virtual void GetColorMatrixCapability			( ESDictionary& dicResult ) = 0;
	virtual void GetBitsPerPixelCapability			( ESDictionary& dicResult );
	virtual void GetGammaModeCapability				( ESDictionary& dicResult );
	virtual void GetGammaTableCapability			( ESDictionary& dicResult );
	virtual void GetThresholdCapability				( ESDictionary& dicResult );
	virtual void GetQuietModeCapability				( ESDictionary& dicResult );
	virtual void GetBufferSizeCapability			( ESDictionary& dicResult );
	virtual void GetDuplexTypeCapability			( ESDictionary& dicResult );
	virtual void GetDuplexCapability				( ESDictionary& dicResult );
	virtual void GetDoubleFeedDetectionCapability	( ESDictionary& dicResult );
	virtual void GetPagesToBeScannedCapability		( ESDictionary& dicResult );
	virtual void GetDocumentLoadedCapability		( ESDictionary& dicResult );
	virtual void GetCleaningCapability				( ESDictionary& dicResult );
	virtual void GetCalibrationCapability			( ESDictionary& dicResult );

	//
	// Accessor
	//
	//----------------------------------------------------------------------

	// WorkFolder
	virtual ESString GetWorkFolder() = 0;
	virtual ESErrorCode SetWorkFolder( ESString strWorkFolder ) = 0;

	// エラーステータスを取得する。
	virtual ESErrorCode GetErrorStatus() = 0;

	// バッファサイズを取得する
	virtual ESNumber GetBufferSize() = 0;
	// バッファサイズを設定する
	virtual ESErrorCode SetBufferSize( ESNumber nBufferSize ) = 0;

	// 認証機能をサポートしているか？
	virtual bool  IsAuthenticationSupported() = 0;

	// 認証機能が有効状態かどうかを取得する
	virtual bool IsAuthenticationEnabled() = 0;
	// 認証機能が有効状態かどうかを設定する
	virtual ESErrorCode  SetAuthenticationEnabled( bool bEnabled ) = 0;

	// 認証ユーザー名を取得する
	virtual ESString GetAuthUserName() = 0;
	// 認証ユーザー名を設定する
	virtual ESErrorCode SetAuthUserName( ESString strAuthUserName ) = 0;

	// 認証パスワードデーターを取得する
	virtual ESString GetAuthPassword() = 0;
	// 認証パスワードデーターを設定する
	virtual ESErrorCode SetAuthPassword( ESString strAuthPasswordData ) = 0;

	// プロダクト名を取得する
	virtual ESString GetProductName() = 0;

	// サポートする給紙タイプを取得する
	virtual ESIndexSet GetSupportedFunctionalUnitTypes() = 0;
	// 給紙タイプを取得する
	virtual ESFunctionalUnitType GetFunctionalUnitType() = 0;
	// 給紙タイプを設定する (ESFunctionalUnitType)
	virtual ESErrorCode SetFunctionalUnitType( ESNumber nFunctionalUnitType ) = 0;
	
	// サポートする解像度を取得する
	virtual ESAny GetSupportedXResolutions() = 0;
	virtual ESAny GetSupportedYResolutions() = 0;
	// 解像度を取得する
	virtual ESNumber GetXResolution() = 0;
	virtual ESNumber GetYResolution() = 0;
	// 解像度を設定する
	virtual ESErrorCode SetXResolution( ESNumber xResolution ) = 0;
	virtual ESErrorCode SetYResolution( ESNumber yResolution ) = 0;
	// 光学解像度を取得する
	virtual ESNumber GetOpticalResolution() = 0;

	// 最大スキャンサイズを取得する(インチ）
	virtual ST_ES_SIZE_F GetMaxScanSize() = 0;

	// スキャンするエリアを取得する(インチ）
	virtual ST_ES_RECT_F GetScanArea() = 0;
	// スキャンするエリアを設定する(インチ）
	virtual ESErrorCode SetScanArea( ST_ES_RECT_F rcScanArea ) = 0;

	// スキャンするエリアをピクセル単位で取得する
	virtual ST_ES_RECT_UN32 GetScanAreaInPixel() = 0;
	// スキャンするエリアをピクセル単位で設定する
	virtual ESErrorCode SetScanAreaInPixel( ST_ES_RECT_UN32 rcScanAreaInPixel ) = 0;

	// スキャンするサイズを取得する（インチ）
	virtual ST_ES_SIZE_F GetScanSize() = 0;
	// スキャンするサイズを設定する（インチ）
	virtual ESErrorCode SetScanSize( ST_ES_SIZE_F sizeScan ) = 0;

	// スキャンするサイズをピクセル単位で取得する
	virtual ST_ES_SIZE_UN32 GetScanSizeInPixel() = 0;
	// スキャンするサイズをピクセル単位で設定する
	virtual ESErrorCode SetScanSizeInPixel( ST_ES_SIZE_UN32 sizeScan ) = 0;

	// オフセットマージン
	virtual ESFloat GetXOffsetMargin() = 0;
	virtual ESErrorCode SetXOffsetMargin( ESFloat fMargin ) = 0;
	virtual ESFloat GetYOffsetMargin() = 0;
	virtual ESErrorCode SetYOffsetMargin( ESFloat fMargin ) = 0;

	// サポートするカラーモードを取得する
	virtual ESIndexSet GetSupportedColorFormats() = 0;
	// カラーフォーマットを取得する
	virtual ESNumber GetColorFormat() = 0;
	// カラーフォーマットを設定する(ESColorFormat)
	virtual ESErrorCode SetColorFormat( ESNumber nColorFormat ) = 0;

	// サンプル当たりのbit数を取得する
	virtual ESNumber GetBitsPerSample() = 0;
	// 1ピクセル当たりのサンプル数を取得する
	virtual ESNumber GetSamplesPerPixel() = 0;
	// 1ピクセル当たりのbit数を取得する
	virtual ESNumber GetBitsPerPixel() = 0;

	// カラーマトリックスを取得する(ESFloatArray or nullptr)
	virtual ESAny GetColorMatrix() = 0;
	// カラーマトリックスを設定する
	virtual ESErrorCode SetColorMatrix( ESFloatArray arColorMatrix ) = 0;

	// サポートするガンマモードを取得する
	virtual ESIndexSet GetSupportedGammaModes() = 0;
	// ガンマモードを取得する(ESGammaMode)
	virtual ESGammaMode GetGammaMode() = 0;
	// ガンマモードを設定する(ESGammaMode)
	virtual ESErrorCode SetGammaMode( ESNumber nGammaMode ) = 0;

	// ガンマテーブルを取得する
	virtual ESAny GetGammaTableMono () = 0;
	virtual ESAny GetGammaTableRed  () = 0;
	virtual ESAny GetGammaTableGreen() = 0;
	virtual ESAny GetGammaTableBlue () = 0;
	// ガンマテーブルを設定する
	virtual ESErrorCode SetGammaTableMono ( ESIndexArray arGammaTable ) = 0;
	virtual ESErrorCode SetGammaTableRed  ( ESIndexArray arGammaTable ) = 0;
	virtual ESErrorCode SetGammaTableGreen( ESIndexArray arGammaTable ) = 0;
	virtual ESErrorCode SetGammaTableBlue ( ESIndexArray arGammaTable ) = 0;

	// サポートするモノクロ閾値を取得する( ESIndexSet or ST_ES_RANGE )
	virtual ESAny GetSupportedThreshold() = 0;
	// モノクロ閾値を取得する
	virtual ESNumber GetThreshold() = 0;
	// モノクロ閾値を設定する
	virtual ESErrorCode SetThreshold( ESNumber nThreshold ) = 0;

	// サポートする静音モードを取得する
	virtual ESIndexSet GetSupportedQuietModes() = 0;
	// 静音モードを取得する(ESQuietMode)
	virtual ESQuietMode GetQuietMode() = 0;
	// 静音モードを設定する(ESQuietMode)
	virtual ESErrorCode SetQuietMode( ESNumber nQuiteMode ) = 0;

	// フィーダー(ADF)機能をサポートしているか?
	virtual bool IsFeederSupported() = 0;
	// フィーダー(ADF)機能が有効か？
	virtual bool IsFeederEnabled() = 0;
	
	// 両面同時読み取りの種類を取得する
	virtual ESDuplexType GetDuplexType() = 0;

	// 両面同時読み取りをサポートしているか?
	virtual bool IsDuplexSupported() = 0;
	// 両面同時読み取り機能が有効か?
	virtual bool IsDuplexEnabled() = 0;
	// 両面同時読み取り機能の有効状態を設定する
	virtual ESErrorCode SetDuplexEnabled( bool bEnabled ) = 0;
	
	// サポートしている重送検知を取得する
	virtual ESIndexSet GetSupportedDoubleFeedDetections() = 0;
	// 重送検知の設定を取得する
	virtual ESDoubleFeedDetection GetDoubleFeedDetection() = 0;
	// 重送検知を設定を変更(ESDoubleFeedDetection)
	virtual ESErrorCode SetDoubleFeedDetection( ESNumber nDoubleFeedDetection ) = 0;

	// ドキュメントが読み込まれたか？
	virtual bool IsDocumentLoaded() = 0;

	// スキャンするページ枚数を取得する
	virtual ESNumber GetPagesToBeScanned() = 0;
	// スキャンするページ枚数を設定する
	virtual ESErrorCode SetPagesToBeScanned( ESNumber nPagesToBeScanned ) = 0;

	// クリーニング機能をサポートするか
	virtual bool IsCleaningSupported() = 0;
	//
	virtual bool IsCalibrationSupported() = 0;

	// Captureコマンドをサポートしているか？
	virtual bool IsCaptureCommandSupported() = 0;
	// Captureコマンドをサポートしているか設定する
	virtual ESErrorCode SetCaptureCommandSupported( bool bSupported ) = 0;


	static void* EnterScannerThread(void* vpContext);

private:
	// Accessorマップ
	ACCESSOR_MAP m_mapAccessor;
	ESStringArray m_arAllKeys;

	typedef std::function<void(CESScanner*, ESDictionary&) >	FN_CAPA;
	typedef std::map< ESString,  FN_CAPA >			CAPA_MAP;
	typedef CAPA_MAP::value_type								CAPA_PAIR;
	CAPA_MAP m_funcCapability;
};

