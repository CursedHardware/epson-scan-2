////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCIAccessor.h
//!
//! @brif     ESCIコマンドアクセス クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ESCICommand.h"
#include "Utils/event_caller.hpp"

enum {
	kBitDepth1  = 0x01,
	kBitDepth8  = 0x08,
	kBitDepth16 = 0x10,
};
typedef UInt8 ENUM_ESCI_BIT_DEPTH;

enum {
	kColorModeMono      = 0x00,
	kColorModeMonoDropR = 0x10,
	kColorModeMonoDropG = 0x20,
	kColorModeMonoDropB = 0x30,
	kColorModeLineSeq   = 0x12,
	kColorModePixelSeq  = 0x13,
	kColorModeRGB       = kColorModePixelSeq,
};
typedef UInt8 ENUM_ESCI_CLOLOR_MODE;

#define FOCUS_PHYSICAL_TO_COMMAND(x) (((x) * 10.0) + 0x40)
#define FOCUS_COMMAND_TO_PHYSICAL(x) (((x) - 0x40) / 10.0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CESCIAccessor
//!
//! @bref      ESCIコマンドアクセス クラス
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CESCIAccessor : public CESCICommand
{
public:
	CESCIAccessor();
	virtual ~CESCIAccessor();

	// 初期値設定処理
	void Setup();
	//　終了処理
	ESErrorCode Finalize();

	//
	// スキャナ操作
	//---------------------------------------
protected:
	// 物理インターフェースのOpenおよびスキャナから情報を取得し、スキャナの使用を開始する。
	ESErrorCode Open();
	// 物理インターフェースをCloseし、スキャナの使用を終了する。
	ESErrorCode Close();
	// スキャナがオープンしているか？
	bool IsOpened() const;

	void DeviceDisconnected() override;

	//
	// Capture/release scanner to control mutual exclusive access for the same scanner
	//---------------------------------------
	ESErrorCode CaptureScanner();
	ESErrorCode ReleaseScanner();

	//
	// ジョブリクエスト
	//---------------------------------------
	// スキャンジョブの開始
	ESErrorCode StartJobInMode( ESJobMode eJobMode );
	// スキャンジョブの終了
	ESErrorCode StopJobInMode( ESJobMode eJobMode );

	ESErrorCode StartJobInStandard();
	ESErrorCode StopJobInStandard();
	ESErrorCode StartJobInContinue();
	ESErrorCode StopJobInContinue();

	ESJobMode GetJobMode();
	ESErrorCode SetJobMode( ESJobMode eJobMode );

	bool IsDisableJobContinue();
	ESErrorCode SetDisableJobContinue( bool bDisableJobContinue );

	//
	// スキャン操作
	//---------------------------------------
protected:
	// 現在のスキャン設定でスキャンを開始する。
	ESErrorCode Scan();

	//
	CESScannedImage* CreateImageInstance();

	//
	ESErrorCode ScanForDigitalICE();
	ESErrorCode ScanForFilmICE();
	ESErrorCode ScanForPrintICE();

	//
	ESErrorCode ScanForDocumentFeeder();

	//
	ESErrorCode ScanForNormal();

	// スキャン処理をキャンセルする。
	ESErrorCode Cancel();

	// ColorMatrixを適用
	virtual ESErrorCode ApplyColorMatrix( ES_CMN_FUNCS::BUFFER::IESBuffer& cScannedData, UInt32 nDataLength )	override;

	//
	// ボタン監視
	//-------------------------------------------------
	// プッシュボタン監視をコマンドでおこなうかUSB Interruptでおこなうか。
	// USB Interruptパイプがあっても、プッシュボタンイベントをサポートしていない機種のため。
	bool ShouldObserveButtonWithCommand(bool initialize = false);
	// ボタン監視コマンド送信間隔。
	DWORD ButtonObservationInterval();

	void StartButtonChecking();
	void StopButtonChecking();

	//
	// スキャナ情報取得
	//----------------------------------------------------------
protected:
	//
	// Identity情報を取得して保持する。
	//
	ESErrorCode GetIdentity();

	//
	// 拡張Identity情報を取得して保持する。
	//
	ESErrorCode GetExtIdentity();

	//
	// ステータス情報を取得して保持する。
	//
	ESErrorCode GetStatus();

	//
	// 設定をリセットする
	//
	ESErrorCode Reset();

public:
	//
	// バージョン取得
	//
	virtual ESString GetVersion();

	//
	// エラーステータスを取得する。(ESErrorStatus)
	//
	virtual ESErrorCode GetErrorStatus() override;

	//
	// スキャナからのキャンセルを無視するか？
	//
	virtual bool IsShouldIgnoreCancelFromScanner() override;
	//
	// スキャナからのキャンセルを無視するか設定する
	//
	ESErrorCode SetShouldIgnoreCancelFromScanner( bool bIgnore );

	//
	// キャプチャー機能をサポートするか？
	//
	virtual bool IsCaptureCommandSupported() override;
	//
	// キャプチャーコマンドをサポートしているか設定する
	//
	ESErrorCode SetCaptureCommandSupported( bool bSupported );

	//
	// キャプチャー中か？
	//
	virtual bool IsCaptured() override;
	//
	// キャプチャー状態をセットする
	//
	virtual void SetCaptured( bool bCaptured ) override;

	//
	// バッファサイズを取得する
	//
	virtual ESNumber GetBufferSize() override;
	//
	// バッファサイズを設定する
	//
	ESErrorCode SetBufferSize( ESNumber nBufferSize );

	//
	// 静止中か？
	//
	virtual bool IsDumb() override;

	//
	// ウォーミングアップ中か？
	//
	virtual bool IsWarmingUp() override;

	//
	// 認証機能をサポートしているか？
	//
	virtual bool IsAuthenticationSupported() override;

	//
	// 認証機能が有効状態かどうかを取得する
	//
	bool IsAuthenticationEnabled() override;
	//
	// 認証機能が有効状態かどうかを設定する
	//
	ESErrorCode  SetAuthenticationEnabled( bool bEnabled );

	//
	// 認証ユーザー名を取得する
	//
	virtual ESString GetAuthUserName() override;
	//
	// 認証ユーザー名を設定する
	//
	virtual ESErrorCode SetAuthUserName( ESString strAuthUserName );
	//
	// 認証パスワードを取得する
	//
	virtual ESString GetAuthPassword() override;
	//
	// 認証パスワードを設定する
	//
	virtual ESErrorCode SetAuthPassword( ESString strAuthPassword );

	//
	// プロダクト名を取得する
	//
	ESString GetProductName() override;

	//
	// パワーオフ機能をサポートしているか？
	//
	bool IsPowerOffSupported();
	//
	// パワーオフ時間を取得する
	//
	ESNumber GetPowerOffTime();
	//
	// パワーオフ時間を設定する
	//
	ESErrorCode SetPowerOffTime( ESNumber nPowerOffTime );

	//
	// プッシュボタンをサポートしているか？
	//
	bool IsPushButtonSupported();
	//
	// プッシュボタンのステータスを取得する
	//
	UInt8 GetButtonStatus();

	//
	// 取り込み装置オプションを取得する
	//
	ENUM_ESCI_OPTION_CONTROL GetOptionControl();
	//
	// 取り込み装置オプションを設定する
	//
	bool SetOptionControl( ENUM_ESCI_OPTION_CONTROL eOptionControl ); 

	//
	// サポートする給紙タイプを取得する
	//
	ESIndexSet GetSupportedFunctionalUnitTypes();
	//
	// 給紙タイプを取得する
	//
	ESFunctionalUnitType GetFunctionalUnitType();
	//
	// 給紙タイプを設定する ( ESFunctionalUnitType )
	//
	ESErrorCode SetFunctionalUnitType( ESNumber nFunctionalUnitType );

	//
	// サポートする解像度を取得する (ESIndexSet or ST_ES_RANGE)
	//
	ESAny GetSupportedXResolutions();
	ESAny GetSupportedYResolutions();
	//
	// 解像度を取得する
	//
	ESNumber GetXResolution();
	ESNumber GetYResolution();
	//
	// 解像度を設定する
	//
	ESErrorCode SetXResolution( ESNumber xResolution );
	ESErrorCode SetYResolution( ESNumber yResolution );
	//
	// 光学解像度を取得する
	//
	ESNumber GetOpticalResolution();

	//
	// 最大スキャンサイズを取得する(インチ）
	//
	ST_ES_SIZE_F GetMaxScanSize();

	//
	// スキャンするエリアを取得する(インチ）
	//
	ST_ES_RECT_F GetScanArea();
	//
	// スキャンするエリアを設定する(インチ）
	//
	ESErrorCode SetScanArea( ST_ES_RECT_F rcScanArea, bool bShouldAlign );
	ESErrorCode SetScanArea( ST_ES_RECT_F rcScanArea );

	//
	//
	//
	ST_ES_SIZE_F GetMaxScanSizeInLongLength();
	//
	//
	//
	ESErrorCode SetMaxScanSizeInLongLength( ST_ES_SIZE_F maxScanSizeInLongLength );

	//
	// スキャンするエリアをピクセル単位で取得する
	//
	ST_ES_RECT_UN32 GetScanAreaInPixel();
	//
	// スキャンするエリアをピクセル単位で設定する
	//
	ESErrorCode SetScanAreaInPixel( ST_ES_RECT_UN32 rcScanAreaInPixel, bool bShouldAlign );
	ESErrorCode SetScanAreaInPixel( ST_ES_RECT_UN32 rcScanAreaInPixel );

	//
	// スキャンするサイズを取得する（インチ）
	//
	ST_ES_SIZE_F GetScanSize();
	//
	// スキャンするサイズを設定する（インチ）
	//
	ESErrorCode SetScanSize( ST_ES_SIZE_F sizeScan );

	//
	// スキャンするサイズをピクセル単位で取得する
	//
	ST_ES_SIZE_UN32 GetScanSizeInPixel();
	//
	// スキャンするサイズをピクセル単位で設定する
	//
	ESErrorCode SetScanSizeInPixel( ST_ES_SIZE_UN32 sizeScan );

	//
	// オフセットマージン
	//
	ESFloat GetXOffsetMargin();
	ESErrorCode SetXOffsetMargin( ESFloat fMargin );
	ESFloat GetYOffsetMargin();
	ESErrorCode SetYOffsetMargin( ESFloat fMargin );

	//
	// スキャンエリア設定のガイド位置を取得する
	//
	ESGuidePosition GetGuidePosition();
	ESGuidePosition GetGuidePositionFB();
	ESGuidePosition GetGuidePositionADF();
	//
	// スキャンエリア設定のガイド位置を設定する
	//
	ESErrorCode SetGuidePositionFB( ESNumber nGuidePosition );
	ESErrorCode SetGuidePositionADF( ESNumber nGuidePosition );

	//
	// bit深度を取得する
	//
	ENUM_ESCI_BIT_DEPTH GetBitDepth();
	//
	// bit深度を設定する
	//
	bool SetBitDepth( ENUM_ESCI_BIT_DEPTH eBitDepth );

	//
	// カラーモードを取得する
	//
	ENUM_ESCI_CLOLOR_MODE GetColorMode();
	//
	// カラーモードを設定する
	//
	bool SetColorMode( ENUM_ESCI_CLOLOR_MODE eColorMode );

	//
	// サポートするカラーモードを取得する
	//
	ESIndexSet GetSupportedColorFormats();
	//
	// カラーフォーマットを取得する
	//
	ESNumber GetColorFormat();
	//
	// カラーフォーマットを設定する(ESColorFormat)
	//
	ESErrorCode SetColorFormat( ESNumber nColorFormat );

	//
	// サンプル当たりのbit数を取得する
	//
	ESNumber GetBitsPerSample();
	//
	// 1ピクセル当たりのサンプル数を取得する
	//
	ESNumber GetSamplesPerPixel();
	//
	// 1ピクセル当たりのbit数を取得する
	//
	ESNumber GetBitsPerPixel();

	//
	// カラーマトリックスを取得する(ESFloatArray or nullptr)
	//
	ESAny GetColorMatrix();
	//
	// カラーマトリックスを設定する
	//
	ESErrorCode SetColorMatrix( ESFloatArray arColorMatrix );

	//
	// サポートするハーフトーンを取得する
	//
	ESIndexSet GetSupportedHalftones();
	//
	// ハーフトーンを取得する
	//
	ESHalftone GetHalftones();
	//
	// ハーフトーンを設定する(ESHalftone)
	//
	ESErrorCode SetHalftones( ESNumber nHalftone );

	//
	// サポートするガンマモードを取得する
	//
	ESIndexSet GetSupportedGammaModes();
	//
	// ガンマモードを取得する
	//
	ESGammaMode GetGammaMode();
	//
	// ガンマモードを設定する(ESGammaMode)
	//
	ESErrorCode SetGammaMode( ESNumber nGammaMode );

	//
	// ガンマテーブルを取得する
	//
	ESAny GetGammaTableMono ();
	ESAny GetGammaTableRed  ();
	ESAny GetGammaTableGreen();
	ESAny GetGammaTableBlue ();
	//
	// ガンマテーブルを設定する
	//
	ESErrorCode SetGammaTable     ( const ESIndexArray& arGammaTable, ESGammaChannel eChannel );
	ESErrorCode SetGammaTableMono ( ESIndexArray arGammaTable );
	ESErrorCode SetGammaTableRed  ( ESIndexArray arGammaTable );
	ESErrorCode SetGammaTableGreen( ESIndexArray arGammaTable );
	ESErrorCode SetGammaTableBlue ( ESIndexArray arGammaTable );

	ESErrorCode RequestSetGammaTables();


	//
	// サポートするモノクロ閾値を取得する( ESIndexSet or ST_ES_RANGE )
	//
	ESAny GetSupportedThreshold();
	//
	// モノクロ閾値を取得する
	//
	ESNumber GetThreshold();
	//
	// モノクロ閾値を設定する
	//
	ESErrorCode SetThreshold( ESNumber nThreshold );

	//
	// オートフォーカスを要求する。
	//
	ESErrorCode DoAutoFocus( ESFloat* pfOutFocus );
	//
	// フォーカスを取得する(mm)
	//
	ESFloat GetFocus();
	ESFloat GetMaxFocus();
	ESFloat GetMinFocus();
	//
	// フォーカスを設定する(mm)
	//
	ESErrorCode SetFocus( ESFloat fFocus );

	//
	// RequestScanningParameterにセットするパラメーターを作成する
	//
	virtual ESErrorCode CreateScanningParametersParam( __out ST_ESCI_SCANNING_PARAMETER& stOutScanningParameter ) override;

	//
	// RequestScanningParameterを呼び出します
	//
	ESErrorCode SetScanningParameters();

	//
	// サポートするDigital ICE設定を取得する
	//
	ESIndexSet GetSupportedDigitalICEs();
	//
	// Digital ICEの設定を取得する
	//
	ESDigitalICE GetDigitalICE();
	//
	// Digital ICEの種類を設定する(ESDigitalICE)
	//
	ESErrorCode SetDigitalICE( ESNumber nDigitalICE );

	//
	// 静音モードをサポートしているか?
	//
	bool IsQuietModeSupported();
	//
	// サポートする静音モードを取得する
	//
	ESIndexSet GetSupportedQuietModes();
	//
	// 静音モードを取得する
	//
	ESQuietMode GetQuietMode();
	//
	// 静音モードを設定する(ESQuietMode)
	//
	ESErrorCode SetQuietMode( ESNumber nQuietMode);

	//
	//
	//
	ESScanningMode GetScanningMode();
	//
	//
	//
	ESErrorCode SetScanningMode( ESNumber nScanningMode);

	//
	//
	//
	bool IsDocumentSizeDetectionSupported();
	//
	// 検知するドキュメントサイズを取得する
	//
	ST_ES_SIZE_F GetDetectedDocumentSize();

	//
	// フィーダー(ADF)機能をサポートしているか?
	//
	bool IsFeederSupported();
	//
	// フィーダー(ADF)機能が有効か？
	//
	virtual bool IsFeederEnabled() override;

	//
	// オートスキャン機能をサポートしているか?
	//
	bool IsAutoScanSupported();
	//
	// オートスキャン機能が有効か?
	//
	bool IsAutoScanEnabled();
	//
	// オートスキャン機能の有効状態を設定する
	//
	void SetAutoScanEnabled( bool bAutoScanEnabled );

	//
	//
	//
	bool IsPageFeedTypeDocumentFeeder();

	//
	// 両面同時読み取りのタイプを取得する
	//
	ESDuplexType GetDuplexType();
	//
	// 両面同時読み取りをサポートしているか?
	//
	bool IsDuplexSupported();
	//
	// 両面同時読み取り機能が有効か?
	//
	bool IsDuplexEnabled();
	//
	// 両面同時読み取り機能の有効状態を設定する
	//
	ESErrorCode SetDuplexEnabled( bool bEnabled );

	//
	// 重送検知機能をサポートしているか?
	//
	bool IsDoubleFeedDetectionSupported();
	//
	// サポートしている重送検知を取得する
	//
	ESIndexSet GetSupportedDoubleFeedDetections();
	//
	// 重送検知の設定を取得する
	//
	ESDoubleFeedDetection GetDoubleFeedDetection();
	//
	// 重送検知を設定を変更(ESDoubleFeedDetection)
	//
	ESErrorCode SetDoubleFeedDetection( ESNumber nDoubleFeedDetection );

	//
	//
	//
	bool IsPaperEndDetectionSupported();
	//
	//
	//
	virtual bool IsPaperEndDetectionEnabled() override;
	//
	//
	//
	ESErrorCode SetPaperEndDetectionEnabled( bool bPperEndDetectionEnabled );

	//
	//
	//
	bool IsOverScanEnabled();
	//
	//
	//
	ESErrorCode SetOverScanEnabled( bool bOverScanEnabled );

	//
	// ドキュメントが読み込まれたか？
	//
	bool IsDocumentLoaded();
	
	//
	// スキャンするページ枚数を取得する
	//
	ESNumber GetPagesToBeScanned();
	//
	// スキャンするページ枚数を設定する
	//
	ESErrorCode SetPagesToBeScanned( ESNumber nPagesToBeScanned );
	
	//
	//
	//
	bool IsPrefeed();

	//
	// TPU(透過読み取り)をサポートしているか？
	//
	bool IsTransparencyUnitSupported();

	//
	// TPUエリアガイドをサポートしているか？
	//
	bool IsTransparencyUnit2ndAreaSupported();

	//
	// 赤外線ランプをサポートしているか?
	//
	bool IsInfraRedLampSupported();

	//
	//
	//
	ESIndexSet GetSupportedFilmTypes();
	//
	//
	//
	ESFilmType GetFilmType();
	//
	// (ESFilmType)
	//
	ESErrorCode SetFilmType( ESNumber nFilmType );

	//
	// フラットベッドをサポートしているか？
	//
	bool IsFlatbedSupported();

	//
	// ランプ切り替え機能をサポートしているか?
	//
	bool IsLampChangeSupported();
	//
	// サポートするランプ切り替え機能を取得する
	//
	virtual ESIndexSet GetSupportedLampModes();
	//
	// 現在のランプ切り替えモードを取得する
	//
	ESLampMode GetLampMode();
	//
	// ランプ切り替えモードを設定する(ESLampMode)
	//
	ESErrorCode SetLampMode( ESNumber nLampMode );

	//
	// クリーニング機能をサポートするか
	//
	bool IsCleaningSupported();
	//
	bool IsCalibrationSupported();

	//
	// 光量補正をサポートするか
	//
	ESAny GetSupportedLightIntensitiesFB();
	ESNumber GetLightIntensityFB();
	ESErrorCode SetLightIntensityFB(ESNumber nLightIntensity);

	//
	// Maintenance
	//--------------------------------------------
protected:
	// スキャナセンサーのクリーニング動作を要求する。
	ESErrorCode DoCleaning();
	// スキャナセンサーのキャリブレーションを要求する。
	ESErrorCode DoCalibration();

	ESNumber GetLamp1Counter();
	ESErrorCode SetLamp1Counter(ESNumber nLamp1Counter);

	ESNumber GetLamp2Counter();
	ESErrorCode SetLamp2Counter(ESNumber nLamp2Counter);

	// Document Feeder
	ESNumber GetSimplexScanCounter();
	ESErrorCode SetSimplexScanCounter(ESNumber nSimplexScanCounter);

	ESNumber GetDuplexScanCounter();
	ESErrorCode SetDuplexScanCounter(ESNumber nDuplexScanCounter);

	ESNumber GetPickupRollerCounter();
	ESErrorCode SetPickupRollerCounter(ESNumber nPickupRollerCounter);

	// Flatbed
	ESNumber GetScanCounter();
	ESErrorCode SetScanCounter(ESNumber nScanCounter);


private:
	bool						m_bCaptureCommandSupport;
	bool						m_bShouldIgnoreCancelFromScanner;

	bool						m_bCaptured;

	ESNumber					m_nBufferSize;

	ESJobMode			m_eJobMode;
	bool						m_bIsDisableJobContinue;

	ST_ESCI_IDENTITY			m_stIdentity;	// ESC I command data
	ST_ESCI_EXTENDED_IDENTITY	m_stExtIdentity;
	ST_ESCI_HW_PROPERTY			m_stHwProperty;
	ST_ESCI_SCANNER_STATUS		m_stScannerStatus;

	ST_ESCI_SCANNING_PARAMETER	m_stParameters;

	ST_ES_SIZE_F				m_sizeMaxScanSizeInLongLength;

	//ESIndexSet				m_arSupportedResolutions;
	ESAny						m_anySupportedResolutions; // ESIndexSet or ST_ES_RANGE
	
	ESDigitalICE			m_eDigitalICE;					
	ESGuidePosition		m_eGuidePositionFB;
	ESGuidePosition		m_eGuidePositionADF;

	ESString					m_strAuthUsername;
	ESString					m_strAuthPassword;
	bool						m_bIsAuthenticationEnabled;

	ESNumber					m_nPagesToBeScanned;
	bool						m_bPaperEndDetectionEnabled;
	bool						m_bOverScanEnabled;

	std::recursive_mutex		m_event_mtx;
	std::unique_ptr<event_caller> m_interruptCheckTimer;

	CESColorMatrix				m_cColorMatrix;

	ESFloat						m_fXOffsetMargin;
	ESFloat						m_fYOffsetMargin;

	ESIndexArray				m_arGammaTableMono;
	ESIndexArray				m_arGammaTableRed;
	ESIndexArray				m_arGammaTableGreen;
	ESIndexArray				m_arGammaTableBlue;
	
	bool						m_bIsShouldObserveButtonWithCommand;
};

