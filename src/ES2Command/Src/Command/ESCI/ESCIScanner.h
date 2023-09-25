////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCIScanner.h
//!
//! @brif     ESCIコマンド用スキャナクラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Command/ESScanner.h"
#include "ESCIAccessor.h"
#include "../ModelInfo.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CESCIScanner
//!
//! @bref      ESCIコマンド用スキャナクラス
//!
//! @par      更新履歴
//! @par        - 新規作成       2014/
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CESCIScanner : public CESCIAccessor, virtual public CESScanner
{
public:
	// コンストラクタ
	CESCIScanner();
	// デストラクタ
	virtual ~CESCIScanner();

	// 初期化処理
	virtual ESErrorCode Initialize() override;

	// DeviceInterfaceをセットします
	virtual ESErrorCode SetDeviceInterface(IInterface* pDeviceInterface ) override;
	// DeviceInterfaceを取得します
	virtual IInterface* GetDeviceInterface() override;

	// スキャン時のプロセス処理を行うDelegateを設定する
	virtual void SetDelegate( IESScannerDelegate* pDelegate ) override;

	virtual void SetDelegate( IESScannerDelegate* pDelegate, IESScanner* pScanner ) override;

	// スキャン時のプロセス処理を行うDelegateを取得する
	virtual IESScannerDelegate* GetDelegate() override;

	//
	// スキャナ操作
	//---------------------------------------
	// スキャナをオープンし、通信可能な状態にする。
	virtual ESErrorCode Open() override;
	// スキャナをクローズし、通信を切断させる。
	virtual ESErrorCode Close() override;
	// スキャナがオープンしているか？
	virtual bool IsOpened() const override;

	//
	// スキャン操作
	//---------------------------------------
	// 現在のスキャン設定でスキャンを開始する。
	virtual ESErrorCode Scan() override;
	// スキャン処理をキャンセルする。
	virtual ESErrorCode Cancel()	override;
	//
	virtual ESErrorCode Abort() override;

	// スキャン中か？
	virtual bool IsScanning() const override;

	// スキャン中フラグのセット
	virtual void SetScanning( bool bScanning ) override;
	// キャンセル中フラグのセット
	virtual void SetCancelled( bool bCanncelled ) override;

	//
	// メンテナンス操作
	//---------------------------------------
	// スキャナセンサーのクリーニング動作を要求する。
	virtual ESErrorCode DoCleaning() override;
	// スキャナセンサーのキャリブレーションを要求する。
	virtual ESErrorCode DoCalibration() override;
	// 設定をリセットする
	virtual ESErrorCode Reset() override;

	//
	// AutoFeedingMode操作
	//---------------------------------------
	// 中断中か？
	virtual bool IsInterrupted() const override;
	// AFM中か？
	virtual bool IsAfmEnabled() const override;
	// Auto Feeding Mode タイムアウトチェック開始
	virtual ESErrorCode ScheduleAutoFeedingModeTimeout() override;

	//
	// ジョブリクエスト
	//---------------------------------------
	// スキャンジョブの開始
	virtual ESErrorCode StartJobInMode( ESJobMode eJobMode ) override;
	// スキャンジョブの終了
	virtual ESErrorCode StopJobInMode( ESJobMode eJobMode ) override;

	//
	// その他
	//---------------------------------------
	// オートフォーカスを要求する。
	virtual ESErrorCode DoAutoFocus( ESFloat* pfOutFocus ) override;

	// スキャナーパネルのプッシュスキャン可能状態を設定する
	virtual ESErrorCode SetPanelToPushScanReady( BOOL bPushScanReady ) override;

	virtual ESErrorCode UnlockAdministratorLock() override;
	virtual ESErrorCode LockAdministratorLock() override;

	//
	// スキャナ情報の取得
	//-------------------------------------------------
	// すべての設定可能なキーを取得する
	virtual const ESStringArray& GetAllKeys() override;

	// 指定したキーの能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )を取得する
	virtual ESErrorCode GetCapabilityForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult ) override;

	//
	// Capabilityの取得関数
	//-------------------------------------------------
protected:
	bool IsSupportedLogFunction(ESCILogFunctionKey function);

	virtual void GetVersionCapability				( ESDictionary& dicResult ) override;
	virtual void GetColorMatrixCapability			( ESDictionary& dicResult ) override;
	void GetDisableJobContinueCapability			( ESDictionary& dicResult );
	void GetMaxScanSizeInLongLengthCapability		( ESDictionary& dicResult );
	void GetWarmingUpCapability						( ESDictionary& dicResult );
	void GetButtonStatusCapability					( ESDictionary& dicResult );
	void GetLampModeCapability						( ESDictionary& dicResult );
	void GetHalftoneCapability						( ESDictionary& dicResult );
	void GetMaxFocusCapability						( ESDictionary& dicResult );
	void GetMinFocusCapability						( ESDictionary& dicResult );
	void GetFocusCapability							( ESDictionary& dicResult );
	void GetDigitalICECapability					( ESDictionary& dicResult );
	void GetFilmTypeCapability						( ESDictionary& dicResult );
	void GetScanningModeCapability					( ESDictionary& dicResult );
	void GetColorMatrixModeCapability				( ESDictionary& dicResult );
	void GetDetectedDocumentSizeCapability			( ESDictionary& dicResult );
	void GetSimplexScanCounterCapability			( ESDictionary& dicResult );
	void GetDuplexScanCounterCapability				( ESDictionary& dicResult );

	void GetCaptureCommandSupportedCapability		( ESDictionary& dicResult );
	void GetShouldIgnoreCancelFromScannerCapability	( ESDictionary& dicResult );
	void GetGuidePositionReadOnlyCapability			( ESDictionary& dicResult );
	void GetGuidePositionCapability					( ESDictionary& dicResult );

	void GetLamp1CounterCapability					( ESDictionary& dicResult );
	void GetLamp2CounterCapability					( ESDictionary& dicResult );
	void GetPickupRollerCounterCapability			( ESDictionary& dicResult );
	void GetScanCounterCapability					( ESDictionary& dicResult );
	void GetPaperEndDetectionCapability				( ESDictionary& dicResult );
	void GetOverScanCapability						( ESDictionary& dicResult );
	void GetPowerOffTimeCapability					( ESDictionary& dicResult );
	void GetLightIntensityCapability				(ESDictionary& dicResult);


	//
	// Accessor ( CESScanner用 override 関数 )
	//--------------------------------------------
public:

	//
	// 指定したキーの設定値を取得する
	//
	virtual ESErrorCode GetValueForKey( ES_CHAR_CPTR pszKey, ESAny& anyValue ) override;

	//
	// 指定したキーの値を設定する
	//
	virtual ESErrorCode SetValueForKey( ES_CHAR_CPTR pszKey, ESAny anyValue ) override;

protected:
	// WorkFolderを取得する
	ESString GetWorkFolder() override;
	// WorkFolderを設定する
	ESErrorCode SetWorkFolder( ESString strWorkFolder ) override;

	// エラーステータスを取得する。
	ESErrorCode GetErrorStatus() override;

	// バッファサイズを取得する
	virtual ESNumber GetBufferSize() override;
	// バッファサイズを設定する
	ESErrorCode SetBufferSize( ESNumber nBufferSize ) override;

	// 認証機能をサポートしているか？
	bool IsAuthenticationSupported() override;

	// 認証機能が有効状態かどうかを取得する
	bool IsAuthenticationEnabled() override;
	// 認証機能が有効状態かどうかを設定する
	ESErrorCode  SetAuthenticationEnabled( bool bEnabled ) override;

	// 認証ユーザー名を取得する
	ESString GetAuthUserName() override;
	// 認証ユーザー名を設定する
	ESErrorCode SetAuthUserName( ESString strAuthUserName ) override;

	// 認証パスワードを取得する
	ESString GetAuthPassword() override;
	// 認証パスワードを設定する
	ESErrorCode SetAuthPassword( ESString strAuthPassword ) override;

	// プロダクト名を取得する
	ESString GetProductName() override;

	// サポートする給紙タイプを取得する
	ESIndexSet GetSupportedFunctionalUnitTypes() override;
	// 給紙タイプを取得する
	ESFunctionalUnitType GetFunctionalUnitType() override;
	// 給紙タイプを設定する ( ESFunctionalUnitType )
	ESErrorCode SetFunctionalUnitType( ESNumber nFunctionalUnitType ) override;

	// サポートする解像度を取得する (ESIndexSet or ST_ES_RANGE)
	ESAny GetSupportedXResolutions() override;
	ESAny GetSupportedYResolutions() override;
	// 解像度を取得する
	ESNumber GetXResolution() override;
	ESNumber GetYResolution() override;
	// 解像度を設定する
	ESErrorCode SetXResolution( ESNumber xResolution ) override;
	ESErrorCode SetYResolution( ESNumber yResolution ) override;
	// 光学解像度を取得する
	ESNumber GetOpticalResolution() override;

	// 最大スキャンサイズを取得する(インチ）
	ST_ES_SIZE_F GetMaxScanSize() override;

	// スキャンするエリアを取得する(インチ）
	ST_ES_RECT_F GetScanArea() override;
	// スキャンするエリアを設定する(インチ）
	ESErrorCode SetScanArea( ST_ES_RECT_F rcScanArea ) override;

	// スキャンするエリアをピクセル単位で取得する
	ST_ES_RECT_UN32 GetScanAreaInPixel() override;
	// スキャンするエリアをピクセル単位で設定する
	ESErrorCode SetScanAreaInPixel( ST_ES_RECT_UN32 rcScanAreaInPixel ) override;

	// スキャンするサイズを取得する（インチ）
	ST_ES_SIZE_F GetScanSize() override;
	// スキャンするサイズを設定する（インチ）
	ESErrorCode SetScanSize( ST_ES_SIZE_F sizeScan ) override;

	// スキャンするサイズをピクセル単位で取得する
	virtual ST_ES_SIZE_UN32 GetScanSizeInPixel() override;
	// スキャンするサイズをピクセル単位で設定する
	virtual ESErrorCode SetScanSizeInPixel( ST_ES_SIZE_UN32 sizeScan ) override;

	// オフセットマージン
	virtual ESFloat GetXOffsetMargin() override;
	virtual ESErrorCode SetXOffsetMargin( ESFloat fMargin ) override;
	virtual ESFloat GetYOffsetMargin() override;
	virtual ESErrorCode SetYOffsetMargin( ESFloat fMargin ) override;

	// サポートするカラーモードを取得する
	ESIndexSet GetSupportedColorFormats() override;
	// カラーフォーマットを取得する
	ESNumber GetColorFormat() override;
	// カラーフォーマットを設定する(ESColorFormat)
	ESErrorCode SetColorFormat( ESNumber nColorFormat ) override;

	// サンプル当たりのbit数を取得する
	ESNumber GetBitsPerSample() override;
	// 1ピクセル当たりのサンプル数を取得する
	ESNumber GetSamplesPerPixel() override;
	// 1ピクセル当たりのbit数を取得する
	ESNumber GetBitsPerPixel() override;

	// カラーマトリックスを取得する(ESFloatArray or nullptr)
	ESAny GetColorMatrix() override;
	// カラーマトリックスを設定する
	ESErrorCode SetColorMatrix( ESFloatArray arColorMatrix ) override;

	// サポートするガンマモードを取得する
	ESIndexSet GetSupportedGammaModes() override;
	// ガンマモードを取得する
	ESGammaMode GetGammaMode() override;
	// ガンマモードを設定する(ESGammaMode)
	ESErrorCode SetGammaMode( ESNumber nGammaMode ) override;

	// ガンマテーブルを取得する
	ESAny GetGammaTableMono () override;
	ESAny GetGammaTableRed  () override;
	ESAny GetGammaTableGreen() override;
	ESAny GetGammaTableBlue () override;
	// ガンマテーブルを設定する
	ESErrorCode SetGammaTableMono ( ESIndexArray arGammaTable ) override;
	ESErrorCode SetGammaTableRed  ( ESIndexArray arGammaTable ) override;
	ESErrorCode SetGammaTableGreen( ESIndexArray arGammaTable ) override;
	ESErrorCode SetGammaTableBlue ( ESIndexArray arGammaTable ) override;

	// サポートするモノクロ閾値を取得する( ESIndexSet or ST_ES_RANGE )
	ESAny GetSupportedThreshold() override;
	// モノクロ閾値を取得する
	ESNumber GetThreshold() override;
	// モノクロ閾値を設定する
	ESErrorCode SetThreshold( ESNumber nThreshold ) override;

	// サポートする静音モードを取得する
	ESIndexSet GetSupportedQuietModes() override; 
	// 静音モードを取得する
	ESQuietMode GetQuietMode() override;
	// 静音モードを設定する(ESQuietMode)
	ESErrorCode SetQuietMode( ESNumber nQuietMode) override;

	// フィーダー(ADF)機能をサポートしているか?
	bool IsFeederSupported() override;
	// フィーダー(ADF)機能が有効か？
	bool IsFeederEnabled() override;

	// 両面同時読み取りの種類を取得する
	ESDuplexType GetDuplexType() override;

	// 両面同時読み取りをサポートしているか?
	bool IsDuplexSupported() override;
	// 両面同時読み取り機能が有効か?
	bool IsDuplexEnabled() override;
	// 両面同時読み取り機能の有効状態を設定する
	ESErrorCode SetDuplexEnabled( bool bEnabled ) override;

	// サポートしている重送検知を取得する
	ESIndexSet GetSupportedDoubleFeedDetections() override;
	// 重送検知の設定を取得する
	ESDoubleFeedDetection GetDoubleFeedDetection() override;
	// 重送検知を設定を変更(ESDoubleFeedDetection)
	ESErrorCode SetDoubleFeedDetection( ESNumber nDoubleFeedDetection ) override;

	// ドキュメントが読み込まれたか？
	bool IsDocumentLoaded() override;

	// スキャンするページ枚数を取得する
	ESNumber GetPagesToBeScanned() override;
	// スキャンするページ枚数を設定する
	ESErrorCode SetPagesToBeScanned( ESNumber nPagesToBeScanned ) override;

	// クリーニング機能をサポートするか
	bool IsCleaningSupported() override;
	//
	bool IsCalibrationSupported() override;

	// Captureコマンドをサポートしているか？
	bool IsCaptureCommandSupported() override;
	// Captureコマンドをサポートしているか設定する
	ESErrorCode SetCaptureCommandSupported( bool bSupported ) override;

private:
	// Accessorマップ
	ACCESSOR_MAP m_mapAccessor;
	ESStringArray m_arAllKeys;

	typedef std::function<void(CESCIScanner*, ESDictionary&)>	FN_CAPA;
	typedef std::map< ESString,  FN_CAPA >			CAPA_MAP;
	typedef CAPA_MAP::value_type								CAPA_PAIR;

	CAPA_MAP m_funcCapability;
};

