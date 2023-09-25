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

#include "ESCI2Command.h"
#include "Utils/event_caller.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CESCIAccessor
//!
//! @bref      ESCIコマンドアクセス クラス
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CESCI2Accessor : public CESCI2Command
{
public:
	CESCI2Accessor();
	virtual ~CESCI2Accessor();

	// 初期化処理
	virtual ESErrorCode Initialize() override;

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

	// スキャナーパネルのプッシュスキャン可能状態を設定する
	ESErrorCode SetPanelToPushScanReady( BOOL bPushScanReady );


	ESErrorCode UnlockAdministratorLock();
	ESErrorCode LockAdministratorLock();
	bool	RequestingUnlockAdminLock();


	BOOL IsScannableDeviceConfig();
	
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

	//
	// スキャン操作
	//---------------------------------------
protected:
	ESErrorCode ParametersForScan( ESDictionaryA& dicParameter );

	// エラー後の継続スキャンをおこなう対象のエラーコード群を返す。
	ESIndexSet ErrorsForInterruption();

	// ページカウントなどの前回スキャン情報を保持した状態でスキャンを開始する。
	ESErrorCode ScanInContext();
	ESErrorCode TransferImage();

	// 現在のスキャン設定でスキャンを開始する。
	ESErrorCode Scan();

	// AFMの中で1回のスキャンをおこなう。通常のスキャンとの違いとして、ページカウンターのリセットと終了通知が行われない。
	ESErrorCode ScanForAFM();
	ESErrorCode ScanForAFMC();

	static void* DoScanForAFMInBackground(void* pParam);
	static void* DoScanForAFMCInBackground(void *pParam);
	static void* DoStopScanningInAutoFeedingModeInBackground(void *pParam);

	ESErrorCode ScanForAFMInBackground();
	ESErrorCode ScanForAFMCInBackground();

	ESErrorCode ProcessPageStartInfo( ESDictionaryA& dicInfo, ESAnyArray* pStrDocumentType, ESStringA& strSurface );
	ESErrorCode ProcessImageDataBlock( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDataBuffer, ESStringA& strSurface );
	ESErrorCode ProcessPageEndInfo( ESDictionaryA& dicInfo, ESStringA& strSurface, bool bIsDoubleFeed  ,ESAnyArray* pStrDocumentTypes);

	ESErrorCode StopScanningInAutoFeedingMode();
	ESErrorCode StopScanningInAutoFeedingModeInBackground();
	ESErrorCode StartAFM();
	ESErrorCode StopAFM();
	ESErrorCode StartAFMC();
	ESErrorCode StopAFMC();

	ESErrorCode StartScanningInAFM();
	ESErrorCode StopScanningInAFM();
	ESErrorCode StartScanningInAFMC();
	ESErrorCode StopScanningInAFMC();

	ESErrorCode ScheduleAutoFeedingModeTimeout();
	ESErrorCode InvalidateAutoFeedingModeTimeout();

	// スキャン処理をキャンセルする。
	ESErrorCode Cancel();

	ESErrorCode Abort();

	void DisposeImageHandles();
	void AbortImageHandles();

	//
	// Notification
	//-------------------------------------------------
	void NotifyBeginContinuousScanning();
	void NotifyEndContinuousScanning();
	void NotifyCompleteScanningWithError( ESErrorCode err );
	void NotifyInterruptScanningWithError( ESErrorCode err );
	void NotifyWillScanToScannedImage( IESScannedImage* pImage );
	void NotifyDidScanToScannedImage( IESScannedImage* pImage );

	virtual ESErrorCode CallDelegateScannerDidPressButton( UInt8 un8ButtonNumber ) override;
	virtual ESErrorCode CallDelegateNetworkScannerDidRequestStartScanning() override;
	virtual ESErrorCode CallDelegateScannerDidRequestStop() override;
	virtual void CALLBACK DidRequestStopScanning() override;
	virtual void CALLBACK DidDisconnect() override;
	virtual void CALLBACK DeviceCommunicationError(ESErrorCode err) override;
	virtual void CALLBACK DidRequestGetImageData() override;

	void StartButtonChecking();
	void StopButtonChecking();
	//
	// スキャナ情報取得
	//----------------------------------------------------------
protected:
	// 初期化
	ESErrorCode Setup();

	// スキャナにスキャナの情報（'INFO'）を要求し、インスタンス変数に格納する。
	ESErrorCode GetInfo();

	ESErrorCode GetExtInfo();

	// スキャナにスキャナのメンテナンス情報（FS Z + 'INFO'）を要求し、インスタンス変数に格納する。
	ESErrorCode GetMaintenanceInfo();

	ESErrorCode GetMaintenanceStatus();

	// スキャナにスキャナのCapability（'CAPA'）を要求し、インスタンス変数に格納する。
	ESErrorCode GetCapabilities();

	// スキャナにスキャナのメンテナンスCapability（FS Z + 'CAPA'）を要求し、インスタンス変数に格納する。
	ESErrorCode GetMaintenanceCapabilities();

	// スキャナステータス（'STAT'）を要求し、インスタンス変数に格納する。
	ESErrorCode GetStatus();

	// スキャナに現在の全スキャン設定（'RESA'）を要求し、インスタンス変数に格納する。
	ESErrorCode GetResults();

	// 現在設定されている内部スキャン設定をクリアする。
	ESErrorCode ResetParametersForKeys( ESStringArrayA* parKeys );

	// 設定をリセットする
	ESErrorCode Reset();

	// フォーカスが設定されるまで待つ
	ESErrorCode WaitUntilSetForcus(ESFloat* pfOutFocus);


	//
	// 取得スキャナデーターから値を取得
	//----------------------------------------------------------

	//
	// バージョン取得
	//
	virtual ESString GetVersion();

	//
	// エラーステータスを取得する。(ESErrorStatus)
	//
	ESErrorCode GetErrorStatus();

	bool IsErrorCodeSupported();
	ESNumber GetErrorCode();

	//
	// バッテリーステータスを取得する。(ESBatteryStatus)
	//
	ESBatteryStatus GetBatteryStatus();

	ESWarningStatus GetWarningStatus();

	bool IsExtInformationSupported();

	//
	// 認証機能をサポートしているか？
	//
	bool IsAuthenticationSupported() override;

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


	virtual bool IsAdminLockSupported()override;

	virtual bool IsAdminLockEnabled()override;
	virtual bool IsAdminLockEnabledWithUnlockTest(bool bUnlockTest)override;

	virtual ESString GetAdminLockPassword()override;
	virtual ESErrorCode SetAdminLockPassword(ESString strPassword)override;

	//
	// プロダクト名を取得する
	//
	virtual ESString GetProductName() override;

	//
	// シリアルナンバーを文字列で取得する
	//
	ESString GetSerialNumber();

	//
	//
	//
	bool IsInterruptionEnabled();
	ESErrorCode SetInterruptionEnabled( bool bIsInterruptionEnabled );

	//
	//
	//
	bool IsLengthPaddingSupported();

	// ADF Paper-Guide
	bool IsGuidelessADF();

	//
	// AFM
	//
	bool IsAutoFeedingModeSupported();
	bool IsLegacyAFMSupported();
	bool IsContinuousAutoFeedingModeSupported();

	ESNumber GetAutoFeedingModeTimeout();

	ESErrorCode SetAutoFeedingModeTimeout( ESNumber nAutoFeedingModeTimeout );

	//
	// サポートする給紙タイプを取得する
	//
	ESIndexSet GetSupportedFunctionalUnitTypes();
	//
	// 給紙タイプを取得する
	//
	virtual ESFunctionalUnitType GetFunctionalUnitType() override;
	//
	// 給紙タイプを設定する (ESFunctionalUnitType)
	//
	ESErrorCode SetFunctionalUnitType( ESNumber nFunctionalUnitType );

	//
	// サポートする解像度を取得する
	//
	ESAny GetFunctionalUnitSpecificResolutions();
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
	//
	//
	ST_ES_SIZE_F GetMaxImagePixels();

	//
	// 最大スキャンサイズを取得する(インチ）
	//
	ST_ES_SIZE_F GetMaxScanSize();

	bool IsMaxScanSizeInNormalSpeedSupported();
	ST_ES_SIZE_F GetMaxScanSizeInNormalSpeed();

	//
	//
	//
	ST_ES_SIZE_F GetMaxScanSizeInLongLength();
	ST_ES_SIZE_F GetMaxScanSizeInLongLengthWithResolution( ESNumber nResolution );

	//
	// 最小スキャンサイズを取得する(インチ）
	//
	ST_ES_SIZE_F GetMinScanSize();

	ESDicArray GetMaxLongLengthTable();

	//
	// スキャンするエリアを設定する(インチ）
	//
	ESErrorCode SetScanArea( ST_ES_RECT_F rcArea, bool bShouldAlign );
	//
	// スキャンするエリアを取得する(インチ）
	//
	ST_ES_RECT_F GetScanArea();
	//
	// スキャンするエリアを設定する(インチ）
	//
	ESErrorCode SetScanArea( ST_ES_RECT_F rcScanArea );
	
	//
	// スキャンするエリアをピクセル単位で取得する
	//
	ST_ES_RECT_UN32 GetScanAreaInPixel() override;
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
	// AFM
	// 
	bool IsAfmEnabled() const;
	ESErrorCode SetAfmEnabled( bool bEnable );

	//
	// Job
	//
	BOOL IsPushScanReadySupported();
	ESIndexSet GetSupportedJobModes();

	ESJobMode GetJobMode();
	ESErrorCode SetJobMode( ESJobMode eJobMode );

	bool IsDisableJobContinue();
	ESErrorCode SetDisableJobContinue( bool bDisableJobContinue );

	bool IsDisableKeepingCapture();
	ESErrorCode SetDisableKeepingCapture( bool bDisableKeepingCapture );
	
	//
	// スキャンエリア設定のガイド位置を取得する
	//
	ESGuidePosition GetGuidePosition();

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

	ESIndexSet GetSupportedColorCounterTypes();
	ESNumber GetColorCounterType();
	ESErrorCode SetColorCounterType( ESNumber nColorCounterType );


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
	// サポートする画像フォーマットタイプを取得する
	//
	ESIndexSet GetSupportedImageFormats();
	//
	// 画像のフォーマットタイプを取得する
	//
	ESImageFormat GetImageFormat();
	//
	// 画像のフォーマットタイプを指定する(ESImageFormat)
	//
	ESErrorCode SetImageFormat( ESNumber nImageFormat );

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
	// サポートするガンマチャネルを取得する
	//
	ESIndexSet GetSupportedGammaChannels();
	//
	// ガンマテーブルを取得する
	//
	ESAny GetGammaTableForChannel( ESGammaChannel eChannel );
	ESAny GetGammaTableMono ();
	ESAny GetGammaTableRed  ();
	ESAny GetGammaTableGreen();
	ESAny GetGammaTableBlue ();
	//
	// ガンマテーブルを設定する
	//
	ESErrorCode SetGammaTable( ESIndexArray arGammaTable, ESGammaChannel eChannel );
	ESErrorCode SetGammaTableMono ( ESIndexArray arGammaTable );
	ESErrorCode SetGammaTableRed  ( ESIndexArray arGammaTable );
	ESErrorCode SetGammaTableGreen( ESIndexArray arGammaTable );
	ESErrorCode SetGammaTableBlue ( ESIndexArray arGammaTable );

	//
	// サポートするカラーマトリクスモードを取得する
	//
	ESIndexSet GetSupportedColorMatrixModes();
	//
	// カラーマトリクスモードを取得する
	//
	ESColorMatrixMode GetColorMatrixMode();
	//
	// カラーマトリクスモードを設定する(ESColorMatrixMode)
	//
	ESErrorCode SetColorMatrixMode( ESNumber nColorMatrixMode );
	//
	// カラーマトリックスを取得する(ESFloatArray or nullptr)
	//
	ESAny GetColorMatrix();
	//
	// カラーマトリックスを設定する
	//
	ESErrorCode _SetColorMatrix( ES_COLOR_MATRIX& matrix );
	ESErrorCode SetColorMatrix( ESFloatArray arColorMatrix );

	//
	// 傾き補正をサポートするか
	//
	bool IsSkewCorrectionSupported();
	//
	// 傾き補正が有効か？
	//
	bool IsSkewCorrectionEnabled();
	//
	// 傾き補正を設定します
	//
	ESErrorCode SetSkewCorrectionEnabled( bool bSkewCorrectionEnabled );

	//
	// サポートするモノクロ閾値を取得する
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
    
    ESAny GetSupportedSharpnessLevel();
    ESNumber GetSharpnessLevel();
    ESErrorCode SetSharpnessLevel( ESNumber nSharpnessLevel );

    ESAny GetSupportedContrast();
    ESNumber GetContrast();
    ESErrorCode SetContrast( ESNumber nContrast );

    ESAny GetSupportedBrightness();
    ESNumber GetBrightness();
    ESErrorCode SetBrightness( ESNumber nBrightness );

    ESAny GetSupportedGammaScale();
    ESNumber GetGammaScale();
    ESErrorCode SetGammaScale( ESNumber nGammaScale );

	virtual bool IsDetectBlankPageSupported();
	virtual bool IsDetectBlankPageEnabled();
	virtual ESErrorCode SetDetectBlankPageEnabled( bool nDetectBlankPageLevel );

	virtual ESAny GetSupportedDetectBlankPageLevel();
	virtual ESNumber GetDetectBlankPageLevel();
	virtual ESErrorCode SetDetectBlankPageLevel( ESNumber nDetectBlankPageLevel );

	virtual bool IsDetectColorTypeSupported();
	virtual bool IsDetectColorTypeEnabled();
	virtual ESErrorCode SetDetectColorTypeEnabled( bool nDetectBlankPageLevel );

	virtual bool IsSkipImageSupported();
	virtual bool IsSkipImageEnabled();
	virtual ESErrorCode SetSkipImageEnabled( bool nDetectBlankPageLevel );


    ESAny GetSupportedBackGroundRemoval();
    ESNumber GetBackGroundRemoval();
    ESErrorCode SetBackGroundRemoval( ESNumber nBackGroundRemoval );
    
	//
	// オートフォーカスを要求する。
	//
	ESErrorCode DoAutoFocus(ESFloat* pfOutFocus);

	//
	// フォーカスを取得する(mm)
	//
	bool IsAutoFocusSupported();
	bool IsManualFocusSupported();
	ESFloat GetFocus();
	ESFloat GetMaxFocus();
	ESFloat GetMinFocus();

	//
	// フォーカスを設定する(mm)
	//
	ESErrorCode SetFocus(ESFloat fFocus);


	//
	// サポートするJpegのQuality値を取得する
	//
	ESAny GetSupportedJPEGQuality();
	//
	// JpegのQuality値を取得する
	//
	ESNumber GetJPEGQuality();
	//
	// JpegのQuality値を設定する
	//
	ESErrorCode SetJPEGQuality( ESNumber nJPEGQuality );

	//
	// サポートする静音モードを取得する
	//
	ESIndexSet GetSupportedQuietModes();
	//
	// 静音モードを取得する(ESQuietMode)
	//
	ESQuietMode GetQuietMode();
	//
	// 静音モードを設定する(ESQuietMode)
	//
	ESErrorCode SetQuietMode( ESNumber nQuietMode);

	//
	//
	//
	bool IsAutoCroppingSupported();
	bool IsAutoCroppingEnabled();
	ESErrorCode SetAutoCroppingEnabled( bool bAutoCroppingEnabled );

	//
	//
	//
	bool IsAutoCroppingInscribedSupported();
	bool IsAutoCroppingInscribedEnabled();
	ESErrorCode SetAutoCroppingInscribedEnabled( bool bAutoCroppingEnabled );

	//
	// HW自動切り出し・傾き補正の適用可能な解像度
	//
	ESAny GetCroppableResolutions();

	// 
	//
	//
	ESAny GetSupportedCroppingSizes();
	ESFloat GetCroppingSize();
	ESErrorCode SetCroppingSize( ESFloat fCroppingSize );

	//
	// バッファサイズ取得
	//
	ESNumber GetBufferSize();
	//
	// バッファサイズ設定
	//
	ESErrorCode SetBufferSize( ESNumber nBufferSize );
	//
	// デバイスの最大バッファサイズ取得
	//
	ESNumber GetDeviceMaxBufferSize() override;


	ESAny GetSupportedFilmType();
	ESNumber GetFilmType();
	ESErrorCode SetFilmType(ESNumber nFilmType);

	//
	//
	//
	bool IsDocumentSizeDetectionSupported();
	//
	// 検知するドキュメントサイズを取得する
	//
	ST_ES_SIZE_F GetDetectedDocumentSize();

	//
	//
	//
	ESIndexSet GetSupportedLaminatedPaperModes();
	ESLaminatedPaperMode GetLaminatedPaperMode();
	ESErrorCode SetLaminatedPaperMode( ESNumber nLaminatedPaperMode );

	//
	// フィーダー(ADF)機能をサポートしているか?
	//
	bool IsFeederSupported();
	//
	// フィーダー(ADF)機能が有効か？
	//
	bool IsFeederEnabled();

	//
	// オートスキャン機能をサポートしているか?
	//
	bool IsAutoScanSupported();
	
	bool IsFeedTypeADF() override;

    bool IsAdfLoadSupported() override;
    

	bool IsSupportedScanningMode();
	ESNumber GetScanningMode();
	ESErrorCode SetScanningMode( ESNumber nScanningMode);


	//
	//
	//
	bool IsCarrierSheetDetectionSupported();

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
	bool IsDuplexEnabled() override;
	//
	// 両面同時読み取り機能の有効状態を設定する
	//
	ESErrorCode SetDuplexEnabled( bool bEnabled );

	//
	//
	//
	bool IsPaperEndDetectionSupported();
	bool IsPaperEndDetectionEnabled();
	ESErrorCode SetPaperEndDetectionEnabled( bool bPaperEndDetectionEnabled );

	//
	//
	//
	bool IsOverScanSupported();
	bool IsOverScanEnabled();
	ESErrorCode SetOverScanEnabled( bool bOverScanEnabled );


	bool IsOverScanForCroppingSupported();
	bool IsOverScanForCroppingEnabled();
	ESErrorCode SetOverScanForCroppingEnabled( bool bOverScanEnabled );

	//
	//
	//
	bool IsPassportCarrierSheetSupported();
	bool IsPassportCarrierSheetEnabled();
	ESErrorCode SetPassportCarrierSheetEnabled(bool bEnabled);

	//
	// サポートする重送検知機能を取得する
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
	ESIndexSet GetSupportedBGColors();
	ESBGColor GetBGColor();
	ESErrorCode SetBGColor(ESNumber nBGColor);

	//
	//
	//
	bool IsDoubleFeedDetectionRangeSupported();
	ESFloat GetMinDoubleFeedDetectionRangeLength();
	ESFloat GetDoubleFeedDetectionRangeOffset();
	ESErrorCode SetDoubleFeedDetectionRangeOffset( ESFloat fDoubleFeedDetectionRangeOffset );
	ESFloat GetDoubleFeedDetectionRangeLength();
	ESErrorCode SetDoubleFeedDetectionRangeLength( ESFloat fDoubleFeedDetectionRangeLength );

	//
	//
	//
	bool IsLengthDoubleFeedDetectionSupported();
	bool IsLengthDoubleFeedDetectionEnabled();
	ESErrorCode SetLengthDoubleFeedDetectionEnabled( bool bLengthDoubleFeedDetectionEnabled );

	//
	//
	//
	ESFloat GetMaxLengthDoubleFeedDetectionLength();

	//
	//
	//
	ESFloat GetLengthDoubleFeedDetectionLength();
	ESErrorCode SetLengthDoubleFeedDetectionLength( ESFloat fLengthDoubleFeedDetectionLength);

	bool IsImageDoubleFeedDetectionSupported();
	bool IsImageDoubleFeedDetectionEnabled();
	ESErrorCode SetImageDoubleFeedDetectionEnabled( bool bImageDoubleFeedDetectionEnabled );

	bool IsImagePaperProtectionSupported();
	bool IsImagePaperProtectionEnabled();
	ESErrorCode SetImagePaperProtectionEnabled( bool bImagePaperProtectionEnabled );

	//
	//
	//
	ESStringA GetDocumentSeparationSTAT();
	//
	//ドキュメントセパレーションをサポートするか？
	//
	//
	bool IsDocumentSeparationSupported();
	//
	//ドキュメントセパレーションが有効か?
	//
	bool IsDocumentSeparationEnabled();
	ESNumber GetDocumentSeparation();
	
	//
	// Unavailable Scan Parameter information.
	//
	ESDictionaryA GetUnavailableScanParameterSTAT();
	bool IsUnavailableScanParameterStatusSupported();
	ESIndexArray GetUnavailableScanParameterStatus();
	
	//
	//
	//
	ESStringA GetScannerPositionSTAT();
	bool IsScannerPositionStatusSupported();
	ESNumber GetScannerPositionStatus();

	//
	// カードレバー状態問い合わせ
	//
	ESStringA GetCardScanningSTAT();
	bool IsCardScanningSupported();
	bool IsCardScanningEnabled();

	//
	//
	//
	ESStringA GetSensorGlassSTAT();
	bool IsSensorGlassStatusSupported();
	ESNumber GetSensorGlassStatus();

	//
	// ドキュメントが読み込まれたか？
	//
	bool IsDocumentLoaded();

	//
    //  クライアントアプリケーションの種類を設定
    //
    ESNumber GetClientApplication();
    ESErrorCode SetClientApplication( ESNumber nClientApplication);
    ESIndexSet GetSupportedClientApplications();

	//
	//
	//
	ESIndexSet GetSupportedEdgeFillColors();
	ESEdgeFillColor GetEdgeFillColor();
	//
	// (ESEdgeFillColor)
	//
	ESErrorCode SetEdgeFillColor( ESNumber nEdgeFillColor );

	//
	//
	//
	ESAny GetSupportedEdgeFillWidth();	// ESFloatArray or ES_RANGE_F

	ESFloat GetEdgeFillWidthLeft();
	ESErrorCode SetEdgeFillWidthLeft( ESFloat fEdgeFillWidthLeft );
	ESFloat GetEdgeFillWidthRight();
	ESErrorCode SetEdgeFillWidthRight( ESFloat fEdgeFillWidthRight );
	ESFloat GetEdgeFillWidthTop();
	ESErrorCode SetEdgeFillWidthTop( ESFloat fEdgeFillWidthTop );
	ESFloat GetEdgeFillWidthBottom();
	ESErrorCode SetEdgeFillWidthBottom( ESFloat fEdgeFillWidthBottom );

	//
	//	クリーニング機能をサポートするか
	//
	bool IsCleaningSupported();

	//
	//
	//
	bool IsCalibrationSupported();

	//
	// スキャンするページ枚数を取得する
	//
	ESNumber GetPagesToBeScanned();
	//
	// スキャンするページ枚数を設定する
	//
	ESErrorCode SetPagesToBeScanned( ESNumber nPagesToBeScanned );

	//
	// Captureコマンドをサポートしているか？
	//
	bool IsCaptureCommandSupported();
	//
	// Captureコマンドをサポートしているか設定する
	//
	ESErrorCode SetCaptureCommandSupported( bool bSupported );

	//
	// 現在のスキャン設定が、圧縮イメージ（JPEGなど）転送有効であるかどうかを返す。モノクロ設定であれば、JPEGは無効となる。
	//
	bool IsCompressed();

	//
	// Maintenance
	//-------------------------------------------------
protected:
	ESAny GetMaintenanceResultForKey( ESStringA strKey );

	// スキャナセンサーのクリーニング動作を要求する。
	ESErrorCode DoCleaning();
	// スキャナセンサーのキャリブレーションを要求する。
	ESErrorCode DoCalibration();

	//
	bool IsPowerSaveSupported();
	ESAny GetSupportedPowerSaveTimes();
	ESNumber GetPowerSaveTime();
	ESErrorCode SetPowerSaveTime( ESNumber nPowerSaveTime );

	//
	ESAny GetSupportedPowerOffTimes();
	ESNumber GetPowerOffTime();
	ESErrorCode SetPowerOffTime( ESNumber nPowerOffTime );

	ESAny GetSupportedPowerOffTimes2nd();
	ESNumber GetPowerOffTime2nd();
	ESErrorCode SetPowerOffTime2nd( ESNumber nPowerOffTime );

	ESAny GetSupportedFirstPCConnectionDates();
	ESString GetFirstPCConnectionDate();     // "%Y-%m-%d %H:%M"
	ESErrorCode SetFirstPCConnectionDate( ESString strFirstPCConnectionDate );

	ESIndexSet GetSupportedSensorGlassDirtSensitivities();
	ESNumber GetSensorGlassDirtSensitivity();
	ESErrorCode SetSensorGlassDirtSensitivity( ESNumber nGlassDirtSensitivity );

	ESAny GetSupportedSimplexScanCounters();
	ESNumber GetSimplexScanCounter();
	ESErrorCode SetSimplexScanCounter( ESNumber nSimplexScanCounter );

	ESAny GetSupportedDuplexScanCounters();
	ESNumber GetDuplexScanCounter();
	ESErrorCode SetDuplexScanCounter( ESNumber nDuplexScanCounter );

	ESAny GetSupportedSimplexCardScanCounters();
	ESNumber GetSimplexCardScanCounter();
	ESErrorCode SetSimplexCardScanCounter( ESNumber nSimplexCardScanCounter );

	ESAny GetSupportedDuplexCardScanCounters();
	ESNumber GetDuplexCardScanCounter();
	ESErrorCode SetDuplexCardScanCounter( ESNumber nDuplexCardScanCounter );

	ESAny GetSupportedDocumentFeederLamp1Counters();
	ESNumber GetDocumentFeederLamp1Counter();
	ESErrorCode SetDocumentFeederLamp1Counter( ESNumber nDocumentFeederLamp1Counter );

	ESAny GetSupportedDocumentFeederLamp2Counters();
	ESNumber GetDocumentFeederLamp2Counter();
	ESErrorCode SetDocumentFeederLamp2Counter( ESNumber nDocumentFeederLamp2Counter );

	ESAny GetSupportedPaperJamCounters();
	ESNumber GetPaperJamCounter();
	ESErrorCode SetPaperJamCounter( ESNumber nPaperJamCounter );

	ESAny GetSupportedUltraSonicDoubleFeedCounters();
	ESNumber GetUltraSonicDoubleFeedCounter();
	ESErrorCode SetUltraSonicDoubleFeedCounter( ESNumber nUltraSonicDoubleFeedCounter );

	ESAny GetSupportedLengthDoubleFeedCounters();
	ESNumber GetLengthDoubleFeedCounter();
	ESErrorCode SetLengthDoubleFeedCounter( ESNumber nLengthDoubleFeedCounter );

	ESAny GetSupportedImageDoubleFeedCounters();
	ESNumber GetImageDoubleFeedCounter();
	ESErrorCode SetImageDoubleFeedCounter( ESNumber nImageDoubleFeedCounter );

	ESAny GetSupportedPaperProtectionCounters();
	ESNumber GetPaperProtectionCounter();
	ESErrorCode SetPaperProtectionCounter(ESNumber nPaperProtectionCounter);

	ESAny GetSupportedRollerKitCounters();
	ESNumber GetRollerKitCounter();
	ESErrorCode SetRollerKitCounter( ESNumber nRollerKitCounter );

	ESAny GetSupportedRetardRollerCounters();
	ESNumber GetRetardRollerCounter();
	ESErrorCode SetRetardRollerCounter(ESNumber nRetardRollerCounter);

	ESAny GetSupportedPickupRollerCounters();
	ESNumber GetPickupRollerCounter();
	ESErrorCode SetPickupRollerCounter( ESNumber nPickupRollerCounter );

	ESAny GetSupportedSeparationPadCounters();
	ESNumber GetSeparationPadCounter();
	ESErrorCode SetSeparationPadCounter( ESNumber nSeparationPadCounter );

	ESAny GetSupportedSpecialDocumentCounters();
	ESNumber GetSpecialDocumentCounter();
	ESErrorCode SetSpecialDocumentCounter( ESNumber nSpecialDocumentCounter );

	ESAny GetSupportedPassportCarrierSheetCounters();
	ESNumber GetPassportCarrierSheetCounter();
	ESErrorCode SetPassportCarrierSheetCounter(ESNumber nCounter);

	ESAny GetSupportedScanCounters();
	ESNumber GetScanCounter();
	ESErrorCode SetScanCounter( ESNumber nScanCounter );

	ESAny GetSupportedFlatbedLamp1Counters();
	ESNumber GetFlatbedLamp1Counter();
	ESErrorCode SetFlatbedLamp1Counter( ESNumber nFlatbedLamp1Counter );

	ESAny GetSupportedFlatbedLamp2Counters();
	ESNumber GetFlatbedLamp2Counter();
	ESErrorCode SetFlatbedLamp2Counter( ESNumber nFlatbedLamp2Counter );

	ESAny GetSupportedDocumentTopCorrectionsFront();
	ESFloat GetDocumentTopCorrectionFront();
	ESErrorCode SetDocumentTopCorrectionFront( ESFloat fDocumentTopCorrectionFront );

	ESAny GetSupportedDocumentTopCorrectionsBack();
	ESFloat GetDocumentTopCorrectionBack();
	ESErrorCode SetDocumentTopCorrectionBack( ESFloat fDocumentTopCorrectionBack );

	ESAny GetSupportedDocumentDriveCorrectionsFront();
	ESFloat GetDocumentDriveCorrectionFront();
	ESErrorCode SetDocumentDriveCorrectionFront( ESFloat fDocumentDriveCorrectionFront );

	ESAny GetSupportedDocumentDriveCorrectionsBack();
	ESFloat GetDocumentDriveCorrectionBack();
	ESErrorCode SetDocumentDriveCorrectionBack( ESFloat fDocumentDriveCorrectionBack );

	ESAny GetSupportedLightIntensitiesBack();
	ESNumber GetLightIntensityBack();
	ESErrorCode SetLightIntensityBack( ESNumber nLightIntensityBack );

	bool IsDefaultPasswordTypeSupported();
	ESNumber GetDefaultPasswordType();

	ESAny GetSupportedDirectPowerOn();
	ESNumber GetDirectPowerOn();
	ESErrorCode SetDirectPowerOn(ESNumber nDirectPowerOn);

	ESAny GetSupportedNonConnectPowerOff();
	ESNumber GetNonConnectPowerOff();
	ESErrorCode SetNonConnectPowerOff(ESNumber nNonConnectPowerOff);

	ESAny GetSupportedADFPaperProtection();
	ESNumber GetADFPaperProtection();
	ESErrorCode SetADFPaperProtection(ESNumber nADFPaperProtection);

	ESAny GetSupportedBehaviorWhenDoubleFeed();
	ESNumber GetBehaviorWhenDoubleFeed();
	ESErrorCode SetBehaviorWhenDoubleFeed(ESNumber nBehaviorWhenDoubleFeed);

	ESAny GetSupportedCleaningWarningNotifyCount();
	ESNumber GetCleaningWarningNotifyCount();
	ESErrorCode SetCleaningWarningNotifyCount(ESNumber nCleaning);

	ESAny GetSupportedRollerKitNotifyCount();
	ESNumber GetRollerKitNotifyCount();
	ESErrorCode SetRollerKitNotifyCount(ESNumber nRKITNearEnd);

	ESAny GetSupportedRetardRollerNotifyCount();
	ESNumber GetRetardRollerNotifyCount();
	ESErrorCode SetRetardRollerNotifyCount(ESNumber nRTRLNearEnd);

	ESAny GetSupportedSeparationPadNotifyCount();
	ESNumber GetSeparationPadNotifyCount();
	ESErrorCode SetSeparationPadNotifyCount(ESNumber nSPADNearEnd);

	ESAny GetSupportedPickupRollerNotifyCount();
	ESNumber GetPickupRollerNotifyCount();
	ESErrorCode SetPickupRollerNotifyCount(ESNumber nPURLNearEnd);

	ESAny GetSupportedCleaningWarningCounter();
	ESNumber GetCleaningWarningCounter();
	ESErrorCode SetCleaningWarningCounter(ESNumber nCleaningWarningCounter);

	ESAny GetSupportedCleaningWarningNotify();
	ESNumber GetCleaningWarningNotify();
	ESErrorCode SetCleaningWarningNotify(ESNumber nCleaningWarningNotify);

	bool IsDocumentFeederBGLevelFrontSupported();
	ESDictionary GetDocumentFeederBGLevelFront();

	bool IsDocumentFeederBGLevelBackSupported();
	ESDictionary GetDocumentFeederBGLevelBack();

	bool IsPickupRollerLifeLimitSupported();
	ESNumber GetPickupRollerLifeLimit();

	bool IsSeparationPadLifeLimitSupported();
	ESNumber GetSeparationPadLifeLimit();

	bool IsRollerKitLifeLimitSupported();
	ESNumber GetRollerKitLifeLimit();

	bool IsRetardRollerLifeLimitSupported();
	ESNumber GetRetardRollerLifeLimit();

	bool IsPickupRollerNearendSupported();
	ESNumber GetPickupRollerNearend();

	bool IsSeparationPadNearendSupported();
	ESNumber GetSeparationPadNearend();

	bool IsRollerKitNearendSupported();
	ESNumber GetRollerKitNearend();

	bool IsRetardRollerNearendSupported();
	ESNumber GetRetardRollerNearend();

	ESAny GetSupportedCapability(const UInt32& key);

private :
	template <typename Function>
	void	ForeachAnyArrayValue(const ESAny &anyValue, Function func) {
		if (!anyValue.empty()) {
			const ESAnyArray* pAnyArray = SafeAnyDataCPtr< ESAnyArray >(anyValue);
			if (pAnyArray) {
				for (auto key = pAnyArray->begin(); key != pAnyArray->end(); key++) {
					const ESStringA* pValue = SafeAnyDataCPtr< ESStringA >(*key);
					if (pValue) {
						func(FourCharCode(*pValue));
					}
				}
			}
		}
	};

	template <typename Function>
	void	StringToNumberFromAnyValue(const ESAny &anyValue, Function func) {
		if (!anyValue.empty()) {
			const ESStringA* pValue = SafeAnyDataCPtr< ESStringA >(anyValue);
			if (pValue) {
				func(FourCharCode(*pValue));
			}
		}
	};

	ESAny GetSupportedIntegerKeyValueToADFTree(const UInt32 &key);
	ESNumber GetIntegerKeyValueToADFTree(const UInt32 &key);
	ESErrorCode SetIntegerKeyValueToADFTree(const UInt32 &key, ESNumber nValue);

	BOOL	IncludedInDocumentType(ESAnyArray* pStrDocumentTypes, UInt32 checkType);
	
protected:
	ESFunctionalUnitType		m_eFunctionalUnitType;
	ESColorMatrixMode			m_eColorMatrixMode;
	bool								m_bCaptureCommandSupport;

	ESJobMode					m_eJobMode;
	bool								m_bIsDisableJobContinue;
	bool								m_bIsDisableKeepingCapture;

	ESString							m_strAuthUsername;
	ESString							m_strAuthPassword;
	bool								m_bIsAuthenticationEnabled;

	bool								m_bIsAdminLockUnlocked;
	ESString							m_strAdminLockPassword;

	ESDictionaryA						m_dicInformation;
	ESDictionaryA						m_dicMaintenanceInformation;
	ESDictionaryA						m_dicMaintenanceStatus;

	ESDictionaryA						m_dicExtInformation;

	ESDictionaryA						m_dicCapabilities;
	ESDictionaryA						m_dicMaintenanceCapabilities;

	ESDictionaryA						m_dicStatus;

	ESDictionaryA						m_dicResult;

	ESDictionaryA						m_dicParameters;
	ESDictionaryA						m_dicMaintenanceParameters;

	ESDictionaryA						m_dicProcessingImagePtr;
	ESDictionaryA						m_dicClosableImagePtr;
	ESDictionaryA						m_dicPageCounts;

	std::recursive_mutex				m_event_mtx;
	std::unique_ptr<event_caller> 		m_interruptCheckTimer;

	ESFloat								m_fXOffsetMargin;
	ESFloat								m_fYOffsetMargin;

	bool								m_bIsAfmEnabled;
	ESNumber							m_nAfmTimeout_s; // Sec
	std::unique_ptr<event_caller> 		m_afmCheckTimer;



	bool								m_bIsInterrutionEnabled;

	bool								m_bIsPushScanReady;
};

