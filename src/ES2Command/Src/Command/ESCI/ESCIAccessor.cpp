////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCIAccessor.cpp
//!
//! @brif     ESCIコマンドアクセス クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CommonUtility/utils/stringutils.h"
#include "CommonUtility/utils/ESAnyCastUtils.h"

#include "ESCIAccessor.h"
#include "Image/ESScannedImage.h"
#include "../ESScannerDefs.h"
#include "../ModelInfo.h"

#include "Utils/Endian.h"
#include "Utils/ESUtils.h"


typedef enum {
	kIDMainStatusPushButtonSupport  = (1 << 0),
	kIDMainStatusInfraRedSupport    = (1 << 1),
	kIDMainStatusLID                = (1 << 2),
	kIDMainStatusFirstFeedADF       = (1 << 3),
	kIDMainStatusDuplexADF          = (1 << 4),
	kIDMainStatusPageTypeADF        = (1 << 5),
	kIDMainStatusNotFlatbed         = (1 << 6),
	kIDMainStatusLampChangeSupport  = (1 << 7),
} ENUM_ESCI_EXT_ID_MAIN_STATUS_MASK;  // FS I

typedef enum {
	kIDMainStatusPaperEndDetectionSupport   = (1 << 0),
	kIDMainStatusEnergySaveTimeSupport      = (1 << 1),
	kIDMainStatusAutoFormFeed               = (1 << 2),
	kIDMainStatusDoubleFeedDetectionSupport = (1 << 3),
	kIDMainStatusErPSupport                 = (1 << 4),
	kIDMainStatusAutoScanSupport            = (1 << 4),
	kIDMainStatusQuietSupport               = (1 << 5),
	kIDMainStatusAuthenticationSupport      = (1 << 6),
	kIDMainStatusESCI2CommandSupport        = (1 << 7),
} ENUM_ESCI_EXT_ID_MAIN_STATUS2_MASK; // FS I

typedef enum {
	kIDMainStatusLightIntensitySupport      = (1 << 2),
} ENUM_ESCI_EXT_ID_MAIN_STATUS3_MASK; // FS I

typedef enum {
	kMainStatusCancelWarmupSupport  = (1 << 0),
	kMainStatusWarmUp               = (1 << 1),
	kMainStatusNotReady             = (1 << 6),
	kMainStatusFatalError           = (1 << 7),
} ENUM_ESCI_MAIN_STATUS_MASK;   // ESC f, FS F

typedef enum {
	kADFStatusDuplexEnabled = (1 << 0),
	kADFStatusCoverOpen     = (1 << 1),
	kADFStatusPaperJam      = (1 << 2),
	kADFStatusPaperEmpty    = (1 << 3),
	kADFStatusErrorOccurred = (1 << 5),
	kADFStatusEnabled       = (1 << 6),
	kADFStatusInsatlled     = (1 << 7),
} ENUM_ESCI_ADF_STATUS_MASK;    // ESC f, FS F

typedef enum {
	kADFStatusTrayClose         = (1 << 1),
	kADFStatusDoubleFeedError   = (1 << 4),
} ENUM_ESCI_ADF_STATUS2_MASK;   // FS F

typedef enum {
	kTPUStatusLampError     = (1 << 0),
	kTPUStatusCoverOpen     = (1 << 1),
	kTPUStatusErrorOccurred = (1 << 5),
	kTPUStatusEnabled       = (1 << 6),
	kTPUStatusInstalled     = (1 << 7),
} ENUM_ESCI_TPU_STATUS_MASK;    // ESC f, FS F

typedef enum {
	kDocumentSizeLetterLandscape        = (1 << 0),
	kDocumentSizeLetterPortrait         = (1 << 1),
	kDocumentSizeA4Landscape            = (1 << 2),
	kDocumentSizeA4Portrait             = (1 << 3),
	kDocumentSizeLegalPortrait          = (1 << 4),
	kDocumentSizeB4Portrait             = (1 << 5),
	kDocumentSizeDoubleLetter           = (1 << 6),
	kDocumentSizeA3Portrait             = (1 << 7),
	kDocumentSizeOther                  = (1 << 8),
	kDocumentSizeReserved               = (1 << 9),
	kDocumentSizeExecutiveLandscape     = (1 << 10),
	kDocumentSizeExecutivePortrait      = (1 << 11),
	kDocumentSizeA5Landscape            = (1 << 12),
	kDocumentSizeA5Portrait             = (1 << 13),
	kDocumentSizeB5Landscape            = (1 << 14),
	kDocumentSizeB5Portrait             = (1 << 15),
} ENUM_ESCI_DOCUMENT_SIZE_MASK; // FS F

typedef enum {
	kAutoScanDisable    = 0x00,
	kAutoScanEnable     = 0xFF,
} ENUM_ESCI_AUTO_SCAN_MODE;

//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESCIAccessor::CESCIAccessor()
{
	m_bCaptureCommandSupport         = false;
	m_bShouldIgnoreCancelFromScanner = false;

	m_bCaptured = false;

	m_nBufferSize = 256 *1024;

	m_eJobMode = kESJobModeNone;
	m_bIsDisableJobContinue = false;

	memset( &m_stIdentity,       0,  sizeof(m_stIdentity)        );
	memset( &m_stExtIdentity,    0,  sizeof(m_stExtIdentity)     );
	memset( &m_stHwProperty,     0,  sizeof(m_stHwProperty)      );
	memset( &m_stScannerStatus,  0,  sizeof(m_stScannerStatus)   );
	memset( &m_stParameters,     0,  sizeof(m_stParameters)      );
	memset( &m_sizeMaxScanSizeInLongLength, 0, sizeof(m_sizeMaxScanSizeInLongLength) );

#ifdef WIN32
	m_anySupportedResolutions.clear();
#else
	ESAny().swap(m_anySupportedResolutions);
#endif // WIN32

	m_eDigitalICE = kESDigitalICEDisable;
	m_eGuidePositionFB = kESGuidePositionLeft;
	m_eGuidePositionADF = kESGuidePositionLeft;

	m_strAuthUsername = ES_STRING("");
	m_strAuthPassword = ES_STRING("");
	m_bIsAuthenticationEnabled = true;

	m_nPagesToBeScanned = 0;
	m_bPaperEndDetectionEnabled = false;
	m_bOverScanEnabled = false;

	m_fXOffsetMargin = 0.0f;
	m_fYOffsetMargin = 0.0f;
	
	m_bIsShouldObserveButtonWithCommand = false;
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESCIAccessor::~CESCIAccessor()
{
	Finalize();
}

//===============================================================
//!
//! 初期化処理
//!
//===============================================================
static ESNumber GetFirstIndex( const ESAny& anyIn )
{
	const ESIndexSet* pIndexSet = SafeAnyDataCPtr<ESIndexSet>( anyIn );
	if ( pIndexSet ){
		ESIndexSet::const_iterator it = pIndexSet->begin();
		if ( it != pIndexSet->end() ){
			return *it;
		}
	}
	const ST_ES_RANGE* pRange = SafeAnyDataCPtr<ST_ES_RANGE>( anyIn );
	if ( pRange ){
		return pRange->nMin;
	}
	return 0;
}

void CESCIAccessor::Setup()
{
	ES_COLOR_MATRIX matrix = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	m_cColorMatrix.SetMatrix(matrix);

	{
		// ハーフトーンは0が「ハーフトーンA」を表すため、「なし」を初期値にしておく必要あり
		m_stParameters.un8HalftoneMode = kESHalftoneNone;
	}
	{
		// デフォルトの解像度として、機種がサポートする最低解像度を入れる。
		SetXResolution( GetFirstIndex( GetSupportedXResolutions() ) );
		SetYResolution( GetFirstIndex( GetSupportedYResolutions() ) );
	}
	{
		ShouldObserveButtonWithCommand(true);
	}
}

//===============================================================
//!
//! 終了処理
//!
//===============================================================
ESErrorCode CESCIAccessor::Finalize()
{
	ES_LOG_TRACE_FUNC();

	//StopButtonChecking();
	return kESErrorNoError;
}

//
//
// スキャナ操作
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! 物理デバイスのOpenおよびスキャナから情報を取得し、スキャナの使用とボタン監視を開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::Open()
{
	ES_LOG_TRACE_FUNC();

	if ( IsOpened() ){
		return kESErrorNoError;
	}

	// デバイスを開く
	//--------------------------------------
	ESErrorCode err = OpenDevice();
	if ( IS_ERR_CODE( err ) ){
		if ( (err != kESErrorDeviceInUse) && 
			 (err != kESErrorDeviceInBusy) )
		{
			err = kESErrorDeviceOpenError;
		}
		return err;
	}

	// デバイス情報取得
	//-------------------------------------
	err = GetIdentity();
	if ( IS_ERR_CODE( err ) ){
		goto EXIT;
	}
	
	err = GetExtIdentity();
	if ( IS_ERR_CODE( err ) ){
		goto EXIT;
	}

	err = GetStatus();
	if ( IS_ERR_CODE( err ) ){
		goto EXIT;
	}

	Setup();

	// ボタン監視開始
	//StartButtonChecking();

EXIT:
	if ( IS_ERR_CODE( err ) ){
		CloseDevice();
	}
	return err;
}

//===============================================================
//!
//! 物理デバイスをCloseし、スキャナの使用とボタン監視を終了する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::Close()
{
	ES_LOG_TRACE_FUNC();

	SYNCHRONIZED_START
	{
		//StopButtonChecking(); // ボタン監視終了

		if ( !IsOpened() ){
			return kESErrorNoError;
		}

		return CloseDevice();
	}
	SYNCHRONIZED_END
}

//===============================================================
//!
//! スキャナがオープンしているか？
//!
//===============================================================
bool CESCIAccessor::IsOpened() const
{
	return CESCIAccessor::IsDeviceOpened();
}

//===============================================================
//!
//! デバイス接続切断時の処理
//!
//===============================================================
void CESCIAccessor::DeviceDisconnected()
{
	// ボタン監視終了
	//StopButtonChecking();
}

//
//
// Capture/release scanner to control mutual exclusive access for the same scanner
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! Capture scanner to control mutual exclusive access for the same scanner
//!
//===============================================================
ESErrorCode CESCIAccessor::CaptureScanner()
{
	return RequestCaptureScanner();
}

//===============================================================
//!
//! Release scanner to control mutual exclusive access for the same scanner
//!
//===============================================================
ESErrorCode CESCIAccessor::ReleaseScanner()
{
	return RequestReleaseScanner();
}

//
//
// ジョブリクエスト
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// スキャンジョブの開始
ESErrorCode CESCIAccessor::StartJobInMode( ESJobMode eJobMode )
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if ( GetJobMode() != kESJobModeNone ){
		return kESErrorNoError;
	}
	switch( eJobMode ){
	case kESJobModeStandard:
		err = StartJobInStandard();
		break;
	case kESJobModeContinue:
		err = StartJobInContinue();
		break;
	default:
		goto BAIL;
		break;
	}
	SetJobMode(eJobMode);
BAIL:
	return err;
}

// スキャンジョブの終了
ESErrorCode CESCIAccessor::StopJobInMode( ESJobMode eJobMode )
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if ( GetJobMode() != eJobMode ){
		return kESErrorNoError;
	}
	switch( eJobMode ){
	case kESJobModeStandard:
		err = StopJobInStandard();
		break;
	case kESJobModeContinue:
		err = StopJobInContinue();
		break;
	default:
		goto BAIL;
		break;
	}
	SetJobMode(kESJobModeNone);
BAIL:
	return err;
}

ESErrorCode CESCIAccessor::StartJobInStandard()
{
	ESErrorCode err = kESErrorNoError;
	if (IsDisableJobContinue()){
		return kESErrorNoError;
	}
	err = CaptureScanner();
	if ( IS_ERR_CODE(err) ){
		goto BAIL;
	}
BAIL:
	return err;
}

// スキャンジョブの終了
ESErrorCode CESCIAccessor::StopJobInStandard()
{
	ESErrorCode err = kESErrorNoError;
	if (IsDisableJobContinue()){
		return kESErrorNoError;
	}
	err = ReleaseScanner();
	if ( IS_ERR_CODE(err) ){
		goto BAIL;
	}
BAIL:
	return err;
}

ESErrorCode CESCIAccessor::StartJobInContinue()
{
	if ( IsDisableJobContinue() ){
		return kESErrorNoError;
	}
	ESErrorCode err = CaptureScanner();
	if ( IS_ERR_CODE(err) ){
		goto BAIL;
	}
BAIL:
	return err;
}

ESErrorCode CESCIAccessor::StopJobInContinue()
{
	if ( IsDisableJobContinue() ){
		return kESErrorNoError;
	}
	ESErrorCode err = ReleaseScanner();
	if ( IS_ERR_CODE(err) ){
		goto BAIL;
	}
BAIL:
	return err;
}

ESJobMode CESCIAccessor::GetJobMode()
{
	return m_eJobMode;
}
ESErrorCode CESCIAccessor::SetJobMode( ESJobMode eJobMode )
{
	m_eJobMode = eJobMode;
	return kESErrorNoError;
}
bool CESCIAccessor::IsDisableJobContinue()
{
	return m_bIsDisableJobContinue;
}
ESErrorCode CESCIAccessor::SetDisableJobContinue( bool bDisableJobContinue )
{
	m_bIsDisableJobContinue = bDisableJobContinue;
	return kESErrorNoError;
}

//
//
// スキャン操作
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! 現在のスキャン設定でスキャンを開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::Scan()
{
	ES_LOG_TRACE_FUNC();

	SYNCHRONIZED_START
	{
        {
            SetCancelled( false );
        }
        
		ESErrorCode err = kESErrorNoError;
		bool bShouldRelease = FALSE;

		if ( !IsDeviceOpened() ){
			err = kESErrorSequenceError;;
			goto BAIL;
		}

		// ボタン監視停止
		//StopButtonChecking();

		if ( IsCaptured() ){
			bShouldRelease = FALSE;
		} else {
			err = RequestCaptureScanner();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}
			bShouldRelease = TRUE;
		}

		{
			SetScanning( true );
		}

		{
			err = GetErrorStatus();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}
		}

		{
			err = SetScanningParameters();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}
		}

		if ( IsFeederEnabled() ){

			err = ScanForDocumentFeeder();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}

		} else if ( GetDigitalICE() != kESDigitalICEDisable ){

			err = ScanForDigitalICE();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}
 
		} else {
		
			err = ScanForNormal();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}
 		}

BAIL:
		SetScanning( false );
		CallDelegateScannerDidCompleteScanningWithError( err );

		{
			RequestInitializeScanner();

			// MFPでは初期化コマンドによって取り込み装置がフラットベッドになり、
			// ADF紙なしのステータスが取れないため、改めて現在の取り込み装置をセットするコマンドを送る
			SetOptionControl( GetOptionControl() );
		}

		if ( bShouldRelease ){
			RequestReleaseScanner();
		}

		if ( IsOpened() ){
			// ボタン監視再開
			//StartButtonChecking();
		}

		return err;
	}
	SYNCHRONIZED_END
}

//
CESScannedImage* CESCIAccessor::CreateImageInstance()
{
	ES_LOG_TRACE_FUNC();

	ESNumber nXResolution = GetXResolution();
	ESNumber nYResolution = GetYResolution();
	assert( nXResolution > 0 && nYResolution );

	ST_ES_RECT_UN32 rcScanAreaInPixel = GetScanAreaInPixel();
	ESNumber nWidth  = RECT_WIDTH( rcScanAreaInPixel );
	ESNumber nHeight = RECT_HEIGHT( rcScanAreaInPixel );

	ESNumber nBitsPerPixel		= GetBitsPerPixel();
	ESNumber nBitsPerSample		= GetBitsPerSample();
	ESNumber nSamplesPerPixel	= GetSamplesPerPixel();
	ESString strWorkFolder		= GetWorkFolder();

	try {
		CESScannedImage* pImage = new CESScannedImage(); // DLL呼び出し側でdeleteすること
		ESErrorCode err = pImage->Init(	nWidth, 
												nHeight,
												ESCIGetBytesPerRow( nWidth, nBitsPerPixel ),
												nBitsPerSample,
												nSamplesPerPixel,
												strWorkFolder ); 
		if ( IS_ERR_CODE(err) ){
			ES_LOG_FAILED_INITIALIZE_IMAGE();
			DeleteScanedImage( &pImage ); // pImageはNULLになる
		}
		return pImage;
	} catch( const std::bad_alloc& ){
		ES_LOG_FAILED_CREATE_IMAGE();
		return NULL;
	}
}

//
ESErrorCode CESCIAccessor::ScanForDigitalICE()
{
	ES_LOG_TRACE_FUNC();

	switch ( GetFunctionalUnitType() ) {
	case kESFunctionalUnitTransparent:
		return ScanForFilmICE();

	case kESFunctionalUnitFlatbed:
		return ScanForPrintICE();

	default:
		break;
	}
	ES_LOG_INVALID_SETTING_PARAM();
	return kESErrorFatalError;
}

//
ESErrorCode CESCIAccessor::ScanForFilmICE()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	ENUM_ESCI_OPTION_CONTROL eCurrentOptionControl = GetOptionControl();
	ESColorFormat eCurrentColorFormat = (ESColorFormat)GetColorFormat();

	{
		CESScannedImage* pImage = CreateImageInstance();
		if ( pImage == NULL ){
			return kESErrorMemoryError;
		}

		pImage->SetDigitalICEImageType( kESDigitalICEImageTypeTransparent );
		pImage->SetPaperSerialNumber( 1 );
		pImage->SetSerialNumber( 1 );

		SetOptionControl( kOptionControlEnableDICERGB );

		err = SetScanningParameters();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}

		err = RequestScanToImage( pImage );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
	}

	{
		err = RequestUseDICE();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}

		// Infra-red image must be monochrome
		SetColorFormat( kESColorFormatMono8 );
	}

	{
		CESScannedImage* pImage = CreateImageInstance();
		if ( pImage == NULL ){
			return kESErrorMemoryError;
		}

		pImage->SetDigitalICEImageType( kESDigitalICEImageTypeTransparentIR );
		pImage->SetPaperSerialNumber( 1 );
		pImage->SetSerialNumber( 2 );

		switch ( GetDigitalICE() ) {
		case kESDigitalICEEnable:
			SetOptionControl( kOptionControlEnableDICEInfraRed );
			break;

		case kESDigitalICELiteEnable:
			SetOptionControl( kOptionControlEnableDICELiteInfraRed );
			break;

		default:
			return kESErrorFatalError;
		}

		err = SetScanningParameters();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}

		err = RequestScanToImage( pImage );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
	}

BAIL:
	SetColorFormat( eCurrentColorFormat );
	SetOptionControl( eCurrentOptionControl );

	return err;
}
ESErrorCode CESCIAccessor::ScanForPrintICE()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	{
		CESScannedImage* pImage = CreateImageInstance();
		if ( pImage == NULL ){
			return kESErrorMemoryError;
		}

		pImage->SetDigitalICEImageType( kESDigitalICEImageTypeFlatbedLamp1 );
		pImage->SetPaperSerialNumber( 1 );
		pImage->SetSerialNumber( 1 );

		SetLampMode( kESLampModeUseLamp1 );

		err = SetScanningParameters();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}

		err = RequestScanToImage( pImage );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
	}
	{
		err = RequestUseDICE();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
	}
	{
		CESScannedImage* pImage = CreateImageInstance();
		if ( pImage == NULL ){
			return kESErrorMemoryError;
		}

		pImage->SetDigitalICEImageType( kESDigitalICEImageTypeFlatbedLamp2 );
		pImage->SetPaperSerialNumber( 1 );
		pImage->SetSerialNumber( 2 );

		SetLampMode( kESLampModeUseLamp2 );

		err = SetScanningParameters();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}

		err = RequestScanToImage( pImage );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
	}

	SetLampMode( kESLampModeDefault );

BAIL:
	return err;
}

ESErrorCode CESCIAccessor::ScanForDocumentFeeder()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	UInt32 un32PageToBeScanned = 0;
	bool bContinuesInfinitely = (GetPagesToBeScanned() == 0 );
	bool bIsFirstScan = true;
	UInt32 un32PageCount = 1;

	un32PageToBeScanned = GetPagesToBeScanned();
	if ( bContinuesInfinitely ){
		un32PageToBeScanned = 1;
	}

	if ( IsFeederEnabled() ){
		CallDelegateScannerWillBeginContinuousScanning();
	}

	while ( !IsCancelled() && (bContinuesInfinitely || un32PageToBeScanned > 0) ){
		CESScannedImage* pImage = NULL;

		if ( IsDuplexEnabled() && un32PageCount % 2 == 0 ) {
			// 裏面読み取り開始時にはすでにトレイ上に紙がないため、
			// 裏面の場合は紙が置かれていなかったとしてもループを抜けないようにする
		} else if ( !IsDocumentLoaded() ){
			if ( !bContinuesInfinitely || bIsFirstScan ){
				err = kESErrorPaperEmpty;
			}
			break;
		}

		if ( IsPageFeedTypeDocumentFeeder() ){
			err = RequestLoadPaper();
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}
		}

		{
			pImage = CreateImageInstance();
			if ( pImage == NULL ){
				err = kESErrorMemoryError;
				goto BAIL;
			}
			// ページのシリアルナンバーを付ける。
			pImage->SetSerialNumber( un32PageCount );

			if ( IsDuplexEnabled() ) {
				// 用紙のシリアルナンバーを付ける。1枚の用紙の表裏両方のページで同一のナンバーが付く。
				pImage->SetPaperSerialNumber( (un32PageCount+1)/2 );

				// 表裏の情報を付ける。ESC/Iコマンドの画像転送仕様であれば、
				// スキャン開始時からのページカウントにおいて奇数 = 表、偶数 = 裏となることが自明。
				if ( un32PageCount % 2 == 1) {
					pImage->SetSurfaceType( kESSurfaceTypeFrontSide );
				} else {
					pImage->SetSurfaceType( kESSurfaceTypeBackSide );

					// 2Pass両面スキャナーでは裏面が逆向きになっていることをを示す。
					if ( GetDuplexType() == kESDuplexType2Pass ){
						pImage->SetUpsideDown( true );
					}
				}
			} else {
				pImage->SetPaperSerialNumber( un32PageCount );
				pImage->SetSurfaceType( kESSurfaceTypeFrontSide );
			}

			err = RequestScanToImage( pImage );
			if ( IS_ERR_CODE( err ) ){
				goto BAIL;
			}

			un32PageCount += 1;
		}

		bIsFirstScan = false;

		if ( !bContinuesInfinitely ){
			un32PageToBeScanned -= 1;
		}
	} // while

	if ( IsFeederEnabled() ){
		CallDelegateScannerDidEndContinuousScanning();
	}

	if ( IsPrefeed() || IsPageFeedTypeDocumentFeeder() ){
		err = RequestEjectPaper();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
	}

BAIL:
	return err;
}

//
ESErrorCode CESCIAccessor::ScanForNormal()
{
	CESScannedImage* pImage = CreateImageInstance();
	if ( pImage == NULL ){
		return kESErrorMemoryError;
	}
	pImage->SetPaperSerialNumber( 1 );
	pImage->SetSerialNumber( 1 );

	return RequestScanToImage( pImage );
}


//===============================================================
//!
//! スキャン処理をキャンセルする。
//!
//===============================================================
ESErrorCode CESCIAccessor::Cancel()
{
	ES_LOG_TRACE_FUNC();
	SetCancelled( true );
	return kESErrorNoError;
}

//===============================================================
//!
//! 色補正係数を画像に適用する。ファームウェアで色補正係数がサポートできないDumbスキャナー用。
//! 
//! @param cScannedData  画像データの格納されているバッファ。画像はRGBカラーである必要がある。
//! @param nDataLength   画像データの長さ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::ApplyColorMatrix( ES_CMN_FUNCS::BUFFER::IESBuffer& cScannedData, UInt32 nDataLength )
{
	// Apply color matrix for dumb scanner
	if ( !IsDumb() || (GetSamplesPerPixel() != 3) || (nDataLength%3 != 0) ){
		return kESErrorFatalError;
	}

	if ( GetBitsPerSample() == 8 ){
		ES_COLOR_MATRIX& matrix = m_cColorMatrix.GetMatrix();
		UInt8* p = cScannedData.GetBufferPtr();
		UInt8 max = ~((UInt8)0);

		for ( UInt32 i = 0; i < nDataLength; i+=3 ){
			UInt8* pPixcel = p + i;
			UInt8 r = pPixcel[0];
			UInt8 g = pPixcel[1];
			UInt8 b = pPixcel[2];

			for ( int nChannel = 0; nChannel < 3; nChannel++) {
				float v = r * matrix[nChannel][0] + g * matrix[nChannel][1] + b * matrix[nChannel][2];

				if (v > max) {
					pPixcel[nChannel] = max;
				} else if (v < 0) {
					pPixcel[nChannel] = 0;
				} else {
					pPixcel[nChannel] = (UInt8)ES_CMN_FUNCS::roundf(v);
				}
			}
		}
	} else if ( GetBitsPerSample() == 16 ){
		ES_COLOR_MATRIX& matrix = m_cColorMatrix.GetMatrix();
		UInt8* p = cScannedData.GetBufferPtr();
		UInt16 max = ~((UInt16)0);

		for ( UInt32 i = 0; i < nDataLength; i += sizeof(UInt16) * 3 ){
			UInt16*  pPixcel = (UInt16*)(p + i);
			UInt16   r       = CFSwapInt16LittleToHost(pPixcel[0]);
			UInt16   g       = CFSwapInt16LittleToHost(pPixcel[1]);
			UInt16   b       = CFSwapInt16LittleToHost(pPixcel[2]);

			for ( int nChannel = 0; nChannel < 3; nChannel++) {
				float v = r * matrix[nChannel][0] + g * matrix[nChannel][1] + b * matrix[nChannel][2];

				if (v > max) {
					pPixcel[nChannel] = max;
				} else if (v < 0) {
					pPixcel[nChannel] = 0;
				} else {
					pPixcel[nChannel] = CFSwapInt16HostToLittle((UInt16)ES_CMN_FUNCS::roundf(v));
				}
			}
		}
	}
	return kESErrorNoError;
}

//
//
// ボタン監視
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// プッシュボタン監視をコマンドでおこなうかUSB Interruptでおこなうか。
// USB Interruptパイプがあっても、プッシュボタンイベントをサポートしていない機種のため。
bool CESCIAccessor::ShouldObserveButtonWithCommand(bool initialize /*= false*/)
{
	if (initialize){
		using namespace epsonscan2::es2command;
		m_bIsShouldObserveButtonWithCommand = ModelInfo::Instance().IsShouldObserveButtonWithCommand(GetProductName());
	}
	return m_bIsShouldObserveButtonWithCommand;
}

// ボタン監視コマンド送信間隔。
// デフォルトは1秒。
DWORD CESCIAccessor::ButtonObservationInterval()
{
	using namespace epsonscan2::es2command;
	int32_t interval = ModelInfo::Instance().CustomButtonObservationInterval_ms(GetProductName());
	if (interval < 0){
		return c_dwButtonCheckIntervalForComunication_ms;
	}
	return static_cast<DWORD>(interval);
}
	
//===============================================================
//!
//! ボタンの監視を開始する
//!
//===============================================================
void CESCIAccessor::StartButtonChecking()
{
	ES_LOG_TRACE_FUNC();

	std::lock_guard<std::recursive_mutex> lock(m_event_mtx);

	if (m_pDevInterface && m_pDevInterface->IsOpened())
	{
		DWORD dwInterval = c_dwButtonCheckIntervalForComunication_ms;

		ESConnectionType connectionType = m_pDevInterface->ConnectionType();
		if (!ShouldObserveButtonWithCommand() && m_pDevInterface->IsInterruptSupported() ){
			dwInterval = c_dwButtonCheckIntervalForInterrupt_ms;
			m_interruptCheckTimer = std::unique_ptr<event_caller>(new event_caller([&]{
					m_pDevInterface->ReceiveInterruptEvent();
				}, dwInterval));
			m_interruptCheckTimer->start();
		} else if (connectionType == kESConnectionLocal || connectionType == kESConnectionInterpreter) {
			dwInterval = ButtonObservationInterval();
			m_interruptCheckTimer = std::unique_ptr<event_caller>(new event_caller([&]{
					// ESCI コマンド送信
					UInt8 un8Status = GetButtonStatus();
					if ( un8Status != 0 ){
						// Delegate通知
						CallDelegateScannerDidPressButton( un8Status );
					}
				}, dwInterval));
			m_interruptCheckTimer->start();
		}
	}
}

//===============================================================
//!
//! ボタンの監視を終了する
//!
//===============================================================
void CESCIAccessor::StopButtonChecking()
{
	ES_LOG_TRACE_FUNC();

	std::lock_guard<std::recursive_mutex> lock(m_event_mtx);

	if (m_interruptCheckTimer){
		m_interruptCheckTimer->stop();
		m_interruptCheckTimer->finalize();
		m_interruptCheckTimer.reset();
	}
}

//
//
// スキャナ情報取得
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! Identity情報を取得して保持する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::GetIdentity()
{
	ES_LOG_TRACE_FUNC();

	ESIndexSet arResolutions;
	ESErrorCode err = RequestIdentity( m_stIdentity, arResolutions);
	if ( IS_ERR_CODE( err ) ){
		return err;
	}
	if ( m_stIdentity.un8CommandLevel[0] == kCommandLevelDumb ){
#ifdef WIN32
		m_anySupportedResolutions.clear();
#else
		ESAny().swap(m_anySupportedResolutions);
#endif
		m_anySupportedResolutions = arResolutions;
		
		ESIndexSet xResolutions, yResolutions;
		err = RequestHWProperty(m_stHwProperty, xResolutions, yResolutions);
	}
	return err;
}

//===============================================================
//!
//! 拡張Identity情報を取得して保持する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::GetExtIdentity()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = RequestExtendedIdentity(m_stExtIdentity);
	if ( IS_ERR_CODE( err ) ){
		return err;
	}
	if ( m_stExtIdentity.un8CommandLevel[0] == kCommandLevelStandard ){

		ST_ES_RANGE stRange;
		stRange.nMin = m_stExtIdentity.un32MinResolution;
		stRange.nMax = m_stExtIdentity.un32MaxResolution;
		stRange.nStep = 1;

		m_anySupportedResolutions = stRange;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! ステータス情報を取得して保持する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::GetStatus()
{
	ES_LOG_TRACE_FUNC();
	return RequestStatus(m_stScannerStatus);
}

//===============================================================
//!
//! 設定をリセットする
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::Reset()
{
	ES_LOG_TRACE_FUNC();
	memset(&m_stParameters, 0, sizeof(m_stParameters));

	SetOverScanEnabled(false);
	SetPaperEndDetectionEnabled(false);

	Setup();
	return kESErrorNoError;
}

//=========================================================================
//!
//! バージョン取得
//!
//=========================================================================
ESString CESCIAccessor::GetVersion()
{
	std::vector<char> buf( sizeof(m_stExtIdentity.chRomVersion) + 1, 0 );
	memcpy_s( &buf[0], buf.size(), m_stExtIdentity.chRomVersion, sizeof(m_stExtIdentity.chRomVersion) );

	ESStringA strVersionA = &buf[0];
	if ( strVersionA.empty() ){
		return ES_STRING("");
	} else {
#ifdef WIN32
		return ES_CMN_FUNCS::STRING::MBStringToUnicode( strVersionA );
#else
		return strVersionA;
#endif
	}
}

//=========================================================================
//!
//! エラーステータスを取得する。(ESErrorStatus)
//!
//=========================================================================
ESErrorCode CESCIAccessor::GetErrorStatus()
{
	ES_LOG_TRACE_FUNC();
	ST_ESCI_SCANNER_STATUS status = {};

	ESErrorCode err = RequestStatus(status);
	if ( IS_ERR_CODE(err) ){
		return err;
	}

	if (status.un8AdfStatus2 & kADFStatusTrayClose) {
		return (ESErrorCode)kESErrorStatusTrayClose;
	}

	if (status.un8AdfStatus2 & kADFStatusDoubleFeedError) {
		return (ESErrorCode)kESErrorStatusPaperDoubleFeed;
	}

	if (status.un8AdfStatus & kADFStatusCoverOpen) {
		return (ESErrorCode)kESErrorStatusCoverOpen;
	}

	if (status.un8AdfStatus & kADFStatusPaperEmpty) {
		return (ESErrorCode)kESErrorStatusPaperEmpty;
	}

	if (status.un8AdfStatus & kADFStatusPaperJam) {
		return (ESErrorCode)kESErrorStatusPaperJam;
	}

	if (status.un8AdfStatus & kADFStatusErrorOccurred) {
		return (ESErrorCode)kESErrorStatusFatalError;
	}

	if (status.un8TpuStatus & kTPUStatusLampError) {
		return (ESErrorCode)kESErrorStatusLampError;
	}

	if (status.un8TpuStatus & kTPUStatusCoverOpen) {
		return (ESErrorCode)kESErrorStatusCoverOpen;
	}

	if (status.un8TpuStatus & kTPUStatusErrorOccurred) {
		return (ESErrorCode)kESErrorStatusFatalError;
	}

	if (status.un8MainStatus & kMainStatusNotReady) {
		return (ESErrorCode)kESErrorStatusDeviceInBusy;
	}

	if (status.un8MainStatus & kMainStatusFatalError) {
		return (ESErrorCode)kESErrorStatusFatalError;
	}

	return (ESErrorCode)kESErrorStatusNoError;
}

//===============================================================
//!
//! スキャナからのキャンセルを無視するか？
//!
//===============================================================
bool CESCIAccessor::IsShouldIgnoreCancelFromScanner()
{
	return m_bShouldIgnoreCancelFromScanner;
}

//===============================================================
//!
//! スキャナからのキャンセルを無視するか設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetShouldIgnoreCancelFromScanner( bool bIgnore )
{
	m_bShouldIgnoreCancelFromScanner = bIgnore;
	return kESErrorNoError;
}

//===============================================================
//!
//! キャプチャー機能をサポートするか？
//!
//===============================================================
bool CESCIAccessor::IsCaptureCommandSupported()
{
	return m_bCaptureCommandSupport;
}

//===============================================================
//!
//! キャプチャーコマンドをサポートしているか設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetCaptureCommandSupported( bool bSupported )
{
	m_bCaptureCommandSupport = bSupported;
	return kESErrorNoError;
}

//===============================================================
//!
//! キャプチャー中か？
//!
//===============================================================
bool CESCIAccessor::IsCaptured()
{
	return m_bCaptured;
}

//===============================================================
//!
//! キャプチャー状態をセットする
//!
//===============================================================
void CESCIAccessor::SetCaptured( bool bCaptured )
{
	m_bCaptured = bCaptured;
}

//===============================================================
//!
//! バッファサイズを取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetBufferSize()
{
	return m_nBufferSize;
}

//===============================================================
//!
//! バッファサイズを設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetBufferSize( ESNumber nBufferSize )
{
	m_nBufferSize = nBufferSize;
	return kESErrorNoError;
}

//===============================================================
//!
//! 静止中か？
//!
//===============================================================
bool CESCIAccessor::IsDumb()
{
	return ( m_stExtIdentity.un8CommandLevel[0] == kCommandLevelDumb );
}

//===============================================================
//!
//! ウォーミングアップ中か？
//!
//===============================================================
bool CESCIAccessor::IsWarmingUp()
{
	ST_ESCI_SCANNER_STATUS status = {};
	if ( RequestStatus(status) != kESErrorNoError) {
		return false;
	}
	return ( status.un8MainStatus & kMainStatusWarmUp) ? true : false;
}

//===============================================================
//!
//! 認証機能をサポートしているか？
//!
//===============================================================
bool CESCIAccessor::IsAuthenticationSupported()
{
	return (m_stExtIdentity.un8MainStatus2 & kIDMainStatusAuthenticationSupport) ? true : false;
}

//===============================================================
//!
//! 認証機能が有効状態かどうかを取得する
//!
//===============================================================
bool CESCIAccessor::IsAuthenticationEnabled()
{
	return m_bIsAuthenticationEnabled;
}

//===============================================================
//!
// 認証機能が有効状態かどうかを設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetAuthenticationEnabled( bool bEnabled )
{
	m_bIsAuthenticationEnabled = bEnabled;
	return kESErrorNoError;
}

//===============================================================
//!
//! 認証ユーザー名を取得する
//!
//===============================================================
ESString CESCIAccessor::GetAuthUserName()
{
	return m_strAuthUsername;
}

//===============================================================
//!
//! 認証ユーザー名を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetAuthUserName( ESString strAuthUserName )
{
	m_strAuthUsername = strAuthUserName;
	return kESErrorNoError;
}

//===============================================================
//!
//! 認証パスワード取得する
//!
//===============================================================
ESString CESCIAccessor::GetAuthPassword()
{
	return m_strAuthPassword;
}

//===============================================================
//!
//! 認証パスワードデーターを設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetAuthPassword( ESString strAuthPassword)
{
	m_strAuthPassword= strAuthPassword;
	return kESErrorNoError;
}

//===============================================================
//!
//! プロダクト名を取得する
//!
//===============================================================
ESString CESCIAccessor::GetProductName()
{
	std::vector<char> buf( sizeof(m_stExtIdentity.chProductName) + 1, 0 );
	memcpy_s( &buf[0], buf.size(), m_stExtIdentity.chProductName, sizeof(m_stExtIdentity.chProductName) );

	ESStringA strProductNameA = &buf[0];
	if ( strProductNameA.empty() ){
		return ES_STRING("");
	} else {
		strProductNameA = TrimSpace( strProductNameA );
#ifdef WIN32
		return ES_CMN_FUNCS::STRING::MBStringToUnicode( strProductNameA );
#else
		return strProductNameA;
#endif
	}
}

//===============================================================
//!
//! パワーセーブ機能をサポートしているか？
//!
//===============================================================
bool CESCIAccessor::IsPowerOffSupported()
{
	return (m_stExtIdentity.un8MainStatus2 & kIDMainStatusErPSupport) ? true : false;
}

//===============================================================
//!
//!	パワーオフ時間を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetPowerOffTime()
{
	UInt32 un32PowerOffTime = 0;
	if ( IS_ERR_SUCCESS( RequestGetPowerOffTime( un32PowerOffTime ) ) ){
		return un32PowerOffTime;
	}
	return 0;
}

//===============================================================
//!
//!	パワーオフ時間を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetPowerOffTime( ESNumber nPowerOffTime )
{
	return RequestSetPowerOffTime( nPowerOffTime );
}

//===============================================================
//!
//! プッシュボタンをサポートしているか？
//!
//===============================================================
bool CESCIAccessor::IsPushButtonSupported()
{
	return (m_stExtIdentity.un8MainStatus & kIDMainStatusPushButtonSupport) ? true : false;
}

//===============================================================
//!
//! プッシュボタンのステータスを取得する
//!
//===============================================================
UInt8 CESCIAccessor::GetButtonStatus()
{
	UInt8 nButtonStatus = 0;
	if ( RequestButtonStatus( nButtonStatus ) != kESErrorNoError ){
		nButtonStatus = 0;
	}
	return nButtonStatus;
}

//===============================================================
//!
//! 取り込み装置オプションを取得する
//!
//===============================================================
CESCICommand::ENUM_ESCI_OPTION_CONTROL CESCIAccessor::GetOptionControl()
{
	return m_stParameters.un8OptionControl;
}

//===============================================================
//!
//! 取り込み装置オプションを設定する
//!
//===============================================================
bool CESCIAccessor::SetOptionControl( ENUM_ESCI_OPTION_CONTROL eOptionControl )
{
	m_stParameters.un8OptionControl = eOptionControl;
	return IS_ERR_SUCCESS( RequestSetFunctionalUnitType( eOptionControl ) );
}

//===============================================================
//!
//! サポートする給紙タイプを取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedFunctionalUnitTypes()
{
	ESIndexSet types;
	if ( IsFlatbedSupported() ){
		types.insert( kESFunctionalUnitFlatbed );
	}

	if ( IsFeederSupported() ) {
		types.insert( kESFunctionalUnitDocumentFeeder );
	}

	if ( IsTransparencyUnitSupported() ) {
		types.insert( kESFunctionalUnitTransparent );
	}

	if ( IsTransparencyUnit2ndAreaSupported() ){
		types.insert( kESFunctionalUnitTPUAreaGuide );
	}

	return types;
}

//===============================================================
//!
//! 給紙タイプを取得する
//!
//===============================================================
ESFunctionalUnitType CESCIAccessor::GetFunctionalUnitType()
{
	switch ( GetOptionControl() ) {
		case kOptionControlEnable:
			if ( IsFeederSupported() ) {
				return kESFunctionalUnitDocumentFeeder;
			} else if ( IsTransparencyUnitSupported() ) {
				return kESFunctionalUnitTransparent;
			}
			break;

		case kOptionControlEnableADFDuplex:
			return kESFunctionalUnitDocumentFeeder;

		case kOptionControlEnableTPUAreaGuide:
			return kESFunctionalUnitTPUAreaGuide;

		case kOptionControlDisable:
		default:
			return kESFunctionalUnitFlatbed;
			break;
	}
	return kESFunctionalUnitFlatbed;
}

//===============================================================
//!
//! 給紙タイプを設定する ( ESFunctionalUnitType )
//!
//===============================================================
ESErrorCode CESCIAccessor::SetFunctionalUnitType( ESNumber nFunctionalUnitType )
{
	if ( GetFunctionalUnitType() == nFunctionalUnitType ) {
		return kESErrorNoError;
	}

	ENUM_ESCI_OPTION_CONTROL option = kOptionControlDisable;

	switch ( nFunctionalUnitType ) {
		case kESFunctionalUnitFlatbed:
			option = kOptionControlDisable;
			break;

		case kESFunctionalUnitDocumentFeeder:
		case kESFunctionalUnitTransparent:
			option = kOptionControlEnable;
			break;

		case kESFunctionalUnitTPUAreaGuide:
			option = kOptionControlEnableTPUAreaGuide;
			break;

		default:
			return kESErrorInvalidParameter;
			break;
	}
	SetOptionControl( option );
	return kESErrorNoError;
}

//===============================================================
//!
//! サポートする解像度を取得する (ESIndexSet or ST_ES_RANGE)
//!
//===============================================================
ESAny CESCIAccessor::GetSupportedXResolutions()
{
	return m_anySupportedResolutions; // X,Y共通値
}
ESAny CESCIAccessor::GetSupportedYResolutions()
{
	return m_anySupportedResolutions; // X,Y共通値
}

//===============================================================
//!
//! 解像度を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetXResolution()
{
	return m_stParameters.un32XResolution;
}

ESNumber CESCIAccessor::GetYResolution()
{
	return m_stParameters.un32YResolution;
}

//===============================================================
//!
//! 解像度を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetXResolution( ESNumber xResolution )
{
	m_stParameters.un32XResolution = (UInt32)xResolution;
	return kESErrorNoError;
}

ESErrorCode CESCIAccessor::SetYResolution( ESNumber yResolution )
{
	m_stParameters.un32YResolution = (UInt32)yResolution;
	return kESErrorNoError;
}

//===============================================================
//!
//! 光学解像度を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetOpticalResolution()
{
	return m_stExtIdentity.un32BasicResolution;
}

//===============================================================
//!
//! 最大スキャンサイズを取得する(インチ） 
//!
//===============================================================
ST_ES_SIZE_F CESCIAccessor::GetMaxScanSize()
{
	UInt32 un32WidthPixels = 0;
	UInt32 un32HeightPixels = 0;

	switch ( GetFunctionalUnitType() ) {
		case kESFunctionalUnitFlatbed:
			un32WidthPixels  = m_stExtIdentity.un32PixelsInBasicXRes;
			un32HeightPixels = m_stExtIdentity.un32PixelsInBasicYRes;
			break;

		case kESFunctionalUnitDocumentFeeder:
			un32WidthPixels  = m_stExtIdentity.un32AdfPixelsInBasicXRes;
			un32HeightPixels = m_stExtIdentity.un32AdfPixelsInBasicYRes;
			break;

		case kESFunctionalUnitTransparent:
			un32WidthPixels  = m_stExtIdentity.un32TpuPixelsInBasicXRes;
			un32HeightPixels = m_stExtIdentity.un32TpuPixelsInBasicYRes;
			break;

		case kESFunctionalUnitTPUAreaGuide:
			un32WidthPixels  = m_stExtIdentity.un32Tpu2PixelsInBasicXRes;
			un32HeightPixels = m_stExtIdentity.un32Tpu2PixelsInBasicYRes;
			break;

		default:
			break;
	}

	UInt32 un32BasicResolution = m_stExtIdentity.un32BasicResolution;
	assert( un32BasicResolution > 0 );

	ST_ES_SIZE_F size = MakeSize< float >( ((float)un32WidthPixels/un32BasicResolution), ((float)un32HeightPixels/un32BasicResolution) );
	return size;
}

//===============================================================
//!
//! スキャンするエリアを取得する（インチ）
//!
//===============================================================
ST_ES_RECT_F CESCIAccessor::GetScanArea()
{
	ESNumber xRes                     = GetXResolution();
	ESNumber yRes                     = GetYResolution();
	ST_ES_RECT_F    rcArea            = {0.0f,0.0f,0.0f,0.0f};
	ST_ES_RECT_UN32 rcScanAreaInPixel = GetScanAreaInPixel();

	if ( xRes == 0 || yRes == 0 ){
		return MakeRect<ESFloat>(0.0f,0.0f,0.0f,0.0f);
	}

	rcArea.left   = (ESFloat)rcScanAreaInPixel.left   / xRes;
	rcArea.top    = (ESFloat)rcScanAreaInPixel.top    / yRes;
	rcArea.right  = (ESFloat)rcScanAreaInPixel.right  / xRes;
	rcArea.bottom = (ESFloat)rcScanAreaInPixel.bottom / yRes;

	return rcArea;
}

//===============================================================
//!
//! スキャンするエリアを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCIAccessor::SetScanArea( ST_ES_RECT_F rcScanArea, bool bShouldAlign )
{
	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("Set scan area in Inch : %f, %f, %f, %f"), rcScanArea.left, rcScanArea.top, rcScanArea.right, rcScanArea.bottom);

	ST_ES_RECT_UN32 rcScanAreaInPixel = {};
	ESNumber xRes                     = GetXResolution();
	ESNumber yRes                     = GetYResolution();

	assert(xRes > 0 && yRes > 0 );

	rcScanAreaInPixel.left   = (UInt32)(rcScanArea.left   * xRes);
	rcScanAreaInPixel.top    = (UInt32)(rcScanArea.top    * yRes);
	rcScanAreaInPixel.right  = (UInt32)(rcScanArea.right  * xRes);
	rcScanAreaInPixel.bottom = (UInt32)(rcScanArea.bottom * yRes);

	return SetScanAreaInPixel( rcScanAreaInPixel, bShouldAlign );
}
ESErrorCode CESCIAccessor::SetScanArea(ST_ES_RECT_F rcScanArea)
{
	return SetScanArea( rcScanArea, false );
}

//
//
//
ST_ES_SIZE_F CESCIAccessor::GetMaxScanSizeInLongLength()
{
	if ( m_sizeMaxScanSizeInLongLength.cx == 0 && m_sizeMaxScanSizeInLongLength.cy == 0 ){
		return GetMaxScanSize();
	}
	return m_sizeMaxScanSizeInLongLength;
}
//
//
//
ESErrorCode CESCIAccessor::SetMaxScanSizeInLongLength( ST_ES_SIZE_F maxScanSizeInLongLength )
{
	m_sizeMaxScanSizeInLongLength = maxScanSizeInLongLength;
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャンするエリアをピクセル単位で取得する
//!
//===============================================================
ST_ES_RECT_UN32 CESCIAccessor::GetScanAreaInPixel()
{
	const int OVERSCAN_HEIGHT = 15; // inches
	const ST_ES_RECT_UN32 ZeroRect = MakeRect< UInt32 >(0,0,0,0);

	ST_ES_RECT_UN32 rcScanAreaInPixel = {m_stParameters.un32XOffset,
	                                     m_stParameters.un32YOffset,
	                                     m_stParameters.un32XOffset + m_stParameters.un32Width,
	                                     m_stParameters.un32YOffset + m_stParameters.un32Height};

	// OverScanは15インチ長を指定された場合に自動でオンとなる仕様
	if ( IsOverScanEnabled() ){
		ESNumber yRes = GetYResolution();
		if ( yRes == 0 ){
			return ZeroRect;
		}
		rcScanAreaInPixel.bottom = rcScanAreaInPixel.top + (OVERSCAN_HEIGHT * yRes);
	}

	if ( GetBitsPerPixel() == 1 ){
		UInt32 un32Width         = rcScanAreaInPixel.right - rcScanAreaInPixel.left;
		UInt32 xOffset           = rcScanAreaInPixel.left;
		ESNumber xRes            = GetXResolution();
		ST_ES_SIZE_F maxScanSize = GetMaxScanSizeInLongLength();

		// Adjust width for 1 bit scan because it should be a multiple of 8
		un32Width = ((un32Width + 7) / 8) * 8;

		while ( un32Width + xOffset > maxScanSize.cx * xRes ) {
			un32Width -= 8;
			rcScanAreaInPixel.right = rcScanAreaInPixel.left + un32Width;
		}
	}
	return rcScanAreaInPixel;
}

//===============================================================
//!
//! スキャンするエリアをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetScanAreaInPixel( ST_ES_RECT_UN32 rcUn32ScanAreaInPixel, bool bShouldAlign )
{
	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("Set scan area in Pixel : %d, %d, %d, %d"), rcUn32ScanAreaInPixel.left, rcUn32ScanAreaInPixel.top, rcUn32ScanAreaInPixel.right, rcUn32ScanAreaInPixel.bottom);
	{
		ST_ES_RECT_S32 rcS32ScanAreaInPixel = MakeRect<SInt32>( (SInt32)rcUn32ScanAreaInPixel.left,
																(SInt32)rcUn32ScanAreaInPixel.top,
																(SInt32)rcUn32ScanAreaInPixel.right,
																(SInt32)rcUn32ScanAreaInPixel.bottom);

		ST_ES_RECT_S32 rcSupportedArea = {};
		ST_ES_SIZE_F sizeValue          = GetMaxScanSizeInLongLength();
		ESNumber xRes                   = GetXResolution();
		ESNumber yRes                   = GetYResolution();

		assert(sizeValue.cx > 0.0f && sizeValue.cy > 0.0f);
		assert(xRes > 0 && yRes > 0);

		rcSupportedArea = MakeRect<SInt32>( 0, 0, SInt32(sizeValue.cx * xRes), (SInt32)(sizeValue.cy * yRes) );

		if ( bShouldAlign ){
			switch( GetGuidePosition() ){
			case kESGuidePositionCenter:
				{
					ESNumber nOffset = RECT_WIDTH(rcSupportedArea)/2 - RECT_WIDTH(rcS32ScanAreaInPixel)/2;
					rcS32ScanAreaInPixel.left  += nOffset;
					rcS32ScanAreaInPixel.right += nOffset;
				}
				break;
			case kESGuidePositionRight:
				{
					ESNumber nOffset = RECT_WIDTH(rcSupportedArea) - RECT_WIDTH(rcS32ScanAreaInPixel);
					rcS32ScanAreaInPixel.left  += nOffset;
					rcS32ScanAreaInPixel.right += nOffset;
				}
				break;
			case kESGuidePositionLeft:
			default:
				break;
			}
		}

		rcS32ScanAreaInPixel.left   += (SInt32)(GetXOffsetMargin() * xRes);
		rcS32ScanAreaInPixel.right  += (SInt32)(GetXOffsetMargin() * xRes);
		rcS32ScanAreaInPixel.top    += (SInt32)(GetYOffsetMargin() * yRes);
		rcS32ScanAreaInPixel.bottom += (SInt32)(GetYOffsetMargin() * yRes);

		assert( ESIntersectsRect(rcSupportedArea, rcS32ScanAreaInPixel));

		rcS32ScanAreaInPixel = ESIntersectionRect(rcSupportedArea, rcS32ScanAreaInPixel);

		rcUn32ScanAreaInPixel = MakeRect<UInt32>( (UInt32)rcS32ScanAreaInPixel.left,
		                                          (UInt32)rcS32ScanAreaInPixel.top,
		                                          (UInt32)rcS32ScanAreaInPixel.right,
		                                          (UInt32)rcS32ScanAreaInPixel.bottom);
	}

	m_stParameters.un32XOffset = rcUn32ScanAreaInPixel.left;
	m_stParameters.un32YOffset = rcUn32ScanAreaInPixel.top;
	m_stParameters.un32Width   = RECT_WIDTH(rcUn32ScanAreaInPixel);
	m_stParameters.un32Height  = RECT_HEIGHT(rcUn32ScanAreaInPixel);

	return kESErrorNoError;
}
ESErrorCode CESCIAccessor::SetScanAreaInPixel( ST_ES_RECT_UN32 rcUn32ScanAreaInPixel)
{
	return SetScanAreaInPixel( rcUn32ScanAreaInPixel, false );
}

//===============================================================
//!
//! スキャンするサイズを取得する（インチ）
//!
//===============================================================
ST_ES_SIZE_F CESCIAccessor::GetScanSize()
{
	ST_ES_RECT_F rc = GetScanArea();
	ST_ES_SIZE_F size = MakeSize( rc.right - rc.left, rc.bottom - rc.top );

	return size;
}

//===============================================================
//!
//! スキャンするサイズを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCIAccessor::SetScanSize( ST_ES_SIZE_F sizeScan )
{
	ST_ES_RECT_F rc = MakeRect( 0.0f, 0.0f, sizeScan.cx, sizeScan.cy );
	return SetScanArea( rc, true );
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で取得する
//!
//===============================================================
ST_ES_SIZE_UN32 CESCIAccessor::GetScanSizeInPixel()
{
	ST_ES_RECT_UN32 rc = GetScanAreaInPixel();
	ST_ES_SIZE_UN32 size = MakeSize( rc.right - rc.left, rc.bottom - rc.top );

	return size;
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetScanSizeInPixel( ST_ES_SIZE_UN32 sizeScan )
{
	ST_ES_RECT_UN32 rc = MakeRect<UInt32>( 0, 0, sizeScan.cx, sizeScan.cy );
	return SetScanAreaInPixel( rc, true );
}


//===============================================================
//!
//! オフセットマージン
//!
//===============================================================
ESFloat CESCIAccessor::GetXOffsetMargin()
{
	return m_fXOffsetMargin;
}
ESErrorCode CESCIAccessor::SetXOffsetMargin( ESFloat fMargin )
{
	m_fXOffsetMargin = fMargin;
	return kESErrorNoError;
}
ESFloat CESCIAccessor::GetYOffsetMargin()
{
	return m_fYOffsetMargin;
}
ESErrorCode CESCIAccessor::SetYOffsetMargin( ESFloat fMargin )
{
	m_fYOffsetMargin = fMargin;
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャンエリア設定のガイド位置を取得する
//!
//===============================================================
ESGuidePosition CESCIAccessor::GetGuidePosition()
{
	ESGuidePosition eGuidePosition = kESGuidePositionLeft;
	switch ( GetFunctionalUnitType() ){
	case kESFunctionalUnitDocumentFeeder:
		eGuidePosition = GetGuidePositionADF();
		break;
	case kESFunctionalUnitFlatbed:
	case kESFunctionalUnitTransparent:
	case kESFunctionalUnitTPUAreaGuide:
	default:
		eGuidePosition = GetGuidePositionFB();
		break;
	}
	return eGuidePosition;
}
ESGuidePosition CESCIAccessor::GetGuidePositionFB()
{
	return m_eGuidePositionFB;
}
ESGuidePosition CESCIAccessor::GetGuidePositionADF()
{
	return m_eGuidePositionADF;
}

//===============================================================
//!
//! スキャンエリア設定のガイド位置を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetGuidePositionFB( ESNumber nGuidePosition )
{
	m_eGuidePositionFB = (ESGuidePosition)nGuidePosition;
	return kESErrorNoError;
}
ESErrorCode CESCIAccessor::SetGuidePositionADF( ESNumber nGuidePosition )
{
	m_eGuidePositionADF = (ESGuidePosition)nGuidePosition;
	return kESErrorNoError;
}

//===============================================================
//!
//! bit深度を取得する(ENUM_ESCI_BIT_DEPTH)
//!
//===============================================================
ENUM_ESCI_BIT_DEPTH CESCIAccessor::GetBitDepth()
{
	return m_stParameters.un8DataFormat;
}

//===============================================================
//!
//! bit深度を設定する(ENUM_ESCI_BIT_DEPTH)
//!
//===============================================================
bool CESCIAccessor::SetBitDepth( ENUM_ESCI_BIT_DEPTH eBitDepth )
{
	m_stParameters.un8DataFormat = eBitDepth;
	return true;
}

//===============================================================
//!
//! カラーモードを取得する(ENUM_ESCI_CLOLOR_MODE)
//!
//===============================================================
ENUM_ESCI_CLOLOR_MODE CESCIAccessor::GetColorMode()
{
	return m_stParameters.un8Color;
}

//===============================================================
//!
//! カラーモードを設定する(ENUM_ESCI_CLOLOR_MODE)
//!
//===============================================================
bool CESCIAccessor::SetColorMode( ENUM_ESCI_CLOLOR_MODE eColorMode )
{
	m_stParameters.un8Color = eColorMode;
	return true;
}

//===============================================================
//!
//! サポートするカラーモードを取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedColorFormats()
{
	ESIndexSet indexSet;

	if ( m_stExtIdentity.un8OutBitDepth >= 1) {
		indexSet.insert(kESColorFormatMono1);
	}

	if (m_stExtIdentity.un8OutBitDepth >= 8) {
		indexSet.insert(kESColorFormatMono8);
		indexSet.insert(kESColorFormatRGB24);
	}

	if (m_stExtIdentity.un8OutBitDepth >= 16) {
		indexSet.insert(kESColorFormatMono16);
		indexSet.insert(kESColorFormatRGB48);
	}

	return indexSet;
}

struct stColorFormatInfo {
	UInt8		un8BitDepth;
	UInt8		un8ColorMode;
	ESNumber	nColorFormat;
};
static const stColorFormatInfo sc_listESCIColorFormats[] = {
	{ kBitDepth1,   kColorModeMono,         kESColorFormatMono1 },
	{ kBitDepth1,   kColorModeMonoDropR,    kESColorFormatMonoDropR1 },
	{ kBitDepth1,   kColorModeMonoDropG,    kESColorFormatMonoDropG1 },
	{ kBitDepth1,   kColorModeMonoDropB,    kESColorFormatMonoDropB1 },

	{ kBitDepth8,   kColorModeMono,         kESColorFormatMono8 },
	{ kBitDepth8,   kColorModeMonoDropR,    kESColorFormatMonoDropR8 },
	{ kBitDepth8,   kColorModeMonoDropG,    kESColorFormatMonoDropG8 },
	{ kBitDepth8,   kColorModeMonoDropB,    kESColorFormatMonoDropB8 },
	{ kBitDepth8,   kColorModeRGB,          kESColorFormatRGB24 },

	{ kBitDepth16,  kColorModeMono,         kESColorFormatMono16 },
	{ kBitDepth16,  kColorModeMonoDropR,    kESColorFormatMonoDropR16 },
	{ kBitDepth16,  kColorModeMonoDropG,    kESColorFormatMonoDropG16 },
	{ kBitDepth16,  kColorModeMonoDropB,    kESColorFormatMonoDropB16 },
	{ kBitDepth16,  kColorModeRGB,          kESColorFormatRGB48 },
};

//===============================================================
//!
//! カラーフォーマットを取得する
//!
//! @return ESColorFormat, 不明時: 0
//!
//===============================================================
ESNumber CESCIAccessor::GetColorFormat()
{
	ENUM_ESCI_BIT_DEPTH eBitDepth  = GetBitDepth();
	ENUM_ESCI_CLOLOR_MODE eColorMode = GetColorMode();

	for ( int i = 0; i < _countof(sc_listESCIColorFormats); i++ ){
		if ( (sc_listESCIColorFormats[i].un8BitDepth == eBitDepth) && (sc_listESCIColorFormats[i].un8ColorMode == eColorMode) ){
			return sc_listESCIColorFormats[i].nColorFormat;
		}
	}
	return 0;
}

//===============================================================
//!
//! カラーフォーマットを設定する(ESColorFormat)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetColorFormat( ESNumber nColorFormat )
{
	for ( int i = 0; i < _countof(sc_listESCIColorFormats); i++ ){
		if ( sc_listESCIColorFormats[i].nColorFormat == nColorFormat ){
			SetColorMode( sc_listESCIColorFormats[i].un8ColorMode );
			SetBitDepth ( sc_listESCIColorFormats[i].un8BitDepth  );
			return kESErrorNoError;
		}
	}
	return kESErrorInvalidParameter;
}

//===============================================================
//!
//! サンプル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetBitsPerSample()
{
	switch (m_stParameters.un8DataFormat) {
		case kBitDepth1:
			return 1;
		case kBitDepth8:
			return 8;
		case kBitDepth16:
			return 16;
		default:
			break;
	}
	return 0;
}

//===============================================================
//!
//! 1ピクセル当たりのサンプル数を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetSamplesPerPixel()
{
	switch (m_stParameters.un8Color) {
		case kColorModeRGB:
			return 3;
		case kColorModeMono:
		case kColorModeMonoDropR:
		case kColorModeMonoDropG:
		case kColorModeMonoDropB:
			return 1;
		default:
			break;
	}
	return 0;
}

//===============================================================
//!
//! 1ピクセル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetBitsPerPixel()
{
	return GetBitsPerSample() * GetSamplesPerPixel();
}

//===============================================================
//!
//! カラーマトリックスを取得する(ESFloatArray or nullptr)
//!
//===============================================================
ESAny CESCIAccessor::GetColorMatrix()
{
	return nullptr;
}

//===============================================================
//!
//! カラーマトリックスを設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetColorMatrix( ESFloatArray arColorMatrix )
{
	if ( arColorMatrix.empty() ){
		m_stParameters.un8ColorCorrection = kColorCorrectionNone;
		return kESErrorNoError;
	}
	if ( !IsDumb() ){
		m_stParameters.un8ColorCorrection = kColorCorrecitonUserDefined;
	}

	ES_COLOR_MATRIX matrix;
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			matrix[r][c] = arColorMatrix.at( r*3 + c );
		}
	}
	m_cColorMatrix.SetMatrix( matrix );
	return kESErrorNoError;
}

//===============================================================
//!
//! サポートするハーフトーンを取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedHalftones()
{
	ESIndexSet indexSet;

	indexSet.insert( kESHalftoneNone );
	indexSet.insert( kESHalftoneA );
	indexSet.insert( kESHalftoneB );
	indexSet.insert( kESHalftoneC );
	indexSet.insert( kESHalftoneTET );
	indexSet.insert( kESHalftoneDitherA );
	indexSet.insert( kESHalftoneDitherB );
	indexSet.insert( kESHalftoneDitherC );
	indexSet.insert( kESHalftoneDitherD );
	indexSet.insert( kESHalftoneUserDefinedDitherA );
	indexSet.insert( kESHalftoneUserDefinedDitherB );

	return indexSet;
}

//===============================================================
//!
//! ハーフトーンを取得する
//!
//===============================================================
ESHalftone CESCIAccessor::GetHalftones()
{
	return (ESHalftone)m_stParameters.un8HalftoneMode;
}

//===============================================================
//!
//! ハーフトーンを設定する(ESHalftone)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetHalftones( ESNumber nHalftone )
{
	m_stParameters.un8HalftoneMode = (ESHalftone)nHalftone;
	return kESErrorNoError;
}

//===============================================================
//!
//! サポートするガンマモードを取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedGammaModes()
{
	ESIndexSet indexSet;
	indexSet.insert(kESGammaMode10);
	indexSet.insert(kESGammaMode18);
	return indexSet;
}

//===============================================================
//!
//! ガンマモードを取得する
//!
//===============================================================
ESGammaMode CESCIAccessor::GetGammaMode()
{
	return m_stParameters.un8GammaCorrection;
}

//===============================================================
//!
//! ガンマモードを設定する(ESGammaMode)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetGammaMode( ESNumber nGammaMode )
{
	m_stParameters.un8GammaCorrection = (ESGammaMode)nGammaMode;
	return kESErrorNoError;
}

//===============================================================
//!
//! ガンマテーブルを取得する
//!
//===============================================================
ESAny CESCIAccessor::GetGammaTableMono()
{
	if ( !m_arGammaTableMono.empty() ){
		return m_arGammaTableMono;
	}
	return nullptr;
}
ESAny CESCIAccessor::GetGammaTableRed()
{
	if ( !m_arGammaTableRed.empty() ){
		return m_arGammaTableRed;
	}
	return nullptr;
}
ESAny CESCIAccessor::GetGammaTableGreen()
{
	if ( !m_arGammaTableGreen.empty() ){
		return m_arGammaTableGreen;
	}
	return nullptr;
}
ESAny CESCIAccessor::GetGammaTableBlue()
{
	if ( !m_arGammaTableBlue.empty() ){
		return m_arGammaTableBlue;
	}
	return nullptr;
}

//===============================================================
//!
//! ガンマテーブルを設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetGammaTable( const ESIndexArray& arGammaTable, ESGammaChannel eChannel )
{
	CESGammaTable cGammaTable;
	ES_GAMMA_TABLE& table = cGammaTable.GetGammaTable();
	for ( int i = 0; i < GAMMA_TABLE_SIZE; i++ ){
		table[i] = (ESByte)arGammaTable[i];
	}
	return RequestSetGammaTable( eChannel, cGammaTable );
}
ESErrorCode CESCIAccessor::SetGammaTableMono( ESIndexArray arGammaTable )
{
	assert( arGammaTable.size() >= GAMMA_TABLE_SIZE );

	m_arGammaTableMono = arGammaTable;
	return kESErrorNoError;
}
ESErrorCode CESCIAccessor::SetGammaTableRed( ESIndexArray arGammaTable )
{
	assert( arGammaTable.size() >= GAMMA_TABLE_SIZE );

	m_arGammaTableRed = arGammaTable;
	return kESErrorNoError;
}
ESErrorCode CESCIAccessor::SetGammaTableGreen( ESIndexArray arGammaTable )
{
	assert( arGammaTable.size() >= GAMMA_TABLE_SIZE );

	m_arGammaTableGreen = arGammaTable;
	return kESErrorNoError;
}
ESErrorCode CESCIAccessor::SetGammaTableBlue( ESIndexArray arGammaTable )
{
	assert( arGammaTable.size() >= GAMMA_TABLE_SIZE );

	m_arGammaTableBlue = arGammaTable;
	return kESErrorNoError;
}

ESErrorCode CESCIAccessor::RequestSetGammaTables()
{
	ESErrorCode err = kESErrorNoError;

	if ( !m_arGammaTableMono.empty() ){
		err = SetGammaTable( m_arGammaTableMono, kESGammaChannelMaster );
		m_arGammaTableMono.clear();
		if ( IS_ERR_CODE(err) ){
			goto BAIL;
		}
	}
	if ( !m_arGammaTableRed.empty() ){
		err = SetGammaTable( m_arGammaTableRed, kESGammaChannelRed );
		m_arGammaTableRed.clear();
		if ( IS_ERR_CODE(err) ){
			goto BAIL;
		}
	}
	if ( !m_arGammaTableGreen.empty() ){
		err = SetGammaTable( m_arGammaTableGreen, kESGammaChannelGreen );
		m_arGammaTableGreen.clear();
		if ( IS_ERR_CODE(err) ){
			goto BAIL;
		}
	}
	if ( !m_arGammaTableBlue.empty() ){
		err = SetGammaTable( m_arGammaTableBlue, kESGammaChannelBlue );
		m_arGammaTableBlue.clear();
		if ( IS_ERR_CODE(err) ){
			goto BAIL;
		}
	}
BAIL:
	return err;
}

//===============================================================
//!
//! サポートするモノクロ閾値を取得する( ESIndexSet or ST_ES_RANGE )
//!
//===============================================================
ESAny CESCIAccessor::GetSupportedThreshold()
{
	ST_ES_RANGE stRange;
	stRange.nMin = 0;
	stRange.nMax = 255;
	stRange.nStep = 1;

	return stRange;
}

//===============================================================
//!
//! モノクロ閾値を取得する
//! 
//===============================================================
ESNumber CESCIAccessor::GetThreshold()
{
	if ( GetBitDepth() != 1) {
		return 0;
	}
	return m_stParameters.un8Threshold;
}

//===============================================================
//!
//! モノクロ閾値を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetThreshold( ESNumber nThreshold )
{
	if ( GetBitDepth() != 1) {
		return kESErrorSequenceError;
	}
	m_stParameters.un8Threshold = (UInt8)nThreshold;
	return kESErrorNoError;
}

//===============================================================
//!
//! オートフォーカスを要求する。
//!
//! @param  pfOutFocus オートフォーカスの結果として焦点位置を受け取るバッファへのポインタ。単位はmm。
//! @return エラーコード。
//!
//===============================================================
#define WAIT_AUTOFOCUS 30000 // 30seconds

#define FOCUS_MAX 6.0
#define FOCUS_MIN (-2.0)

ESErrorCode CESCIAccessor::DoAutoFocus( ESFloat* pfOutFocus )
{
	ESErrorCode err = kESErrorNoError;
	SYNCHRONIZED_START
	{
		// Since the auto focus depends on the resolution, the area and so on,
		// we send the parameters before requesting auto focus.
		err = SetScanningParameters();
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
		err = RequestSetFocus( kESFocusAuto );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
		Sleep(WAIT_AUTOFOCUS);
	}
	SYNCHRONIZED_END

	if ( pfOutFocus ) {
		ESFocus focus = kESFocusDefault;
		err = RequestFocus( focus );
		if ( IS_ERR_CODE(err) ){
			goto BAIL;
		}
		*pfOutFocus = (ESFloat)FOCUS_COMMAND_TO_PHYSICAL(focus);
	}
BAIL:
	return err;
}

//===============================================================
//!
//! フォーカスを取得する
//!
//===============================================================
ESFloat CESCIAccessor::GetFocus()
{
	ESFocus focus = 0;
	if ( RequestFocus( focus ) != kESErrorNoError ){
		focus = 0;
	}
	return (ESFloat)FOCUS_COMMAND_TO_PHYSICAL(focus);
}
ESFloat CESCIAccessor::GetMaxFocus()
{
	return FOCUS_MAX;
}

ESFloat CESCIAccessor::GetMinFocus()
{
	return FOCUS_MIN;
}

//===============================================================
//!
//! フォーカスを設定する(ESFocus)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetFocus( ESFloat fFocus )
{
	if ( fFocus < GetMinFocus() ){
		fFocus = GetMinFocus();
	} else if ( fFocus > GetMaxFocus() ){
		fFocus = GetMaxFocus();
	}

	ESFocus eCommandFocus = (ESFocus)FOCUS_PHYSICAL_TO_COMMAND(fFocus);
	if ( eCommandFocus >= kESFocusAuto ){
		eCommandFocus = kESFocusAuto-1;
	}
	return RequestSetFocus( eCommandFocus );
}

//===============================================================
//!
//! RequestScanningParameterにセットするパラメーターを作成する
//!
//===============================================================
ESErrorCode CESCIAccessor::CreateScanningParametersParam( __out ST_ESCI_SCANNING_PARAMETER& stOutScanningParameter )
{
	ES_LOG_TRACE_FUNC();

	try {
		stOutScanningParameter  = m_stParameters;
		ST_ES_RECT_UN32 rcScanAreaInPixel = GetScanAreaInPixel();

		stOutScanningParameter.un32XOffset  = rcScanAreaInPixel.left;
		stOutScanningParameter.un32YOffset  = rcScanAreaInPixel.top;
		stOutScanningParameter.un32Width    = rcScanAreaInPixel.right - rcScanAreaInPixel.left;
		stOutScanningParameter.un32Height   = rcScanAreaInPixel.bottom - rcScanAreaInPixel.top;

		if ( GetBitsPerPixel() == 1 ){
			// Adjust width for 1 bit scan because it should be a multiple of 8
			stOutScanningParameter.un32Width = ((stOutScanningParameter.un32Width + 7) / 8) * 8;
		}

		// Set max lines per block according to buffer size
		ESNumber nBufferSize        = GetBufferSize();
		ESNumber nBytesPerRow       = ESCIGetBytesPerRow(stOutScanningParameter.un32Width, GetBitsPerPixel());
		ESNumber nBlockLineNumber   = ( (nBytesPerRow != 0) ? (nBufferSize / nBytesPerRow) : 0 );

		// 1ライン当たりのバイト数が、ファームウェアの限界を超えているかどうかのチェック
		if ( stOutScanningParameter.un32Width > m_stExtIdentity.un32MaxWidthPixels ){
			return kESErrorScanAreaTooLargeError;
		}

		if (nBlockLineNumber > 0xFF) {
			// Cannot represent more than 255 because the parameter size is 1 byte.
			nBlockLineNumber = 0xFF;
		}

		// 特定の機種（Nutmeg等）で、偶数のライン数のみをサポートするものが存在するため、必ず偶数にする
		if ( nBlockLineNumber > 1 && nBlockLineNumber % 2 != 0) {
			nBlockLineNumber -= 1;
		}

		stOutScanningParameter.un8BlockLineNumber = (UInt8)nBlockLineNumber;

		return kESErrorNoError;
	} catch(...){
		ES_LOG_EXCEPTION_ERROR();
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//! RequestScanningParameterを呼び出します
//!
//===============================================================
ESErrorCode CESCIAccessor::SetScanningParameters()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = RequestSetGammaTables();
	if ( IS_ERR_CODE( err ) ){
		return err;
	}

	if ( !IsDumb() ){
		err = RequestSetColorMatrix( m_cColorMatrix.GetMatrix() );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
	}

	ST_ESCI_SCANNING_PARAMETER parameters_temp = {};
	err = CreateScanningParametersParam( parameters_temp );
	if ( IS_ERR_CODE( err ) ){
		return err;
	}
	return RequestSetScanningParameters( parameters_temp );
}

//===============================================================
//!
//! サポートするDigital ICE設定を取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedDigitalICEs()
{
	ESIndexSet types;

	types.insert(kESDigitalICEDisable);
	types.insert(kESDigitalICEEnable);
	types.insert(kESDigitalICELiteEnable);

	return types;
}

//===============================================================
//!
//! Digital ICEの設定を取得する(ESDigitalICE)
//!
//===============================================================
ESDigitalICE CESCIAccessor::GetDigitalICE()
{
	return m_eDigitalICE;
}

//===============================================================
//!
//!　Digital ICEの種類を設定する(ESDigitalICE)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetDigitalICE( ESNumber nDigitalICE )
{
	m_eDigitalICE = (ESDigitalICE)nDigitalICE;
	return kESErrorNoError;
}

//===============================================================
//!
//! 静音モードをサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsQuietModeSupported()
{
	return (m_stExtIdentity.un8MainStatus2 & kIDMainStatusQuietSupport) ? true : false;
}

//===============================================================
//!
//! サポートする静音モードを取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedQuietModes()
{
	ESIndexSet indexSet;

	if ( !IsQuietModeSupported() ) {
		return indexSet;
	}

	indexSet.insert(kESQuietModeOff);
	indexSet.insert(kESQuietModeOn);
	indexSet.insert(kESQuietModePreferDeviceSetting);

	return indexSet;
}

//===============================================================
//!
//!　静音モードを取得する (ESQuietMode)
//!
//===============================================================
ESQuietMode CESCIAccessor::GetQuietMode()
{
	if ( !IsQuietModeSupported() ) {
		return kESQuietModeOff;
	}
	return m_stParameters.un8QuietMode;
}

//===============================================================
//!
//! 静音モードを設定する (ESQuietMode)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetQuietMode( ESNumber nQuietMode )
{
	if ( !IsQuietModeSupported() ) {
		return kESErrorSequenceError;
	}
	m_stParameters.un8QuietMode = (ESQuietMode)nQuietMode;
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
ESScanningMode CESCIAccessor::GetScanningMode()
{
	return (ESScanningMode)m_stParameters.un8ScanningMode;
}
//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCIAccessor::SetScanningMode( ESNumber nScanningMode)
{
	m_stParameters.un8ScanningMode = (UInt8)nScanningMode;
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsDocumentSizeDetectionSupported()
{
	UInt16 un16DocumentSize = 0;
	if ( GetFunctionalUnitType() == kESFunctionalUnitDocumentFeeder ){
		un16DocumentSize = m_stScannerStatus.un16AdfPaperSize;
	} else {
		un16DocumentSize = m_stScannerStatus.un16MainPaperSize;
	}
	if ( un16DocumentSize > 0 ){
		return true;
	}
	return false;
}

//===============================================================
//!
//! 検知するドキュメントサイズを取得する
//!
//===============================================================
ST_ES_SIZE_F CESCIAccessor::GetDetectedDocumentSize()
{
	const ST_ES_SIZE_F stZeroSize = MakeSize(0.0f, 0.0f);

	struct stDocumentInfo{
		UInt16			un16DocumentSizeType;
		ST_ES_SIZE_F	stfSizeDocument;
	};
	const stDocumentInfo c_listDocumentInfo[] = {
		{ kDocumentSizeLetterLandscape,    MakeSize(11.0f, 8.5f)    },
		{ kDocumentSizeLetterPortrait,     MakeSize(8.5f, 11.0f)    },
		{ kDocumentSizeA4Landscape,        MakeSize(11.69f, 8.27f)  },
		{ kDocumentSizeA4Portrait,         MakeSize(8.27f, 11.69f)  },
		{ kDocumentSizeLegalPortrait,      MakeSize(8.5f, 14.0f)    },
		{ kDocumentSizeB4Portrait,         MakeSize(10.12f, 14.33f) },
		{ kDocumentSizeDoubleLetter,       MakeSize(11.0f, 17.0f)   },
		{ kDocumentSizeA3Portrait,         MakeSize(11.69f, 16.54f) },
		{ kDocumentSizeExecutiveLandscape, MakeSize(10.5f, 7.25f)   },
		{ kDocumentSizeExecutivePortrait,  MakeSize(7.25f, 10.5f)   },
		{ kDocumentSizeA5Landscape,        MakeSize(8.27f, 5.83f)   },
		{ kDocumentSizeA5Portrait,         MakeSize(5.83f, 8.27f)   },
		{ kDocumentSizeB5Landscape,        MakeSize(10.12f, 7.17f)  },
		{ kDocumentSizeB5Portrait,         MakeSize(7.17f, 10.12f)  },
		{ kDocumentSizeOther,              stZeroSize },
		{ kDocumentSizeReserved,           stZeroSize },
	};

	ST_ESCI_SCANNER_STATUS status = {};
	ESErrorCode err = RequestStatus( status );
	if ( err != kESErrorNoError ){
		return stZeroSize;
	}

	UInt16 un16DocumentSize = 0;
	if ( GetFunctionalUnitType() == kESFunctionalUnitDocumentFeeder ) {
		un16DocumentSize = status.un16AdfPaperSize;
	} else {
		un16DocumentSize = status.un16MainPaperSize;
	}

	for ( int i = 0; i < _countof(c_listDocumentInfo); i++ ){
		if ( c_listDocumentInfo[i].un16DocumentSizeType == un16DocumentSize ){
			return c_listDocumentInfo[i].stfSizeDocument;
		}
	}
	return stZeroSize;
}

//===============================================================
//!
//! フィーダー(ADF)機能をサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsFeederSupported()
{
	return (m_stScannerStatus.un8AdfStatus & kADFStatusInsatlled) ? true : false;
}

//===============================================================
//!
//! フィーダー(ADF)機能が有効か？
//!
//===============================================================
bool CESCIAccessor::IsFeederEnabled()
{
	if (!IsFeederSupported() ){
		return false;
	}
	return (m_stParameters.un8OptionControl != kOptionControlDisable);
}

//===============================================================
//!
//! オートスキャン機能をサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsAutoScanSupported()
{
	return (m_stExtIdentity.un8MainStatus2 & kIDMainStatusAutoScanSupport) ? true : false;
}

//===============================================================
//!
//! オートスキャン機能が有効か?
//!
//===============================================================
bool CESCIAccessor::IsAutoScanEnabled()
{
	return (m_stParameters.un8AutoScan == kAutoScanEnable);
}

//===============================================================
//!
//! オートスキャン機能の有効状態を設定する
//!
//===============================================================
void CESCIAccessor::SetAutoScanEnabled( bool bAutoScanEnabled )
{
	if ( !IsAutoScanSupported() ) {
		return;
	}
	if ( bAutoScanEnabled ) {
		m_stParameters.un8AutoScan = kAutoScanEnable;
	} else {
		m_stParameters.un8AutoScan = kAutoScanDisable;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsPageFeedTypeDocumentFeeder()
{
	return ( m_stExtIdentity.un8MainStatus & kIDMainStatusPageTypeADF ) ? true : false;
}

//===============================================================
//!
//! 両面同時読み取りのタイプを取得する
//!
//===============================================================
ESDuplexType CESCIAccessor::GetDuplexType()
{
	if ( m_stExtIdentity.un8MainStatus & kIDMainStatusNotFlatbed ) {
		return kESDuplexType1Pass;
	}
	return kESDuplexType2Pass;
}

//===============================================================
//!
//! 両面同時読み取りをサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsDuplexSupported()
{
    return (m_stExtIdentity.un8MainStatus & kIDMainStatusDuplexADF) ? true : false;
}

//===============================================================
//!
//! 両面同時読み取り機能が有効か?
//!
//===============================================================
bool CESCIAccessor::IsDuplexEnabled()
{
	if ( !IsDuplexSupported() ){
		return false;
	}
    return (m_stParameters.un8OptionControl == kOptionControlEnableADFDuplex);
}

//===============================================================
//!
//! 両面同時読み取り機能の有効状態を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetDuplexEnabled( bool bDuplexEnabled )
{
	if ( !IsDuplexSupported() ){
		return kESErrorSequenceError;
	}
	if ( !IsFeederEnabled() ){
		return kESErrorSequenceError;
	}
	m_stParameters.un8OptionControl = bDuplexEnabled ? kOptionControlEnableADFDuplex : kOptionControlEnable;
	return kESErrorNoError;
}

//===============================================================
//!
//! 重送検知機能をサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsDoubleFeedDetectionSupported()
{
	return (m_stExtIdentity.un8MainStatus2 & kIDMainStatusDoubleFeedDetectionSupport) ? true : false;
}

//===============================================================
//!
//! サポートしている重送検知を取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedDoubleFeedDetections()
{
	ESIndexSet indexSet;

	if ( !IsDoubleFeedDetectionSupported() ){
		return indexSet;
	}
	indexSet.insert(kESDoubleFeedDetectionDisable);
	indexSet.insert(kESDoubleFeedDetectionLow);
	indexSet.insert(kESDoubleFeedDetectionHigh);

	return indexSet;
}

//===============================================================
//!
//! 重送検知の設定を取得する
//!
//===============================================================
ESDoubleFeedDetection CESCIAccessor::GetDoubleFeedDetection()
{
	if ( !IsDoubleFeedDetectionSupported() ){
		return kESDoubleFeedDetectionDisable;
	}
	return m_stParameters.un8DoubleFeedDetection;
}

//===============================================================
//!
//! 重送検知を設定を変更する(ESDoubleFeedDetection)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetDoubleFeedDetection( ESNumber nDoubleFeedDetection )
{
	if ( !IsDoubleFeedDetectionSupported() ){
		return kESErrorSequenceError;
	}
	m_stParameters.un8DoubleFeedDetection = (UInt8)nDoubleFeedDetection;
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsPaperEndDetectionSupported()
{
	return ( m_stExtIdentity.un8MainStatus2 & kIDMainStatusPaperEndDetectionSupport) ? true : false;
}
//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsPaperEndDetectionEnabled()
{
	if ( !IsPaperEndDetectionSupported() ){
		return false;
	}
	return m_bPaperEndDetectionEnabled;
}
//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCIAccessor::SetPaperEndDetectionEnabled( bool bPperEndDetectionEnabled )
{
	if ( !IsPaperEndDetectionSupported() ){
		return kESErrorSequenceError;
	}
	m_bPaperEndDetectionEnabled = bPperEndDetectionEnabled;
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsOverScanEnabled()
{
	return m_bOverScanEnabled;
}
//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCIAccessor::SetOverScanEnabled( bool bOverScanEnabled )
{
	m_bOverScanEnabled = bOverScanEnabled;
	return kESErrorNoError;
}

//===============================================================
//!
//!ドキュメントが読み込まれたか？
//!
//===============================================================
bool CESCIAccessor::IsDocumentLoaded()
{
	ST_ESCI_SCANNER_STATUS status = {};
	if ( RequestStatus(status) != kESErrorNoError) {
		return false;
	}
	return !(status.un8AdfStatus & kADFStatusPaperEmpty);
}


//===============================================================
//!
//! スキャンするページ枚数を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetPagesToBeScanned()
{
	return m_nPagesToBeScanned;
}

//===============================================================
//!
//! スキャンするページ枚数を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetPagesToBeScanned( ESNumber nPagesToBeScanned )
{
	m_nPagesToBeScanned = nPagesToBeScanned;
	if ( nPagesToBeScanned == 0) {
		SetAutoScanEnabled( true );
	} else {
		SetAutoScanEnabled( false );
	}
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsPrefeed()
{
	return (m_stExtIdentity.un8MainStatus2 & kIDMainStatusAutoFormFeed) ? true : false;
}

//===============================================================
//!
//! TPU(透過読み取り)をサポートしているか？
//!
//===============================================================
bool CESCIAccessor::IsTransparencyUnitSupported()
{
	return (m_stScannerStatus.un8TpuStatus & kTPUStatusInstalled) ? true : false;
}

//===============================================================
//!
//! TPUエリアガイドをサポートしているか？
//!
//===============================================================
bool CESCIAccessor::IsTransparencyUnit2ndAreaSupported()
{
	return (m_stScannerStatus.un8TpuStatus2 & kTPUStatusInstalled) ? true : false;
}

//===============================================================
//!
//! 赤外線ランプをサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsInfraRedLampSupported()
{
	return (m_stExtIdentity.un8MainStatus & kIDMainStatusInfraRedSupport) ? true : false;
}

//===============================================================
//!
//!
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedFilmTypes()
{
	ESIndexSet indexSet;
	if ( IsTransparencyUnitSupported() ){
		indexSet.insert(kESFilmTypePositive);
		indexSet.insert(kESFilmTypeNegative);
	}
	return indexSet;
}

//===============================================================
//!
//!
//!
//===============================================================
ESFilmType CESCIAccessor::GetFilmType()
{
	return (ESFilmType)m_stParameters.un8FilmType;
}

//===============================================================
//!
//! (ESFilmType)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetFilmType( ESNumber nFilmType )
{
	m_stParameters.un8FilmType = (ESFilmType)nFilmType;
	return kESErrorNoError;
}

//===============================================================
//!
//! フラットベッドをサポートしているか？
//!
//===============================================================
bool CESCIAccessor::IsFlatbedSupported()
{
	return !(m_stExtIdentity.un8MainStatus & kIDMainStatusNotFlatbed) ? true : false;
}

//===============================================================
//!
//! ランプ切り替え機能をサポートしているか?
//!
//===============================================================
bool CESCIAccessor::IsLampChangeSupported()
{
	return (m_stExtIdentity.un8MainStatus & kIDMainStatusLampChangeSupport) ? true : false;
}

//===============================================================
//!
//! サポートするランプ切り替え機能を取得する
//!
//===============================================================
ESIndexSet CESCIAccessor::GetSupportedLampModes()
{
	ESIndexSet indexSet;
	if ( !IsLampChangeSupported() ){
		return indexSet;
	}
	indexSet.insert(kESLampModeDefault);
	indexSet.insert(kESLampModeUseLamp1);
	indexSet.insert(kESLampModeUseLamp2);

	return indexSet;
}

//===============================================================
//!
//! 現在のランプ切り替えモードを取得する
//!
//===============================================================
ESLampMode CESCIAccessor::GetLampMode()
{
	return (ESLampMode)m_stParameters.un8LampMode;
}

//===============================================================
//!
//! ランプ切り替えモードを設定する(ESLampMode)
//!
//===============================================================
ESErrorCode CESCIAccessor::SetLampMode( ESNumber nLampMode )
{
	m_stParameters.un8LampMode = (UInt8)nLampMode;
	return kESErrorNoError;
}

//===============================================================
//!
//!	クリーニング機能をサポートするか
//!
//===============================================================
bool CESCIAccessor::IsCleaningSupported()
{
	// ESCIではfalse
	return false;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIAccessor::IsCalibrationSupported()
{
	// ESCIではfalse
	return false;
}

//===============================================================
//!
//! 光量補正機能をサポートしているか？
//!
//===============================================================
ESAny CESCIAccessor::GetSupportedLightIntensitiesFB()
{
	ESAny	anyValue;
	if (m_stExtIdentity.un8MainStatus3 & kIDMainStatusLightIntensitySupport) {
		ST_ES_RANGE		range;
		range.nMax = 15000;
		range.nMin = 5000;
		range.nStep = 1;
		anyValue = range;
	}
	return anyValue;
}

//===============================================================
//!
//! 光量補正係数を取得する
//!
//===============================================================
ESNumber CESCIAccessor::GetLightIntensityFB()
{
	if (GetSupportedLightIntensitiesFB().empty()) {
		return 10000;	// default value(100%)
	}
	return m_stParameters.un8QuietMode;
}

//===============================================================
//!
//! 光量補正係数を設定する
//!
//===============================================================
ESErrorCode CESCIAccessor::SetLightIntensityFB(ESNumber nLightIntensity)
{
	if (GetSupportedLightIntensitiesFB().empty()) {
		return kESErrorSequenceError;
	}
	if (nLightIntensity < 5000 || 15000 < nLightIntensity) {
		//小さくても大きくても、範囲外の場合は100%にする
		nLightIntensity = 10000;
	}
	m_stParameters.un16LightIntensity = (UInt16)nLightIntensity;
	return kESErrorNoError;
}

//
// Maintenance
//
///////////////////////////////////////////////////////////////////////////////////////
//===============================================================
//!
//! スキャナセンサーのクリーニング動作を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::DoCleaning()
{
	const int WAIT_INTERVAL = 1000;   // 1 second

	ESErrorCode err = kESErrorNoError;

	ST_ESCI_MAINTENANCE_PARAMETER param = {0};
	param.un16Mode = kMaintenanceCleaning;

	SYNCHRONIZED_START
	{
		err = RequestMaintenanceWithParameter( param );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
		while ( RequestMaintenanceStatus() == kESErrorDeviceInUse ){
			Sleep(WAIT_INTERVAL);
		}
	}
	SYNCHRONIZED_END

BAIL:
	return err;
}

//===============================================================
//!
//! スキャナセンサーのキャリブレーションを要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCIAccessor::DoCalibration()
{
	const int WAIT_INTERVAL = 1000;   // 1 second

	ESErrorCode err = kESErrorNoError;

	ST_ESCI_MAINTENANCE_PARAMETER param = {0};
	param.un16Mode = kMaintenanceCalibration;

	SYNCHRONIZED_START
	{
		err = RequestMaintenanceWithParameter( param );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}
		while ( RequestMaintenanceStatus() == kESErrorDeviceInUse ){
			Sleep(WAIT_INTERVAL);
		}
	}
	SYNCHRONIZED_END

BAIL:
	return err;
}

ESNumber CESCIAccessor::GetLamp1Counter()
{
	UInt32 value = 0;
	RequestReadLogForFunction(kLogFunctionLamp1Counter, value);
	return (ESNumber)value;
}

ESErrorCode CESCIAccessor::SetLamp1Counter(ESNumber nLamp1Counter)
{
	return RequestWriteLogForFunction(kLogFunctionLamp1Counter, (UInt32)nLamp1Counter);
}

ESNumber CESCIAccessor::GetLamp2Counter()
{
	UInt32 value = 0;
	RequestReadLogForFunction(kLogFunctionLamp2Counter, value);
	return (ESNumber)value;
}

ESErrorCode CESCIAccessor::SetLamp2Counter(ESNumber nLamp2Counter)
{
	return RequestWriteLogForFunction(kLogFunctionLamp2Counter, (UInt32)nLamp2Counter);
}

ESNumber CESCIAccessor::GetSimplexScanCounter()
{
	UInt32 value = 0;
	RequestReadLogForFunction(kLogFunctionSimplexScanCounter, value);
	return (ESNumber)value;
}

ESErrorCode CESCIAccessor::SetSimplexScanCounter(ESNumber nSimplexScanCounter)
{
	return RequestWriteLogForFunction(kLogFunctionSimplexScanCounter, (UInt32)nSimplexScanCounter);
}

ESNumber CESCIAccessor::GetDuplexScanCounter()
{
	UInt32 value = 0;
	RequestReadLogForFunction(kLogFunctionDuplexScanCounter, value);
	return (ESNumber)value;
}

ESErrorCode CESCIAccessor::SetDuplexScanCounter(ESNumber nDuplexScanCounter)
{
	return RequestWriteLogForFunction(kLogFunctionDuplexScanCounter, (UInt32)nDuplexScanCounter);
}

ESNumber CESCIAccessor::GetPickupRollerCounter()
{
	UInt32 value = 0;
	RequestReadLogForFunction(kLogFunctionPickupRollerCounter, value);
	return (ESNumber)value;
}

ESErrorCode CESCIAccessor::SetPickupRollerCounter(ESNumber nPickupRollerCounter)
{
	return RequestWriteLogForFunction(kLogFunctionPickupRollerCounter, (UInt32)nPickupRollerCounter);
}

ESNumber CESCIAccessor::GetScanCounter()
{
	UInt32 value = 0;
	RequestReadLogForFunction(kLogFunctionScanCounter, value);
	return (ESNumber)value;
}

ESErrorCode CESCIAccessor::SetScanCounter(ESNumber nScanCounter)
{
	return RequestWriteLogForFunction(kLogFunctionScanCounter, (UInt32)nScanCounter);
}
