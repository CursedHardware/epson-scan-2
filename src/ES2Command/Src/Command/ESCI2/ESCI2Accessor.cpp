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
#include <boost/foreach.hpp>
#include "CommonUtility/utils/stringutils.h"
#include "CommonUtility/utils/ESAnyCastUtils.h"
#include "Utils/ESUtils.h"

#include "ESCI2Accessor.h"
#include "../ESScannerDefs.h"
#include "Image/ESCI2ScannedImage.h"

#include "Utils/Endian.h"
#include "Utils/ESUtils.h"

#ifdef WIN32
#include <process.h>
#else
#include "Platform.h"
#endif

#ifndef WIN32
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <time.h>
#define DATEFORMAT "%Y-%m-%d %H:%M"

//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESCI2Accessor::CESCI2Accessor()
{
	m_bIsAfmEnabled = false;
	m_nAfmTimeout_s = 0;

	m_eFunctionalUnitType = kESFunctionalUnitUnknown;
	m_eColorMatrixMode = kESColorMatrixUnit;
	m_bCaptureCommandSupport = true;

	m_eJobMode = kESJobModeNone;
	m_bIsDisableJobContinue = false;
	m_bIsDisableKeepingCapture = false;

	m_strAuthUsername = ES_STRING("");
	m_strAuthPassword = ES_STRING("");
	m_bIsAuthenticationEnabled = true;

	m_fXOffsetMargin = 0.0f;
	m_fYOffsetMargin = 0.0f;

	m_bIsInterrutionEnabled = true;

	m_bIsPushScanReady = false;
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESCI2Accessor::~CESCI2Accessor()
{
}

//===============================================================
//!
//! 初期化処理
//!
//===============================================================
ESErrorCode CESCI2Accessor::Initialize()
{
	return CESCI2Command::Initialize();
}

//
//
// スキャナ操作
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! 物理インターフェースのOpenおよびスキャナから情報を取得し、スキャナの使用を開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::Open()
{
	ES_LOG_TRACE_FUNC();

	if (IsOpened())
	{
		return kESErrorNoError;
	}

	// デバイスを開く
	//--------------------------------------
	ESErrorCode err = OpenDevice();
	if (IS_ERR_CODE(err))
	{
		if ((err != kESErrorDeviceInUse) &&
			(err != kESErrorDeviceInBusy))
		{
			err = kESErrorDeviceOpenError;
		}
		return err;
	}

	// デバイス情報取得
	//-------------------------------------
	{
		err = SetMode(kModeInquiry);
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = GetInfo();
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = GetExtInfo();
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = GetCapabilities();
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = SetMode(kModeNone);
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
	}
	{
		err = SetMode(kModeMaintenance);
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = GetMaintenanceInfo();
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = GetMaintenanceCapabilities();
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		if (!GetSupportedFirstPCConnectionDates().empty())
		{
#ifdef WIN32
			time_t t = time(NULL);
			struct tm date = {};
			errno_t err = gmtime_s(&date, &t);
			if (!err)
			{
				char buf[64] = {};
				if (strftime(buf, sizeof(buf), DATEFORMAT, &date) > 0)
				{
					SetFirstPCConnectionDate(ES_CMN_FUNCS::STRING::MBStringToUnicode(&buf[0]));
				}
			}
#else
#ifdef __STDC_LIB_EXT1__
			time_t t = time(NULL);
			struct tm date = {};
			struct tm *ret = gmtime_s(&t, &data);
			if (ret)
			{
				char buf[64] = {};
				if (strftime(buf, sizeof(buf), DATEFORMAT, &date) > 0)
				{
					SetFirstPCConnectionDate(&buf[0]);
				}
			}
#endif
#endif
		}
		ESDictionaryA dicResults;
		err = GetMaintenanceResults(dicResults, NULL);
		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
		err = SetMode(kModeNone);

		if (IS_ERR_CODE(err))
		{
			goto EXIT;
		}
	}

	if (!IsDisableKeepingCapture())
	{
		if (IsShouldKeepControlMode() || IsPushScanReadySupported())
		{
			SetMode(kModeControl);

			// FS Xだけでは他のクライアントとの排他ができないので、STATを送って#nrdや#errが返らないかをチェックする
			err = GetStatus();
			if (IS_ERR_CODE(err))
			{
				goto EXIT;
			}
		}
	}

	err = Setup();
	if (IS_ERR_CODE(err))
	{
		goto EXIT;
	}

	// ボタン監視開始
	StartButtonChecking();

EXIT:
	if (IS_ERR_CODE(err))
	{
		Close();
	}
	return err;
}

static ESNumber GetFirstIndex(const ESAny &anyIn)
{
	const ESIndexSet *pIndexSet = SafeAnyDataCPtr<ESIndexSet>(anyIn);
	if (pIndexSet)
	{
		ESIndexSet::const_iterator it = pIndexSet->begin();
		if (it != pIndexSet->end())
		{
			return *it;
		}
	}
	const ST_ES_RANGE *pRange = SafeAnyDataCPtr<ST_ES_RANGE>(anyIn);
	if (pRange)
	{
		return pRange->nMin;
	}
	return 0;
}

ESErrorCode CESCI2Accessor::Setup()
{
	ESErrorCode err = kESErrorNoError;
	{
		ESIndexSet indexSupported = GetSupportedFunctionalUnitTypes();
		if (indexSupported.find(kESFunctionalUnitFlatbed) != indexSupported.end())
		{

			err = SetFunctionalUnitType(kESFunctionalUnitFlatbed);
		}
		else if (indexSupported.find(kESFunctionalUnitDocumentFeeder) != indexSupported.end())
		{

			err = SetFunctionalUnitType(kESFunctionalUnitDocumentFeeder);
		}
		else
		{
			if (indexSupported.size() > 0)
			{
				ESIndexSet::iterator it = indexSupported.begin();
				err = SetFunctionalUnitType((ESFunctionalUnitType)*it);
			}
		}
	}
	{
		// デフォルトの解像度として、機種がサポートする最低解像度を入れる。
		SetXResolution(GetFirstIndex(GetSupportedXResolutions()));
		SetYResolution(GetFirstIndex(GetSupportedYResolutions()));
	}
	return err;
}

//===============================================================
//!
//! 物理インターフェースをCloseし、スキャナの使用を終了する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::Close()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;
	SYNCHRONIZED_START
	{
		StopButtonChecking(); // ボタン監視終了

		if (!IsOpened())
		{
			err = kESErrorNoError;
			goto BAIL;
		}

		SetMode(kModeNone);

		err = CloseDevice();
	}
	SYNCHRONIZED_END

BAIL:
	ES_TRACE_LOG(ES_STRING("Leave Close"));
	return err;
}

//===============================================================
//!
//! スキャナがオープンしているか？
//!
//===============================================================
bool CESCI2Accessor::IsOpened() const
{
	return IsDeviceOpened();
}

//===============================================================
//!
//! デバイス接続切断時の処理
//!
//===============================================================
void CESCI2Accessor::DeviceDisconnected()
{
	// ボタン監視終了
	StopButtonChecking();
}

//===============================================================
//!
//! スキャナーパネルのプッシュスキャン可能状態を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPanelToPushScanReady(BOOL bPushScanReady)
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;
	bool bPushScanReady_ = bPushScanReady ? true : false;

	if (IsPushScanReadySupported())
	{
		if (GetMode() != kModeControl)
		{
			goto BAIL;
		}
		if (bPushScanReady_ == m_bIsPushScanReady)
		{
			goto BAIL;
		}
		err = RequestPanelToPushScanReady(bPushScanReady_);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
		m_bIsPushScanReady = bPushScanReady_;
	}
BAIL:
	return err;
}

//===============================================================
//!
//! �Ǘ��҃��b�N�̉���
//!
//===============================================================
ESErrorCode CESCI2Accessor::UnlockAdministratorLock()
{
	ES_LOG_TRACE_FUNC();
	ESErrorCode err = kESErrorNoError;
	m_bIsAdminLockUnlocked = true;
	if (IsAdminLockSupported())
	{
		if (IsAdminLockEnabled())
		{

			ESCI2Mode eCurrentMode = GetMode();
			err = SetMode(kModeMaintenance);
			if (IS_ERR_CODE(err))
			{
				m_bIsAdminLockUnlocked = false;
				return err;
			}

			err = RequestAdministratorLock(false, GetAdminLockPassword());
			if (IS_ERR_CODE(err))
			{
				m_bIsAdminLockUnlocked = false;
			}

			ESErrorCode err2 = SetMode(eCurrentMode);
			if (IS_ERR_SUCCESS(err) && IS_ERR_CODE(err2))
			{
				m_bIsAdminLockUnlocked = false;
				err = err2;
			}
		}
	}
	return err;
}

ESErrorCode CESCI2Accessor::LockAdministratorLock()
{
	ES_LOG_TRACE_FUNC();
	ESErrorCode err = kESErrorNoError;
	m_bIsAdminLockUnlocked = false;
	return err;
}

bool CESCI2Accessor::RequestingUnlockAdminLock()
{
	return m_bIsAdminLockUnlocked;
}

BOOL CESCI2Accessor::IsScannableDeviceConfig()
{
	ESIndexArray status = GetUnavailableScanParameterStatus();
	BOOST_FOREACH (ESNumber value, status)
	{
		switch (value)
		{
		case kESUnavailableScanParameterStatusAll:
			return FALSE;

		case kESUnavailableScanParameterStatusAFMC:
			if (GetJobMode() == kESJobModeAFMC)
			{
				return FALSE;
			}
			break;

		case kESUnavailableScanParameterStatusPCS:
			if (IsPassportCarrierSheetEnabled())
			{
				return FALSE;
			}
			break;

		default:
			break;
		}
	}
	return TRUE;
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
ESErrorCode CESCI2Accessor::CaptureScanner()
{
	ES_LOG_TRACE_FUNC();

	return SetMode(kModeControl);
}

//===============================================================
//!
//! Release scanner to control mutual exclusive access for the same scanner
//!
//===============================================================
ESErrorCode CESCI2Accessor::ReleaseScanner()
{
	ES_LOG_TRACE_FUNC();

	return SetMode(kModeNone);
}

//===============================================================
//!
//! スキャンジョブの開始
//!
//===============================================================
ESErrorCode CESCI2Accessor::StartJobInMode(ESJobMode eJobMode)
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	switch (eJobMode)
	{
	case kESJobModeStandard:
		err = StartJobInStandard();
		break;
	case kESJobModeContinue:
		err = StartJobInContinue();
		break;
	case kESJobModeAFM:
		err = StartScanningInAFM();
		break;
	case kESJobModeAFMC:
		err = StartScanningInAFMC();
		break;
	default:
		goto BAIL;
		break;
	}
BAIL:
	return err;
}

//===============================================================
//!
//! スキャンジョブの終了
//!
//===============================================================
ESErrorCode CESCI2Accessor::StopJobInMode(ESJobMode eJobMode)
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	switch (eJobMode)
	{
	case kESJobModeStandard:
		err = StopJobInStandard();
		break;
	case kESJobModeContinue:
		err = StopJobInContinue();
		break;
	case kESJobModeAFM:
		err = StopScanningInAFM();
		break;
	case kESJobModeAFMC:
		err = StopScanningInAFMC();
		break;
	default:
		goto BAIL;
		break;
	}
BAIL:
	return err;
}

ESErrorCode CESCI2Accessor::StartJobInStandard()
{
	ESErrorCode err = kESErrorNoError;
	if (IsDisableJobContinue())
	{
		return kESErrorNoError;
	}

	if (GetJobMode() != kESJobModeNone)
	{
		return kESErrorNoError;
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeStandard) != indexSupportedJobModes.end())
	{
		err = RequestJobMode(JOB_STANDARD);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		err = SetMode(kModeControl);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	SetJobMode(kESJobModeStandard);

BAIL:
	return err;
}
ESErrorCode CESCI2Accessor::StopJobInStandard()
{
	ESErrorCode err = kESErrorNoError;
	if (IsDisableJobContinue())
	{
		return kESErrorNoError;
	}

	if (GetJobMode() != kESJobModeStandard)
	{
		return kESErrorNoError;
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeStandard) != indexSupportedJobModes.end())
	{
		err = RequestJobMode(JOB_END);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		err = SetMode(kModeNone);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	SetJobMode(kESJobModeNone);

BAIL:
	return err;
}

ESErrorCode CESCI2Accessor::StartJobInContinue()
{
	ESErrorCode err = kESErrorNoError;

	if (IsDisableJobContinue())
	{
		return kESErrorNoError;
	}

	if (GetJobMode() != kESJobModeNone)
	{
		return kESErrorNoError;
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeContinue) != indexSupportedJobModes.end())
	{
		err = RequestJobMode(JOB_CONTINUE);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		err = SetMode(kModeControl);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	SetJobMode(kESJobModeContinue);

BAIL:
	return err;
}
ESErrorCode CESCI2Accessor::StopJobInContinue()
{
	ESErrorCode err = kESErrorNoError;

	if (IsDisableJobContinue())
	{
		return kESErrorNoError;
	}

	if (GetJobMode() != kESJobModeContinue)
	{
		return kESErrorNoError;
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeContinue) != indexSupportedJobModes.end())
	{
		err = RequestJobMode(JOB_END);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		err = SetMode(kModeNone);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	SetJobMode(kESJobModeNone);

BAIL:
	return err;
}

//
//
// スキャン操作
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! スキャン時に送るべきパラメーターを返す。
//!
//! @return パラメーター。
//!
//===============================================================
ESErrorCode CESCI2Accessor::ParametersForScan(ESDictionaryA &dicParameter)
{
	dicParameter = m_dicParameters;

	// 境界補正のゼロクリア。
	// Risottoではフラットベッドでの境界補正が非サポートのため、
	// ADFで境界補正でのスキャン後にフラットベッドでスキャンすると、
	// ファームが境界補正の値を保持し続けているため、キャリッジが不正動作を起こす。
	if (IsShouldZeroClearEdgeFillForFB() && !GetSupportedEdgeFillWidth().empty())
	{
		if (!IsFeederEnabled())
		{
			ESDictionaryA dicArea;
			dicArea[ESCI2_DIC_KEY_LEFT] = (ESNumber)0;
			dicArea[ESCI2_DIC_KEY_RIGHT] = (ESNumber)0;
			dicArea[ESCI2_DIC_KEY_TOP] = (ESNumber)0;
			dicArea[ESCI2_DIC_KEY_BOTTOM] = (ESNumber)0;

			dicParameter[FCCSTR(CAP_EDGEFILL_AREA)] = dicArea;
		}
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! エラー後の継続スキャンをおこなう対象のエラーコード群を返す。
//!
//===============================================================
ESIndexSet CESCI2Accessor::ErrorsForInterruption()
{
	ESIndexSet errors;
	if (IsInterruptionEnabled())
	{
		errors.insert(kESErrorPaperDoubleFeed);
		errors.insert(kESErrorPaperJam);
		errors.insert(kESErrorPaperProtect);
		errors.insert(kESErrorPaperDoubleFeed2);
		errors.insert(kESErrorPoseError);
		errors.insert(kESErrorSeparationLeverError);
	}
	return errors;
}

//===============================================================
//!
//! ページカウントなどの前回スキャン情報を保持した状態でスキャンを開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::ScanInContext()
{
	ES_LOG_TRACE_FUNC();

	SYNCHRONIZED_START
	{
		// Check if there is any error
		ESErrorCode err = GetErrorStatus();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

		// Set scanning parameter
		{
			ESDictionaryA dicParameter;
			// 設定パラメーター作成
			err = ParametersForScan(dicParameter);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
			ES_Dump_Json(dicParameter, "commandParamSet");
			// 設定パラメーターセット
			err = SendParameters(dicParameter);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}

		if (IsFeedTypeADF() && IsAdfLoadSupported() && IsCancelled())
		{
			CallDelegateScannerWillCancelScanning();
			AbortImageHandles();
			CallDelegateScannerDidCancelScanning();
			goto BAIL;
		}

		// Transition to data transfer mode
		err = StartScanning();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

		// Perform data transfer
		err = TransferImage();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

	BAIL:
		if (IsDeviceOpened() && IsShouldSendStatAfterADFScan())
		{
			// Bao expect to be fetched status by drvier after scanning.
			// Otherwise, the status (e.g. '#ADF PE  ') may remain at the next scanning.
			GetStatus();
		}
		{
			// カウンター等のメンテナンス情報は更新されるとみなされるため、キャッシュしている情報を消去
			m_dicMaintenanceResults.clear();
		}

#ifdef WIN32
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTIONW__);
#else
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTION__);
#endif
		return err;
	}
	SYNCHRONIZED_END
}

ESErrorCode CESCI2Accessor::TransferImage()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (IsFeederEnabled())
	{
		NotifyBeginContinuousScanning();
	}

	{
		ESNumber nPageCount = 0;
		bool bShouldCountinue = true;
		bool bDoubleFeed = false;

		m_dicProcessingImagePtr.clear();
		m_dicClosableImagePtr.clear();

		while (bShouldCountinue)
		{
			ESDictionaryA dicHeaderInfo;
			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cImageBuffer;

			// REQUEST IMAGE
			err = RequestImage(dicHeaderInfo, cImageBuffer);
			if (IS_ERR_CODE(err))
			{
				m_bIsTransferring = false;
				if (err == kESErrorPaperEmpty && nPageCount > 0)
				{
					err = kESErrorNoError;
				}
				else
				{
					AbortImageHandles();
				}
				break;
			}

			// Check left over pages
			{
				ESNumber *pnLeftOver = SafeKeysDataPtr<ESNumber>(dicHeaderInfo, FCCSTR(HEADER_LEFTOVER_PAGE).c_str());
				if ((pnLeftOver != NULL) && (*pnLeftOver == 0))
				{
					m_bIsTransferring = false;
					bShouldCountinue = false;
				}
			}

			// Check suspend notification
			{
				bool *pSuspend = SafeKeysDataPtr<bool>(dicHeaderInfo, FCCSTR(HEADER_SUSPEND_NOTIFY).c_str());
				if (pSuspend && *pSuspend)
				{
					bShouldCountinue = false;
				}
			}

			// Check double feed information
			{
				ESStringA *pDoubleFeedInfo = SafeKeysDataPtr<ESStringA>(dicHeaderInfo, FCCSTR(HEADER_DOUBLEFEED_INFORMATION).c_str());
				if (pDoubleFeedInfo && (FourCharCode(*pDoubleFeedInfo) == DOUBLEFEED_INFORMATION_DF))
				{
					bDoubleFeed = true;
				}
			}

			// Check cancel
			{
				ESStringA *pStrAttention = SafeKeysDataPtr<ESStringA>(dicHeaderInfo, FCCSTR(HEADER_ATTENTION).c_str());
				if (((pStrAttention != NULL) && (FourCharCode(*pStrAttention) == ATTENTION_CANCEL)) || IsCancelled())
				{
					CallDelegateScannerWillCancelScanning();
					RequestCancel();
					AbortImageHandles();
					CallDelegateScannerDidCancelScanning();
					bShouldCountinue = false;
					break;
				}
			}

			// Surface
			ESStringA *pStrSurface = SafeKeysDataPtr<ESStringA>(dicHeaderInfo, FCCSTR(HEADER_SURFACETYPE).c_str());

			// Prepare image handle for data storage
			{
				ESDictionaryA *pDicPageStartInfo = SafeKeysDataPtr<ESDictionaryA>(dicHeaderInfo, FCCSTR(HEADER_PAGESTART).c_str());
				ESAnyArray *pDocumentType = SafeKeysDataPtr<ESAnyArray>(dicHeaderInfo, FCCSTR(HEADER_DOCUMENTTYPE).c_str());
				if (pDicPageStartInfo)
				{
					assert(pStrSurface != NULL);

					err = ProcessPageStartInfo(*pDicPageStartInfo, pDocumentType, *pStrSurface);
					if (IS_ERR_CODE(err))
					{
						RequestCancel();
						AbortImageHandles();
						break;
					}
					nPageCount += 1;
				}
			}

			// Write data to image handle
			if (!cImageBuffer.IsEmpty())
			{
				assert(pStrSurface != NULL);
				ESStringA strSurface = (pStrSurface ? *pStrSurface : "");
				err = ProcessImageDataBlock(cImageBuffer, strSurface);
				if (IS_ERR_CODE(err))
				{
					RequestCancel();
					AbortImageHandles();
					break;
				}
			}

			// Close image handle
			{
				ESDictionaryA *pDicPageEndInfo = SafeKeysDataPtr<ESDictionaryA>(dicHeaderInfo, FCCSTR(HEADER_PAGEEND).c_str());
				ESAnyArray *pDocumentType = SafeKeysDataPtr<ESAnyArray>(dicHeaderInfo, FCCSTR(HEADER_DOCUMENTTYPE).c_str());
				if (pDicPageEndInfo != NULL)
				{
					assert(pStrSurface != NULL);
					ESStringA strSurface = (pStrSurface ? *pStrSurface : "");
					err = ProcessPageEndInfo(*pDicPageEndInfo, strSurface, bDoubleFeed, pDocumentType);
					if (IS_ERR_CODE(err))
					{
						RequestCancel();
						AbortImageHandles();
						break;
					}
					if (!IsFeederEnabled())
					{
						bShouldCountinue = false;
						m_bIsTransferring = false;
					}
				}
			}
		} // while ( bShouldCountinue )

		if (bDoubleFeed)
		{
			err = kESErrorPaperDoubleFeed2;
		}
	}

BAIL:
	if (IS_ERR_SUCCESS(err) && IsFeederEnabled())
	{
		NotifyEndContinuousScanning();
	}

	if (m_dicProcessingImagePtr.size() != 0)
	{
		BOOST_FOREACH (ESDicPairA &value, m_dicProcessingImagePtr)
		{
			try
			{
				CESCI2ScannedImage *pImage = boost::any_cast<CESCI2ScannedImage *>(value.second);
				DeleteScanedImage(&pImage);
			}
			catch (...)
			{
			}
		}
		m_dicProcessingImagePtr.clear();
	}

#ifdef WIN32
	ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTIONW__);
#else
	ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTION__);
#endif

	return err;
}

//===============================================================
//!
//! 現在のスキャン設定でスキャンを開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::Scan()
{
	ES_LOG_TRACE_FUNC();

	SYNCHRONIZED_START
	{
		ESErrorCode err = kESErrorNoError;

		ESCI2Mode eCurrentMode = GetMode();

		SetScanning(true);
		SetCancelled(false);

		SetMode(kModeControl);

		if (!IsInterrupted())
		{ // 前回のスキャンが中断でなかった場合のみページカウントをリセットする
			m_dicPageCounts.clear();
		}
		else
		{
			SetInterrupted(false);
		}

		err = ScanInContext();

		SetMode(eCurrentMode);

		SetScanning(false);

		{
			ESIndexSet indexErrorsForInterruption = ErrorsForInterruption();
			if (indexErrorsForInterruption.find(err) != indexErrorsForInterruption.end())
			{
				SetInterrupted(true);
				NotifyInterruptScanningWithError(err);
			}
			else
			{
				NotifyCompleteScanningWithError(err);
			}
		}

#ifdef WIN32
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTIONW__);
#else
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTION__);
#endif
		return err;
	}
	SYNCHRONIZED_END
}

//===============================================================
//!
//! AFMの中で1回のスキャンをおこなう。通常のスキャンとの違いとして、ページカウンターのリセットと終了通知が行われない。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::ScanForAFM()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	SYNCHRONIZED_START
	{
		// AFM中のスキャン開始直前に、紙なし状態にするとエラーでAFMを抜けてしまうことを避けるため、
		// スキャン開始直前に紙がなかった場合は、スキャン動作に入らないようにする
		if (!IsDocumentLoaded())
		{
			SetScanning(false);

#ifdef WIN32
			ES_TRACE_LOG(ES_STRING("LEAVE %s (Dose not document loaded.)"), __FUNCTIONW__);
#else
			ES_TRACE_LOG(ES_STRING("LEAVE %s (Dose not document loaded.)"), __FUNCTION__);
#endif

			return kESErrorNoError;
		}

		{
			// 操作が発生したため、AFMのタイムアウトを終了
			err = InvalidateAutoFeedingModeTimeout();
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}

		SetInterrupted(false);

		SetScanning(true);

		err = ScanInContext();

		SetScanning(false);

		{
			ESIndexSet indexErrorsForInterruption = ErrorsForInterruption();
			if (indexErrorsForInterruption.find(err) != indexErrorsForInterruption.end())
			{
				SetInterrupted(true);
				NotifyInterruptScanningWithError(err);
			}
			else if (IS_ERR_CODE(err))
			{
				NotifyCompleteScanningWithError(err);
			}
		}

		if (IS_ERR_CODE(err))
		{
			if (IsLegacyAFMSupported())
			{
				// JobでないAFMサポート機では、エラー発生時にはAFMをオフにする
				StopAFM();
				SetJobMode(kESJobModeNone);
			}
		}

#ifdef WIN32
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTIONW__);
#else
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTION__);
#endif
	}
	SYNCHRONIZED_END
BAIL:
	return err;
}

ESErrorCode CESCI2Accessor::ScanForAFMC()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	SYNCHRONIZED_START
	{
		{
			// 操作が発生したため、AFMのタイムアウトを終了
			err = InvalidateAutoFeedingModeTimeout();
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}

		SetInterrupted(false);
		SetScanning(true);

		// CANによってTRDTを抜けている場合は、再度TRDTを送る必要がある
		if (!m_bIsTransferring)
		{
			StartScanning();
		}

		err = TransferImage();

		SetScanning(false);

		{
			ESIndexSet indexErrorsForInterruption = ErrorsForInterruption();
			if (indexErrorsForInterruption.find(err) != indexErrorsForInterruption.end())
			{
				SetInterrupted(true);
				NotifyInterruptScanningWithError(err);
			}
			else if (IS_ERR_CODE(err))
			{
				NotifyCompleteScanningWithError(err);
			}
		}

#ifdef WIN32
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTIONW__);
#else
		ES_TRACE_LOG(ES_STRING("LEAVE %s"), __FUNCTION__);
#endif
	}
	SYNCHRONIZED_END
BAIL:
	return err;
}

void *CESCI2Accessor::DoScanForAFMInBackground(void *pParam)
{
	CESCI2Accessor *pScanner = reinterpret_cast<CESCI2Accessor *>(pParam);
	if (pScanner)
	{
		pScanner->ScanForAFM();
	}
	pthread_exit(0);
	return (void *)NULL;
}

ESErrorCode CESCI2Accessor::ScanForAFMInBackground()
{
	SetScanning(true);
	SetCancelled(false);

	pthread_t thread;
	if (pthread_create(&thread, NULL, &CESCI2Accessor::DoScanForAFMInBackground, this) == 0)
	{
		pthread_detach(thread);
	}
	return kESErrorNoError;
}

void *CESCI2Accessor::DoScanForAFMCInBackground(void *pParam)
{
	CESCI2Accessor *pScanner = reinterpret_cast<CESCI2Accessor *>(pParam);
	if (pScanner)
	{
		pScanner->ScanForAFMC();
	}
	pthread_exit(0);
	return (void *)NULL;
}

ESErrorCode CESCI2Accessor::ScanForAFMCInBackground()
{
	SetScanning(true);
	SetCancelled(false);

	pthread_t thread;
	if (pthread_create(&thread, NULL, &CESCI2Accessor::DoScanForAFMCInBackground, this) == 0)
	{
		pthread_detach(thread);
	}
	return kESErrorNoError;
}

BOOL CESCI2Accessor::IncludedInDocumentType(ESAnyArray *pStrDocumentTypes, UInt32 checkType)
{
	if (pStrDocumentTypes && !pStrDocumentTypes->empty())
	{
		BOOST_FOREACH (ESAny &anyValue, *pStrDocumentTypes)
		{

			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL && FourCharCode(*pKey) == checkType)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//===============================================================
//!
//! 1ページの画像転送開始時の画像情報を受けて、画像データを受け取るためのImageHandleを生成し、インスタンス変数に格納する。
//! このとき、delegateに対して1ページ分のスキャンが開始されることを通知する。
//!
//! ESC/I-2での画像データ転送では、表面／裏面のデータがブロックに分割されて順不同に転送される可能性がある。
//! このため、受け取り用のImageHandleは、表面／裏面で別々に生成し保持しておく必要があり、内部的には「表面か裏面か」を示す文字列をキーとして辞書にそれぞれの対応するImageHandleを格納する。
//!
//! @param dicInfo 画像情報（'IMG '応答のヘッダにある'#pst'）辞書。
//! @param pStrDocumentType ドキュメントタイプ（キャリアシートなど）。
//! @param strSurface 表面か裏面かの情報（'IMG '応答のヘッダにある'#typ'）。
//===============================================================
ESErrorCode CESCI2Accessor::ProcessPageStartInfo(ESDictionaryA &dicInfo, ESAnyArray *pStrDocumentTypes, ESStringA &strSurface)
{
	ES_LOG_TRACE_FUNC();

	CESCI2ScannedImage *pImage = NULL;
	try
	{
		ESNumber nWidth = boost::any_cast<ESNumber>(dicInfo[ESCI2_DIC_KEY_WIDTH]);
		ESNumber nHeight = boost::any_cast<ESNumber>(dicInfo[ESCI2_DIC_KEY_HEIGHT]);
		ESNumber nBitsPerPixel = GetBitsPerPixel();
		ESNumber nBytesPerRow = ESCIGetBytesPerRow(nWidth, nBitsPerPixel);
		ESNumber nBitsPerSample = GetBitsPerSample();
		ESNumber nSamplesPerPixel = GetSamplesPerPixel();
		ESString strWorkFolder = GetWorkFolder();

		pImage = new CESCI2ScannedImage();
		ESErrorCode err = pImage->Init(nWidth,
									   nHeight,
									   nBytesPerRow,
									   nBitsPerSample,
									   nSamplesPerPixel,
									   strWorkFolder);
		ESNumber nBufferSize = GetBufferSize();
		if (nBufferSize > 0)
		{
			pImage->SetStoreBufferSize(GetBufferSize());
		}

		if (IS_ERR_CODE(err))
		{
			ES_LOG_FAILED_INITIALIZE_IMAGE();
			DeleteScanedImage(&pImage); // pImageはNULLになる
			goto BAIL;
		}

		if (IsCompressed())
		{
			pImage->SetImageFormat(kESImageFormatJPEG);
		}
		else
		{
			pImage->SetImageFormat(kESImageFormatRaw);
		}

		{
			ESNumber nDummyBytesPerLine = boost::any_cast<ESNumber>(dicInfo[ESCI2_DIC_KEY_DUMMY_BYTES_PER_LINE]);
			pImage->SetDummyBytesPerLine(nDummyBytesPerLine);
		}

		if (IncludedInDocumentType(pStrDocumentTypes, DOCUMENTTYPE_CRSHEET))
		{
			pImage->SetDocumentType(kESDocumentTypeCarrierSheet);
		}
		if (IncludedInDocumentType(pStrDocumentTypes, DOCUMENTTYPE_BLANKPAGE))
		{
			pImage->SetAutoDetectBlankPage(true);
		}
		if (IncludedInDocumentType(pStrDocumentTypes, DOCUMENTTYPE_MONOPAGE))
		{
			pImage->SetAutoDetectMonoPage(true);
		}
		{
			if (IsAutoCroppingEnabled())
			{
				pImage->SetAutoCropped(TRUE);

				// キャリアシートと判別された場合は、オートクロップはおこなわれない
				if (IncludedInDocumentType(pStrDocumentTypes, DOCUMENTTYPE_CRSHEET))
				{
					pImage->SetAutoCropped(FALSE);
				}
			}
		}
		{
			// ESC/I-2コマンドでは、ページ開始情報も含めて表面と裏面のデータのどちらが先に送信されてくるか未定義であるため、
			// ページカウントは表裏で別々に独立して保持しておき、それぞれのページ開始情報が来るたびに1ずつ増やす。
			ESNumber nPageCount = 0;
			if (m_dicPageCounts.count(strSurface) == 0)
			{
				nPageCount = 1;
			}
			else
			{
				nPageCount = boost::any_cast<ESNumber>(m_dicPageCounts[strSurface]) + 1;
			}
			if (IsDuplexEnabled())
			{
				// 表裏の情報とページのシリアルナンバーを付ける。
				// ページのシリアルナンバーはソートしたときに、表面 -> 裏面 -> 表面 -> ... となるようにページカウントを
				// 表面：ページカウント x 2 - 1
				// 裏面：ページカウント x 2
				// とする。

				UInt32 un32Surface = FourCharCode(strSurface);
				switch (un32Surface)
				{
				case SURFACETYPE_BACK:
					pImage->SetSurfaceType(kESSurfaceTypeBackSide);
					pImage->SetSerialNumber(nPageCount * 2);

					// 2Pass両面スキャナーでは裏面が逆向きになっていることをを示す。
					if (GetDuplexType() == kESDuplexType2Pass)
					{
						pImage->SetUpsideDown(true);
					}

					break;
				case SURFACETYPE_FRONT:
					pImage->SetSurfaceType(kESSurfaceTypeFrontSide);
					pImage->SetSerialNumber(nPageCount * 2 - 1);
					break;
				default:
					DeleteScanedImage(&pImage);
					err = kESErrorFatalError;
					goto BAIL;
				}
			}
			else
			{
				pImage->SetSurfaceType(kESSurfaceTypeFrontSide);
				pImage->SetSerialNumber(nPageCount);
			}
			// 用紙のシリアルナンバーを付ける。1枚の用紙の表裏両方のページで同一のナンバーが付く。
			pImage->SetPaperSerialNumber(nPageCount);

			m_dicProcessingImagePtr[strSurface] = pImage;

			NotifyWillScanToScannedImage(pImage);

			pImage->Open();
		}
	BAIL:
		return err;
	}
	catch (const std::bad_alloc &)
	{
		ES_LOG_FAILED_CREATE_IMAGE();
		return kESErrorMemoryError;
	}
	catch (...)
	{
		ES_LOG_EXCEPTION_ERROR();
		DeleteScanedImage(&pImage);
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//! インスタンス変数として保持しているImageHandleに対し、画像データを書き込む。
//!
//! @param cDataBuffer 画像データ。
//! @param strSurface 表面か裏面かの情報（'IMG '応答のヘッダにある'#typ'）。
//!
//===============================================================
ESErrorCode CESCI2Accessor::ProcessImageDataBlock(ES_CMN_FUNCS::BUFFER::CESHeapBuffer &cDataBuffer, ESStringA &strSurface)
{
	ES_LOG_TRACE_FUNC();

	try
	{
		if (m_dicProcessingImagePtr.count(strSurface) == 0)
		{
			return kESErrorFatalError;
		}
		CESCI2ScannedImage *pImage = boost::any_cast<CESCI2ScannedImage *>(m_dicProcessingImagePtr[strSurface]);
		return pImage->WriteData(cDataBuffer);
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//! インスタンス変数として保持しているImageHandleに対し、クローズ処理をおこなう。
//! このとき、delegateに対して1ページ分のスキャンが終了したことを通知する。
//!
//! @param dicInfo 画像情報（'IMG '応答のヘッダにある'#pen'）辞書。実際にスキャンしたデータのサイズが開始時の情報（'#pst'）と異なる場合がある。
//! @param strSurface 表面か裏面かの情報（'IMG '応答のヘッダにある'#typ'）。
//!
//===============================================================
ESErrorCode CESCI2Accessor::ProcessPageEndInfo(ESDictionaryA &dicInfo, ESStringA &strSurface, bool bIsDoubleFeed, ESAnyArray *pStrDocumentTypes)
{
	ES_LOG_TRACE_FUNC();

	try
	{
		if (m_dicProcessingImagePtr.count(strSurface) == 0)
		{
			return kESErrorFatalError;
		}
		CESCI2ScannedImage **ppImage = SafeKeysDataPtr<CESCI2ScannedImage *>(m_dicProcessingImagePtr, strSurface.c_str());
		if (ppImage && *ppImage)
		{
			CESCI2ScannedImage *pImage = *ppImage;
			ESNumber nWidth = boost::any_cast<ESNumber>(dicInfo[ESCI2_DIC_KEY_WIDTH]);
			ESNumber nHeight = boost::any_cast<ESNumber>(dicInfo[ESCI2_DIC_KEY_HEIGHT]);

			pImage->SetImageSize(MakeSize<UInt32>(nWidth, nHeight));
			m_dicClosableImagePtr[strSurface] = pImage;

			if (IncludedInDocumentType(pStrDocumentTypes, DOCUMENTTYPE_BLANKPAGE))
			{
				pImage->SetAutoDetectBlankPage(true);
			}
			if (IncludedInDocumentType(pStrDocumentTypes, DOCUMENTTYPE_MONOPAGE))
			{
				pImage->SetAutoDetectMonoPage(true);
			}

			if (IsDuplexEnabled())
			{
				CESCI2ScannedImage **ppImgFront = SafeKeysDataPtr<CESCI2ScannedImage *>(m_dicClosableImagePtr, FCCSTR(SURFACETYPE_FRONT).c_str());
				CESCI2ScannedImage **ppImgBack = SafeKeysDataPtr<CESCI2ScannedImage *>(m_dicClosableImagePtr, FCCSTR(SURFACETYPE_BACK).c_str());

				// 紙ジャムなどでイメージハンドルに対してabortを呼ぶ可能性があり、その場合は表裏あわせて捨てる必要がある。
				// 両方にabortが送れるよう、ペアが揃うまではprocessingImageHandlesに保持しておく。
				if (ppImgFront && *ppImgFront && ppImgBack && *ppImgBack && (*ppImgFront)->GetPaperSerialNumber() == (*ppImgBack)->GetPaperSerialNumber())
				{
					CESCI2ScannedImage *pImgFront = *ppImgFront;
					CESCI2ScannedImage *pImgBack = *ppImgBack;

					pImgFront->SetDoubleFeed(bIsDoubleFeed);
					pImgFront->Close();
					NotifyDidScanToScannedImage(pImgFront);
					m_dicPageCounts[FCCSTR(SURFACETYPE_FRONT)] = (ESNumber)pImgFront->GetPaperSerialNumber();

					pImgBack->SetDoubleFeed(bIsDoubleFeed);
					pImgBack->Close();
					NotifyDidScanToScannedImage(pImgBack);
					m_dicPageCounts[FCCSTR(SURFACETYPE_BACK)] = (ESNumber)pImgBack->GetPaperSerialNumber();

					DisposeImageHandles();
				}
			}
			else
			{
				pImage->SetDoubleFeed(bIsDoubleFeed);
				pImage->Close();
				NotifyDidScanToScannedImage(pImage);
				m_dicPageCounts[strSurface] = (ESNumber)pImage->GetPaperSerialNumber();

				DisposeImageHandles();
			}
		}
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

void CESCI2Accessor::DisposeImageHandles()
{
	ES_LOG_TRACE_FUNC();

	if (m_dicProcessingImagePtr.size() != 0)
	{
		BOOST_FOREACH (ESDicPairA &value, m_dicProcessingImagePtr)
		{
			try
			{
				CESCI2ScannedImage *pImage = boost::any_cast<CESCI2ScannedImage *>(value.second);
				DeleteScanedImage(&pImage);
			}
			catch (...)
			{
			}
		}
		m_dicProcessingImagePtr.clear();
	}
	m_dicClosableImagePtr.clear();
}

void CESCI2Accessor::AbortImageHandles()
{
	ES_LOG_TRACE_FUNC();

	if (m_dicProcessingImagePtr.size() != 0)
	{
		BOOST_FOREACH (ESDicPairA &value, m_dicProcessingImagePtr)
		{
			try
			{
				CESCI2ScannedImage *pImage = boost::any_cast<CESCI2ScannedImage *>(value.second);
				pImage->Abort();
			}
			catch (...)
			{
			}
		}
	}
	DisposeImageHandles();
}

ESErrorCode CESCI2Accessor::StopScanningInAutoFeedingMode()
{
	ESErrorCode err = kESErrorNoError;

	switch (GetJobMode())
	{
	case kESJobModeAFM:
		err = StopScanningInAFM();
		break;

	case kESJobModeAFMC:
		err = StopScanningInAFMC();
		break;

	default:
		break;
	}

	return err;
}

void *CESCI2Accessor::DoStopScanningInAutoFeedingModeInBackground(void *pParam)
{
	CESCI2Accessor *pScanner = reinterpret_cast<CESCI2Accessor *>(pParam);
	if (pScanner)
	{
		pScanner->StopScanningInAutoFeedingMode();
	}
	pthread_exit(0);
	return (void *)NULL;
}

ESErrorCode CESCI2Accessor::StopScanningInAutoFeedingModeInBackground()
{
	if (IsScanning())
	{
		Cancel();
	}

	pthread_t thread;
	if (pthread_create(&thread, NULL, &CESCI2Accessor::DoStopScanningInAutoFeedingModeInBackground, this) == 0)
	{
		pthread_detach(thread);
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! AFMによる待ち受けを開始する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::StartAFM()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (IsAfmEnabled())
	{
		return kESErrorSequenceError;
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeAFM) != indexSupportedJobModes.end())
	{

		if (GetMode() != kModeControl)
		{
			return kESErrorNoError;
		}

		err = RequestJobMode(JOB_AFM);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else if (IsLegacyAFMSupported())
	{
		// FS Xモードに遷移し、AFMをオフにするまでモードを戻さない
		SetMode(kModeControl);

		err = RequestAfmMode(true);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		return kESErrorFatalError;
	}

	// AfmTimeoutチェック開始
	err = ScheduleAutoFeedingModeTimeout();
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}

	SetAfmEnabled(true);

BAIL:
{
	ESIndexSet indexErrorsForInterruption = ErrorsForInterruption();
	if (indexErrorsForInterruption.find(err) != indexErrorsForInterruption.end())
	{
		NotifyInterruptScanningWithError(err);
	}
}

	return err;
}

//===============================================================
//!
//! AFMによる待ち受けを終了する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::StopAFM()
{
	ES_LOG_TRACE_FUNC();

	const int WAIT_INTERVAL = 200; // milliseconds

	ESErrorCode err = kESErrorNoError;

	if (!IsAfmEnabled() || (GetMode() != kModeControl))
	{
		return kESErrorNoError;
	}

	SetAfmEnabled(false);

	if (m_bIsTransferring)
	{
		ES_INFO_LOG(ES_STRING("--Scanning--"));
		// スキャンをキャンセル
		Cancel();
		// スキャンが終わるまで待つ
		while (m_bIsTransferring)
		{
			Sleep(WAIT_INTERVAL);
		}
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeAFM) != indexSupportedJobModes.end())
	{
		err = RequestJobMode(JOB_END);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else if (IsLegacyAFMSupported())
	{
		err = RequestAfmMode(false);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		return kESErrorFatalError;
	}

BAIL:
{
	// AfmTimeoutチェック終了
	InvalidateAutoFeedingModeTimeout();
}

	if (IsLegacyAFMSupported())
	{
		SetMode(kModeNone); // TODO: AFMオンにする前のモードに戻す？
	}

	return err;
}

ESErrorCode CESCI2Accessor::StartAFMC()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (IsAfmEnabled())
	{
		return kESErrorSequenceError;
	}

	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.find(kESJobModeAFMC) != indexSupportedJobModes.end())
	{

		if (GetMode() != kModeControl)
		{
			return kESErrorNoError;
		}

		err = RequestJobMode(JOB_AFMC);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}
	else
	{
		return kESErrorFatalError;
	}

	// AfmTimeoutチェック開始
	err = ScheduleAutoFeedingModeTimeout();
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}

	SetAfmEnabled(true);

BAIL:
{
	ESIndexSet indexErrorsForInterruption = ErrorsForInterruption();
	if (indexErrorsForInterruption.find(err) != indexErrorsForInterruption.end())
	{
		NotifyInterruptScanningWithError(err);
	}
}

	return err;
}

ESErrorCode CESCI2Accessor::StopAFMC()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (!IsAfmEnabled() || (GetMode() != kModeControl))
	{
		return kESErrorNoError;
	}

	SetAfmEnabled(false);

	if (m_bIsTransferring)
	{
		// TRDTからの脱出（モーター停止）
		err = RequestCancel();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

		SetScanning(false);
	}

	{
		ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
		if (indexSupportedJobModes.find(kESJobModeAFMC) != indexSupportedJobModes.end())
		{
			err = RequestJobMode(JOB_END);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}
		else if (IsLegacyAFMSupported())
		{
			err = RequestAfmMode(false);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}
		else
		{
			return kESErrorFatalError;
		}
	}
BAIL:
{
	// AfmTimeoutチェック終了
	InvalidateAutoFeedingModeTimeout();
}

	return err;
}

//===============================================================
//!
//! ページカウントをリセットし、AFMによる待ち受けを開始する。
//!
//! @discussion スキャン中にエラーが発生した場合はAFMを抜ける。再開したい場合には再度このメソッドを呼ぶ。
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::StartScanningInAFM()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (GetJobMode() != kESJobModeNone)
	{

		// エラー中断中にAFM開始要求が来た場合はエラー解除されたとみなし、
		// * エラー中断状態を解除
		// * 紙がすでに置かれている場合を想定して、AFMでのスキャンを試みる
		if (GetJobMode() == kESJobModeAFM && IsInterrupted())
		{
			SetInterrupted(false);

			// エラー中断していた場合はAFMタイムアウト処理が無効化されているので再スケジュールする
			err = ScheduleAutoFeedingModeTimeout();

			ScanForAFMInBackground();

			return err;
		}

		return kESErrorNoError;
	}

	if (!IsInterrupted())
	{ // 前回のスキャンが中断でなかった場合のみページカウントをリセットする
		m_dicPageCounts.clear();
	}

	{
		// 本体がAFM開始要求を受付可能であるか状態チェックする
		err = GetErrorStatus();

		// AFM開始時に紙がないことは許容されるのでエラーとしては無視する
		if (err == kESErrorPaperEmpty)
		{
			err = kESErrorNoError;
		}
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	err = StartAFM();
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}

	SetInterrupted(false);
	SetJobMode(kESJobModeAFM);

BAIL:
	return err;
}

//===============================================================
//!
//! AFMによる待ち受けを終了し、didCompleteScanningWithErrorのDelegate通知を発生させる。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::StopScanningInAFM()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (GetJobMode() != kESJobModeAFM)
	{
		return kESErrorNoError;
	}

	if (IsAfmEnabled())
	{

		err = StopAFM();
		NotifyCompleteScanningWithError(err);
	}

	SetJobMode(kESJobModeNone);

	return err;
}

ESErrorCode CESCI2Accessor::StartScanningInAFMC()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (GetJobMode() != kESJobModeNone)
	{

		// エラー中断中にAFM開始要求が来た場合はエラー解除されたとみなし、エラー中断状態を解除
		if (GetJobMode() == kESJobModeAFMC && IsInterrupted())
		{
			// エラー中断していた場合はAFMタイムアウト処理が無効化されているので再スケジュールする
			err = ScheduleAutoFeedingModeTimeout();

			// エラー中断していた場合はTRDTを抜けているので、再度TRDTモードに入る
			if (!m_bIsTransferring)
			{
				err = StartScanning();
				if (IS_ERR_CODE(err))
				{
					goto BAIL;
				}
			}

			SetInterrupted(false);

			return err;
		}
		return kESErrorNoError;
	}

	if (!IsInterrupted())
	{ // 前回のスキャンが中断でなかった場合のみページカウントをリセットする
		m_dicPageCounts.clear();
	}

	{
		// 本体がAFM開始要求を受付可能であるか状態チェックする
		err = GetErrorStatus();

		// AFM開始時に紙がないことは許容されるのでエラーとしては無視する
		if (err == kESErrorPaperEmpty)
		{
			err = kESErrorNoError;
		}
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	if (GetJobMode() == kESJobModeNone)
	{
		err = StartAFMC();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

		SetJobMode(kESJobModeAFMC);

		// Set scanning parameter
		{
			ESDictionaryA dicParameter;
			// 設定パラメーター作成
			err = ParametersForScan(dicParameter);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
			// 設定パラメーターセット
			err = SendParameters(dicParameter);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}
	}

	if (!m_bIsTransferring)
	{
		// Transition to data transfer mode
		err = StartScanning();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	SetInterrupted(false);

BAIL:
	if (IS_ERR_CODE(err))
	{
		ESIndexSet indexErrorsForInterruption = ErrorsForInterruption();
		if (indexErrorsForInterruption.find(err) == indexErrorsForInterruption.end())
		{
			Abort();
		}
	}

	return err;
}

ESErrorCode CESCI2Accessor::StopScanningInAFMC()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if (GetJobMode() != kESJobModeAFMC)
	{
		return kESErrorNoError;
	}

	if (IsAfmEnabled())
	{

		err = StopAFMC();
		NotifyCompleteScanningWithError(err);
	}

	SetJobMode(kESJobModeNone);

	return err;
}

//===============================================================
//!
//! スキャン処理をキャンセルする。
//!
//===============================================================
ESErrorCode CESCI2Accessor::Cancel()
{
	ES_LOG_TRACE_FUNC();

	SetCancelled(true);
	return kESErrorNoError;
}

//===============================================================
//!
//! 紙ジャム・重送検知で一時停止しているスキャンを中止する。
//! これにより、didCompleteScanningが通知され、次回のスキャンではページカウントが初期化されて開始される。
//! また、AFM中であればAFMを終了する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::Abort()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	assert(!IsScanning());

	if (IsInterrupted())
	{
		NotifyCompleteScanningWithError(kESErrorNoError);
		SetInterrupted(false);
	}

	if (IsAfmEnabled())
	{
		err = StopScanningInAutoFeedingModeInBackground();
	}

	return err;
}

//
//
// Notification
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CESCI2Accessor::NotifyBeginContinuousScanning()
{
	ES_LOG_TRACE_FUNC();
	CallDelegateScannerWillBeginContinuousScanning();
}

void CESCI2Accessor::NotifyEndContinuousScanning()
{
	ES_LOG_TRACE_FUNC();
	CallDelegateScannerDidEndContinuousScanning();
}

void CESCI2Accessor::NotifyCompleteScanningWithError(ESErrorCode err)
{
	ES_LOG_TRACE_FUNC();
	CallDelegateScannerDidCompleteScanningWithError(err);
}

void CESCI2Accessor::NotifyInterruptScanningWithError(ESErrorCode err)
{
	ES_LOG_TRACE_FUNC();
	CallDelegateScannerDidInterruptScanningWithError(err);
}

void CESCI2Accessor::NotifyWillScanToScannedImage(IESScannedImage *pImage)
{
	// シリアルナンバーが要求ページ数を超えていない場合のみ、imageHandleをDelegateに伝える
	// e.g.) 両面スキャンで奇数ページ分のスキャンをしたい場合、
	// コマンド仕様上は一つ大きい偶数ページ分読み出す必要があるが、最後のページが不要となる
	if (GetPagesToBeScanned() == 0 || pImage->GetSerialNumber() <= GetPagesToBeScanned())
	{
		CallDelegateScannerWillScanToScannedImage(pImage);
	}
}

void CESCI2Accessor::NotifyDidScanToScannedImage(IESScannedImage *pImage)
{
	// シリアルナンバーが要求ページ数を超えていない場合のみ、imageHandleをDelegateに伝える
	// e.g.) 両面スキャンで奇数ページ分のスキャンをしたい場合、
	// コマンド仕様上は一つ大きい偶数ページ分読み出す必要があるが、最後のページが不要となる
	if (GetPagesToBeScanned() == 0 || pImage->GetSerialNumber() <= GetPagesToBeScanned())
	{
		CallDelegateScannerDidScanToScannedImage(pImage);
	}
}

ESErrorCode CESCI2Accessor::CallDelegateScannerDidPressButton(UInt8 un8ButtonNumber)
{
	ES_LOG_TRACE_FUNC();

	if (GetJobMode() == kESJobModeAFMC)
	{
		return StartScanning();
	}
	else if ((IsAfmEnabled() && IsInterrupted()) || !IsAfmEnabled())
	{
		return CESCI2Command::CallDelegateScannerDidPressButton(un8ButtonNumber);
	}
	else
	{
		if (IsScanning())
		{
			return kESErrorNoError;
		}
		return ScanForAFMInBackground();
	}
}

ESErrorCode CESCI2Accessor::CallDelegateNetworkScannerDidRequestStartScanning()
{
	ES_LOG_TRACE_FUNC();

	if ((IsAfmEnabled() && IsInterrupted()) || !IsAfmEnabled())
	{
		return CESCI2Command::CallDelegateNetworkScannerDidRequestStartScanning();
	}
	else
	{
		if (IsScanning())
		{
			return kESErrorNoError;
		}
		return ScanForAFMInBackground();
	}
}

ESErrorCode CESCI2Accessor::CallDelegateScannerDidRequestStop()
{
	ES_LOG_TRACE_FUNC();

	if (!IsAfmEnabled() || (IsAfmEnabled() && IsInterrupted()))
	{
		return CESCI2Command::CallDelegateScannerDidRequestStop();
	}
	else
	{
		return StopScanningInAutoFeedingModeInBackground();
	}
}

void CALLBACK CESCI2Accessor::DidRequestStopScanning()
{
	if (!IsAfmEnabled())
	{
		CESCI2Command::DidRequestStopScanning();
	}
	else
	{
		Cancel();
	}
}

void CALLBACK CESCI2Accessor::DidDisconnect()
{
	ES_LOG_TRACE_FUNC();

	Close();
	CESCI2Command::DidDisconnect();
	if (IsAfmEnabled())
	{
		StopScanningInAutoFeedingModeInBackground();
	}
	else if (IsInterrupted())
	{
		NotifyCompleteScanningWithError(kESErrorNoError);
	}
}

void CALLBACK CESCI2Accessor::DeviceCommunicationError(ESErrorCode err)
{
	ES_LOG_TRACE_FUNC();

	CESCI2Command::DeviceCommunicationError(err);
	if (IsAfmEnabled())
	{
		StopScanningInAutoFeedingModeInBackground();
	}
	else if (IsInterrupted())
	{
		NotifyCompleteScanningWithError(kESErrorNoError);
	}
}
void CALLBACK CESCI2Accessor::DidRequestGetImageData()
{
	ES_LOG_TRACE_FUNC();

	if (GetJobMode() == kESJobModeAFMC)
	{
		ScanForAFMCInBackground();
	}
}

//
//
// ボタン監視
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! ボタンの監視を開始する
//!
//===============================================================
void CESCI2Accessor::StartButtonChecking()
{
	ES_LOG_TRACE_FUNC();

	std::lock_guard<std::recursive_mutex> lock(m_event_mtx);

	if (m_pDevInterface && m_pDevInterface->IsOpened())
	{
		if (m_pDevInterface->IsInterruptSupported())
		{
			m_interruptCheckTimer = std::unique_ptr<event_caller>(new event_caller([&]
																				   { m_pDevInterface->ReceiveInterruptEvent(); },
																				   c_dwButtonCheckIntervalForInterrupt_ms));
			m_interruptCheckTimer->start();
		}
	}
}

//===============================================================
//!
//! ボタンの監視を終了する
//!
//===============================================================
void CESCI2Accessor::StopButtonChecking()
{
	ES_LOG_TRACE_FUNC();

	std::lock_guard<std::recursive_mutex> lock(m_event_mtx);

	if (m_interruptCheckTimer && m_interruptCheckTimer->isValidTask())
	{
		m_interruptCheckTimer->stop();
		m_interruptCheckTimer->finalize();
	}

	ES_TRACE_LOG(ES_STRING("Leave StopButtonChecking"));
}

//===============================================================
//!
//! Auto Feeding Mode タイムアウトチェック開始
//!
//===============================================================
ESErrorCode CESCI2Accessor::ScheduleAutoFeedingModeTimeout()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	{
		err = InvalidateAutoFeedingModeTimeout();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

	if (m_nAfmTimeout_s > 0)
	{
		std::lock_guard<std::recursive_mutex> lock(m_event_mtx);

		m_afmCheckTimer = std::unique_ptr<event_caller>(new event_caller([&]
																		 { StopScanningInAutoFeedingMode(); },
																		 m_nAfmTimeout_s * 1000));
		m_afmCheckTimer->start();
	}

BAIL:
	return err;
}

//===============================================================
//!
//! AfmTimeoutチェックを終了する
//!
//===============================================================
ESErrorCode CESCI2Accessor::InvalidateAutoFeedingModeTimeout()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	std::lock_guard<std::recursive_mutex> lock(m_event_mtx);
	if (m_afmCheckTimer)
	{
		m_afmCheckTimer->stop();
		m_afmCheckTimer->finalize();
		m_afmCheckTimer.reset();
	}
	return err;
}

//
//
// スキャナ情報取得
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! スキャナにスキャナの情報（'INFO'）を要求し、インスタンス変数に格納する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::GetInfo()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetInfo(m_dicInformation);
}

//===============================================================
//!
//! スキャナにスキャナのメンテナンス情報（FS Z + 'INFO'）を要求し、インスタンス変数に格納する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::GetMaintenanceInfo()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetMaintenanceInfo(m_dicMaintenanceInformation);
}

ESErrorCode CESCI2Accessor::GetMaintenanceStatus()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetMaintenanceStatus(m_dicMaintenanceStatus);
}

ESErrorCode CESCI2Accessor::GetExtInfo()
{
	ES_LOG_TRACE_FUNC();
	if (!IsExtInformationSupported())
	{
		return kESErrorNoError;
	}
	return CESCI2Command::GetExtInfo(m_dicExtInformation);
}

//===============================================================
//!
//! スキャナにスキャナのCapability（'CAPA'）を要求し、インスタンス変数に格納する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::GetCapabilities()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetCapabilities(m_dicCapabilities);
}

//===============================================================
//!
//! スキャナにスキャナのメンテナンスCapability（FS Z + 'CAPA'）を要求し、インスタンス変数に格納する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::GetMaintenanceCapabilities()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetMaintenanceCapabilities(m_dicMaintenanceCapabilities);
}

//===============================================================
//!
//! スキャナステータス（'STAT'）を要求し、インスタンス変数に格納する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::GetStatus()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetStatus(m_dicStatus);
}

//===============================================================
//!
//! スキャナに現在の全スキャン設定（'RESA'）を要求し、インスタンス変数に格納する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::GetResults()
{
	ES_LOG_TRACE_FUNC();
	return CESCI2Command::GetResults(m_dicResult, NULL);
}

//=========================================================================
//!
//! 現在設定されている内部スキャン設定をクリアする。
//!
//! @param parKeys    クリアしたいスキャン設定のキー。NULLを与えると全設定がクリアされる。
//!
//=========================================================================
ESErrorCode CESCI2Accessor::ResetParametersForKeys(ESStringArrayA *parKeys)
{
	ES_LOG_TRACE_FUNC();
	try
	{
		if (parKeys != NULL)
		{
			for (ESStringArrayA::iterator it = parKeys->begin(); it != parKeys->end(); ++it)
			{
				if (m_dicParameters.count(*it) != 0)
				{
					m_dicParameters.erase(*it);
				}
			}
		}
		else
		{
			m_dicParameters.clear();
		}
		if ((parKeys == NULL) || std::find(parKeys->begin(), parKeys->end(), FCCSTR(CAP_EDGEFILL_AREA)) != parKeys->cend())
		{
			if (!GetSupportedEdgeFillWidth().empty())
			{
				SetEdgeFillWidthLeft(0.0f);
				SetEdgeFillWidthTop(0.0f);
				SetEdgeFillWidthRight(0.0f);
				SetEdgeFillWidthBottom(0.0f);
			}
		}
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//=========================================================================
//!
//! 設定をリセットする
//!
//=========================================================================
ESErrorCode CESCI2Accessor::Reset()
{
	ES_LOG_TRACE_FUNC();

	ResetParametersForKeys(NULL);

	m_eFunctionalUnitType = kESFunctionalUnitUnknown;
	m_fXOffsetMargin = 0.0f;
	m_fYOffsetMargin = 0.0f;

	ESErrorCode err = Setup();
	return err;
}

//
//
// 取得スキャナデーターから値を取得
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=========================================================================
//!
//! バージョン取得
//!
//=========================================================================
ESString CESCI2Accessor::GetVersion()
{
	ESStringA *pStrVersionA = SafeKeysDataPtr<ESStringA>(m_dicInformation, FCCSTR(INFO_VERSION).c_str());
	if (pStrVersionA != NULL && !pStrVersionA->empty())
	{
#ifdef WIN32
		return ES_CMN_FUNCS::STRING::MBStringToUnicode(*pStrVersionA);
#else
		return *pStrVersionA;
#endif
	}
	else
	{
		return ES_STRING("");
	}
}

//=========================================================================
//!
//! エラーステータスを取得する。(ESErrorStatus)
//!
//=========================================================================
ESErrorCode CESCI2Accessor::GetErrorStatus()
{
	ESErrorCode err = GetStatus();
	if (IS_ERR_CODE(err))
	{
		return err;
	}

	ESStringA *pStrFactor = NULL;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitFlatbed:
		pStrFactor = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_ERROR).c_str(), FCCSTR(ERROR_PART_FB).c_str());
		break;
	case kESFunctionalUnitDocumentFeeder:
		pStrFactor = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_ERROR).c_str(), FCCSTR(ERROR_PART_ADF).c_str());
		break;
	case kESFunctionalUnitTransparent:
		pStrFactor = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_ERROR).c_str(), FCCSTR(ERROR_PART_TPU).c_str());
		break;
	default:
		pStrFactor = NULL;
		break;
	}
	if (pStrFactor == NULL)
	{
		return (ESErrorCode)kESErrorStatusNoError;
	}

	switch (FourCharCode(*pStrFactor))
	{
	case ERROR_FACTOR_COVEROPEN:
		return (ESErrorCode)kESErrorStatusCoverOpen;

	case ERROR_FACTOR_PAPERJAM:
		return (ESErrorCode)kESErrorStatusPaperJam;

	case ERROR_FACTOR_PAPEREMPTY:
		return (ESErrorCode)kESErrorStatusPaperEmpty;

	case ERROR_FACTOR_LAMPTIME:
		return (ESErrorCode)kESErrorStatusLampTime;

	case ERROR_FACTOR_CRLOCK:
		return (ESErrorCode)kESErrorStatusCRLock;

	case ERROR_FACTOR_DOUBLEFEED:
		return (ESErrorCode)kESErrorStatusPaperDoubleFeed;

	case ERROR_FACTOR_TRAYCLOSE:
		return (ESErrorCode)kESErrorStatusTrayClose;

	case ERROR_FACTOR_CRSHEETSENSOR:
		return (ESErrorCode)kESErrorStatusCRSheetSensorError;

	case ERROR_FACTOR_ETSENSOR:
		return (ESErrorCode)kESErrorStatusETSensorError;

	case ERROR_FACTOR_PAPERPROTECT:
		return (ESErrorCode)kESErrorStatusPaperProtect;

	case ERROR_FACTOR_SEPARATIONLEVER:
		return (ESErrorCode)kESErrorStatusSeparationLeverError;

	case ERROR_FACTOR_POSE:
		return (ESErrorCode)kESErrorStatusPoseError;

	case ERROR_FACTOR_GAPSENSOR:
		return (ESErrorCode)kESErrorStatusGAPSensorError;

	case ERROR_FACTOR_PAPER_REMOVE_ERROR:
		return (ESErrorCode)kESErrorStatusPaperRemoveError;

	case ERROR_FACTOR_STACKER_ERROR:
		return (ESErrorCode)kESErrorStatusPrintStackerError;

	case ERROR_FACTOR_SKEW_ERROR:
		return (ESErrorCode)kESErrorStatusSkewDetectError;

	case ERROR_FACTOR_PESENSOR:
		return (ESErrorCode)kESErrorStatusPESensorError;

	default:
		return (ESErrorCode)kESErrorStatusFatalError;
	}
}

//===============================================================
//!
//! バッテリーステータスを取得する。(ESBatteryStatus)
//!
//===============================================================
ESBatteryStatus CESCI2Accessor::GetBatteryStatus()
{
	try
	{
		ESErrorCode err = GetStatus();
		if (IS_ERR_CODE(err))
		{
			return kESBatteryStatusNone;
		}

		ESStringA *pStrBatteryStatus = SafeKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_BATTERY).c_str());
		if (!pStrBatteryStatus)
		{
			return kESBatteryStatusNone;
		}

		switch (FourCharCode(*pStrBatteryStatus))
		{
		case STAT_BATTERY_LOW:
			return kESBatteryStatusLow;
		default:
			break;
		}
	}
	catch (...)
	{
	}
	return kESBatteryStatusNone;
}

bool CESCI2Accessor::IsExtInformationSupported()
{
	bool *pValue = SafeKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_EXTINFO).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//! 認証機能をサポートしているか？
//!
//===============================================================
bool CESCI2Accessor::IsAuthenticationSupported()
{
	bool *pValue = SafeKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_AUTHENTICATION).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//! 認証機能が有効状態かどうかを取得する
//!
//===============================================================
bool CESCI2Accessor::IsAuthenticationEnabled()
{
	return m_bIsAuthenticationEnabled;
}

//===============================================================
//!
// 認証機能が有効状態かどうかを設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetAuthenticationEnabled(bool bEnabled)
{
	m_bIsAuthenticationEnabled = bEnabled;
	return kESErrorNoError;
}

//===============================================================
//!
//! 認証ユーザー名を取得する
//!
//===============================================================
ESString CESCI2Accessor::GetAuthUserName()
{
	return m_strAuthUsername;
}

//===============================================================
//!
//! 認証ユーザー名を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetAuthUserName(ESString strAuthUserName)
{
	m_strAuthUsername = strAuthUserName;
	return kESErrorNoError;
}

//===============================================================
//!
//! 認証パスワードを取得する
//!
//===============================================================
ESString CESCI2Accessor::GetAuthPassword()
{
	return m_strAuthPassword;
}

//===============================================================
//!
//! 認証パスワードを設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetAuthPassword(ESString strAuthPassword)
{
	m_strAuthPassword = strAuthPassword;
	return kESErrorNoError;
}

//===============================================================
//!
//! プロダクト名を取得する
//!
//===============================================================
ESString CESCI2Accessor::GetProductName()
{
	ESStringA *pStrProductNameA = SafeKeysDataPtr<ESStringA>(m_dicInformation, FCCSTR(INFO_PRODUCTNAME).c_str());
	if (pStrProductNameA != NULL && !pStrProductNameA->empty())
	{
		*pStrProductNameA = TrimSpace(*pStrProductNameA);
#ifdef WIN32
		return ES_CMN_FUNCS::STRING::MBStringToUnicode(*pStrProductNameA);
#else
		return *pStrProductNameA;
#endif
	}
	else
	{
		return ES_STRING("");
	}
}

//===============================================================
//!
//! シリアルナンバーを文字列で取得する
//!
//===============================================================
ESString CESCI2Accessor::GetSerialNumber()
{
	ESStringA *strSerialNumberA = SafeKeysDataPtr<ESStringA>(m_dicInformation, FCCSTR(INFO_SERIALNUMBER).c_str());
	if (strSerialNumberA != NULL && !strSerialNumberA->empty())
	{
#ifdef WIN32
		return ES_CMN_FUNCS::STRING::MBStringToUnicode(*strSerialNumberA);
#else
		return *strSerialNumberA;
#endif
	}
	else
	{
		return ES_STRING("");
	}
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsInterruptionEnabled()
{
	return m_bIsInterrutionEnabled;
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetInterruptionEnabled(bool bIsInterruptionEnabled)
{
	m_bIsInterrutionEnabled = bIsInterruptionEnabled;
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsLengthPaddingSupported()
{
	bool *pValue = SafeKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_DUMMYLESS).c_str());
	return (pValue ? !(*pValue) : true); // キーがない場合はtrue
}

//===============================================================
//!
//! AFMをサポートしているか
//!
//===============================================================
bool CESCI2Accessor::IsAutoFeedingModeSupported()
{
	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.count(kESJobModeAFM) != 0)
	{
		return true;
	}
	if (indexSupportedJobModes.count(kESJobModeAFMC) != 0)
	{
		return true;
	}
	return IsLegacyAFMSupported();
}
bool CESCI2Accessor::IsLegacyAFMSupported()
{
	bool *pAFMValue = SafeKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_AUTOFEEDINGMODE).c_str());
	return (pAFMValue ? (*pAFMValue) : false);
}
bool CESCI2Accessor::IsContinuousAutoFeedingModeSupported()
{
	ESIndexSet indexSupportedJobModes = GetSupportedJobModes();
	if (indexSupportedJobModes.count(kESJobModeAFMC) != 0)
	{
		return true;
	}
	return false;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetAutoFeedingModeTimeout()
{
	return m_nAfmTimeout_s;
}
//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetAutoFeedingModeTimeout(ESNumber nAutoFeedingModeTimeout)
{
	m_nAfmTimeout_s = nAutoFeedingModeTimeout;
	return kESErrorNoError;
}

//===============================================================
//!
//! サポートする給紙タイプを取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedFunctionalUnitTypes()
{
	ESIndexSet indexs;
	if (m_dicInformation.count(FCCSTR(INFO_FB)) != 0)
	{
		indexs.insert(kESFunctionalUnitFlatbed);
	}
	if (m_dicInformation.count(FCCSTR(INFO_ADF)) != 0)
	{
		indexs.insert(kESFunctionalUnitDocumentFeeder);
	}
	if (m_dicInformation.count(FCCSTR(INFO_TPU)) != 0)
	{
		indexs.insert(kESFunctionalUnitTransparent);
	}
	return indexs;
}

//===============================================================
//!
//! 給紙タイプを取得する
//!
//===============================================================
ESFunctionalUnitType CESCI2Accessor::GetFunctionalUnitType()
{
	return m_eFunctionalUnitType;
}

//===============================================================
//!
//! 給紙タイプを設定する (ESFunctionalUnitType)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetFunctionalUnitType(ESNumber nFunctionalUnitType)
{
	if (m_eFunctionalUnitType == (ESFunctionalUnitType)nFunctionalUnitType)
	{
		return kESErrorNoError;
	}

	ESErrorCode err = kESErrorNoError;
	UInt32 un32TargetType = INFO_FB;

	ResetParametersForKeys(NULL);

	m_eFunctionalUnitType = (ESFunctionalUnitType)nFunctionalUnitType;
	switch (nFunctionalUnitType)
	{
	case kESFunctionalUnitFlatbed:
		un32TargetType = CAP_FB;
		break;

	case kESFunctionalUnitDocumentFeeder:
		un32TargetType = CAP_ADF;
		break;

	case kESFunctionalUnitTransparent:
		un32TargetType = CAP_TPU;
		break;

	case kESFunctionalUnitUnknown:
	default:
		err = kESErrorInvalidParameter;
		break;
	}

	ESStringSetA strSet;
	m_dicParameters[FCCSTR(un32TargetType)] = strSet;

	return err;
}

//===============================================================
//!
//! サポートする解像度を取得する
//!
//===============================================================
ESAny CESCI2Accessor::GetFunctionalUnitSpecificResolutions()
{
	ESAny empty;
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_RESOLUTION);
		break;
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_RESOLUTION);
		break;
	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_RESOLUTION);
		break;
	default:
		return empty;
	}
	try
	{
		ESDictionaryA &dicFunctional = boost::any_cast<ESDictionaryA &>(m_dicCapabilities[strKey1]);
		ESAny &anyResolutions = dicFunctional[strKey2]; // List of Range

		if (anyResolutions.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyResolutions);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyResolutions.type() == typeid(ST_ES_RANGE))
		{
			return boost::any_cast<ST_ES_RANGE>(anyResolutions);
		}
	}
	catch (...)
	{
	}
	return empty;
}

ESAny CESCI2Accessor::GetSupportedXResolutions()
{
	ESAny anyResolutions = GetFunctionalUnitSpecificResolutions();
	if (!anyResolutions.empty())
	{
		return anyResolutions;
	}

	try
	{
		ESAny &anyXResolutions = m_dicCapabilities[FCCSTR(CAP_RESOLUTION_MAIN)];

		if (anyXResolutions.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyXResolutions);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyXResolutions.type() == typeid(ST_ES_RANGE))
		{
			return boost::any_cast<ST_ES_RANGE>(anyXResolutions);
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}
ESAny CESCI2Accessor::GetSupportedYResolutions()
{
	ESAny anyResolutions = GetFunctionalUnitSpecificResolutions();
	if (!anyResolutions.empty())
	{
		return anyResolutions;
	}

	try
	{
		ESAny &anyYResolutions = m_dicCapabilities[FCCSTR(CAP_RESOLUTION_SUB)];

		if (anyYResolutions.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyYResolutions);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyYResolutions.type() == typeid(ST_ES_RANGE))
		{
			return boost::any_cast<ST_ES_RANGE>(anyYResolutions);
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//! 解像度を取得する
//!
//===============================================================
ESNumber CESCI2Accessor::GetXResolution()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_RESOLUTION_MAIN).c_str());
	return (pValue ? *pValue : 0);
}
ESNumber CESCI2Accessor::GetYResolution()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_RESOLUTION_SUB).c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//! 解像度を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetXResolution(ESNumber xResolution)
{
	m_dicParameters[FCCSTR(CAP_RESOLUTION_MAIN)] = (ESNumber)xResolution;
	return kESErrorNoError;
}
ESErrorCode CESCI2Accessor::SetYResolution(ESNumber yResolution)
{
	m_dicParameters[FCCSTR(CAP_RESOLUTION_SUB)] = (ESNumber)yResolution;
	return kESErrorNoError;
}

//===============================================================
//!
//! 光学解像度を取得する
//!
//===============================================================
ESNumber CESCI2Accessor::GetOpticalResolution()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(INFO_ADF);
		strKey2 = FCCSTR(INFO_ADF_BASIC_RESOLUTION);
		break;

	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(INFO_FB);
		strKey2 = FCCSTR(INFO_FB_BASICRESOLUTION);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(INFO_TPU);
		strKey2 = FCCSTR(INFO_TPU_BASICRESOLUTION);
		break;

	default:
		return 0;
	}
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicInformation, strKey1.c_str(), strKey2.c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//!
//!
//===============================================================
ST_ES_SIZE_F CESCI2Accessor::GetMaxImagePixels()
{
	ST_ES_SIZE_F *pValue = SafeKeysDataPtr<ST_ES_SIZE_F>(m_dicInformation, FCCSTR(INFO_MAX_IMAGEPIX).c_str());
	return (pValue ? *pValue : MakeSize(0.0f, 0.0f));
}

//===============================================================
//!
//! 最大スキャンサイズを取得する(インチ）
//!
//===============================================================
ST_ES_SIZE_F CESCI2Accessor::GetMaxScanSize()
{
	const ST_ES_SIZE_F stZeroSize = MakeSize(0.0f, 0.0f);

	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(INFO_ADF);
		strKey2 = FCCSTR(INFO_ADF_SCANAREA);
		break;

	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(INFO_FB);
		strKey2 = FCCSTR(INFO_FB_SCANAREA);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(INFO_TPU);
		strKey2 = FCCSTR(INFO_TPU_SCANAREA);
		break;

	default:
		return stZeroSize;
	}

	ST_ES_SIZE_F *pValue = SafeKeyDicInKeysDataPtr<ST_ES_SIZE_F>(m_dicInformation, strKey1.c_str(), strKey2.c_str());
	return (pValue ? MakeSize(pValue->cx / 100, pValue->cy / 100) : stZeroSize);
}
bool CESCI2Accessor::IsMaxScanSizeInNormalSpeedSupported()
{
	return (m_dicExtInformation.count("NormalSpeedLength") != 0) ? true : false;
}

ST_ES_SIZE_F CESCI2Accessor::GetMaxScanSizeInNormalSpeed()
{
	ST_ES_SIZE_F size = GetMaxScanSize();

	try
	{
		switch (GetFunctionalUnitType())
		{
		case kESFunctionalUnitDocumentFeeder:
		{
			// FIXME: 判断は主操作解像度で良いか？
			const ESAnyArray *pTable = SafeKeysDataPtr<ESAnyArray>(m_dicExtInformation, "NormalSpeedLength");
			if (!pTable)
			{
				break;
			}
			// 一番後ろのレコードに最も大きい解像度での最大長が入っているため、一旦これをmaxLengthとする。
			// 下記の探索で（curResol <= resol）にマッチしなかった場合はこれが使用される。
			const ESAny &anyMaxNormalSpeedLength = pTable->back();
			if (anyMaxNormalSpeedLength.type().name() != typeid(ESDictionaryA).name())
			{
				break;
			}
			const ESDictionaryA &dicMaxNormalSpeedLength = boost::any_cast<const ESDictionaryA &>(anyMaxNormalSpeedLength);
			const ESNumber *pMaxLength = SafeKeysDataCPtr<ESNumber>(dicMaxNormalSpeedLength, "Length");
			if (!pMaxLength)
			{
				break;
			}

			ESNumber nMaxLength = *pMaxLength;
			ESNumber curResol = GetYResolution();
			for (auto it = pTable->begin(); it != pTable->end(); ++it)
			{

				const ESDictionaryA &recode = boost::any_cast<const ESDictionaryA &>(*it);

				const ESNumber resol = boost::any_cast<const ESNumber>(recode.at("Resolution"));
				const ESNumber length = boost::any_cast<const ESNumber>(recode.at("Length"));

				if (curResol <= resol)
				{
					nMaxLength = length;
					break;
				}
			}
			return MakeSize<ESFloat>(size.cx, (ESFloat)((double)nMaxLength / 100.0));
		}
		break;

		case kESFunctionalUnitFlatbed:
		case kESFunctionalUnitTransparent:
		default:
			break;
		}
	}
	catch (...)
	{
	}
	return size;
}

ST_ES_SIZE_F CESCI2Accessor::GetMaxScanSizeInLongLength()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(INFO_ADF);
		strKey2 = FCCSTR(INFO_ADF_MAX_DOCUMENTSIZE);
		break;

	case kESFunctionalUnitFlatbed:
	case kESFunctionalUnitTransparent:
	default:
		return GetMaxScanSize();
	}

	ST_ES_SIZE_F *pValue = SafeKeyDicInKeysDataPtr<ST_ES_SIZE_F>(m_dicInformation, strKey1.c_str(), strKey2.c_str());
	return (pValue ? MakeSize(pValue->cx / 100, pValue->cy / 100) : GetMaxScanSize());
}

#define CENTI 100.0

ST_ES_SIZE_F CESCI2Accessor::GetMaxScanSizeInLongLengthWithResolution(ESNumber nResolution)
{
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
	{
		ST_ES_SIZE_F maxScanSizeInLongLength = GetMaxScanSizeInLongLength();
		ESDicArray maxLongLengthTable = GetMaxLongLengthTable();
		if (maxLongLengthTable.size())
		{
			for (auto record = maxLongLengthTable.begin(); record != maxLongLengthTable.end(); record++)
			{
				ESNumber curResol = nResolution;
				ESNumber resol = SAFE_ANY_VALUE_FROM_DICT(*record, ES_STRING("Resolution"), ESNumber, 0);
				ESNumber length = SAFE_ANY_VALUE_FROM_DICT(*record, ES_STRING("Length"), ESNumber, 0);
				if (curResol <= resol)
				{
					// lengthの単位は1/100インチのため、通常のインチに換算する
					return MakeSize<ESFloat>(maxScanSizeInLongLength.cx, (ESFloat)((double)length / CENTI));
				}
			}
			// 該当する解像度が見つからなかった場合は通常スキャンでの最大値を返す
			return GetMaxScanSize();
		}
		else
		{
			return GetMaxScanSizeInLongLength();
		}
	}
	break;
	case kESFunctionalUnitFlatbed:
	case kESFunctionalUnitTransparent:
	default:
		return GetMaxScanSize();
	}
}

//===============================================================
//!
//! 最小スキャンサイズを取得する(インチ）
//!
//===============================================================
ST_ES_SIZE_F CESCI2Accessor::GetMinScanSize()
{
	const ST_ES_SIZE_F stZeroSize = MakeSize(0.0f, 0.0f);

	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(INFO_ADF);
		strKey2 = FCCSTR(INFO_ADF_MIN_DOCUMENTSIZE);
		break;

	case kESFunctionalUnitFlatbed:
	case kESFunctionalUnitTransparent:
	default:
		return stZeroSize;
	}

	ST_ES_SIZE_F *pValue = SafeKeyDicInKeysDataPtr<ST_ES_SIZE_F>(m_dicInformation, strKey1.c_str(), strKey2.c_str());
	return (pValue ? MakeSize(pValue->cx / 100, pValue->cy / 100) : stZeroSize);
}

ESDicArray CESCI2Accessor::GetMaxLongLengthTable()
{
	ESDicArray dicArray;
	{
		ESAnyArray *maxLongLengthTable = SafeKeysDataPtr<ESAnyArray>(m_dicExtInformation, "MaxLength");
		if (maxLongLengthTable)
		{
			for (auto itr = maxLongLengthTable->begin(); itr != maxLongLengthTable->end(); itr++)
			{
				const ESDictionaryA &dicElem = boost::any_cast<const ESDictionaryA &>(*itr);
				const ESNumber *pLength = SafeKeysDataCPtr<ESNumber>(dicElem, "Length");
				const ESNumber *pResolution = SafeKeysDataCPtr<ESNumber>(dicElem, "Resolution");
				if (pLength && pResolution)
				{
					ESDictionary elm;
					elm[ES_STRING("Length")] = ESNumber(*pLength);
					elm[ES_STRING("Resolution")] = ESNumber(*pResolution);

					dicArray.push_back(elm);
				}
			}
		}
	}
	return dicArray;
}

//===============================================================
//!
//! スキャンするエリアを設定する(インチ）
//!
//===============================================================
SInt32 PixelFromInch(ESFloat fInch, ESNumber dpi)
{
	SInt32 n32Inch100 = (SInt32)(fInch * 100 + 0.5); // インチ 下２桁を有効
	return (SInt32)(n32Inch100 * dpi / 100 + 0.5);	 // 桁落ち対応
}
ESErrorCode CESCI2Accessor::SetScanArea(ST_ES_RECT_F rcScanArea, bool bShouldAlign)
{
	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG(ES_STRING("Set scan area in Inch : %f, %f, %f, %f"), rcScanArea.left, rcScanArea.top, rcScanArea.right, rcScanArea.bottom);

	ST_ES_RECT_UN32 rcScanAreaInPixel = {};
	ESNumber xRes = GetXResolution();
	ESNumber yRes = GetYResolution();

	assert(xRes > 0 && yRes > 0);

	rcScanAreaInPixel.left = (UInt32)PixelFromInch(rcScanArea.left, xRes);
	rcScanAreaInPixel.top = (UInt32)PixelFromInch(rcScanArea.top, yRes);
	rcScanAreaInPixel.right = (UInt32)PixelFromInch(rcScanArea.right, xRes);
	rcScanAreaInPixel.bottom = (UInt32)PixelFromInch(rcScanArea.bottom, yRes);

	return SetScanAreaInPixel(rcScanAreaInPixel, bShouldAlign);
}

//===============================================================
//!
//! スキャンするエリアを取得する（インチ）
//!
//===============================================================
ST_ES_RECT_F CESCI2Accessor::GetScanArea()
{
	ESNumber xRes = GetXResolution();
	ESNumber yRes = GetYResolution();
	ST_ES_RECT_F rcArea = {0.0f, 0.0f, 0.0f, 0.0f};
	ST_ES_RECT_UN32 rcScanAreaInPixel = GetScanAreaInPixel();

	if (xRes == 0 || yRes == 0)
	{
		return MakeRect<ESFloat>(0.0f, 0.0f, 0.0f, 0.0f);
	}

	rcArea.left = (ESFloat)rcScanAreaInPixel.left / xRes;
	rcArea.top = (ESFloat)rcScanAreaInPixel.top / yRes;
	rcArea.right = (ESFloat)rcScanAreaInPixel.right / xRes;
	rcArea.bottom = (ESFloat)rcScanAreaInPixel.bottom / yRes;

	return rcArea;
}

//===============================================================
//!
//! スキャンするエリアを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetScanArea(ST_ES_RECT_F rcScanArea)
{
	return SetScanArea(rcScanArea, false);
}

//===============================================================
//!
//! スキャンするエリアをピクセル単位で取得する
//!
//===============================================================
ST_ES_RECT_UN32 CESCI2Accessor::GetScanAreaInPixel()
{
	ST_ES_RECT_UN32 *pValue = SafeKeysDataPtr<ST_ES_RECT_UN32>(m_dicParameters, FCCSTR(PAR_ACQUISITIONAREA).c_str());
	return (pValue ? *pValue : MakeRect<UInt32>(0, 0, 0, 0));
}

//===============================================================
//!
//! スキャンするエリアをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetScanAreaInPixel(ST_ES_RECT_UN32 rcUn32ScanAreaInPixel, bool bShouldAlign)
{
	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG(ES_STRING("Set scan area in Pixel : %d, %d, %d, %d"), rcUn32ScanAreaInPixel.left, rcUn32ScanAreaInPixel.top, rcUn32ScanAreaInPixel.right, rcUn32ScanAreaInPixel.bottom);
	{
		ST_ES_RECT_S32 rcS32ScanAreaInPixel = MakeRect<SInt32>((SInt32)rcUn32ScanAreaInPixel.left,
															   (SInt32)rcUn32ScanAreaInPixel.top,
															   (SInt32)rcUn32ScanAreaInPixel.right,
															   (SInt32)rcUn32ScanAreaInPixel.bottom);

		ST_ES_RECT_S32 rcSupportedArea = {};
		ST_ES_SIZE_F sizeValue = GetMaxScanSizeInLongLengthWithResolution(GetYResolution());
		ESNumber xRes = GetXResolution();
		ESNumber yRes = GetYResolution();

		assert(sizeValue.cx > 0.0f && sizeValue.cy > 0.0f);
		assert(xRes > 0 && yRes > 0);

		rcSupportedArea = MakeRect<SInt32>(0, 0, PixelFromInch(sizeValue.cx, xRes), PixelFromInch(sizeValue.cy, yRes));

		if (bShouldAlign)
		{
			switch (GetGuidePosition())
			{
			case kESGuidePositionCenter:
			{
				ESNumber nOffset = RECT_WIDTH(rcSupportedArea) / 2 - RECT_WIDTH(rcS32ScanAreaInPixel) / 2;
				rcS32ScanAreaInPixel.left += nOffset;
				rcS32ScanAreaInPixel.right += nOffset;
			}
			break;
			case kESGuidePositionRight:
			{
				ESNumber nOffset = RECT_WIDTH(rcSupportedArea) - RECT_WIDTH(rcS32ScanAreaInPixel);
				rcS32ScanAreaInPixel.left += nOffset;
				rcS32ScanAreaInPixel.right += nOffset;
			}
			break;
			case kESGuidePositionLeft:
			default:
				break;
			}
		}

		rcS32ScanAreaInPixel.left += (SInt32)(GetXOffsetMargin() * xRes);
		rcS32ScanAreaInPixel.right += (SInt32)(GetXOffsetMargin() * xRes);
		rcS32ScanAreaInPixel.top += (SInt32)(GetYOffsetMargin() * yRes);
		rcS32ScanAreaInPixel.bottom += (SInt32)(GetYOffsetMargin() * yRes);

		assert(ESIntersectsRect(rcSupportedArea, rcS32ScanAreaInPixel));

		// Hiroshigeの上限 #IMX = 65535 暫定対処
		ST_ES_SIZE_F maxImagePixels = GetMaxImagePixels();
		if (maxImagePixels.cy >= 1)
		{
			SInt32 height = (SInt32)maxImagePixels.cy;
			rcS32ScanAreaInPixel.bottom = (std::min)(rcS32ScanAreaInPixel.bottom, height);
		}
		else
		{
			rcS32ScanAreaInPixel.bottom = (std::min)(rcS32ScanAreaInPixel.bottom, 65535);
		}

		// libjpeg制限のため、暫定対処
		rcS32ScanAreaInPixel.bottom = (std::min)(rcS32ScanAreaInPixel.bottom, 65500);
		rcS32ScanAreaInPixel.top = (std::min)(rcS32ScanAreaInPixel.top, rcS32ScanAreaInPixel.bottom);

		rcS32ScanAreaInPixel = ESIntersectionRect(rcSupportedArea, rcS32ScanAreaInPixel);

		rcUn32ScanAreaInPixel = MakeRect<UInt32>((UInt32)rcS32ScanAreaInPixel.left,
												 (UInt32)rcS32ScanAreaInPixel.top,
												 (UInt32)rcS32ScanAreaInPixel.right,
												 (UInt32)rcS32ScanAreaInPixel.bottom);
	}

	m_dicParameters[FCCSTR(PAR_ACQUISITIONAREA)] = rcUn32ScanAreaInPixel;
	return kESErrorNoError;
}
ESErrorCode CESCI2Accessor::SetScanAreaInPixel(ST_ES_RECT_UN32 rcUn32ScanAreaInPixel)
{
	return SetScanAreaInPixel(rcUn32ScanAreaInPixel, false);
}

//===============================================================
//!
//! スキャンするサイズを取得する（インチ）
//!
//===============================================================
ST_ES_SIZE_F CESCI2Accessor::GetScanSize()
{
	ST_ES_RECT_F rcScanArea = GetScanArea();
	return MakeSize(RECT_WIDTH(rcScanArea), RECT_HEIGHT(rcScanArea));
}

//===============================================================
//!
//! スキャンするサイズを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetScanSize(ST_ES_SIZE_F sizeScan)
{
	return SetScanArea(MakeRect(0.0f, 0.0f, sizeScan.cx, sizeScan.cy), true);
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で取得する
//!
//===============================================================
ST_ES_SIZE_UN32 CESCI2Accessor::GetScanSizeInPixel()
{
	ST_ES_RECT_UN32 rc = GetScanAreaInPixel();
	ST_ES_SIZE_UN32 size = MakeSize(rc.right - rc.left, rc.bottom - rc.top);

	return size;
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetScanSizeInPixel(ST_ES_SIZE_UN32 sizeScan)
{
	ST_ES_RECT_UN32 rc = MakeRect<UInt32>(0, 0, sizeScan.cx, sizeScan.cy);
	return SetScanAreaInPixel(rc, true);
}

//===============================================================
//!
//! オフセットマージン
//!
//===============================================================
ESFloat CESCI2Accessor::GetXOffsetMargin()
{
	return m_fXOffsetMargin;
}
ESErrorCode CESCI2Accessor::SetXOffsetMargin(ESFloat fMargin)
{
	m_fXOffsetMargin = fMargin;
	return kESErrorNoError;
}
ESFloat CESCI2Accessor::GetYOffsetMargin()
{
	return m_fYOffsetMargin;
}
ESErrorCode CESCI2Accessor::SetYOffsetMargin(ESFloat fMargin)
{
	m_fYOffsetMargin = fMargin;
	return kESErrorNoError;
}

//===============================================================
//!
//! AFM
//!
//===============================================================
bool CESCI2Accessor::IsAfmEnabled() const
{
	return m_bIsAfmEnabled;
}
ESErrorCode CESCI2Accessor::SetAfmEnabled(bool bEnable)
{
	m_bIsAfmEnabled = bEnable;
	return kESErrorNoError;
}

//===============================================================
//!
//! Job
//!
//===============================================================
BOOL CESCI2Accessor::IsPushScanReadySupported()
{
	bool *pValue = SafeKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_PUSHSCANREADY).c_str());
	return (pValue ? *pValue : false);
}
ESIndexSet CESCI2Accessor::GetSupportedJobModes()
{
	ESIndexSet indexSupportedJobModes;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicInformation, FCCSTR(INFO_JOB).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{

			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case INFO_JOB_STANDARD:
					indexSupportedJobModes.insert(kESJobModeStandard);
					break;

				case INFO_JOB_CONTINUE:
					indexSupportedJobModes.insert(kESJobModeContinue);
					break;

				case INFO_JOB_AFM:
					indexSupportedJobModes.insert(kESJobModeAFM);
					break;

				case INFO_JOB_AFMC:
					indexSupportedJobModes.insert(kESJobModeAFMC);
					break;

				case INFO_JOB_END:
					indexSupportedJobModes.insert(kESJobModeNone);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexSupportedJobModes;
}
ESJobMode CESCI2Accessor::GetJobMode()
{
	return m_eJobMode;
}
ESErrorCode CESCI2Accessor::SetJobMode(ESJobMode eJobMode)
{
	m_eJobMode = eJobMode;
	return kESErrorNoError;
}
bool CESCI2Accessor::IsDisableJobContinue()
{
	return m_bIsDisableJobContinue;
}
ESErrorCode CESCI2Accessor::SetDisableJobContinue(bool bDisableJobContinue)
{
	m_bIsDisableJobContinue = bDisableJobContinue;
	return kESErrorNoError;
}
bool CESCI2Accessor::IsDisableKeepingCapture()
{
	return m_bIsDisableKeepingCapture;
}
ESErrorCode CESCI2Accessor::SetDisableKeepingCapture(bool bDisableKeepingCapture)
{
	m_bIsDisableKeepingCapture = bDisableKeepingCapture;
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャンエリア設定のガイド位置を取得する
//!
//===============================================================
ESGuidePosition CESCI2Accessor::GetGuidePosition()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(INFO_ADF);
		strKey2 = FCCSTR(INFO_ADF_GUIDEPOSITION);
		break;
	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(INFO_FB);
		strKey2 = FCCSTR(INFO_FB_GUIDEPOSITION);
		break;
	case kESFunctionalUnitTransparent: // No implementation
	default:
		return kESGuidePositionLeft;
	}
	ESStringA *pValue = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicInformation, strKey1, strKey2);
	if (pValue)
	{
		switch (FourCharCode(*pValue))
		{
		case INFO_GUIDEPOSITION_CENTER:
			return kESGuidePositionCenter;

		case INFO_GUIDEPOSITION_RIGHT:
			return kESGuidePositionRight;

		case INFO_GUIDEPOSITION_LEFT:
		default:
			break;
		}
	}
	return kESGuidePositionLeft;
}

//===============================================================
//!
//! ADF Paper-Guide
//!
//===============================================================
bool CESCI2Accessor::IsGuidelessADF()
{
	ESDictionaryA *pDicInfo = SafeKeysDataPtr<ESDictionaryA>(m_dicInformation, FCCSTR(INFO_ADF).c_str());
	if ((pDicInfo != NULL) && (pDicInfo->count(FCCSTR(INFO_ADF_GUIDELESS)) != 0))
	{
		return true;
	}
	return false;
}

//===============================================================
//!
//! カラーフォーマットを取得する
//!
//! @return ESColorFormat, 不明時: 0
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedColorFormats()
{
	ESIndexSet indexSupportedColorFormat;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_COLORFORMAT).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{

			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_COLORFORMAT_MONO1:
					indexSupportedColorFormat.insert(kESColorFormatMono1);
					break;

				case CAP_COLORFORMAT_DROP_R1:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropR1);
					break;

				case CAP_COLORFORMAT_DROP_G1:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropG1);
					break;

				case CAP_COLORFORMAT_DROP_B1:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropB1);
					break;

				case CAP_COLORFORMAT_MONO8:
					indexSupportedColorFormat.insert(kESColorFormatMono8);
					break;

				case CAP_COLORFORMAT_DROP_R8:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropR8);
					break;

				case CAP_COLORFORMAT_DROP_G8:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropG8);
					break;

				case CAP_COLORFORMAT_DROP_B8:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropB8);
					break;

				case CAP_COLORFORMAT_MONO16:
					indexSupportedColorFormat.insert(kESColorFormatMono16);
					break;

				case CAP_COLORFORMAT_DROP_R16:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropR16);
					break;

				case CAP_COLORFORMAT_DROP_G16:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropG16);
					break;

				case CAP_COLORFORMAT_DROP_B16:
					indexSupportedColorFormat.insert(kESColorFormatMonoDropB16);
					break;

				case CAP_COLORFORMAT_RGB3:
					indexSupportedColorFormat.insert(kESColorFormatRGB3);
					break;

				case CAP_COLORFORMAT_RGB24:
					indexSupportedColorFormat.insert(kESColorFormatRGB24);
					break;

				case CAP_COLORFORMAT_RGB48:
					indexSupportedColorFormat.insert(kESColorFormatRGB48);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexSupportedColorFormat;
}

//===============================================================
//!
//! カラーフォーマットを取得する
//!
//! @return ESColorFormat, 不明時: 0
//!
//===============================================================
ESNumber CESCI2Accessor::GetColorFormat()
{
	ESStringA *pStrCode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_COLORFORMAT).c_str());
	if (pStrCode != NULL)
	{
		switch (FourCharCode(*pStrCode))
		{
		case CAP_COLORFORMAT_MONO1:
			return kESColorFormatMono1;

		case CAP_COLORFORMAT_DROP_R1:
			return kESColorFormatMonoDropR1;

		case CAP_COLORFORMAT_DROP_G1:
			return kESColorFormatMonoDropG1;

		case CAP_COLORFORMAT_DROP_B1:
			return kESColorFormatMonoDropB1;

		case CAP_COLORFORMAT_MONO8:
			return kESColorFormatMono8;

		case CAP_COLORFORMAT_DROP_R8:
			return kESColorFormatMonoDropR8;

		case CAP_COLORFORMAT_DROP_G8:
			return kESColorFormatMonoDropG8;

		case CAP_COLORFORMAT_DROP_B8:
			return kESColorFormatMonoDropB8;

		case CAP_COLORFORMAT_MONO16:
			return kESColorFormatMono16;

		case CAP_COLORFORMAT_DROP_R16:
			return kESColorFormatMonoDropR16;

		case CAP_COLORFORMAT_DROP_G16:
			return kESColorFormatMonoDropG16;

		case CAP_COLORFORMAT_DROP_B16:
			return kESColorFormatMonoDropB16;

		case CAP_COLORFORMAT_RGB3:
			return kESColorFormatRGB3;

		case CAP_COLORFORMAT_RGB24:
			return kESColorFormatRGB24;

		case CAP_COLORFORMAT_RGB48:
			return kESColorFormatRGB48;

		default:
			break;
		}
	}
	return 0;
}

//===============================================================
//!
//! カラーフォーマットを設定する(ESColorFormat)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetColorFormat(ESNumber nColorFormat)
{
	ESCI2ColorFormat eESCI2ColorFormat = CAP_COLORFORMAT_RGB24;

	ESIndexSet indexSupportedColorFormat = GetSupportedColorFormats();
	assert(indexSupportedColorFormat.find(nColorFormat) != indexSupportedColorFormat.end());

	ESErrorCode err = kESErrorNoError;
	switch (nColorFormat)
	{
	case kESColorFormatMono1:
		eESCI2ColorFormat = CAP_COLORFORMAT_MONO1;
		break;

	case kESColorFormatMonoDropR1:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_R1;
		break;

	case kESColorFormatMonoDropG1:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_G1;
		break;

	case kESColorFormatMonoDropB1:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_B1;
		break;

	case kESColorFormatMono8:
		eESCI2ColorFormat = CAP_COLORFORMAT_MONO8;
		break;

	case kESColorFormatMonoDropR8:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_R8;
		break;

	case kESColorFormatMonoDropG8:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_G8;
		break;

	case kESColorFormatMonoDropB8:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_B8;
		break;

	case kESColorFormatMono16:
		eESCI2ColorFormat = CAP_COLORFORMAT_MONO16;
		break;

	case kESColorFormatMonoDropR16:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_R16;
		break;

	case kESColorFormatMonoDropG16:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_G16;
		break;

	case kESColorFormatMonoDropB16:
		eESCI2ColorFormat = CAP_COLORFORMAT_DROP_B16;
		break;

	case kESColorFormatRGB3:
		eESCI2ColorFormat = CAP_COLORFORMAT_RGB3;
		break;

	case kESColorFormatRGB24:
		eESCI2ColorFormat = CAP_COLORFORMAT_RGB24;
		break;

	case kESColorFormatRGB48:
		eESCI2ColorFormat = CAP_COLORFORMAT_RGB48;
		break;

	default:
		err = kESErrorInvalidParameter;
		break;
	}
	m_dicParameters[FCCSTR(CAP_COLORFORMAT)] = FCCSTR(eESCI2ColorFormat);
	return err;
}

//===============================================================
//!
//! サンプル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCI2Accessor::GetBitsPerSample()
{
	switch (GetColorFormat())
	{
	case kESColorFormatRGB48:
	case kESColorFormatMono16:
	case kESColorFormatMonoDropR16:
	case kESColorFormatMonoDropG16:
	case kESColorFormatMonoDropB16:
		return 16;

	case kESColorFormatRGB24:
	case kESColorFormatMono8:
	case kESColorFormatMonoDropR8:
	case kESColorFormatMonoDropG8:
	case kESColorFormatMonoDropB8:
		return 8;

	case kESColorFormatMono1:
	case kESColorFormatMonoDropR1:
	case kESColorFormatMonoDropG1:
	case kESColorFormatMonoDropB1:
		return 1;

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
ESNumber CESCI2Accessor::GetSamplesPerPixel()
{
	switch (GetColorFormat())
	{
	case kESColorFormatRGB48:
	case kESColorFormatRGB24:
		return 3;

	case kESColorFormatMono16:
	case kESColorFormatMonoDropR16:
	case kESColorFormatMonoDropG16:
	case kESColorFormatMonoDropB16:
	case kESColorFormatMono8:
	case kESColorFormatMonoDropR8:
	case kESColorFormatMonoDropG8:
	case kESColorFormatMonoDropB8:
	case kESColorFormatMono1:
	case kESColorFormatMonoDropR1:
	case kESColorFormatMonoDropG1:
	case kESColorFormatMonoDropB1:
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
ESNumber CESCI2Accessor::GetBitsPerPixel()
{
	return GetBitsPerSample() * GetSamplesPerPixel();
}

//===============================================================
//!
//! サポートする画像フォーマットタイプを取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedImageFormats()
{
	ESIndexSet indexSupportedImageFormats;

	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_IMAGEFORMAT).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{

			ESStringA *pStrCode = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pStrCode != NULL)
			{
				switch (FourCharCode(*pStrCode))
				{
				case CAP_IMAGEFORMAT_RAW:
					indexSupportedImageFormats.insert(kESImageFormatRaw);
					break;
				case CAP_IMAGEFORMAT_JPEG:
					indexSupportedImageFormats.insert(kESImageFormatJPEG);
					break;
				default:
					break;
				}
			}
		}
	}
	return indexSupportedImageFormats;
}

//===============================================================
//!
//! 画像のフォーマットタイプを取得する
//!
//===============================================================
ESImageFormat CESCI2Accessor::GetImageFormat()
{
	ESStringA *pStrImageFormat = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_IMAGEFORMAT).c_str());
	if (pStrImageFormat != NULL)
	{
		switch (FourCharCode(*pStrImageFormat))
		{
		case CAP_IMAGEFORMAT_JPEG:
			return kESImageFormatJPEG;
		case CAP_IMAGEFORMAT_RAW:
		default:
			break;
		}
	}
	return kESImageFormatRaw;
}

//===============================================================
//!
//! 画像のフォーマットタイプを指定する(ESImageFormat)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetImageFormat(ESNumber nImageFormat)
{

	ESIndexSet indexSupported = GetSupportedImageFormats();
	if (indexSupported.find(nImageFormat) == indexSupported.end())
	{
		return kESErrorInvalidParameter;
	}

	switch (nImageFormat)
	{
	case kESImageFormatJPEG:
		m_dicParameters[FCCSTR(CAP_IMAGEFORMAT)] = FCCSTR(CAP_IMAGEFORMAT_JPEG);
		break;

	case kESImageFormatRaw:
		m_dicParameters[FCCSTR(CAP_IMAGEFORMAT)] = FCCSTR(CAP_IMAGEFORMAT_RAW);
		break;

	default:
		return kESErrorInvalidParameter;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! サポートするガンマモードを取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedGammaModes()
{
	ESIndexSet indexSupportedGammaModes;

	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_GAMMAMODE).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pStrCode = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pStrCode != NULL)
			{
				switch (FourCharCode(*pStrCode))
				{
				case CAP_GAMMAMODE_10:
					indexSupportedGammaModes.insert(kESGammaMode10);
					break;

				case CAP_GAMMAMODE_18:
					indexSupportedGammaModes.insert(kESGammaMode18);
					break;

				case CAP_GAMMAMODE_22:
					indexSupportedGammaModes.insert(kESGammaMode22);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexSupportedGammaModes;
}

//===============================================================
//!
//! ガンマモードを取得する
//!
//===============================================================
ESGammaMode CESCI2Accessor::GetGammaMode()
{
	ESStringA *pStrGammaMode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_GAMMAMODE).c_str());
	if (pStrGammaMode != NULL)
	{
		switch (FourCharCode(*pStrGammaMode))
		{
		case CAP_GAMMAMODE_10:
			return kESGammaMode10;

		case CAP_GAMMAMODE_18:
			return kESGammaMode18;

		case CAP_GAMMAMODE_22:
			return kESGammaMode22;

		default:
			break;
		}
	}
	return 0;
}

//===============================================================
//!
//! ガンマモードを設定する(ESGammaMode)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetGammaMode(ESNumber nGammaMode)
{
	ESIndexSet indexSupported = GetSupportedGammaModes();
	assert(indexSupported.find(nGammaMode) != indexSupported.end());

	ESErrorCode err = kESErrorNoError;
	ESCI2GammaMode eESCI2GammaMode = CAP_GAMMAMODE_10;
	switch (nGammaMode)
	{
	case kESGammaMode10:
		eESCI2GammaMode = CAP_GAMMAMODE_10;
		break;

	case kESGammaMode18:
		eESCI2GammaMode = CAP_GAMMAMODE_18;
		break;

	case kESGammaMode22:
		eESCI2GammaMode = CAP_GAMMAMODE_22;
		break;

	default:
		err = kESErrorInvalidParameter;
		break;
	}

	m_dicParameters[FCCSTR(CAP_GAMMAMODE)] = FCCSTR(eESCI2GammaMode);
	return err;
}

//===============================================================
//!
//! サポートするガンマテーブルを取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedGammaChannels()
{
	ESIndexSet indexSupportedGammaChannels;

	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_GAMMATABLE).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pStrChannel = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pStrChannel != NULL)
			{
				switch (FourCharCode(*pStrChannel))
				{
				case CAP_GAMMATABLE_MONO:
					indexSupportedGammaChannels.insert(kESGammaChannelMaster);
					break;

				case CAP_GAMMATABLE_RED:
					indexSupportedGammaChannels.insert(kESGammaChannelRed);
					break;

				case CAP_GAMMATABLE_GREEN:
					indexSupportedGammaChannels.insert(kESGammaChannelGreen);
					break;

				case CAP_GAMMATABLE_BLUE:
					indexSupportedGammaChannels.insert(kESGammaChannelBlue);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexSupportedGammaChannels;
}

//===============================================================
//!
//! ガンマテーブルを取得する
//!
//===============================================================
ESAny CESCI2Accessor::GetGammaTableForChannel(ESGammaChannel eChannel)
{
	UInt32 un32ESCI2Channel = 0;
	switch (eChannel)
	{
	case kESGammaChannelMaster:
		un32ESCI2Channel = CAP_GAMMATABLE_MONO;
		break;

	case kESGammaChannelRed:
		un32ESCI2Channel = CAP_GAMMATABLE_RED;
		break;

	case kESGammaChannelGreen:
		un32ESCI2Channel = CAP_GAMMATABLE_GREEN;
		break;

	case kESGammaChannelBlue:
		un32ESCI2Channel = CAP_GAMMATABLE_BLUE;
		break;

	default:
		return nullptr;
	}
	ESByteData *pData = SafeKeyDicInKeysDataPtr<ESByteData>(m_dicParameters, FCCSTR(CAP_GAMMATABLE).c_str(), FCCSTR(un32ESCI2Channel).c_str());
	if (pData != NULL && (pData->size() >= GAMMA_TABLE_SIZE))
	{
		ESIndexArray arReturn;
		for (int i = 0; i < GAMMA_TABLE_SIZE; i++)
		{
			ESNumber value = (ESNumber)(*pData)[i];
			arReturn.push_back(value);
		}
		return arReturn;
	}
	return nullptr;
}

ESAny CESCI2Accessor::GetGammaTableMono()
{
	return GetGammaTableForChannel(kESGammaChannelMaster);
}
ESAny CESCI2Accessor::GetGammaTableRed()
{
	return GetGammaTableForChannel(kESGammaChannelRed);
}
ESAny CESCI2Accessor::GetGammaTableGreen()
{
	return GetGammaTableForChannel(kESGammaChannelGreen);
}
ESAny CESCI2Accessor::GetGammaTableBlue()
{
	return GetGammaTableForChannel(kESGammaChannelBlue);
}

//===============================================================
//!
//! ガンマテーブルを設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetGammaTable(ESIndexArray arGammaTable, ESGammaChannel eChannel)
{
	ESIndexSet indexSupported = GetSupportedGammaChannels();
	assert(indexSupported.find(eChannel) != indexSupported.end());
	assert(arGammaTable.size() == GAMMA_TABLE_SIZE);

	ESStringA strGammaTable = FCCSTR(CAP_GAMMATABLE);

	if (m_dicParameters.count(strGammaTable) == 0)
	{
		ESDictionaryA dicTmp;
		m_dicParameters[strGammaTable] = dicTmp;
	}

	ESDictionaryA *pDicGamma = SafeKeysDataPtr<ESDictionaryA>(m_dicParameters, strGammaTable.c_str());
	if (pDicGamma != NULL)
	{

		UInt32 un32Channel = 0;
		switch (eChannel)
		{
		case kESGammaChannelMaster:
			un32Channel = CAP_GAMMATABLE_MONO;
			break;

		case kESGammaChannelRed:
			un32Channel = CAP_GAMMATABLE_RED;
			break;

		case kESGammaChannelGreen:
			un32Channel = CAP_GAMMATABLE_GREEN;
			break;

		case kESGammaChannelBlue:
			un32Channel = CAP_GAMMATABLE_BLUE;
			break;

		default:
			return kESErrorInvalidParameter;
		}

		ESByteData data(GAMMA_TABLE_SIZE, 0);
		for (int i = 0; i < GAMMA_TABLE_SIZE; i++)
		{
			data[i] = (ESByte)arGammaTable[i];
		}
		(*pDicGamma)[FCCSTR(un32Channel)] = data;

		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

ESErrorCode CESCI2Accessor::SetGammaTableMono(ESIndexArray arGammaTable)
{
	return SetGammaTable(arGammaTable, kESGammaChannelMaster);
}
ESErrorCode CESCI2Accessor::SetGammaTableRed(ESIndexArray arGammaTable)
{
	return SetGammaTable(arGammaTable, kESGammaChannelRed);
}
ESErrorCode CESCI2Accessor::SetGammaTableGreen(ESIndexArray arGammaTable)
{
	return SetGammaTable(arGammaTable, kESGammaChannelGreen);
}
ESErrorCode CESCI2Accessor::SetGammaTableBlue(ESIndexArray arGammaTable)
{
	return SetGammaTable(arGammaTable, kESGammaChannelBlue);
}

//===============================================================
//!
//! サポートするカラーマトリックスモードを取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedColorMatrixModes()
{
	ESIndexSet indexSupportedColorMatrixModes;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_COLORMATRIX).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_COLORMATRIX_UNIT:
					indexSupportedColorMatrixModes.insert(kESColorMatrixUnit);
					break;

				case CAP_COLORMATRIX_08:
					indexSupportedColorMatrixModes.insert(kESColorMatrix8);
					break;

				case CAP_COLORMATRIX_16:
					indexSupportedColorMatrixModes.insert(kESColorMatrix16);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexSupportedColorMatrixModes;
}

//===============================================================
//!
//! カラーマトリクスモードを取得する
//!
//===============================================================
ESColorMatrixMode CESCI2Accessor::GetColorMatrixMode()
{
	return m_eColorMatrixMode;
}

//===============================================================
//!
//! カラーマトリクスモードを設定する(ESColorMatrixMode)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetColorMatrixMode(ESNumber nColorMatrixMode)
{
	ESIndexSet indexSupported = GetSupportedColorMatrixModes();
	assert(indexSupported.find(nColorMatrixMode) != indexSupported.end());

	m_eColorMatrixMode = (ESColorMatrixMode)nColorMatrixMode;
	return kESErrorNoError;
}

//===============================================================
//!
//! カラーマトリックスを取得する(ESFloatArray or nullptr)
//!
//===============================================================
ESAny CESCI2Accessor::GetColorMatrix()
{
	return nullptr;
}

//===============================================================
//!
//! カラーマトリックスを設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::_SetColorMatrix(ES_COLOR_MATRIX &matrix)
{
	ESColorMatrixMode eColorMatrixMode = GetColorMatrixMode();
	if (eColorMatrixMode == kESColorMatrixUnit)
	{
		m_dicParameters[FCCSTR(CAP_COLORMATRIX)] = FCCSTR(CAP_COLORMATRIX_UNIT);
		return kESErrorNoError;
	}
	try
	{
		ESDictionaryA dic;
		m_dicParameters[FCCSTR(CAP_COLORMATRIX)] = dic;

		ESDictionaryA &dicColorMatrix = boost::any_cast<ESDictionaryA &>(m_dicParameters[FCCSTR(CAP_COLORMATRIX)]);

		// Matrix to be sent to scanner must be following d1-d9 order:
		//
		//     G  R  B
		// G [d1 d4 d7]
		// R [d2 d5 d8]
		// B [d3 d6 d9]
		//
		// So, we will convert it with index table.
		UInt8 index[9] = {4, 1, 7, 3, 0, 6, 5, 2, 8};

		double flatten[9] = {0};
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				flatten[row * 3 + col] = matrix[row][col];
			}
		}

		ESByteData data;

		ESCI2ColorMatrixMode eESCI2ColorMatrixMode = CAP_COLORMATRIX_08;
		switch (GetColorMatrixMode())
		{
		case kESColorMatrix8:
		{
			int rounded[9] = {0};
			ESCIRoundColorCorrectionMatrix(32, flatten, rounded);

			SInt8 ordered[9] = {0};
			for (int row = 0; row < 3; row++)
			{
				for (int col = 0; col < 3; col++)
				{
					int val = rounded[row * 3 + col];
					UInt8 oct = (UInt8)abs(val);
					oct |= ((val < 0) ? (1 << 7) : 0);
					ordered[index[row * 3 + col]] = oct;
				}
			}

			PESByte pStart = (PESByte)&ordered[0];
			PESByte pEnd = pStart + sizeof(ordered);
			data.assign(pStart, pEnd);
			eESCI2ColorMatrixMode = CAP_COLORMATRIX_08;
		}
		break;

		case kESColorMatrix16:
		{
			int rounded[9] = {0};
			ESCIRoundColorCorrectionMatrix(8192, flatten, rounded);

			SInt16 ordered[9] = {0};
			for (int row = 0; row < 3; row++)
			{
				for (int col = 0; col < 3; col++)
				{
					int val = rounded[row * 3 + col];
					UInt16 d_oct = CFSwapInt16HostToBig((UInt16)abs(val));
					d_oct |= ((val < 0) ? (1 << 15) : 0);
					ordered[index[row * 3 + col]] = d_oct;
				}
			}

			PESByte pStart = (PESByte)&ordered[0];
			PESByte pEnd = pStart + sizeof(ordered);
			data.assign(pStart, pEnd);
			eESCI2ColorMatrixMode = CAP_COLORMATRIX_16;
		}
		break;

		default:
			return kESErrorFatalError;
		}

		dicColorMatrix[FCCSTR(eESCI2ColorMatrixMode)] = data;

		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

ESErrorCode CESCI2Accessor::SetColorMatrix(ESFloatArray arColorMatrix)
{
	ES_COLOR_MATRIX matrix;
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			matrix[r][c] = arColorMatrix.at(r * 3 + c);
		}
	}
	return _SetColorMatrix(matrix);
}

//===============================================================
//!
//! 傾き補正をサポートするか
//!
//===============================================================
bool CESCI2Accessor::IsSkewCorrectionSupported()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_SKEWCORRECTION);
		break;
	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_SKEWCORRECTION);
		break;
	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_SKEWCORRECTION);
		break;
	default:
		return false;
	}
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, strKey1.c_str(), strKey2.c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//! 傾き補正が有効か？
//!
//===============================================================
bool CESCI2Accessor::IsSkewCorrectionEnabled()
{
	if (!IsSkewCorrectionSupported())
	{
		return false;
	}
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_SKEWCORRECTION);
		break;
	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_SKEWCORRECTION);
		break;
	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_SKEWCORRECTION);
		break;
	default:
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	return (pValue ? ((pValue->find(strKey2) != pValue->end()) ? true : false) : false);
}

//===============================================================
//!
//! 傾き補正を設定します
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetSkewCorrectionEnabled(bool bSkewCorrectionEnabled)
{
	if (!IsSkewCorrectionSupported())
	{
		return kESErrorSequenceError;
	}
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_SKEWCORRECTION);
		break;
	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_SKEWCORRECTION);
		break;
	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_SKEWCORRECTION);
		break;
	default:
		return kESErrorInvalidParameter;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	if (pStrSet != NULL)
	{
		if (bSkewCorrectionEnabled)
		{
			if (pStrSet->find(strKey2) == pStrSet->end())
			{
				pStrSet->insert(strKey2);
			}
		}
		else
		{
			pStrSet->erase(strKey2);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

//===============================================================
//!
//! サポートするモノクロ閾値を取得する
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedThreshold()
{
	try
	{
		ESAny &anyValue = m_dicCapabilities[FCCSTR(CAP_THRESHOLD)];
		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
		else
		{
			; // return empty
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//! モノクロ閾値を取得する
//!
//===============================================================
ESNumber CESCI2Accessor::GetThreshold()
{
	if (GetBitsPerSample() != 1)
	{
		return 0;
	}
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_THRESHOLD).c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//! モノクロ閾値を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetThreshold(ESNumber nThreshold)
{
	if (GetBitsPerSample() != 1)
	{
		return kESErrorSequenceError;
	}
	// assert
	ESAny anySupportedThreshold = GetSupportedThreshold();
	if (anySupportedThreshold.type() == typeid(ST_ES_RANGE))
	{
		ST_ES_RANGE &stRange = boost::any_cast<ST_ES_RANGE &>(anySupportedThreshold);
		assert((stRange.nMin <= (ESNumber)nThreshold) && ((ESNumber)nThreshold <= stRange.nMax));
	}
	else if (anySupportedThreshold.type() == typeid(ESIndexSet))
	{
		ESIndexSet &index = boost::any_cast<ESIndexSet &>(anySupportedThreshold);
		assert(index.find(nThreshold) != index.end());
	}
	else
	{
		return kESErrorFatalError;
	}

	m_dicParameters[FCCSTR(CAP_THRESHOLD)] = (ESNumber)nThreshold;
	return kESErrorNoError;
}

ESAny CESCI2Accessor::GetSupportedCapability(const UInt32 &key)
{
	try
	{
		ESAny &anyValue = m_dicCapabilities[FCCSTR(key)];
		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	return ESAny();
}

ESAny CESCI2Accessor::GetSupportedBackGroundRemoval()
{
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_BG_REMOVAL).c_str());
	if (pAnyArray != NULL)
	{
		ESIndexSet indexs;
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_BG_REMOVAL_OFF:
					indexs.insert(kESBackgroundRemovalLevelOff);
					break;

				case CAP_BG_REMOVAL_LOW:
					indexs.insert(kESBackgroundRemovalLevelLow);
					break;

				case CAP_BG_REMOVAL_HIGH:
					indexs.insert(kESBackgroundRemovalLevelHigh);
					break;

				default:
					break;
				}
			}
		}
		return indexs.size() ? indexs : ESAny();
	}
	return ESAny();
}
ESNumber CESCI2Accessor::GetBackGroundRemoval()
{
	ESStringA *pStrCode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_BG_REMOVAL).c_str());
	if (pStrCode != NULL)
	{
		switch (FourCharCode(*pStrCode))
		{
		case CAP_BG_REMOVAL_OFF:
			return kESBackgroundRemovalLevelOff;

		case CAP_BG_REMOVAL_LOW:
			return kESBackgroundRemovalLevelLow;

		case CAP_BG_REMOVAL_HIGH:
			return kESBackgroundRemovalLevelHigh;

		default:
			break;
		}
	}
	return kESBackgroundRemovalLevelOff;
}
ESErrorCode CESCI2Accessor::SetBackGroundRemoval(ESNumber nBackGroundRemoval)
{
	UInt32 bgRemoval = CAP_EDGEFILL_COLOR_WH;
	switch (nBackGroundRemoval)
	{
	case kESBackgroundRemovalLevelOff:
		bgRemoval = CAP_BG_REMOVAL_OFF;
		break;

	case kESBackgroundRemovalLevelLow:
		bgRemoval = CAP_BG_REMOVAL_LOW;
		break;

	case kESBackgroundRemovalLevelHigh:
		bgRemoval = CAP_BG_REMOVAL_HIGH;
		break;

	default:
		return kESErrorInvalidParameter;
	}
	m_dicParameters[FCCSTR(CAP_BG_REMOVAL)] = FCCSTR(bgRemoval);
	return kESErrorNoError;
}

ESAny CESCI2Accessor::GetSupportedSharpnessLevel()
{
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_SHARPNESSFILTER).c_str());
	if (pAnyArray != NULL)
	{
		ESIndexSet indexs;
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_SHARPNESSFILTER_NORMAL:
					indexs.insert(kESSharpnessFilterLevelNorm);
					break;

				case CAP_SHARPNESSFILTER_SHARPNESS1:
					indexs.insert(kESSharpnessFilterLevelSharpVeryLow);
					break;

				case CAP_SHARPNESSFILTER_SHARPNESS2:
					indexs.insert(kESSharpnessFilterLevelSharpLow);
					break;

				case CAP_SHARPNESSFILTER_SHARPNESS3:
					indexs.insert(kESSharpnessFilterLevelSharpMiddle);
					break;

				case CAP_SHARPNESSFILTER_SHARPNESS4:
					indexs.insert(kESSharpnessFilterLevelSharpHigh);
					break;

				case CAP_SHARPNESSFILTER_SMOOTHING1:
					indexs.insert(kESSharpnessFilterLevelSmoothingVeryLow);
					break;

				case CAP_SHARPNESSFILTER_SMOOTHING2:
					indexs.insert(kESSharpnessFilterLevelSmoothingLow);
					break;

				case CAP_SHARPNESSFILTER_SMOOTHING3:
					indexs.insert(kESSharpnessFilterLevelSmoothingMiddle);
					break;

				case CAP_SHARPNESSFILTER_SMOOTHING4:
					indexs.insert(kESSharpnessFilterLevelSmoothingHigh);
					break;

				default:
					break;
				}
			}
		}
		return indexs.size() ? indexs : ESAny();
	}
	return ESAny();
}
ESNumber CESCI2Accessor::GetSharpnessLevel()
{
	ESStringA *pStrCode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_SHARPNESSFILTER).c_str());
	if (pStrCode != NULL)
	{
		switch (FourCharCode(*pStrCode))
		{
		case CAP_SHARPNESSFILTER_NORMAL:
			return kESSharpnessFilterLevelNorm;

		case CAP_SHARPNESSFILTER_SHARPNESS1:
			return kESSharpnessFilterLevelSharpVeryLow;

		case CAP_SHARPNESSFILTER_SHARPNESS2:
			return kESSharpnessFilterLevelSharpLow;

		case CAP_SHARPNESSFILTER_SHARPNESS3:
			return kESSharpnessFilterLevelSharpMiddle;

		case CAP_SHARPNESSFILTER_SHARPNESS4:
			return kESSharpnessFilterLevelSharpHigh;

		case CAP_SHARPNESSFILTER_SMOOTHING1:
			return kESSharpnessFilterLevelSmoothingVeryLow;

		case CAP_SHARPNESSFILTER_SMOOTHING2:
			return kESSharpnessFilterLevelSmoothingLow;

		case CAP_SHARPNESSFILTER_SMOOTHING3:
			return kESSharpnessFilterLevelSmoothingMiddle;

		case CAP_SHARPNESSFILTER_SMOOTHING4:
			return kESSharpnessFilterLevelSmoothingHigh;

		default:
			break;
		}
	}
	return kESSharpnessFilterLevelNorm;
}
ESErrorCode CESCI2Accessor::SetSharpnessLevel(ESNumber nSharpnessFilterLevel)
{
	UInt32 sharpnessFilter = CAP_SHARPNESSFILTER_NORMAL;
	switch (nSharpnessFilterLevel)
	{
	case kESSharpnessFilterLevelNorm:
		sharpnessFilter = CAP_SHARPNESSFILTER_NORMAL;
		break;

	case kESSharpnessFilterLevelSharpVeryLow:
		sharpnessFilter = CAP_SHARPNESSFILTER_SHARPNESS1;
		break;

	case kESSharpnessFilterLevelSharpLow:
		sharpnessFilter = CAP_SHARPNESSFILTER_SHARPNESS2;
		break;

	case kESSharpnessFilterLevelSharpMiddle:
		sharpnessFilter = CAP_SHARPNESSFILTER_SHARPNESS3;
		break;

	case kESSharpnessFilterLevelSharpHigh:
		sharpnessFilter = CAP_SHARPNESSFILTER_SHARPNESS4;
		break;

	case kESSharpnessFilterLevelSmoothingVeryLow:
		sharpnessFilter = CAP_SHARPNESSFILTER_SMOOTHING1;
		break;

	case kESSharpnessFilterLevelSmoothingLow:
		sharpnessFilter = CAP_SHARPNESSFILTER_SMOOTHING2;
		break;

	case kESSharpnessFilterLevelSmoothingMiddle:
		sharpnessFilter = CAP_SHARPNESSFILTER_SMOOTHING3;
		break;

	case kESSharpnessFilterLevelSmoothingHigh:
		sharpnessFilter = CAP_SHARPNESSFILTER_SMOOTHING4;
		break;

	default:
		break;
	}
	m_dicParameters[FCCSTR(CAP_SHARPNESSFILTER)] = FCCSTR(sharpnessFilter);
	return kESErrorNoError;
}

//===============================================================
ESAny CESCI2Accessor::GetSupportedContrast()
{
	return GetSupportedCapability(CAP_CONSTRAST);
}
ESNumber CESCI2Accessor::GetContrast()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_CONSTRAST).c_str());
	return (pValue ? *pValue : 0);
}
ESErrorCode CESCI2Accessor::SetContrast(ESNumber nContrast)
{
	ESAny anySupported = GetSupportedContrast();
	if (anySupported.type() == typeid(ST_ES_RANGE))
	{
		ST_ES_RANGE &stRange = boost::any_cast<ST_ES_RANGE &>(anySupported);
		assert((stRange.nMin <= nContrast) && (nContrast <= stRange.nMax));
	}
	else if (anySupported.type() == typeid(ESIndexSet))
	{
		ESIndexSet &index = boost::any_cast<ESIndexSet &>(anySupported);
		assert(index.find(nContrast) != index.end());
	}
	else
	{
		return kESErrorFatalError;
	}
	m_dicParameters[FCCSTR(CAP_CONSTRAST)] = nContrast;
	return kESErrorNoError;
}

//===============================================================
ESAny CESCI2Accessor::GetSupportedBrightness()
{
	return GetSupportedCapability(CAP_BRTIGHTNESS);
}
ESNumber CESCI2Accessor::GetBrightness()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_BRTIGHTNESS).c_str());
	return (pValue ? *pValue : 0);
}
ESErrorCode CESCI2Accessor::SetBrightness(ESNumber nBrightness)
{
	ESAny anySupported = GetSupportedBrightness();
	if (anySupported.type() == typeid(ST_ES_RANGE))
	{
		ST_ES_RANGE &stRange = boost::any_cast<ST_ES_RANGE &>(anySupported);
		assert((stRange.nMin <= nBrightness) && (nBrightness <= stRange.nMax));
	}
	else if (anySupported.type() == typeid(ESIndexSet))
	{
		ESIndexSet &index = boost::any_cast<ESIndexSet &>(anySupported);
		assert(index.find(nBrightness) != index.end());
	}
	else
	{
		return kESErrorFatalError;
	}
	m_dicParameters[FCCSTR(CAP_BRTIGHTNESS)] = nBrightness;
	return kESErrorNoError;
}

//===============================================================
ESAny CESCI2Accessor::GetSupportedGammaScale()
{
	return GetSupportedCapability(CAP_GAMMA_SCALE);
}
ESNumber CESCI2Accessor::GetGammaScale()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_GAMMA_SCALE).c_str());
	return (pValue ? *pValue : 0);
}
ESErrorCode CESCI2Accessor::SetGammaScale(ESNumber nGammaScale)
{
	ESAny anySupported = GetSupportedGammaScale();
	if (anySupported.type() == typeid(ST_ES_RANGE))
	{
		ST_ES_RANGE &stRange = boost::any_cast<ST_ES_RANGE &>(anySupported);
		assert((stRange.nMin <= nGammaScale) && (nGammaScale <= stRange.nMax));
	}
	else if (anySupported.type() == typeid(ESIndexSet))
	{
		ESIndexSet &index = boost::any_cast<ESIndexSet &>(anySupported);
		assert(index.find(nGammaScale) != index.end());
	}
	else
	{
		return kESErrorFatalError;
	}
	m_dicParameters[FCCSTR(CAP_GAMMA_SCALE)] = nGammaScale;
	return kESErrorNoError;
}

//===============================================================
bool CESCI2Accessor::IsDetectBlankPageSupported()
{
	bool bSupported = false;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_DETECTBP).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL && FourCharCode(*pKey) == CAP_DETECTBP_ON)
			{
				bSupported = true;
			}
		}
	}
	return bSupported;
}

bool CESCI2Accessor::IsDetectBlankPageEnabled()
{
	ESStringA *pStrValue = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_DETECTBP).c_str());
	if (pStrValue)
	{
		switch (FourCharCode(*pStrValue))
		{
		case CAP_DETECTBP_OFF:
			return false;

		case CAP_DETECTBP_ON:
			return true;

		default:
			break;
		}
	}
	return false;
}

ESErrorCode CESCI2Accessor::SetDetectBlankPageEnabled(bool nDetectBlankPage)
{
	ESCI2DetectBlankPageMode eESCI2DetectBlankPageMode = CAP_DETECTBP_OFF;
	switch (nDetectBlankPage)
	{
	case false:
		eESCI2DetectBlankPageMode = CAP_DETECTBP_OFF;
		break;

	case true:
		eESCI2DetectBlankPageMode = CAP_DETECTBP_ON;
		break;

	default:
		return kESErrorInvalidParameter;
	}
	m_dicParameters[FCCSTR(CAP_DETECTBP)] = FCCSTR(eESCI2DetectBlankPageMode);
	return kESErrorNoError;
}

//===============================================================
ESAny CESCI2Accessor::GetSupportedDetectBlankPageLevel()
{
	return GetSupportedCapability(CAP_DETECTBPLEVEL);
}
ESNumber CESCI2Accessor::GetDetectBlankPageLevel()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_DETECTBPLEVEL).c_str());
	return (pValue ? *pValue : 0);
}
ESErrorCode CESCI2Accessor::SetDetectBlankPageLevel(ESNumber nDetectBlankPageLevel)
{
	ESAny anySupported = GetSupportedDetectBlankPageLevel();
	if (anySupported.type() == typeid(ST_ES_RANGE))
	{
		ST_ES_RANGE &stRange = boost::any_cast<ST_ES_RANGE &>(anySupported);
		assert((stRange.nMin <= nDetectBlankPageLevel) && (nDetectBlankPageLevel <= stRange.nMax));
	}
	else if (anySupported.type() == typeid(ESIndexSet))
	{
		ESIndexSet &index = boost::any_cast<ESIndexSet &>(anySupported);
		assert(index.find(nDetectBlankPageLevel) != index.end());
	}
	else
	{
		return kESErrorFatalError;
	}
	m_dicParameters[FCCSTR(CAP_DETECTBPLEVEL)] = nDetectBlankPageLevel;
	return kESErrorNoError;
}

//===============================================================
bool CESCI2Accessor::IsDetectColorTypeSupported()
{
	bool bSupported = false;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_DETECTCT).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL && FourCharCode(*pKey) == CAP_DETECTCT_ON)
			{
				bSupported = true;
			}
		}
	}
	return bSupported;
}

bool CESCI2Accessor::IsDetectColorTypeEnabled()
{
	ESStringA *pStrValue = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_DETECTCT).c_str());
	if (pStrValue)
	{
		switch (FourCharCode(*pStrValue))
		{
		case CAP_DETECTCT_OFF:
			return false;

		case CAP_DETECTCT_ON:
			return true;

		default:
			break;
		}
	}
	return false;
}

ESErrorCode CESCI2Accessor::SetDetectColorTypeEnabled(bool nDetectColorTypeMode)
{
	ESCI2DetectColorTypeMode eESCI2DetectColorTypeMode = CAP_DETECTCT_OFF;
	switch (nDetectColorTypeMode)
	{
	case false:
		eESCI2DetectColorTypeMode = CAP_DETECTCT_OFF;
		break;

	case true:
		eESCI2DetectColorTypeMode = CAP_DETECTCT_ON;
		break;

	default:
		return kESErrorInvalidParameter;
	}
	m_dicParameters[FCCSTR(CAP_DETECTCT)] = FCCSTR(eESCI2DetectColorTypeMode);
	return kESErrorNoError;
}

//===============================================================
bool CESCI2Accessor::IsSkipImageSupported()
{
	bool bSupported = false;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_SKIPIMAGE).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL && FourCharCode(*pKey) == CAP_SKIPIMAGE_ON)
			{
				bSupported = true;
			}
		}
	}
	return bSupported;
}

bool CESCI2Accessor::IsSkipImageEnabled()
{
	ESStringA *pStrValue = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_SKIPIMAGE).c_str());
	if (pStrValue)
	{
		switch (FourCharCode(*pStrValue))
		{
		case CAP_SKIPIMAGE_OFF:
			return false;

		case CAP_SKIPIMAGE_ON:
			return true;

		default:
			break;
		}
	}
	return false;
}

ESErrorCode CESCI2Accessor::SetSkipImageEnabled(bool nSkipImageEnhancementMode)
{
	ESCI2SkipImageEnhancementMode eESCI2SkipImageEnhancementMode = CAP_SKIPIMAGE_OFF;
	switch (nSkipImageEnhancementMode)
	{
	case false:
		eESCI2SkipImageEnhancementMode = CAP_SKIPIMAGE_OFF;
		break;

	case true:
		eESCI2SkipImageEnhancementMode = CAP_SKIPIMAGE_ON;
		break;

	default:
		return kESErrorInvalidParameter;
	}
	m_dicParameters[FCCSTR(CAP_SKIPIMAGE)] = FCCSTR(eESCI2SkipImageEnhancementMode);
	return kESErrorNoError;
}

//===============================================================

//===============================================================
//!
//! サポートするJpegのQuality値を取得する
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedJPEGQuality()
{
	try
	{
		ESAny &anyValue = m_dicCapabilities[FCCSTR(CAP_JPEGQUALITY)];
		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//! JpegのQuality値を取得する
//!
//===============================================================
ESNumber CESCI2Accessor::GetJPEGQuality()
{
	if (GetImageFormat() != kESImageFormatJPEG)
	{
		return 0;
	}
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_JPEGQUALITY).c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//! JpegのQuality値を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetJPEGQuality(ESNumber nJPEGQuality)
{
	if (GetImageFormat() != kESImageFormatJPEG)
	{
		return kESErrorFatalError;
	}
	ESAny anySupported = GetSupportedJPEGQuality();
	if (anySupported.type() == typeid(ST_ES_RANGE))
	{
		ST_ES_RANGE &stRange = boost::any_cast<ST_ES_RANGE &>(anySupported);
		assert((stRange.nMin <= nJPEGQuality) && (nJPEGQuality <= stRange.nMax));
	}
	else if (anySupported.type() == typeid(ESIndexSet))
	{
		ESIndexSet &index = boost::any_cast<ESIndexSet &>(anySupported);
		assert(index.find(nJPEGQuality) != index.end());
	}
	else
	{
		return kESErrorFatalError;
	}
	m_dicParameters[FCCSTR(CAP_JPEGQUALITY)] = nJPEGQuality;
	return kESErrorNoError;
}

//===============================================================
//!
//! サポートする静音モードを取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedQuietModes()
{
	ESIndexSet indexs;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_QUIETMODE).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_QUIETMODE_OFF:
					indexs.insert(kESQuietModeOff);
					break;

				case CAP_QUIETMODE_ON:
					indexs.insert(kESQuietModeOn);
					break;

				case CAP_QUIETMODE_PREF:
					indexs.insert(kESQuietModePreferDeviceSetting);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexs;
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

ESErrorCode CESCI2Accessor::WaitUntilSetForcus(ESFloat *pfOutFocus)
{
	ESErrorCode err = kESErrorNoError;
	while (true)
	{
		err = GetStatus();
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

		ESNumber *pStrFactor = nullptr;
		pStrFactor = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicStatus, FCCSTR(STAT_FOCUS).c_str(), FCCSTR(STAT_FOCUS_VALID).c_str());
		if (pStrFactor)
		{
			if (pfOutFocus)
			{
				*pfOutFocus = (ESFloat)(*pStrFactor) / 10.0;
			}
			break;
		}
	}

BAIL:
	return err;
}

ESErrorCode CESCI2Accessor::DoAutoFocus(ESFloat *pfOutFocus)
{

	if (!IsAutoFocusSupported())
	{
		return kESErrorNoError;
	}

	ESCI2Mode eCurrentMode = GetMode();
	ESErrorCode err = SetMode(kModeControl);
	if (IS_ERR_CODE(err))
	{
		return err;
	}

	{
		{
			ESDictionaryA dicParameter;

			// 設定パラメーター作成

			err = ParametersForScan(dicParameter);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}

			// 設定パラメーターセット
			err = SendParameters(dicParameter);
			if (IS_ERR_CODE(err))
			{
				goto BAIL;
			}
		}

		CESCI2DataConstructor cDataConstructor;
		cDataConstructor.AppendFourCharString(FCCSTR(MECH_AUTOFOCUS));
		ESByteData &data = cDataConstructor.GetData();

		err = RequestMechanicalControlWithParameter(data);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}

		Sleep(WAIT_AUTOFOCUS);

		err = WaitUntilSetForcus(pfOutFocus);
		if (IS_ERR_CODE(err))
		{
			goto BAIL;
		}
	}

BAIL:

	ESErrorCode err2 = SetMode(eCurrentMode);
	if (IS_ERR_SUCCESS(err) && IS_ERR_CODE(err2))
	{
		err = err2;
	}

	return err;
}

//===============================================================

//!

//! フォーカスを取得する

//!

//===============================================================

bool CESCI2Accessor::IsAutoFocusSupported()
{
	return (m_dicInformation.count(FCCSTR(INFO_AUTOFOCUS)) != 0) ? true : false;
}

bool CESCI2Accessor::IsManualFocusSupported()
{
	return (m_dicInformation.count(FCCSTR(INFO_MANUALFOCUS)) != 0) ? true : false;
}

ESFloat CESCI2Accessor::GetFocus()
{
	return 0.0f;
}

ESFloat CESCI2Accessor::GetMaxFocus()
{

	try
	{
		ESAny anySupported = m_dicInformation[FCCSTR(INFO_MANUALFOCUS)];
		ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anySupported);
		return (ESFloat)(range.nMax) / 10.0f;
	}
	catch (...)
	{
	}

	return FOCUS_MAX;
}

ESFloat CESCI2Accessor::GetMinFocus()
{

	try
	{
		ESAny anySupported = m_dicInformation[FCCSTR(INFO_MANUALFOCUS)];
		ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anySupported);
		return (ESFloat)(range.nMin) / 10.0f;
	}
	catch (...)
	{
	}

	return FOCUS_MIN;
}

//===============================================================
//!
//! フォーカスを設定する(ENUM_ES_FOCUS)
//!
//===============================================================

ESErrorCode CESCI2Accessor::SetFocus(ESFloat fFocus)
{
	if (!IsManualFocusSupported())
	{
		return kESErrorNoError;
	}

	if (fFocus < GetMinFocus())
	{
		fFocus = GetMinFocus();
	}
	else if (fFocus > GetMaxFocus())
	{
		fFocus = GetMaxFocus();
	}

	CESCI2DataConstructor cDataConstructor;

	cDataConstructor.AppendFourCharString(FCCSTR(MECH_MANUALFOCUS));

	cDataConstructor.AppendInteger((ESNumber)(fFocus * 10));

	ESByteData &data = cDataConstructor.GetData();

	ESErrorCode err = RequestMechanicalControlWithParameter(data);

	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}

	err = WaitUntilSetForcus(nullptr);
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}

BAIL:

	return err;
}

//===============================================================
//!
//!　静音モードを取得する
//!
//===============================================================
ESQuietMode CESCI2Accessor::GetQuietMode()
{
	ESStringA *pStrValue = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_QUIETMODE).c_str());
	if (pStrValue)
	{
		switch (FourCharCode(*pStrValue))
		{
		case CAP_QUIETMODE_OFF:
			return kESQuietModeOff;

		case CAP_QUIETMODE_ON:
			return kESQuietModeOn;

		case CAP_QUIETMODE_PREF:
			return kESQuietModePreferDeviceSetting;

		default:
			break;
		}
	}
	return 0;
}

//===============================================================
//!
//! 静音モードを設定する(ESQuietMode)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetQuietMode(ESNumber nQuietMode)
{
	ESIndexSet indexSupported = GetSupportedQuietModes();
	assert(indexSupported.find(nQuietMode) != indexSupported.end());

	ESCI2QuietMode eESCI2QuietMode = CAP_QUIETMODE_OFF;
	switch (nQuietMode)
	{
	case kESQuietModeOff:
		eESCI2QuietMode = CAP_QUIETMODE_OFF;
		break;

	case kESQuietModeOn:
		eESCI2QuietMode = CAP_QUIETMODE_ON;
		break;

	case kESQuietModePreferDeviceSetting:
		eESCI2QuietMode = CAP_QUIETMODE_PREF;
		break;

	default:
		return kESErrorInvalidParameter;
	}
	m_dicParameters[FCCSTR(CAP_QUIETMODE)] = FCCSTR(eESCI2QuietMode);
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsAutoCroppingSupported()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_CROPPING);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_CROPPING);
		break;

	case kESFunctionalUnitFlatbed:
	default:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_CROPPING);
		break;
	}
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, strKey1.c_str(), strKey2.c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsAutoCroppingEnabled()
{
	if (!IsAutoCroppingSupported())
	{
		return false;
	}
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_CROPPING);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_CROPPING);
		break;

	case kESFunctionalUnitFlatbed:
	default:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_CROPPING);
		break;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	return (pValue ? ((pValue->find(strKey2) != pValue->end()) ? true : false) : false);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetAutoCroppingEnabled(bool bAutoCroppingEnabled)
{
	if (!IsAutoCroppingSupported())
	{
		return kESErrorSequenceError;
	}
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_CROPPING);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_CROPPING);
		break;

	case kESFunctionalUnitFlatbed:
	default:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_CROPPING);
		break;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	if (pStrSet != NULL)
	{
		if (bAutoCroppingEnabled)
		{
			if (pStrSet->find(strKey2) == pStrSet->end())
			{
				pStrSet->insert(strKey2);
			}
			if (IsMaxScanSizeInNormalSpeedSupported())
			{
				// サイズ自動では、ターゲット解像度における通常速度での最大サイズをエリア指定する。
				return SetScanSize(GetMaxScanSizeInNormalSpeed());
			}
			else
			{
				return SetScanSize(GetMaxScanSize());
			}
		}
		else
		{
			pStrSet->erase(strKey2);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

bool CESCI2Accessor::IsAutoCroppingInscribedSupported()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_CROPPING_INSCRIBED);
		break;
	}
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, strKey1.c_str(), strKey2.c_str());
	return (pValue ? *pValue : false);
}

bool CESCI2Accessor::IsAutoCroppingInscribedEnabled()
{
	if (!IsAutoCroppingInscribedSupported())
	{
		return false;
	}
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_CROPPING_INSCRIBED);
		break;

	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	return (pValue ? ((pValue->find(strKey2) != pValue->end()) ? true : false) : false);
}

ESErrorCode CESCI2Accessor::SetAutoCroppingInscribedEnabled( bool bAutoCroppingEnabled )
{
	if (!IsAutoCroppingInscribedSupported())
	{
		return kESErrorSequenceError;
	}
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_CROPPING_INSCRIBED);
		break;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	if (pStrSet != NULL)
	{
		if (bAutoCroppingEnabled)
		{
			if (pStrSet->find(strKey2) == pStrSet->end())
			{
				pStrSet->insert(strKey2);
			}
			if (IsMaxScanSizeInNormalSpeedSupported())
			{
				// サイズ自動では、ターゲット解像度における通常速度での最大サイズをエリア指定する。
				return SetScanSize(GetMaxScanSizeInNormalSpeed());
			}
			else
			{
				return SetScanSize(GetMaxScanSize());
			}
		}
		else
		{
			pStrSet->erase(strKey2);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

//===============================================================
//!
//! HW自動切り出し・傾き補正の適用可能な解像度
//!
//===============================================================
ESAny CESCI2Accessor::GetCroppableResolutions()
{
	try
	{
		ESAny &anyValue = m_dicInformation[FCCSTR(INFO_CROPPABLERESOL)];
		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedCroppingSizes()
{
	try
	{
		ESAny anySupported = m_dicCapabilities[FCCSTR(CAP_CROPPINGSIZE)];

		if (anySupported.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anySupported);
			ESFloatArray index;
			BOOST_FOREACH (const ESAny &anyValue, anyArray)
			{
				if (anyValue.type() == typeid(ESNumber))
				{
					index.push_back(((ESFloat)boost::any_cast<ESNumber>(anyValue)) / 100.0f);
				}
			}
			return index;
		}
		else if (anySupported.type() == typeid(ST_ES_RANGE))
		{
			ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anySupported);
			ST_ES_RANGE_F fRange;
			fRange.fMax = (ESFloat)(range.nMax) / 100.0f;
			fRange.fMin = (ESFloat)(range.nMin) / 100.0f;
			fRange.fStep = (ESFloat)(range.nStep) / 100.0f;
			return fRange;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESFloat CESCI2Accessor::GetCroppingSize()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_CROPPINGSIZE).c_str());
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetCroppingSize(ESFloat fCroppingSize)
{
	m_dicParameters[FCCSTR(CAP_CROPPINGSIZE)] = (ESNumber)(fCroppingSize * 100.0f);
	return kESErrorNoError;
}

//===============================================================
//!
//! バッファサイズ取得
//!
//===============================================================
ESNumber CESCI2Accessor::GetBufferSize()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(PAR_MAX_BUFFERSIZE).c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//! バッファサイズ設定
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetBufferSize(ESNumber nBufferSize)
{
	m_dicParameters[FCCSTR(PAR_MAX_BUFFERSIZE)] = (ESNumber)nBufferSize;
	return kESErrorNoError;
}

//===============================================================
//!
//! デバイスの最大バッファサイズ取得
//!
//===============================================================
ESNumber CESCI2Accessor::GetDeviceMaxBufferSize()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicInformation, FCCSTR(INFO_DEV_BUFFERSIZE).c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//! ドキュメントサイズ自動検知をサポートするか？
//!
//===============================================================
bool CESCI2Accessor::IsDocumentSizeDetectionSupported()
{
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitDocumentFeeder:
	{
		ESDictionaryA *pDicInfo = SafeKeysDataPtr<ESDictionaryA>(m_dicInformation, FCCSTR(INFO_ADF).c_str());
		if ((pDicInfo != NULL) &&
			((pDicInfo->count(FCCSTR(INFO_ADF_WIDTHDETECTION)) != 0) ||
			 (pDicInfo->count(FCCSTR(INFO_ADF_HEIGHTDETECTION)) != 0)))
		{
			return true;
		}
	}
	break;

	case kESFunctionalUnitFlatbed:
	{
		ESDictionaryA *pDicInfo = SafeKeysDataPtr<ESDictionaryA>(m_dicInformation, FCCSTR(INFO_FB).c_str());
		if ((pDicInfo != NULL) &&
			((pDicInfo->count(FCCSTR(INFO_FB_WIDTHDETECTION)) != 0) ||
			 (pDicInfo->count(FCCSTR(INFO_FB_HEIGHTDETECTION)) != 0)))
		{
			return true;
		}
	}
	break;

	default:
		return false;
	}

	return false;
}

//===============================================================
//!
//! 検知するドキュメントサイズを取得する
//!
//===============================================================
ST_ES_SIZE_F CESCI2Accessor::GetDetectedDocumentSize()
{
	const ST_ES_SIZE_F stZeroSize = MakeSize(0.0f, 0.0f);
	struct stDocumentInfo
	{
		UInt32 un32DocumentSizeType;
		ST_ES_SIZE_F stfSizeDocument;
	};
	const stDocumentInfo c_listDocumentInfo[] = {
		{STAT_PAPERSIZE_A3V, MakeSize(11.69f, 16.54f)},
		{STAT_PAPERSIZE_WLT, MakeSize(11.0f, 17.0f)},
		{STAT_PAPERSIZE_B4V, MakeSize(10.12f, 14.33f)},
		{STAT_PAPERSIZE_LGV, MakeSize(8.5f, 14.0f)},
		{STAT_PAPERSIZE_A4V, MakeSize(8.27f, 11.69f)},
		{STAT_PAPERSIZE_A4H, MakeSize(11.69f, 8.27f)},
		{STAT_PAPERSIZE_LTV, MakeSize(8.5f, 11.0f)},
		{STAT_PAPERSIZE_LTH, MakeSize(11.0f, 8.5f)},
		{STAT_PAPERSIZE_B5V, MakeSize(7.17f, 10.12f)},
		{STAT_PAPERSIZE_B5H, MakeSize(10.12f, 7.17f)},
		{STAT_PAPERSIZE_A5V, MakeSize(5.83f, 8.27f)},
		{STAT_PAPERSIZE_A5H, MakeSize(8.27f, 5.83f)},
		{STAT_PAPERSIZE_B6V, MakeSize(5.04f, 7.17f)},
		{STAT_PAPERSIZE_B6H, MakeSize(7.17f, 5.04f)},
		{STAT_PAPERSIZE_A6V, MakeSize(4.13f, 5.83f)},
		{STAT_PAPERSIZE_A6H, MakeSize(5.83f, 4.13f)},
		{STAT_PAPERSIZE_EXV, MakeSize(7.25f, 10.5f)},
		{STAT_PAPERSIZE_EXH, MakeSize(10.5f, 7.25f)},
		{STAT_PAPERSIZE_HLTV, MakeSize(5.50f, 8.50f)},
		{STAT_PAPERSIZE_HLTH, MakeSize(8.50f, 5.50f)},
		{STAT_PAPERSIZE_PCV, MakeSize(3.50f, 5.00f)},
		{STAT_PAPERSIZE_PCH, MakeSize(5.00f, 3.50f)},
		{STAT_PAPERSIZE_KGV, MakeSize(4.00f, 6.00f)},
		{STAT_PAPERSIZE_KGH, MakeSize(6.00f, 4.00f)},
		{STAT_PAPERSIZE_CKV, stZeroSize},
		{STAT_PAPERSIZE_CKH, stZeroSize},
		{STAT_PAPERSIZE_8KV, MakeSize(10.63f, 15.35f)}, // 270x390[mm]
		{STAT_PAPERSIZE_16KV, MakeSize(7.68f, 10.63f)}, // 195x270[mm]
		{STAT_PAPERSIZE_16KH, MakeSize(10.63f, 7.68f)}, // 270x195[mm]
		{STAT_PAPERSIZE_OTHR, stZeroSize},
		{STAT_PAPERSIZE_INVD, stZeroSize},
	};

	if (GetStatus() != kESErrorNoError)
	{
		return stZeroSize;
	}

	try
	{
		ESDictionaryA &dicDocumentSize = boost::any_cast<ESDictionaryA &>(m_dicStatus[FCCSTR(STAT_PAPERSIZE)]);
		ESStringA strDocumentSize;

		switch (GetFunctionalUnitType())
		{
		case kESFunctionalUnitDocumentFeeder:
			strDocumentSize = boost::any_cast<ESStringA>(dicDocumentSize[FCCSTR(ERROR_PART_ADF)]);
			break;
		case kESFunctionalUnitFlatbed:
			strDocumentSize = boost::any_cast<ESStringA>(dicDocumentSize[FCCSTR(ERROR_PART_FB)]);
			break;
		default:
			return stZeroSize;
		}

		UInt32 un32DocumentSizeCode = FourCharCode(strDocumentSize);
		for (int i = 0; i < _countof(c_listDocumentInfo); i++)
		{
			if (c_listDocumentInfo[i].un32DocumentSizeType == un32DocumentSizeCode)
			{
				return c_listDocumentInfo[i].stfSizeDocument;
			}
		}
	}
	catch (...)
	{
	}
	return stZeroSize;
}

//===============================================================
//!
//!
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedLaminatedPaperModes()
{
	ESIndexSet indexs;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_LAMINATEDPAPER).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_LAMINATEDPAPER_OFF:
					indexs.insert(kESLaminatedPaperModeOff);
					break;

				case CAP_LAMINATEDPAPER_ON:
					indexs.insert(kESLaminatedPaperModeOn);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexs;
}

//===============================================================
//!
//!
//!
//===============================================================
ESLaminatedPaperMode CESCI2Accessor::GetLaminatedPaperMode()
{
	ESStringA *pStrCode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_LAMINATEDPAPER).c_str());
	if (pStrCode != NULL)
	{
		switch (FourCharCode(*pStrCode))
		{
		case CAP_LAMINATEDPAPER_OFF:
			return kESLaminatedPaperModeOff;

		case CAP_LAMINATEDPAPER_ON:
			return kESLaminatedPaperModeOn;

		default:
			break;
		}
	}
	return 0;
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetLaminatedPaperMode(ESNumber nLaminatedPaperMode)
{
	ESIndexSet indexSupported = GetSupportedLaminatedPaperModes();
	assert(indexSupported.find(nLaminatedPaperMode) != indexSupported.end());

	UInt32 eLaminatedPaper = CAP_LAMINATEDPAPER_OFF;
	switch (nLaminatedPaperMode)
	{
	case kESLaminatedPaperModeOff:
		eLaminatedPaper = CAP_LAMINATEDPAPER_OFF;
		break;

	case kESLaminatedPaperModeOn:
		eLaminatedPaper = CAP_LAMINATEDPAPER_ON;
		break;

	default:
		break;
	}
	m_dicParameters[FCCSTR(CAP_LAMINATEDPAPER)] = FCCSTR(eLaminatedPaper);
	return kESErrorNoError;
}

//===============================================================
//!
//! フィーダー(ADF)機能をサポートしているか?
//!
//===============================================================
bool CESCI2Accessor::IsFeederSupported()
{
	ESIndexSet index = GetSupportedFunctionalUnitTypes();
	if (index.find(kESFunctionalUnitDocumentFeeder) != index.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//===============================================================
//!
//! フィーダー(ADF)機能が有効か？
//!
//===============================================================
bool CESCI2Accessor::IsFeederEnabled()
{
	if (m_eFunctionalUnitType == kESFunctionalUnitDocumentFeeder)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//===============================================================
//!
//! オートスキャン機能をサポートしているか?
//!
//===============================================================
bool CESCI2Accessor::IsAutoScanSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_AUTOSCAN).c_str());
	return (pValue ? *pValue : false);
}

bool CESCI2Accessor::IsFeedTypeADF()
{
	ESStringA *pStrADFType = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_TYPE).c_str());
	if (pStrADFType != NULL)
	{
		switch (FourCharCode(*pStrADFType))
		{
		case INFO_ADF_TYPE_PAGE:
			return false;
		case INFO_ADF_TYPE_SHEETFEED:
			return true;
		default:
			break;
		}
	}
	return true;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsCarrierSheetDetectionSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_CRSHEETDETECTION).c_str());
	return (pValue ? *pValue : false);
}

bool CESCI2Accessor::IsAdfLoadSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_LOAD).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//! 両面同時読み取りのタイプを取得する
//!
//===============================================================
ESDuplexType CESCI2Accessor::GetDuplexType()
{
	ESStringA *pStrDuplexType = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_DUPLEX).c_str());
	if (pStrDuplexType != NULL)
	{
		switch (FourCharCode(*pStrDuplexType))
		{
		case INFO_ADF_DUPLEX_1PASS:
			return kESDuplexType1Pass;
		case INFO_ADF_DUPLEX_2PASS:
			return kESDuplexType2Pass;
		default:
			break;
		}
	}
	return (ESDuplexType)0;
}

//===============================================================
//!
//! 両面同時読み取りをサポートしているか?
//!
//===============================================================
bool CESCI2Accessor::IsDuplexSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_DUPLEX).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//! 両面同時読み取り機能が有効か?
//!
//===============================================================
bool CESCI2Accessor::IsDuplexEnabled()
{
	if (!IsDuplexSupported())
	{
		return false;
	}
	if (!IsFeederSupported())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_DUPLEX)) != pValue->end()) ? true : false) : false);
}

//===============================================================
//!
//! 両面同時読み取り機能の有効状態を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetDuplexEnabled(bool bDuplexEnabled)
{
	if (!IsDuplexSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_DUPLEX);
		if (bDuplexEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsPaperEndDetectionSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_PAPEREND_DETECTION).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsPaperEndDetectionEnabled()
{
	if (!IsPaperEndDetectionSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_PAPEREND_DETECTION)) != pValue->end()) ? true : false) : false);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPaperEndDetectionEnabled(bool bPaperEndDetectionEnabled)
{
	if (!IsPaperEndDetectionSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_PAPEREND_DETECTION);
		if (bPaperEndDetectionEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsOverScanSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_OVERSCAN_SIZE).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsOverScanEnabled()
{
	if (!IsOverScanSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_OVERSCAN_SIZE)) != pValue->end()) ? true : false) : false);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetOverScanEnabled(bool bOverScanEnabled)
{
	if (!IsOverScanSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_OVERSCAN_SIZE);
		if (bOverScanEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}


//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsOverScanForCroppingSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_OVERSCAN_SIZE_CROPPING).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsOverScanForCroppingEnabled()
{
	if (!IsOverScanForCroppingSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_OVERSCAN_SIZE_CROPPING)) != pValue->end()) ? true : false) : false);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetOverScanForCroppingEnabled(bool bOverScanEnabled)
{
	if (!IsOverScanForCroppingSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_OVERSCAN_SIZE_CROPPING);
		if (bOverScanEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}


//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsPassportCarrierSheetSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_PASSPORTCRSHEET).c_str());
	return (pValue ? *pValue : false);
}
bool CESCI2Accessor::IsPassportCarrierSheetEnabled()
{
	if (!IsPassportCarrierSheetSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_PASSPORTCRSHEET)) != pValue->end()) ? true : false) : false);
}
ESErrorCode CESCI2Accessor::SetPassportCarrierSheetEnabled(bool bEnabled)
{
	if (!IsPassportCarrierSheetSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_PASSPORTCRSHEET);
		if (bEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

//===============================================================
//!
//! サポートする重送検知機能を取得する
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedDoubleFeedDetections()
{
	ESIndexSet indexSupported;
	try
	{
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(m_dicCapabilities[FCCSTR(CAP_ADF)]);
		if (dic.count(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION0)) != 0)
		{
			indexSupported.insert(kESDoubleFeedDetectionDisable);
		}
		if (dic.count(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION3)) != 0)
		{
			indexSupported.insert(kESDoubleFeedDetectionVeryLow);
		}
		if (dic.count(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION1)) != 0)
		{
			indexSupported.insert(kESDoubleFeedDetectionLow);
		}
		if (dic.count(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION2)) != 0)
		{
			indexSupported.insert(kESDoubleFeedDetectionHigh);
		}
		if (indexSupported.size() > 0)
		{
			if (indexSupported.find(kESDoubleFeedDetectionDisable) == indexSupported.end())
			{
				indexSupported.insert(kESDoubleFeedDetectionDisable);
			}
		}
	}
	catch (...)
	{
	}
	return indexSupported;
}

//===============================================================
//!
//! 重送検知の設定を取得する
//!
//===============================================================
ESDoubleFeedDetection CESCI2Accessor::GetDoubleFeedDetection()
{
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pValue != NULL)
	{
		if (pValue->find(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION0)) != pValue->end())
		{
			return kESDoubleFeedDetectionDisable;
		}
		if (pValue->find(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION1)) != pValue->end())
		{
			return kESDoubleFeedDetectionLow;
		}
		if (pValue->find(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION2)) != pValue->end())
		{
			return kESDoubleFeedDetectionHigh;
		}
		if (pValue->find(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION3)) != pValue->end())
		{
			return kESDoubleFeedDetectionVeryLow;
		}
	}
	return kESDoubleFeedDetectionDisable;
}

//===============================================================
//!
//! 重送検知を設定を変更(ESDoubleFeedDetection)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetDoubleFeedDetection(ESNumber nDoubleFeedDetection)
{
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}
	ESIndexSet indexSupportedDFD = GetSupportedDoubleFeedDetections();
	if (indexSupportedDFD.find(nDoubleFeedDetection) == indexSupportedDFD.end())
	{
		return kESErrorInvalidParameter;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey0 = FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION0);
		ESStringA strKey1 = FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION1);
		ESStringA strKey2 = FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION2);
		ESStringA strKey3 = FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION3);

		pStrSet->erase(strKey0);
		pStrSet->erase(strKey1);
		pStrSet->erase(strKey2);
		pStrSet->erase(strKey3);

		switch (nDoubleFeedDetection)
		{
		case kESDoubleFeedDetectionLow:
			pStrSet->insert(strKey1);
			break;
		case kESDoubleFeedDetectionHigh:
			pStrSet->insert(strKey2);
			break;
		case kESDoubleFeedDetectionVeryLow:
			pStrSet->insert(strKey3);
			break;
		case kESDoubleFeedDetectionDisable:
		default:
		{
			const ESDictionaryA *pDic = SafeKeysDataCPtr<ESDictionaryA>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str());
			if (pDic && pDic->count(FCCSTR(CAP_ADF_DOUBLEFEED_DETECTION0)) != 0)
			{
				pStrSet->insert(strKey0);
			}
		}
		break;
		}
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

ESIndexSet CESCI2Accessor::GetSupportedBGColors()
{
	ESIndexSet indexSupported;
	try
	{
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(m_dicCapabilities[FCCSTR(CAP_ADF)]);
		if (dic.count(FCCSTR(CAP_ADF_BGCOLOR_WHITE)) != 0)
		{
			indexSupported.insert(kESBGColorWhite);
		}
		if (dic.count(FCCSTR(CAP_ADF_BGCOLOR_BLACK)) != 0)
		{
			indexSupported.insert(kESBGColorBlack);
		}
		if (dic.count(FCCSTR(CAP_ADF_BGCOLOR_GRAY)) != 0)
		{
			indexSupported.insert(kESBGColorGray);
		}
	}
	catch (...)
	{
	}
	return indexSupported;
}

ESBGColor CESCI2Accessor::GetBGColor()
{
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pValue != NULL)
	{
		if (pValue->find(FCCSTR(CAP_ADF_BGCOLOR_WHITE)) != pValue->end())
		{
			return kESBGColorWhite;
		}
		if (pValue->find(FCCSTR(CAP_ADF_BGCOLOR_BLACK)) != pValue->end())
		{
			return kESBGColorBlack;
		}
		if (pValue->find(FCCSTR(CAP_ADF_BGCOLOR_GRAY)) != pValue->end())
		{
			return kESBGColorGray;
		}
	}
	return kESBGColorBlack;
}

ESErrorCode CESCI2Accessor::SetBGColor(ESNumber nBGColor)
{
	if (!IsFeederEnabled())
	{
		return kESErrorNoError;
	}
	ESIndexSet indexSupported = GetSupportedBGColors();
	if (indexSupported.find(nBGColor) == indexSupported.end())
	{
		return kESErrorNoError;
	}
	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey0 = FCCSTR(CAP_ADF_BGCOLOR_WHITE);
		ESStringA strKey1 = FCCSTR(CAP_ADF_BGCOLOR_BLACK);
		ESStringA strKey2 = FCCSTR(CAP_ADF_BGCOLOR_GRAY);

		pStrSet->erase(strKey0);
		pStrSet->erase(strKey1);
		pStrSet->erase(strKey2);

		switch (nBGColor)
		{
		case kESBGColorWhite:
			pStrSet->insert(strKey0);
			break;

		case kESBGColorBlack:
			pStrSet->insert(strKey1);
			break;

		case kESBGColorGray:
			pStrSet->insert(strKey2);
			break;

		default:
			break;
		}
	}
	return kESErrorNoError;
}

//
//
//
bool CESCI2Accessor::IsDoubleFeedDetectionRangeSupported()
{
	return (m_dicInformation.count(FCCSTR(INFO_MIN_DOUBLEFEEDLEN).c_str()) != 0) ? true : false;
}
//
//
//
ESFloat CESCI2Accessor::GetMinDoubleFeedDetectionRangeLength()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicInformation, FCCSTR(INFO_MIN_DOUBLEFEEDLEN).c_str());
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}
//
//
//
ESFloat CESCI2Accessor::GetDoubleFeedDetectionRangeOffset()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(PAR_DOUBLEFEED_AREA).c_str(), ESCI2_DIC_KEY_OFFSET);
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}
//
//
//
ESErrorCode CESCI2Accessor::SetDoubleFeedDetectionRangeOffset(ESFloat fDoubleFeedDetectionRangeOffset)
{
	if (!IsDoubleFeedDetectionRangeSupported())
	{
		return kESErrorSequenceError;
	}
	try
	{
		ESStringA strKey = FCCSTR(PAR_DOUBLEFEED_AREA);
		if (m_dicParameters.count(strKey) == 0)
		{
			ESDictionaryA dicValue;
			m_dicParameters[strKey] = dicValue;
		}
		ESDictionaryA *pDicValue = SAFE_KEYS_DATA_PTR(m_dicParameters, strKey.c_str(), ESDictionaryA);
		if (pDicValue != NULL)
		{
			(*pDicValue)[ESCI2_DIC_KEY_OFFSET] = (ESNumber)(fDoubleFeedDetectionRangeOffset * 100.0f);
		}
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//
//
//
ESFloat CESCI2Accessor::GetDoubleFeedDetectionRangeLength()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(PAR_DOUBLEFEED_AREA).c_str(), ESCI2_DIC_KEY_LENGTH);
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}
//
//
//
ESErrorCode CESCI2Accessor::SetDoubleFeedDetectionRangeLength(ESFloat fDoubleFeedDetectionRangeLength)
{
	if (!IsDoubleFeedDetectionRangeSupported())
	{
		return kESErrorSequenceError;
	}
	try
	{
		if (fDoubleFeedDetectionRangeLength < GetMinDoubleFeedDetectionRangeLength())
		{
			fDoubleFeedDetectionRangeLength = GetMinDoubleFeedDetectionRangeLength();
		}

		ESStringA strKey = FCCSTR(PAR_DOUBLEFEED_AREA);
		if (m_dicParameters.count(strKey) == 0)
		{
			ESDictionaryA dicValue;
			m_dicParameters[strKey] = dicValue;
		}
		ESDictionaryA *pDicValue = SAFE_KEYS_DATA_PTR(m_dicParameters, strKey.c_str(), ESDictionaryA);
		if (pDicValue != NULL)
		{
			(*pDicValue)[ESCI2_DIC_KEY_LENGTH] = (ESNumber)(fDoubleFeedDetectionRangeLength * 100.0f);
		}
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//
//
//
bool CESCI2Accessor::IsLengthDoubleFeedDetectionSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_LENGTHDOUBLEFEED_DETECTION).c_str());
	return (pValue ? *pValue : false);
}
//
//
//
bool CESCI2Accessor::IsLengthDoubleFeedDetectionEnabled()
{
	if (!IsLengthDoubleFeedDetectionSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_LENGTHDOUBLEFEED_DETECTION)) != pValue->end()) ? true : false) : false);
}
//
//
//
ESErrorCode CESCI2Accessor::SetLengthDoubleFeedDetectionEnabled(bool bLengthDoubleFeedDetectionEnabled)
{
	if (!IsLengthDoubleFeedDetectionSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_LENGTHDOUBLEFEED_DETECTION);
		if (bLengthDoubleFeedDetectionEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
	}
	return kESErrorNoError;
}
//
//
//
ESFloat CESCI2Accessor::GetMaxLengthDoubleFeedDetectionLength()
{
	if (!IsLengthDoubleFeedDetectionSupported())
	{
		return 0.0f;
	}
	ST_ES_SIZE_F *pValue = SafeKeyDicInKeysDataPtr<ST_ES_SIZE_F>(m_dicInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_SCANAREA).c_str());
	if (pValue)
	{
		return pValue->cy;
	}
	return 0.0f;
}

//
//
//
ESFloat CESCI2Accessor::GetLengthDoubleFeedDetectionLength()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(PAR_LENGTHDOUBLEFEED_LEN).c_str());
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}
//
//
//
ESErrorCode CESCI2Accessor::SetLengthDoubleFeedDetectionLength(ESFloat fLengthDoubleFeedDetectionLength)
{
	m_dicParameters[FCCSTR(PAR_LENGTHDOUBLEFEED_LEN)] = (ESNumber)(fLengthDoubleFeedDetectionLength * 100.0f);
	return kESErrorNoError;
}

bool CESCI2Accessor::IsImageDoubleFeedDetectionSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_SWDOUBLEFEED_DETECTION).c_str());
	return (pValue ? *pValue : false);
}
bool CESCI2Accessor::IsImageDoubleFeedDetectionEnabled()
{
	if (!IsImageDoubleFeedDetectionSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_SWDOUBLEFEED_DETECTION)) != pValue->end()) ? true : false) : false);
}
ESErrorCode CESCI2Accessor::SetImageDoubleFeedDetectionEnabled(bool bImageDoubleFeedDetectionEnabled)
{
	if (!IsImageDoubleFeedDetectionSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_SWDOUBLEFEED_DETECTION);
		if (bImageDoubleFeedDetectionEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
	}
	return kESErrorNoError;
}

bool CESCI2Accessor::IsImagePaperProtectionSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_SWPAPERPROTECTION).c_str());
	return (pValue ? *pValue : false);
}
bool CESCI2Accessor::IsImagePaperProtectionEnabled()
{
	if (!IsImagePaperProtectionSupported())
	{
		return false;
	}
	if (!IsFeederEnabled())
	{
		return false;
	}
	ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	return (pValue ? ((pValue->find(FCCSTR(CAP_ADF_SWPAPERPROTECTION)) != pValue->end()) ? true : false) : false);
}
ESErrorCode CESCI2Accessor::SetImagePaperProtectionEnabled(bool bPaperProtectionEnabled)
{
	if (!IsImagePaperProtectionSupported())
	{
		return kESErrorSequenceError;
	}
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
	if (pStrSet != NULL)
	{
		ESStringA strKey = FCCSTR(CAP_ADF_SWPAPERPROTECTION);
		if (bPaperProtectionEnabled)
		{
			if (pStrSet->find(strKey) == pStrSet->end())
			{
				pStrSet->insert(strKey);
			}
		}
		else
		{
			pStrSet->erase(strKey);
		}
	}
	return kESErrorNoError;
}


bool CESCI2Accessor::IsErrorCodeSupported()
{
	return ( GetErrorCode() ? true : false );
}

ESNumber CESCI2Accessor::GetErrorCode()
{
	if ( IS_ERR_CODE( GetStatus() ) ){
		return 0;
	}
	ESNumber* pValue = SafeKeysDataPtr< ESNumber >( m_dicStatus, FCCSTR(STAT_ERRORCODE).c_str() );
	if ( pValue != NULL ) {
		return *pValue;
	}

	return 0;
}

//===============================================================
//!
//!
//!
//===============================================================
ESStringA CESCI2Accessor::GetDocumentSeparationSTAT()
{
	if (IS_ERR_CODE(GetStatus()))
	{
		return "";
	}
	ESStringA *pValue = SafeKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_DOCSEPARATION));
	if (pValue != NULL)
	{
		return *pValue;
	}
	return "";
}

//===============================================================
//!
//!ドキュメントセパレーションをサポートするか？
//!
//===============================================================
bool CESCI2Accessor::IsDocumentSeparationSupported()
{
	return (GetDocumentSeparationSTAT().empty() ? false : true);
}

//===============================================================
//!
//!ドキュメントセパレーションが有効か？
//!
//===============================================================
bool CESCI2Accessor::IsDocumentSeparationEnabled()
{
	if (!IsFeederEnabled())
	{
		return false;
	}
	return (GetDocumentSeparationSTAT() == FCCSTR(STAT_DOCSEPARATION_ON));
}

ESNumber CESCI2Accessor::GetDocumentSeparation()
{
	ESStringA documentSeparation = GetDocumentSeparationSTAT();
	if (!documentSeparation.empty())
	{
		switch (FourCharCode(documentSeparation))
		{
		case STAT_DOCSEPARATION_ON:
			return kESDocumentSeparationStatusOn;
		case STAT_DOCSEPARATION_OFF:
			return kESDocumentSeparationStatusOff;
		case STAT_DOCSEPARATION_SOFT:
			return kESDocumentSeparationStatusSoft;
		default:
			break;
		}
	}
	return kESDocumentSeparationStatusOff;
}

//===============================================================
//!
//! Unavailable Scan Parameter information.
//!
//===============================================================
ESDictionaryA CESCI2Accessor::GetUnavailableScanParameterSTAT()
{
	if (IS_ERR_CODE(GetStatus()))
	{
		goto BAIL;
	}
	{
		ESDictionaryA *pValue = SafeKeysDataPtr<ESDictionaryA>(m_dicStatus, FCCSTR(STAT_SCANPARAMETER).c_str());
		if (pValue != NULL)
		{
			return *pValue;
		}
	}
BAIL:
	return ESDictionaryA();
}

bool CESCI2Accessor::IsUnavailableScanParameterStatusSupported()
{
	return (GetUnavailableScanParameterSTAT().empty() ? false : true);
}

ESIndexArray CESCI2Accessor::GetUnavailableScanParameterStatus()
{
	ESIndexArray indexArray;
	{
		ESDictionaryA dict = GetUnavailableScanParameterSTAT();
		if (dict.size())
		{
			if (dict.count(FCCSTR(STAT_SCANPARAMETER_ALL)))
			{
				indexArray.push_back(kESUnavailableScanParameterStatusAll);
			}
			if (dict.count(FCCSTR(STAT_SCANPARAMETER_AFMC)))
			{
				indexArray.push_back(kESUnavailableScanParameterStatusAFMC);
			}
			if (dict.count(FCCSTR(STAT_SCANPARAMETER_PCS)))
			{
				indexArray.push_back(kESUnavailableScanParameterStatusPCS);
			}
		}
	}
	return indexArray;
}

//===============================================================
//!
//!
//!
//===============================================================
ESStringA CESCI2Accessor::GetScannerPositionSTAT()
{
	if (IS_ERR_CODE(GetStatus()))
	{
		goto BAIL;
	}
	{
		ESStringA *pValue = SafeKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_SCANNERPOSITION).c_str());
		if (pValue != NULL)
		{
			return *pValue;
		}
	}
BAIL:
	return "";
}

bool CESCI2Accessor::IsScannerPositionStatusSupported()
{
	return (GetScannerPositionSTAT().empty() ? false : true);
}

ESNumber CESCI2Accessor::GetScannerPositionStatus()
{
	ESStringA scannerPosition = GetScannerPositionSTAT();
	if (!scannerPosition.empty())
	{
		switch (FourCharCode(scannerPosition))
		{
		case STAT_SCANNERPOSITION_FLAT:
			return kESScannerPositionStatusFlat;
		case STAT_SCANNERPOSITION_TILT:
			return kESScannerPositionStatusTilt;
		case STAT_SCANNERPOSITION_INVALID:
			return kESScannerPositionStatusInvalid;
		default:
			break;
		}
	}
	return kESScannerPositionStatusInvalid;
}

//===============================================================
//!
//! カードレバー状態問い合わせ
//!
//===============================================================
ESStringA CESCI2Accessor::GetCardScanningSTAT()
{
	if (IS_ERR_CODE(GetStatus()))
	{
		return "";
	}
	ESStringA *pValue = SafeKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_CARDSCANNING));
	if (pValue != NULL)
	{
		return *pValue;
	}
	return "";
}
bool CESCI2Accessor::IsCardScanningSupported()
{
	return (GetCardScanningSTAT().empty() ? false : true);
}
bool CESCI2Accessor::IsCardScanningEnabled()
{
	if (!IsFeederEnabled())
	{
		return false;
	}
	return (GetCardScanningSTAT() == FCCSTR(STAT_CARDSCANNING_ON));
}

//===============================================================
//!
//!
//!
//===============================================================
ESStringA CESCI2Accessor::GetSensorGlassSTAT()
{
	if (IS_ERR_CODE(GetStatus()))
	{
		return "";
	}
	ESStringA *pValue = SafeKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_SENSORGLASS).c_str());
	if (pValue != NULL)
	{
		return *pValue;
	}
	return "";
}

bool CESCI2Accessor::IsSensorGlassStatusSupported()
{
	return (GetSensorGlassSTAT().empty() ? false : true);
}

#define SENSORGLASS_INTERVAL 100

ESNumber CESCI2Accessor::GetSensorGlassStatus()
{
	ESStringA sensorGlass;
	while (true)
	{

		sensorGlass = GetSensorGlassSTAT();

		if (sensorGlass.empty())
		{
			return kESSensorGlassNone;
		}

		// INVDが来ている間はSTATを要求し続ける
		if (sensorGlass == FCCSTR(STAT_SENSORGLASS_INVALID))
		{
			Sleep(SENSORGLASS_INTERVAL);
		}
		else
		{
			break;
		}
	}

	switch (FourCharCode(sensorGlass))
	{
	case STAT_SENSORGLASS_DIRTY:
		return kESSensorGlassDirty;
	default:
		break;
	}

	return kESSensorGlassNone;
}

ESWarningStatus CESCI2Accessor::GetWarningStatus()
{
	try
	{
		if (!IS_ERR_CODE(GetStatus()))
		{
			ESDictionaryA &dict = boost::any_cast<ESDictionaryA &>(m_dicStatus[FCCSTR(STAT_WARNING)]);
			// 何か警告情報あればとにかく返す動作とするため、警告情報辞書中の任意のキーを取り出す（基本的に一つしかないはず）
			if (dict.size())
			{
				auto it = dict.begin();
				if (it->second.type() == typeid(ESStringA))
				{
					ESStringA &factor = boost::any_cast<ESStringA &>(it->second);

					switch (FourCharCode(factor))
					{
					case STAT_WARNING_NEEDCLEANING:
						return kESWarningNeedCleaning;
						break;
					default:
						break;
					}
				}
			}
		}
	}
	catch (...)
	{
		// any_cast error
	}
	return kESWarningNone;
}

//===============================================================
//!
//!ドキュメントが読み込まれたか？
//!
//===============================================================
bool CESCI2Accessor::IsDocumentLoaded()
{
	if (!IsFeederEnabled())
	{
		return false;
	}
	if (IS_ERR_CODE(GetStatus()))
	{
		// ステータス取得でエラーが起きたとしても、YES/NOを決定できる訳ではないため、ここでreturnはしない
	}
	ESStringA *pStrFactor = SafeKeyDicInKeysDataPtr<ESStringA>(m_dicStatus, FCCSTR(STAT_ERROR).c_str(), FCCSTR(ERROR_PART_ADF).c_str());
	if (pStrFactor != NULL && (*pStrFactor == FCCSTR(ERROR_FACTOR_PAPEREMPTY)))
	{
		return false;
	}
	else
	{
		return true;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedEdgeFillColors()
{
	ESIndexSet indexs;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_EDGEFILL_COLOR).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{
			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_EDGEFILL_COLOR_WH:
					indexs.insert(kESEdgeFillColorWhite);
					break;

				case CAP_EDGEFILL_COLOR_BK:
					indexs.insert(kESEdgeFillColorBlack);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexs;
}

//===============================================================
//!
//!
//!
//===============================================================
ESEdgeFillColor CESCI2Accessor::GetEdgeFillColor()
{
	ESStringA *pStrCode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_EDGEFILL_COLOR).c_str());
	if (pStrCode != NULL)
	{
		switch (FourCharCode(*pStrCode))
		{
		case CAP_EDGEFILL_COLOR_BK:
			return kESEdgeFillColorBlack;

		case CAP_EDGEFILL_COLOR_WH:
		default:
			break;
		}
	}
	return kESEdgeFillColorWhite;
}

//===============================================================
//!
//! (ESEdgeFillColor)
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetEdgeFillColor(ESNumber nEdgeFillColor)
{
	UInt32 eESCI2EdgeFillColor = CAP_EDGEFILL_COLOR_WH;
	switch (nEdgeFillColor)
	{
	case kESEdgeFillColorBlack:
		eESCI2EdgeFillColor = CAP_EDGEFILL_COLOR_BK;
		break;

	case kESEdgeFillColorWhite:
		eESCI2EdgeFillColor = CAP_EDGEFILL_COLOR_WH;
		break;

	default:
		return kESErrorInvalidParameter;
	}
	m_dicParameters[FCCSTR(CAP_EDGEFILL_COLOR)] = FCCSTR(eESCI2EdgeFillColor);
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedEdgeFillWidth()
{
	try
	{

		ESAny anySupported = m_dicCapabilities[FCCSTR(CAP_EDGEFILL_AREA)];
		if (anySupported.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anySupported);
			ESFloatArray index;
			BOOST_FOREACH (const ESAny &anyValue, anyArray)
			{
				if (anyValue.type() == typeid(ESNumber))
				{
					index.push_back(((ESFloat)boost::any_cast<ESNumber>(anyValue)) / 100.0f);
				}
			}
			return index;
		}
		else if (anySupported.type() == typeid(ST_ES_RANGE))
		{
			ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anySupported);
			ST_ES_RANGE_F fRange;
			fRange.fMax = (ESFloat)(range.nMax) / 100.0f;
			fRange.fMin = (ESFloat)(range.nMin) / 100.0f;
			fRange.fStep = (ESFloat)(range.nStep) / 100.0f;
			return fRange;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESFloat CESCI2Accessor::GetEdgeFillWidthLeft()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_EDGEFILL_AREA).c_str(), ESCI2_DIC_KEY_LEFT);
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetEdgeFillWidthLeft(ESFloat fEdgeFillWidthLeft)
{
	try
	{
		ESStringA strAreaKey = FCCSTR(CAP_EDGEFILL_AREA);
		if (m_dicParameters.count(strAreaKey) == 0)
		{
			ESDictionaryA dicAdd;
			m_dicParameters[strAreaKey] = dicAdd;
		}
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(m_dicParameters[strAreaKey]);
		dic[ESCI2_DIC_KEY_LEFT] = (ESNumber)(fEdgeFillWidthLeft * 100.0f);
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESFloat CESCI2Accessor::GetEdgeFillWidthRight()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_EDGEFILL_AREA).c_str(), ESCI2_DIC_KEY_RIGHT);
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetEdgeFillWidthRight(ESFloat fEdgeFillWidthRight)
{
	try
	{
		ESStringA strAreaKey = FCCSTR(CAP_EDGEFILL_AREA);
		if (m_dicParameters.count(strAreaKey) == 0)
		{
			ESDictionaryA dicAdd;
			m_dicParameters[strAreaKey] = dicAdd;
		}
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(m_dicParameters[strAreaKey]);
		dic[ESCI2_DIC_KEY_RIGHT] = (ESNumber)(fEdgeFillWidthRight * 100.0f);
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESFloat CESCI2Accessor::GetEdgeFillWidthTop()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_EDGEFILL_AREA).c_str(), ESCI2_DIC_KEY_TOP);
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetEdgeFillWidthTop(ESFloat fEdgeFillWidthTop)
{
	try
	{
		ESStringA strAreaKey = FCCSTR(CAP_EDGEFILL_AREA);
		if (m_dicParameters.count(strAreaKey) == 0)
		{
			ESDictionaryA dicAdd;
			m_dicParameters[strAreaKey] = dicAdd;
		}
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(m_dicParameters[strAreaKey]);
		dic[ESCI2_DIC_KEY_TOP] = (ESNumber)(fEdgeFillWidthTop * 100.0f);
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESFloat CESCI2Accessor::GetEdgeFillWidthBottom()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(CAP_EDGEFILL_AREA).c_str(), ESCI2_DIC_KEY_BOTTOM);
	return (pValue ? (ESFloat)(*pValue) / 100.0f : 0.0f);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetEdgeFillWidthBottom(ESFloat fEdgeFillWidthBottom)
{
	try
	{
		ESStringA strAreaKey = FCCSTR(CAP_EDGEFILL_AREA);
		if (m_dicParameters.count(strAreaKey) == 0)
		{
			ESDictionaryA dicAdd;
			m_dicParameters[strAreaKey] = dicAdd;
		}
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(m_dicParameters[strAreaKey]);
		dic[ESCI2_DIC_KEY_BOTTOM] = (ESNumber)(fEdgeFillWidthBottom * 100.0f);
		return kESErrorNoError;
	}
	catch (...)
	{
		return kESErrorFatalError;
	}
}

//===============================================================
//!
//!	クリーニング機能をサポートするか
//!
//===============================================================
bool CESCI2Accessor::IsCleaningSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_CLEANING).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsCalibrationSupported()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_ADF).c_str(), FCCSTR(CAP_ADF_CALIBRATION).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//! スキャンするページ枚数を取得する
//!
//===============================================================
ESNumber CESCI2Accessor::GetPagesToBeScanned()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicParameters, FCCSTR(PAR_PAGECOUNT).c_str());
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//! スキャンするページ枚数を設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPagesToBeScanned(ESNumber nPagesToBeScanned)
{
	if (!IsFeederEnabled())
	{
		return kESErrorSequenceError;
	}
	m_dicParameters[FCCSTR(PAR_PAGECOUNT)] = (ESNumber)nPagesToBeScanned;
	return kESErrorNoError;
}

//===============================================================
//!
//! Captureコマンドをサポートしているか？
//!
//===============================================================
bool CESCI2Accessor::IsCaptureCommandSupported()
{
	return m_bCaptureCommandSupport;
}

//===============================================================
//!
//! Captureコマンドをサポートしているか設定する
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetCaptureCommandSupported(bool bSupported)
{
	m_bCaptureCommandSupport = bSupported;
	return kESErrorNoError;
}

//===============================================================
//!
//! 現在のスキャン設定が、圧縮イメージ（JPEGなど）転送有効であるかどうかを返す。モノクロ設定であれば、JPEGは無効となる。
//!
//===============================================================
bool CESCI2Accessor::IsCompressed()
{
	ESNumber nColorFormat = GetColorFormat();

	switch (nColorFormat)
	{
	case kESColorFormatMono1:
	case kESColorFormatMonoDropR1:
	case kESColorFormatMonoDropG1:
	case kESColorFormatMonoDropB1:
		return false;
	default:
		return (GetImageFormat() == kESImageFormatJPEG);
	}
}

//
//
// Maintenance
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

ESAny CESCI2Accessor::GetMaintenanceResultForKey(ESStringA strKey)
{
	ESDictionaryA dicResults;
#if 0
	ESStringArrayA arString;
	arString.push_back( strKey );
	ESErrorCode err = GetMaintenanceResults( dicResults, &arString );
#else
	// FWの問題でキーを指定すると値が取れないケースがあるため、キーを指定しないよう修正
	ESErrorCode err = GetMaintenanceResults(dicResults, NULL);
#endif
	if (IS_ERR_SUCCESS(err) && dicResults.count(strKey) != 0)
	{
		return dicResults[strKey];
	}
	else
	{
		ESAny empty;
		return empty;
	}
}

//===============================================================
//!
//! スキャナセンサーのクリーニング動作を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCI2Accessor::DoCleaning()
{
	if (!IsCleaningSupported())
	{
		return kESErrorNoError;
	}

	CESCI2DataConstructor cDataConstructor;
	cDataConstructor.AppendFourCharString(FCCSTR(CAP_ADF));
	cDataConstructor.AppendFourCharString(FCCSTR(CAP_ADF_CLEANING));

	ESByteData &data = cDataConstructor.GetData();
	ESErrorCode err = RequestMechanicalControlWithParameter(data);
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}
	err = WaitUntilDone();
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}

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
ESErrorCode CESCI2Accessor::DoCalibration()
{
	if (!IsCalibrationSupported())
	{
		return kESErrorNoError;
	}

	CESCI2DataConstructor cDataConstructor;
	cDataConstructor.AppendFourCharString(FCCSTR(CAP_ADF));
	cDataConstructor.AppendFourCharString(FCCSTR(CAP_ADF_CALIBRATION));

	ESByteData &data = cDataConstructor.GetData();
	ESErrorCode err = RequestMechanicalControlWithParameter(data);
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}
	err = WaitUntilDone();
	if (IS_ERR_CODE(err))
	{
		goto BAIL;
	}
BAIL:
	return err;
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsPowerSaveSupported()
{
	bool *pValue = SafeKeysDataPtr<bool>(m_dicMaintenanceInformation, FCCSTR(INFO_POWERSAVE).c_str());
	return (pValue ? *pValue : false);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPowerSaveTimes()
{
	try
	{
		ESAny &anyResolutions = m_dicMaintenanceCapabilities[FCCSTR(CAP_POWERSAVE)]; // List of Range

		if (anyResolutions.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyResolutions);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyResolutions.type() == typeid(ST_ES_RANGE))
		{
			return boost::any_cast<ST_ES_RANGE>(anyResolutions);
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetPowerSaveTime()
{
	ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_POWERSAVE));
	ESNumber *pValue = SafeAnyDataPtr<ESNumber>(anyValue);
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPowerSaveTime(ESNumber nPowerSaveTime)
{
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_POWERSAVE)] = nPowerSaveTime;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPowerOffTimes()
{
	try
	{
		ESAny anyValue = m_dicMaintenanceCapabilities[FCCSTR(CAP_POWEROFF)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetPowerOffTime()
{
	ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_POWEROFF));
	ESNumber *pValue = SafeAnyDataPtr<ESNumber>(anyValue);
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPowerOffTime(ESNumber nPowerOffTime)
{
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_POWEROFF)] = (ESNumber)nPowerOffTime;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPowerOffTimes2nd()
{
	try
	{
		ESAny anyValue = m_dicMaintenanceCapabilities[FCCSTR(CAP_POWEROFF2ND)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetPowerOffTime2nd()
{
	ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_POWEROFF2ND));
	ESNumber *pValue = SafeAnyDataPtr<ESNumber>(anyValue);
	return (pValue ? *pValue : 0);
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPowerOffTime2nd(ESNumber nPowerOffTime2nd)
{
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_POWEROFF2ND)] = (ESNumber)nPowerOffTime2nd;
	return SendMaintenanceParameters(dicParameters);
}

static struct tm ESCI2GetDateWithRaw(UInt32 rawDate)
{
	struct tm date = {};

	UInt16 yearFrom2000 = ((rawDate)&0x0FF00000) >> 20;
	UInt8 month = ((rawDate)&0x000F0000) >> 16;
	UInt16 minutesFrom1stDay = ((rawDate)&0x0000FFFF);

	date.tm_year = (yearFrom2000 + 2000) - 1900; // tm_yearは1900からのカウント
	date.tm_mon = month - 1;					 // tm_monは0からのカウント
	date.tm_mday = minutesFrom1stDay / 60 / 24 + 1;
	date.tm_hour = minutesFrom1stDay / 60 % 24;
	date.tm_min = minutesFrom1stDay % 60;

	return date;
}

static bool IsValidDate(struct tm &date)
{
	const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// 月の範囲チェック
	if (date.tm_mon < 0 || date.tm_mon > 11)
	{
		return false;
	}
	// 日の範囲チェック
	int lastd = days[date.tm_mon];
	if (date.tm_mon == 1)
	{
		int year = date.tm_year + 1900;
		if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		{
			lastd = 29; // うるう年なら末日は29
		}
	}
	if (date.tm_mday < 1 || date.tm_mday > lastd)
	{
		return false;
	}
	return true;
}

static UInt32 ESCI2GetRawWithDate(struct tm date)
{
	UInt32 rawDate = 0;

	rawDate |= ((date.tm_year + 1900) - 2000) << 20; // tm_yearは1900からのカウント
	rawDate |= (date.tm_mon + 1) << 16;				 // tm_monは0からのカウント
	rawDate |= ((date.tm_mday - 1) * 24 * 60) + (date.tm_hour * 60) + date.tm_min;

	return rawDate;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedFirstPCConnectionDates()
{
	try
	{
		if (!IsShouldIgnoreDateAndTime())
		{
			ESAny anyValue = m_dicMaintenanceCapabilities[FCCSTR(CAP_DATEANDTIME)]; // List of Range

			if (anyValue.type() == typeid(ESAnyArray))
			{
				ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
				return AnyArrayToIndexSet(anyArray);
			}
			else if (anyValue.type() == typeid(ST_ES_RANGE))
			{
				return anyValue;
			}
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//! "%Y-%m-%d %H:%M"
//===============================================================
ESString CESCI2Accessor::GetFirstPCConnectionDate()
{
	ESString strDate;

	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_DATEANDTIME));
		ESNumber *pValue = SafeAnyDataPtr<ESNumber>(anyValue);
		if (pValue && *pValue)
		{
			UInt32 u32RawData = (UInt32)*pValue;
			struct tm date = ESCI2GetDateWithRaw(u32RawData);
			if (IsValidDate(date))
			{
				char buf[64] = {};
				if (strftime(buf, sizeof(buf), DATEFORMAT, &date) > 0)
				{
#ifdef WIN32
					strDate = ES_CMN_FUNCS::STRING::MBStringToUnicode(&buf[0]);
#else
					strDate = &buf[0];
#endif
				}
			}
		}
	}
	catch (...)
	{
	}
	return strDate;
}

ESErrorCode CESCI2Accessor::SetFirstPCConnectionDate(ESString strFirstPCConnectionDate)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 u32RawData = 0;
		if (!strFirstPCConnectionDate.empty())
		{

#ifdef WIN32
			ESStringA strFirstPCConnectionDateA = ES_CMN_FUNCS::STRING::UnicodeToMBString(strFirstPCConnectionDate);
#else
			ESStringA strFirstPCConnectionDateA = strFirstPCConnectionDate;
#endif
			if (!strFirstPCConnectionDateA.empty())
			{
				struct tm date = {};
#ifdef WIN32
				int ret = sscanf_s(strFirstPCConnectionDateA.c_str(), "%d-%d-%d %d:%d", &date.tm_year, &date.tm_mon, &date.tm_mday, &date.tm_hour, &date.tm_min);
#else
				int ret = sscanf(strFirstPCConnectionDateA.c_str(), "%d-%d-%d %d:%d", &date.tm_year, &date.tm_mon, &date.tm_mday, &date.tm_hour, &date.tm_min);
#endif
				if (ret != EOF)
				{
					date.tm_year -= 1900;
					date.tm_mon -= 1;
					u32RawData = ESCI2GetRawWithDate(date);
				}
			}
		}
		{
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_DATEANDTIME)] = (ESNumber)u32RawData;
			err = SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}
	return err;
}

//===============================================================
//!
//!
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedSensorGlassDirtSensitivities()
{
	ESIndexSet supportedSensorGlassDirtSensitivities;

	try
	{
		ESAny anyValue = m_dicMaintenanceCapabilities[FCCSTR(CAP_SENSORGLASS)];
		if (anyValue.empty())
		{
			return supportedSensorGlassDirtSensitivities;
		}

		ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
		for (auto key = anyArray.begin(); key != anyArray.end(); key++)
		{
			ESStringA *pValue = SafeAnyDataPtr<ESStringA>(*key);
			if (pValue)
			{
				switch (FourCharCode(*pValue))
				{
				case CAP_SENSORGLASS_OFF:
					supportedSensorGlassDirtSensitivities.insert(kESSensorGlassDirtSensitivityOff);
					break;
				case CAP_SENSORGLASS_LOW:
					supportedSensorGlassDirtSensitivities.insert(kESSensorGlassDirtSensitivityLow);
					break;
				case CAP_SENSORGLASS_NORMAL:
					supportedSensorGlassDirtSensitivities.insert(kESSensorGlassDirtSensitivityNormal);
					break;
				default:
					break;
				}
			}
		}
	}
	catch (...)
	{
	}
	return supportedSensorGlassDirtSensitivities;
}

ESNumber CESCI2Accessor::GetSensorGlassDirtSensitivity()
{
	ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_SENSORGLASS));
	if (!anyValue.empty())
	{
		ESStringA *pValue = SafeAnyDataPtr<ESStringA>(anyValue);
		if (pValue)
		{
			switch (FourCharCode(*pValue))
			{
			case CAP_SENSORGLASS_OFF:
				return kESSensorGlassDirtSensitivityOff;
				break;
			case CAP_SENSORGLASS_LOW:
				return kESSensorGlassDirtSensitivityLow;
				break;
			case CAP_SENSORGLASS_NORMAL:
				return kESSensorGlassDirtSensitivityNormal;
				break;
			default:
				break;
			}
		}
	}
	return kESSensorGlassDirtSensitivityOff;
}

ESErrorCode CESCI2Accessor::SetSensorGlassDirtSensitivity(ESNumber nGlassDirtSensitivity)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 eSensitivity = CAP_SENSORGLASS_OFF;
		switch (nGlassDirtSensitivity)
		{
		case kESSensorGlassDirtSensitivityOff:
			eSensitivity = CAP_SENSORGLASS_OFF;
			break;

		case kESSensorGlassDirtSensitivityLow:
			eSensitivity = CAP_SENSORGLASS_LOW;
			break;

		case kESSensorGlassDirtSensitivityNormal:
			eSensitivity = CAP_SENSORGLASS_NORMAL;
			break;

		default:
			break;
		}
		{
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_SENSORGLASS)] = FCCSTR(eSensitivity);
			err = SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}

	return err;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedSimplexScanCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny &anyValue = dicADF[FCCSTR(CAP_ADF_SIMPLEXSCANCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetSimplexScanCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_SIMPLEXSCANCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetSimplexScanCounter(ESNumber nSimplexScanCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_SIMPLEXSCANCOUNTER)] = (ESNumber)nSimplexScanCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedDuplexScanCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny &anyValue = dicADF[FCCSTR(CAP_ADF_DUPLEXSCANCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetDuplexScanCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_DUPLEXSCANCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetDuplexScanCounter(ESNumber nDuplexScanCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DUPLEXSCANCOUNTER)] = (ESNumber)nDuplexScanCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedSimplexCardScanCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		if (dicADF.count(FCCSTR(CAP_ADF_SIMPLEXCARDSCANCOUNTER)))
		{
			ESAny &anyValue = dicADF[FCCSTR(CAP_ADF_SIMPLEXCARDSCANCOUNTER)]; // List of Range

			if (anyValue.type() == typeid(ESAnyArray))
			{
				ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
				return AnyArrayToIndexSet(anyArray);
			}
			else if (anyValue.type() == typeid(ST_ES_RANGE))
			{
				return anyValue;
			}
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetSimplexCardScanCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_SIMPLEXCARDSCANCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetSimplexCardScanCounter(ESNumber nSimplexCardScanCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_SIMPLEXCARDSCANCOUNTER)] = (ESNumber)nSimplexCardScanCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedDuplexCardScanCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny &anyValue = dicADF[FCCSTR(CAP_ADF_DUPLEXCARDSCANCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetDuplexCardScanCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_DUPLEXCARDSCANCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetDuplexCardScanCounter(ESNumber nDuplexCardScanCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DUPLEXCARDSCANCOUNTER)] = (ESNumber)nDuplexCardScanCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedDocumentFeederLamp1Counters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_LAMP1COUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetDocumentFeederLamp1Counter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_LAMP1COUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetDocumentFeederLamp1Counter(ESNumber nDocumentFeederLamp1Counter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_LAMP1COUNTER)] = (ESNumber)nDocumentFeederLamp1Counter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedDocumentFeederLamp2Counters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_LAMP2COUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetDocumentFeederLamp2Counter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_LAMP2COUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetDocumentFeederLamp2Counter(ESNumber nDocumentFeederLamp2Counter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_LAMP2COUNTER)] = (ESNumber)nDocumentFeederLamp2Counter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPaperJamCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_PAPERJAMCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetPaperJamCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_PAPERJAMCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPaperJamCounter(ESNumber nPaperJamCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_PAPERJAMCOUNTER)] = (ESNumber)nPaperJamCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedUltraSonicDoubleFeedCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_DFEEDCOUNTER_ULTRASONIC)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetUltraSonicDoubleFeedCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_DFEEDCOUNTER_ULTRASONIC)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetUltraSonicDoubleFeedCounter(ESNumber nUltraSonicDoubleFeedCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DFEEDCOUNTER_ULTRASONIC)] = (ESNumber)nUltraSonicDoubleFeedCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedLengthDoubleFeedCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_DFEEDCOUNTER_LENGTH)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetLengthDoubleFeedCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_DFEEDCOUNTER_LENGTH)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetLengthDoubleFeedCounter(ESNumber nLengthDoubleFeedCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DFEEDCOUNTER_LENGTH)] = (ESNumber)nLengthDoubleFeedCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedImageDoubleFeedCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_DFEEDCOUNTER_SOFTWARE)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetImageDoubleFeedCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_DFEEDCOUNTER_SOFTWARE)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetImageDoubleFeedCounter(ESNumber nImageDoubleFeedCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DFEEDCOUNTER_SOFTWARE)] = (ESNumber)nImageDoubleFeedCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}
//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPaperProtectionCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_PAPERPROTECTCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetPaperProtectionCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_PAPERPROTECTCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPaperProtectionCounter(ESNumber nPaperProtectionCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_PAPERPROTECTCOUNTER)] = (ESNumber)nPaperProtectionCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedRollerKitCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_ROLLERKITCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetRollerKitCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_ROLLERKITCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetRollerKitCounter(ESNumber nRollerKitCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_ROLLERKITCOUNTER)] = (ESNumber)nRollerKitCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedRetardRollerCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_RETARDROLLERCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetRetardRollerCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_RETARDROLLERCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetRetardRollerCounter(ESNumber nRetardRollerCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_RETARDROLLERCOUNTER)] = (ESNumber)nRetardRollerCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPickupRollerCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_PICKUPROLLERCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetPickupRollerCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_PICKUPROLLERCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetPickupRollerCounter(ESNumber nPickupRollerCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_PICKUPROLLERCOUNTER)] = (ESNumber)nPickupRollerCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedSeparationPadCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_SEPARATIONPADCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetSeparationPadCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_SEPARATIONPADCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetSeparationPadCounter(ESNumber nSeparationPadCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_SEPARATIONPADCOUNTER)] = (ESNumber)nSeparationPadCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedSpecialDocumentCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_SPECIALDOCUMENTCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetSpecialDocumentCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_SPECIALDOCUMENTCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetSpecialDocumentCounter(ESNumber nSpecialDocumentCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_SPECIALDOCUMENTCOUNTER)] = (ESNumber)nSpecialDocumentCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedPassportCarrierSheetCounters()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_PASSPORT_CS_COUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

ESNumber CESCI2Accessor::GetPassportCarrierSheetCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_PASSPORT_CS_COUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

ESErrorCode CESCI2Accessor::SetPassportCarrierSheetCounter(ESNumber nCounter)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_PASSPORT_CS_COUNTER)] = (ESNumber)nCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedScanCounters()
{
	try
	{
		ESDictionaryA &dicFB = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_FB)]);
		ESAny anyValue = dicFB[FCCSTR(CAP_FB_SCANCOUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetScanCounter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_FB));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_FB_SCANCOUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetScanCounter(ESNumber nScanCounter)
{
	ESDictionaryA dicFB;
	dicFB[FCCSTR(CAP_FB_SCANCOUNTER)] = (ESNumber)nScanCounter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_FB)] = dicFB;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedFlatbedLamp1Counters()
{
	try
	{
		ESDictionaryA &dicFB = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_FB)]);
		ESAny anyValue = dicFB[FCCSTR(CAP_FB_LAMP1COUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetFlatbedLamp1Counter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_FB));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_FB_LAMP1COUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetFlatbedLamp1Counter(ESNumber nFlatbedLamp1Counter)
{
	ESDictionaryA dicFB;
	dicFB[FCCSTR(CAP_FB_LAMP1COUNTER)] = (ESNumber)nFlatbedLamp1Counter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_FB)] = dicFB;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedFlatbedLamp2Counters()
{
	try
	{
		ESDictionaryA &dicFB = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_FB)]);
		ESAny anyValue = dicFB[FCCSTR(CAP_FB_LAMP2COUNTER)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetFlatbedLamp2Counter()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_FB));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_FB_LAMP2COUNTER)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetFlatbedLamp2Counter(ESNumber nFlatbedLamp2Counter)
{
	ESDictionaryA dicFB;
	dicFB[FCCSTR(CAP_FB_LAMP2COUNTER)] = (ESNumber)nFlatbedLamp2Counter;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_FB)] = dicFB;
	return SendMaintenanceParameters(dicParameters);
}

// TOPF, TOPBのRawデータの単位は 1/1000 inch を表す。
// また、ファームウェアとしてはスキップ量を表すが、UIとしては先読み量を表す必要があり真逆であるため、-1を掛けて符号を反転する。
#define TOPCORRECTION_COEF (-1000.0f)

//===============================================================
//!
//!
//! ( 1/1000 inch -> inch )
//===============================================================
ESAny CESCI2Accessor::GetSupportedDocumentTopCorrectionsFront()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_TOPCORRECTION_FRONT)]; // List of Range
		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			ESFloatArray index;
			BOOST_FOREACH (const ESAny &anyValue, anyArray)
			{
				if (anyValue.type() == typeid(ESNumber))
				{
					// 外向けには符号が反転するため、リストも逆順で作る
					index.push_front(((ESFloat)boost::any_cast<ESNumber>(anyValue)) / TOPCORRECTION_COEF);
				}
			}
			return index;
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anyValue);
			ST_ES_RANGE_F fRange;
			// 外向けには符号が反転するため、min, maxも逆で作る
			fRange.fMax = (ESFloat)(range.nMin) / TOPCORRECTION_COEF;
			fRange.fMin = (ESFloat)(range.nMax) / TOPCORRECTION_COEF;
			fRange.fStep = (ESFloat)(range.nStep) / abs(TOPCORRECTION_COEF);
			return fRange;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//! ( 1/1000 inch -> inch )
//===============================================================
ESFloat CESCI2Accessor::GetDocumentTopCorrectionFront()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		ESNumber *pValue = SafeKeysDataPtr<ESNumber>(dic, FCCSTR(CAP_ADF_TOPCORRECTION_FRONT).c_str());
		return (pValue ? (ESFloat)(*pValue) / TOPCORRECTION_COEF : 0.0f);
	}
	catch (...)
	{
		return 0.0f;
	}
}

//===============================================================
//!
//!
//! ( inch -> 1/1000inch )
//===============================================================
ESErrorCode CESCI2Accessor::SetDocumentTopCorrectionFront(ESFloat fDocumentTopCorrectionFront)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_TOPCORRECTION_FRONT)] = (ESNumber)(fDocumentTopCorrectionFront * TOPCORRECTION_COEF);
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//! ( 1/1000 inch -> inch )
//===============================================================
ESAny CESCI2Accessor::GetSupportedDocumentTopCorrectionsBack()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_TOPCORRECTION_BACK)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			ESFloatArray index;
			BOOST_FOREACH (const ESAny &anyValue, anyArray)
			{
				if (anyValue.type() == typeid(ESNumber))
				{
					// 外向けには符号が反転するため、リストも逆順で作る
					index.push_front(((ESFloat)boost::any_cast<ESNumber>(anyValue)) / TOPCORRECTION_COEF);
				}
			}
			return index;
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anyValue);
			ST_ES_RANGE_F fRange;
			// 外向けには符号が反転するため、min, maxも逆で作る
			fRange.fMax = (ESFloat)(range.nMin) / TOPCORRECTION_COEF;
			fRange.fMin = (ESFloat)(range.nMax) / TOPCORRECTION_COEF;
			fRange.fStep = (ESFloat)(range.nStep) / abs(TOPCORRECTION_COEF);
			return fRange;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//! ( 1/1000 inch -> inch )
//===============================================================
ESFloat CESCI2Accessor::GetDocumentTopCorrectionBack()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		ESNumber *pValue = SafeKeysDataPtr<ESNumber>(dic, FCCSTR(CAP_ADF_TOPCORRECTION_BACK).c_str());
		return (pValue ? (ESFloat)(*pValue) / TOPCORRECTION_COEF : 0.0f);
	}
	catch (...)
	{
		return 0.0f;
	}
}

//===============================================================
//!
//!
//! ( inch -> 1/1000 inch )
//===============================================================
ESErrorCode CESCI2Accessor::SetDocumentTopCorrectionBack(ESFloat fDocumentTopCorrectionBack)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_TOPCORRECTION_BACK)] = (ESNumber)(fDocumentTopCorrectionBack * TOPCORRECTION_COEF);
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

// MAGF, MAGBのRawデータの単位は 1/10 % を表す。
#define DRIVECORRECTION_COEF 10.0f

//===============================================================
//!
//!
//! ( 0.1% -> % )
//===============================================================
ESAny CESCI2Accessor::GetSupportedDocumentDriveCorrectionsFront()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_DOCUMENTDRIVECORRECTION_FRONT)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			ESFloatArray index;
			BOOST_FOREACH (const ESAny &anyValue, anyArray)
			{
				if (anyValue.type() == typeid(ESNumber))
				{
					index.push_back(((ESFloat)boost::any_cast<ESNumber>(anyValue)) / DRIVECORRECTION_COEF);
				}
			}
			return index;
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anyValue);
			ST_ES_RANGE_F fRange;
			fRange.fMax = (ESFloat)(range.nMax) / DRIVECORRECTION_COEF;
			fRange.fMin = (ESFloat)(range.nMin) / DRIVECORRECTION_COEF;
			fRange.fStep = (ESFloat)(range.nStep) / DRIVECORRECTION_COEF;
			return fRange;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//! ( 0.1% -> % )
//===============================================================
ESFloat CESCI2Accessor::GetDocumentDriveCorrectionFront()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		ESNumber *pValue = SafeKeysDataPtr<ESNumber>(dic, FCCSTR(CAP_ADF_DOCUMENTDRIVECORRECTION_FRONT).c_str());
		return (pValue ? (ESFloat)(*pValue) / DRIVECORRECTION_COEF : 0.0f);
	}
	catch (...)
	{
		return 0.0f;
	}
}

//===============================================================
//!
//!
//! ( % -> 0.1% )
//===============================================================
ESErrorCode CESCI2Accessor::SetDocumentDriveCorrectionFront(ESFloat fDocumentDriveCorrectionFront)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DOCUMENTDRIVECORRECTION_FRONT)] = (ESNumber)ES_CMN_FUNCS::roundf(fDocumentDriveCorrectionFront * DRIVECORRECTION_COEF);
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//! ( 0.1% -> % )
//===============================================================
ESAny CESCI2Accessor::GetSupportedDocumentDriveCorrectionsBack()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_DOCUMENTDRIVECORRECTION_BACK)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			ESFloatArray index;
			BOOST_FOREACH (const ESAny &anyValue, anyArray)
			{
				if (anyValue.type() == typeid(ESNumber))
				{
					index.push_back(((ESFloat)boost::any_cast<ESNumber>(anyValue)) / DRIVECORRECTION_COEF);
				}
			}
			return index;
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			ST_ES_RANGE &range = boost::any_cast<ST_ES_RANGE &>(anyValue);
			ST_ES_RANGE_F fRange;
			fRange.fMax = (ESFloat)(range.nMax) / DRIVECORRECTION_COEF;
			fRange.fMin = (ESFloat)(range.nMin) / DRIVECORRECTION_COEF;
			fRange.fStep = (ESFloat)(range.nStep) / DRIVECORRECTION_COEF;
			return fRange;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//! ( 0.1% -> % )
//===============================================================
ESFloat CESCI2Accessor::GetDocumentDriveCorrectionBack()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		ESNumber *pValue = SafeKeysDataPtr<ESNumber>(dic, FCCSTR(CAP_ADF_DOCUMENTDRIVECORRECTION_BACK).c_str());
		return (pValue ? (ESFloat)(*pValue) / DRIVECORRECTION_COEF : 0.0f);
	}
	catch (...)
	{
		return 0.0f;
	}
}

//===============================================================
//!
//!
//! ( % -> 0.1% )
//===============================================================
ESErrorCode CESCI2Accessor::SetDocumentDriveCorrectionBack(ESFloat fDocumentDriveCorrectionBack)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_DOCUMENTDRIVECORRECTION_BACK)] = (ESNumber)ES_CMN_FUNCS::roundf(fDocumentDriveCorrectionBack * DRIVECORRECTION_COEF);
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedLightIntensitiesBack()
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(CAP_ADF_LIGHTINTENSITY_BACK)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

//===============================================================
//!
//!
//!
//===============================================================
ESNumber CESCI2Accessor::GetLightIntensityBack()
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(CAP_ADF_LIGHTINTENSITY_BACK)]);
	}
	catch (...)
	{
		return 0;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
ESErrorCode CESCI2Accessor::SetLightIntensityBack(ESNumber nLightIntensityBack)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(CAP_ADF_LIGHTINTENSITY_BACK)] = (ESNumber)nLightIntensityBack;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsDefaultPasswordTypeSupported()
{
	ESNumber *pValue = SafeKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_DEF_PASSWDTYPE).c_str());
	return (pValue ? (*pValue ? true : false) : false);
}

ESNumber CESCI2Accessor::GetDefaultPasswordType()
{
	ESNumber *defaultPasswordType = SafeKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_DEF_PASSWDTYPE).c_str());

	switch (defaultPasswordType ? *defaultPasswordType : 0)
	{
	case 0:
		return kESDefaultPasswordTypeSerial;
		break;
	case 1:
		return kESDefaultPasswordTypeUnique;
		break;
	default:
		break;
	}
	return kESDefaultPasswordTypeSerial;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedDirectPowerOn()
{
	ESIndexSet supportedValue;
	ForeachAnyArrayValue(m_dicMaintenanceCapabilities[FCCSTR(CAP_DIRECTPOWERON)], [&supportedValue](const UInt32 &value)
						 {
		switch (value) {
			case CAP_DIRECTPOWERON_ON:
				supportedValue.insert(kESDirectPowerOn_On);
				break;
			case CAP_DIRECTPOWERON_OFF:
				supportedValue.insert(kESDirectPowerOn_Off);
				break;
			default:
				break;
		} });
	return ESAny(supportedValue);
}

ESNumber CESCI2Accessor::GetDirectPowerOn()
{
	ESNumber supportedValue = kESDirectPowerOn_Off;
	StringToNumberFromAnyValue(GetMaintenanceResultForKey(FCCSTR(CAP_DIRECTPOWERON)), [&supportedValue](const UInt32 &value)
							   {
		switch (value) {
			case CAP_DIRECTPOWERON_ON:
				supportedValue = kESDirectPowerOn_On;
				break;
			case CAP_DIRECTPOWERON_OFF:
				supportedValue = kESDirectPowerOn_Off;
				break;
			default:
				break;
		} });
	return supportedValue;
}

ESErrorCode CESCI2Accessor::SetDirectPowerOn(ESNumber nDirectPowerOn)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 eValue = CAP_DIRECTPOWERON_OFF;
		switch (nDirectPowerOn)
		{
		case kESDirectPowerOn_On:
			eValue = CAP_DIRECTPOWERON_ON;
			break;
		case kESDirectPowerOn_Off:
			eValue = CAP_DIRECTPOWERON_OFF;
			break;
		default:
			break;
		}
		{
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_DIRECTPOWERON)] = FCCSTR(eValue);
			err = SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}

	return err;
}

ESAny CESCI2Accessor::GetSupportedNonConnectPowerOff()
{
	ESIndexSet supportedValue;
	ForeachAnyArrayValue(m_dicMaintenanceCapabilities[FCCSTR(CAP_NONCONNECT_POWEROFF)], [&supportedValue](const UInt32 &value)
						 {
		switch (value) {
			case CAP_NONCONNECT_POWEROFF_ON:
				supportedValue.insert(kESNonConnectPowerOff_On);
				break;
			case CAP_NONCONNECT_POWEROFF_OFF:
				supportedValue.insert(kESNonConnectPowerOff_Off);
				break;
			default:
				break;
		} });
	return ESAny(supportedValue);
}
ESNumber CESCI2Accessor::GetNonConnectPowerOff()
{
	ESNumber supportedValue = kESNonConnectPowerOff_Off;
	StringToNumberFromAnyValue(GetMaintenanceResultForKey(FCCSTR(CAP_NONCONNECT_POWEROFF)), [&supportedValue](const UInt32 &value)
							   {
		switch (value) {
			case CAP_NONCONNECT_POWEROFF_ON:
				supportedValue = kESNonConnectPowerOff_On;
				break;
			case CAP_NONCONNECT_POWEROFF_OFF:
				supportedValue = kESNonConnectPowerOff_Off;
				break;
			default:
				break;
		} });
	return supportedValue;
}
ESErrorCode CESCI2Accessor::SetNonConnectPowerOff(ESNumber nNonConnectPowerOff)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 eValue = CAP_NONCONNECT_POWEROFF_OFF;
		switch (nNonConnectPowerOff)
		{
		case kESNonConnectPowerOff_On:
			eValue = CAP_NONCONNECT_POWEROFF_ON;
			break;
		case kESNonConnectPowerOff_Off:
			eValue = CAP_NONCONNECT_POWEROFF_OFF;
			break;
		default:
			break;
		}
		{
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_NONCONNECT_POWEROFF)] = FCCSTR(eValue);
			err = SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}

	return err;
}

ESAny CESCI2Accessor::GetSupportedADFPaperProtection()
{
	ESIndexSet supportedValue;
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ForeachAnyArrayValue(dicADF[FCCSTR(CAP_ADF_PAPERPROTECTION)], [&supportedValue](const UInt32 &value)
							 {
			switch (value) {
				case CAP_ADF_PAPERPROTECTION_OFF:
					supportedValue.insert(kESADFPaperProtection_Off);
					break;
				case CAP_ADF_PAPERPROTECTION_LOW:
					supportedValue.insert(kESADFPaperProtection_Low);
					break;
				case CAP_ADF_PAPERPROTECTION_NORM:
					supportedValue.insert(kESADFPaperProtection_Normal);
					break;
				case CAP_ADF_PAPERPROTECTION_HIGH:
					supportedValue.insert(kESADFPaperProtection_High);
					break;
				default:
					break;
			} });
	}
	catch (...)
	{
	}
	return ESAny(supportedValue);
}
ESNumber CESCI2Accessor::GetADFPaperProtection()
{
	ESNumber supportedValue = kESADFPaperProtection_Off;
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);

		StringToNumberFromAnyValue(dic[FCCSTR(CAP_ADF_PAPERPROTECTION)], [&supportedValue](const UInt32 &value)
								   {
			switch (value) {
				case CAP_ADF_PAPERPROTECTION_OFF:
					supportedValue = kESADFPaperProtection_Off;
					break;
				case CAP_ADF_PAPERPROTECTION_LOW:
					supportedValue = kESADFPaperProtection_Low;
					break;
				case CAP_ADF_PAPERPROTECTION_NORM:
					supportedValue = kESADFPaperProtection_Normal;
					break;
				case CAP_ADF_PAPERPROTECTION_HIGH:
					supportedValue = kESADFPaperProtection_High;
					break;
				default:
					break;
			} });
	}
	catch (...)
	{
	}
	return supportedValue;
}

ESErrorCode CESCI2Accessor::SetADFPaperProtection(ESNumber nADFPaperProtection)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 eValue = CAP_ADF_PAPERPROTECTION_OFF;
		switch (nADFPaperProtection)
		{
		case kESADFPaperProtection_Off:
			eValue = CAP_ADF_PAPERPROTECTION_OFF;
			break;
		case kESADFPaperProtection_Low:
			eValue = CAP_ADF_PAPERPROTECTION_LOW;
			break;
		case kESADFPaperProtection_Normal:
			eValue = CAP_ADF_PAPERPROTECTION_NORM;
			break;
		case kESADFPaperProtection_High:
			eValue = CAP_ADF_PAPERPROTECTION_HIGH;
			break;
		default:
			break;
		}
		{
			ESDictionaryA dicADF;
			dicADF[FCCSTR(CAP_ADF_PAPERPROTECTION)] = FCCSTR(eValue);
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_ADF)] = dicADF;
			return SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}

	return err;
}

ESAny CESCI2Accessor::GetSupportedBehaviorWhenDoubleFeed()
{
	ESIndexSet supportedValue;
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ForeachAnyArrayValue(dicADF[FCCSTR(CAP_ADF_BEHAVIORWHEN_DF)], [&supportedValue](const UInt32 &value)
							 {
			switch (value) {
				case CAP_ADF_BEHAVIORWHEN_DF_SIMD:
					supportedValue.insert(kESBehaviorWhenDoubleFeed_Immediately);
					break;
				case CAP_ADF_BEHAVIORWHEN_DF_SEJ1:
					supportedValue.insert(kESBehaviorWhenDoubleFeed_StopAfterEjecting);
					break;
				default:
					break;
			} });
	}
	catch (...)
	{
	}
	return ESAny(supportedValue);
}
ESNumber CESCI2Accessor::GetBehaviorWhenDoubleFeed()
{
	ESNumber supportedValue = kESBehaviorWhenDoubleFeed_Immediately;
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		StringToNumberFromAnyValue(dic[FCCSTR(CAP_ADF_BEHAVIORWHEN_DF)], [&supportedValue](const UInt32 &value)
								   {
			switch (value) {
				case CAP_ADF_BEHAVIORWHEN_DF_SIMD:
					supportedValue = kESBehaviorWhenDoubleFeed_Immediately;
					break;
				case CAP_ADF_BEHAVIORWHEN_DF_SEJ1:
					supportedValue = kESBehaviorWhenDoubleFeed_StopAfterEjecting;
					break;
				default:
					break;
			} });
	}
	catch (...)
	{
	}
	return supportedValue;
}
ESErrorCode CESCI2Accessor::SetBehaviorWhenDoubleFeed(ESNumber nBehaviorWhenDoubleFeed)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 eValue = CAP_ADF_BEHAVIORWHEN_DF_SIMD;
		switch (nBehaviorWhenDoubleFeed)
		{
		case kESBehaviorWhenDoubleFeed_Immediately:
			eValue = CAP_ADF_BEHAVIORWHEN_DF_SIMD;
			break;
		case kESBehaviorWhenDoubleFeed_StopAfterEjecting:
			eValue = CAP_ADF_BEHAVIORWHEN_DF_SEJ1;
			break;
		default:
			break;
		}
		{
			ESDictionaryA dicADF;
			dicADF[FCCSTR(CAP_ADF_BEHAVIORWHEN_DF)] = FCCSTR(eValue);
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_ADF)] = dicADF;
			return SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}

	return err;
}

ESAny CESCI2Accessor::GetSupportedCleaningWarningNotify()
{
	ESIndexSet supportedValue;
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ForeachAnyArrayValue(dicADF[FCCSTR(CAP_ADF_CLEANINGWARNING)], [&supportedValue](const UInt32 &value)
							 {
			switch (value) {
				case CAP_ADF_CLEANINGWARNING_OFF:
					supportedValue.insert(kESCleaningWarningNotify_Off);
					break;
				case CAP_ADF_CLEANINGWARNING_ON:
					supportedValue.insert(kESCleaningWarningNotify_On);
					break;
				default:
					break;
			} });
	}
	catch (...)
	{
	}
	return ESAny(supportedValue);
}
ESNumber CESCI2Accessor::GetCleaningWarningNotify()
{
	ESNumber supportedValue = kESCleaningWarningNotify_Off;
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		StringToNumberFromAnyValue(dic[FCCSTR(CAP_ADF_CLEANINGWARNING)], [&supportedValue](const UInt32 &value)
								   {
			switch (value) {
				case CAP_ADF_CLEANINGWARNING_OFF:
					supportedValue = kESCleaningWarningNotify_Off;
					break;
				case CAP_ADF_CLEANINGWARNING_ON:
					supportedValue = kESCleaningWarningNotify_On;
					break;
				default:
					break;
			} });
	}
	catch (...)
	{
	}
	return supportedValue;
}
ESErrorCode CESCI2Accessor::SetCleaningWarningNotify(ESNumber nCleaningWarningNotify)
{
	ESErrorCode err = kESErrorNoError;
	try
	{
		UInt32 eValue = CAP_ADF_CLEANINGWARNING_OFF;
		switch (nCleaningWarningNotify)
		{
		case kESCleaningWarningNotify_Off:
			eValue = CAP_ADF_CLEANINGWARNING_OFF;
			break;
		case kESCleaningWarningNotify_On:
			eValue = CAP_ADF_CLEANINGWARNING_ON;
			break;
		default:
			break;
		}
		{
			ESDictionaryA dicADF;
			dicADF[FCCSTR(CAP_ADF_CLEANINGWARNING)] = FCCSTR(eValue);
			ESDictionaryA dicParameters;
			dicParameters[FCCSTR(CAP_ADF)] = dicADF;
			return SendMaintenanceParameters(dicParameters);
		}
	}
	catch (...)
	{
		err = kESErrorFatalError;
	}

	return err;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedCleaningWarningNotifyCount()
{
	return GetSupportedIntegerKeyValueToADFTree(CAP_ADF_CLEANINGWARNING_TH);
}
ESNumber CESCI2Accessor::GetCleaningWarningNotifyCount()
{
	return GetIntegerKeyValueToADFTree(CAP_ADF_CLEANINGWARNING_TH);
}
ESErrorCode CESCI2Accessor::SetCleaningWarningNotifyCount(ESNumber nCleaning)
{
	return SetIntegerKeyValueToADFTree(CAP_ADF_CLEANINGWARNING_TH, nCleaning);
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedRollerKitNotifyCount()
{
	return GetSupportedIntegerKeyValueToADFTree(CAP_ADF_ROLLERKITCOUNTER_TH);
}
ESNumber CESCI2Accessor::GetRollerKitNotifyCount()
{
	return GetIntegerKeyValueToADFTree(CAP_ADF_ROLLERKITCOUNTER_TH);
}
ESErrorCode CESCI2Accessor::SetRollerKitNotifyCount(ESNumber nRKITNearEnd)
{
	return SetIntegerKeyValueToADFTree(CAP_ADF_ROLLERKITCOUNTER_TH, nRKITNearEnd);
}
ESAny CESCI2Accessor::GetSupportedRetardRollerNotifyCount()
{
	return GetSupportedIntegerKeyValueToADFTree(CAP_ADF_RETARDROLLERCOUNTER_TH);
}
ESNumber CESCI2Accessor::GetRetardRollerNotifyCount()
{
	return GetIntegerKeyValueToADFTree(CAP_ADF_RETARDROLLERCOUNTER_TH);
}
ESErrorCode CESCI2Accessor::SetRetardRollerNotifyCount(ESNumber nRTRLNearEnd)
{
	return SetIntegerKeyValueToADFTree(CAP_ADF_RETARDROLLERCOUNTER_TH, nRTRLNearEnd);
}
ESAny CESCI2Accessor::GetSupportedSeparationPadNotifyCount()
{
	return GetSupportedIntegerKeyValueToADFTree(CAP_ADF_SEPARATIONPADCOUNTER_TH);
}
ESNumber CESCI2Accessor::GetSeparationPadNotifyCount()
{
	return GetIntegerKeyValueToADFTree(CAP_ADF_SEPARATIONPADCOUNTER_TH);
}
ESErrorCode CESCI2Accessor::SetSeparationPadNotifyCount(ESNumber nSPADNearEnd)
{
	return SetIntegerKeyValueToADFTree(CAP_ADF_SEPARATIONPADCOUNTER_TH, nSPADNearEnd);
}
ESAny CESCI2Accessor::GetSupportedPickupRollerNotifyCount()
{
	return GetSupportedIntegerKeyValueToADFTree(CAP_ADF_PICKUPROLLERCOUNTER_TH);
}
ESNumber CESCI2Accessor::GetPickupRollerNotifyCount()
{
	return GetIntegerKeyValueToADFTree(CAP_ADF_PICKUPROLLERCOUNTER_TH);
}
ESErrorCode CESCI2Accessor::SetPickupRollerNotifyCount(ESNumber nPURLNearEnd)
{
	return SetIntegerKeyValueToADFTree(CAP_ADF_PICKUPROLLERCOUNTER_TH, nPURLNearEnd);
}

ESAny CESCI2Accessor::GetSupportedCleaningWarningCounter()
{
	return GetSupportedIntegerKeyValueToADFTree(CAP_ADF_CLEANINGWARNINGCOUNTER);
}
ESNumber CESCI2Accessor::GetCleaningWarningCounter()
{
	return GetIntegerKeyValueToADFTree(CAP_ADF_CLEANINGWARNINGCOUNTER);
}
ESErrorCode CESCI2Accessor::SetCleaningWarningCounter(ESNumber nCleaningWarningCounter)
{
	return SetIntegerKeyValueToADFTree(CAP_ADF_CLEANINGWARNINGCOUNTER, nCleaningWarningCounter);
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsDocumentFeederBGLevelFrontSupported()
{
	ESDictionary dic = GetDocumentFeederBGLevelFront();
	return (dic.empty() ? false : true);
}

//===============================================================
//!
//!
//!
//===============================================================
ESDictionary CESCI2Accessor::GetDocumentFeederBGLevelFront()
{
	ESDictionary *pDic = SafeKeyDicInKeysDataPtr<ESDictionary>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_BACKGROUNDGLEVEL_FRONT).c_str());
	if (pDic)
	{
		return *pDic;
	}
	else
	{
		ESDictionary dicEmpty;
		return dicEmpty;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsDocumentFeederBGLevelBackSupported()
{
	ESDictionary dic = GetDocumentFeederBGLevelBack();
	return (dic.empty() ? false : true);
}

//===============================================================
//!
//!
//!
//===============================================================
ESDictionary CESCI2Accessor::GetDocumentFeederBGLevelBack()
{
	ESDictionary *pDic = SafeKeyDicInKeysDataPtr<ESDictionary>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_BACKGROUNDGLEVEL_BACK).c_str());
	if (pDic)
	{
		return *pDic;
	}
	else
	{
		ESDictionary dicEmpty;
		return dicEmpty;
	}
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Accessor::IsPickupRollerLifeLimitSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_PICKUPROLLERLIFELIMIT).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetPickupRollerLifeLimit()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_PICKUPROLLERLIFELIMIT).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsSeparationPadLifeLimitSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_SEPARATIONPADLIFELIMIT).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetSeparationPadLifeLimit()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_SEPARATIONPADLIFELIMIT).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsRollerKitLifeLimitSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_ROLLERKITLIFELIMIT).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetRollerKitLifeLimit()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_ROLLERKITLIFELIMIT).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsRetardRollerLifeLimitSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_RETARDROLLERLIFELIMIT).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetRetardRollerLifeLimit()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_RETARDROLLERLIFELIMIT).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsPickupRollerNearendSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_PICKUPROLLERLIFENEAREND).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetPickupRollerNearend()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_PICKUPROLLERLIFENEAREND).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsSeparationPadNearendSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_SEPARATIONPADLIFENEAREND).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetSeparationPadNearend()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_SEPARATIONPADLIFENEAREND).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsRollerKitNearendSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_ROLLERKITLIFENEAREND).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetRollerKitNearend()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_ROLLERKITLIFENEAREND).c_str());
	return (pValue ? *pValue : 0);
}

bool CESCI2Accessor::IsRetardRollerNearendSupported()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_RETARDROLLERLIFENEAREND).c_str());
	return (pValue ? true : false);
}

ESNumber CESCI2Accessor::GetRetardRollerNearend()
{
	ESNumber *pValue = SafeKeyDicInKeysDataPtr<ESNumber>(m_dicMaintenanceInformation, FCCSTR(INFO_ADF).c_str(), FCCSTR(INFO_ADF_RETARDROLLERLIFENEAREND).c_str());
	return (pValue ? *pValue : 0);
}

ESNumber CESCI2Accessor::GetClientApplication()
{
	ESStringA *pStrImageFormat = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_CLIENT_APPLICATION).c_str());
	if (pStrImageFormat != NULL)
	{
		switch (FourCharCode(*pStrImageFormat))
		{
		case APL_ES2:
			return kESClientApplicationES2;
		case APL_ESS:
			return kESClientApplicationESS;
		default:
			break;
		}
	}
	return kESClientApplicationES2;
}

ESErrorCode CESCI2Accessor::SetClientApplication(ESNumber nClientApplication)
{
	ESIndexSet indexSupported = GetSupportedClientApplications();
	if (indexSupported.find(nClientApplication) == indexSupported.end())
	{
		return kESErrorInvalidParameter;
	}

	switch (nClientApplication)
	{
	case kESClientApplicationES2:
		m_dicParameters[FCCSTR(CAP_CLIENT_APPLICATION)] = FCCSTR(APL_ES2);
		break;

	case kESClientApplicationESS:
		m_dicParameters[FCCSTR(CAP_CLIENT_APPLICATION)] = FCCSTR(APL_ESS);
		break;

	default:
		return kESErrorInvalidParameter;
	}
	return kESErrorNoError;
}

ESIndexSet CESCI2Accessor::GetSupportedClientApplications()
{
	ESIndexSet indexClientApplications;

	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_CLIENT_APPLICATION).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{

			ESStringA *pStrCode = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pStrCode != NULL)
			{
				switch (FourCharCode(*pStrCode))
				{
				case APL_ES2:
					indexClientApplications.insert(kESClientApplicationES2);
					break;
				case APL_ESS:
					indexClientApplications.insert(kESClientApplicationESS);
					break;
				default:
					break;
				}
			}
		}
	}
	return indexClientApplications;
}

//===============================================================
//!
//!
//!
//===============================================================
ESAny CESCI2Accessor::GetSupportedIntegerKeyValueToADFTree(const UInt32 &key)
{
	try
	{
		ESDictionaryA &dicADF = boost::any_cast<ESDictionaryA &>(m_dicMaintenanceCapabilities[FCCSTR(CAP_ADF)]);
		ESAny anyValue = dicADF[FCCSTR(key)]; // List of Range

		if (anyValue.type() == typeid(ESAnyArray))
		{
			ESAnyArray &anyArray = boost::any_cast<ESAnyArray &>(anyValue);
			return AnyArrayToIndexSet(anyArray);
		}
		else if (anyValue.type() == typeid(ST_ES_RANGE))
		{
			return anyValue;
		}
	}
	catch (...)
	{
	}
	ESAny empty;
	return empty;
}

ESNumber CESCI2Accessor::GetIntegerKeyValueToADFTree(const UInt32 &key)
{
	try
	{
		ESAny anyValue = GetMaintenanceResultForKey(FCCSTR(CAP_ADF));
		ESDictionaryA &dic = boost::any_cast<ESDictionaryA &>(anyValue);
		return boost::any_cast<ESNumber>(dic[FCCSTR(key)]);
	}
	catch (...)
	{
		return 0;
	}
}

ESErrorCode CESCI2Accessor::SetIntegerKeyValueToADFTree(const UInt32 &key, ESNumber nValue)
{
	ESDictionaryA dicADF;
	dicADF[FCCSTR(key)] = nValue;
	ESDictionaryA dicParameters;
	dicParameters[FCCSTR(CAP_ADF)] = dicADF;
	return SendMaintenanceParameters(dicParameters);
}

bool CESCI2Accessor::IsAdminLockSupported()
{
	bool *pValue = SafeKeysDataPtr<bool>(m_dicMaintenanceInformation, FCCSTR(INFO_ADMIN_LOCK).c_str());
	return (pValue ? *pValue : false);
}

bool CESCI2Accessor::IsAdminLockEnabled()
{
	return IsAdminLockEnabledWithUnlockTest(true);
}

bool CESCI2Accessor::IsAdminLockEnabledWithUnlockTest(bool bUnlockTest)
{
	try
	{
		ESErrorCode err = GetMaintenanceStatus();
		if (IS_ERR_CODE(err))
		{
			return false;
		}

		ESStringA *pStrAdminLockStatus = SafeKeysDataPtr<ESStringA>(m_dicMaintenanceStatus, FCCSTR(STAT_ADMIN_LOCK).c_str());
		if (!pStrAdminLockStatus)
		{
			return false;
		}

		switch (FourCharCode(*pStrAdminLockStatus))
		{
		case ADMINLOCK_ON:
		{
			if (bUnlockTest)
			{
				ESCI2Mode eCurrentMode = GetMode();
				ESErrorCode err2 = SetMode(kModeMaintenance);
				if (!IS_ERR_CODE(err2))
				{
					err2 = RequestAdministratorLock(false, GetAdminLockPassword());
					if (IS_ERR_CODE(err2))
					{
						m_bIsAdminLockUnlocked = false;
					}
					else
					{
						m_bIsAdminLockUnlocked = true;
					}

					err2 = SetMode(eCurrentMode);
					if (IS_ERR_CODE(err2))
					{
						err = err2;
					}
				}
				return !m_bIsAdminLockUnlocked;
			}
			else
			{
				return true;
			}
		}
		case ADMINLOCK_OFF:
			return false;
		default:
			break;
		}
	}
	catch (...)
	{
	}
	return false;
}

ESString CESCI2Accessor::GetAdminLockPassword()
{
	return m_strAdminLockPassword;
}

ESErrorCode CESCI2Accessor::SetAdminLockPassword(ESString strPassword)
{
	m_strAdminLockPassword = strPassword;
	return kESErrorNoError;
}

//===============================================================
//!
//!
//!
//===============================================================
ESIndexSet CESCI2Accessor::GetSupportedColorCounterTypes()
{
	ESIndexSet indexSupportedColorCounterTypes;
	ESAnyArray *pAnyArray = SafeKeysDataPtr<ESAnyArray>(m_dicCapabilities, FCCSTR(CAP_COLORCOUNTERTYPE).c_str());
	if (pAnyArray != NULL)
	{
		BOOST_FOREACH (ESAny &anyValue, *pAnyArray)
		{

			ESStringA *pKey = SafeAnyDataPtr<ESStringA>(anyValue);
			if (pKey != NULL)
			{
				switch (FourCharCode(*pKey))
				{
				case CAP_COLORCOUNTERTYPE_PREF:
					indexSupportedColorCounterTypes.insert(kESColorCounterTypePref);
					break;

				case CAP_COLORCOUNTERTYPE_COLOR:
					indexSupportedColorCounterTypes.insert(kESColorCounterTypeColor);
					break;

				case CAP_COLORCOUNTERTYPE_MONO:
					indexSupportedColorCounterTypes.insert(kESColorCounterTypeMono);
					break;

				default:
					break;
				}
			}
		}
	}
	return indexSupportedColorCounterTypes;
}

ESNumber CESCI2Accessor::GetColorCounterType()
{
	ESStringA *pStrCode = SafeKeysDataPtr<ESStringA>(m_dicParameters, FCCSTR(CAP_COLORCOUNTERTYPE).c_str());
	if (pStrCode != NULL)
	{
		switch (FourCharCode(*pStrCode))
		{
		case CAP_COLORCOUNTERTYPE_PREF:
			return kESColorCounterTypePref;

		case CAP_COLORCOUNTERTYPE_COLOR:
			return kESColorCounterTypeColor;

		case CAP_COLORCOUNTERTYPE_MONO:
			return kESColorCounterTypeMono;

		default:
			break;
		}
	}
	return 0;
}

ESErrorCode CESCI2Accessor::SetColorCounterType(ESNumber nColorCounterType)
{
	ESCI2ColorCounterType eESCI2ColorCounterType = CAP_COLORCOUNTERTYPE_PREF;

	ESIndexSet indexSupportedColorFormat = GetSupportedColorCounterTypes();
	assert(indexSupportedColorFormat.find(nColorCounterType) != indexSupportedColorFormat.end());

	ESErrorCode err = kESErrorNoError;
	switch (nColorCounterType)
	{
	case kESColorCounterTypePref:
		eESCI2ColorCounterType = CAP_COLORCOUNTERTYPE_PREF;
		break;

	case kESColorCounterTypeColor:
		eESCI2ColorCounterType = CAP_COLORCOUNTERTYPE_COLOR;
		break;

	case kESColorCounterTypeMono:
		eESCI2ColorCounterType = CAP_COLORCOUNTERTYPE_MONO;
		break;

	default:
		err = kESErrorInvalidParameter;
		break;
	}
	m_dicParameters[FCCSTR(CAP_COLORCOUNTERTYPE)] = FCCSTR(eESCI2ColorCounterType);
	return err;
}

ESAny CESCI2Accessor::GetSupportedFilmType()
{
	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, FCCSTR(CAP_TPU).c_str(), FCCSTR(CAP_TPU_NEGATIVE).c_str());
	return (pValue ? *pValue : false);
}

ESNumber CESCI2Accessor::GetFilmType()
{
	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitTransparent:
	{
		ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_TPU).c_str());
		if (pValue != NULL)
		{
			if (pValue->find(FCCSTR(CAP_TPU_NEGATIVE)) != pValue->end())
			{
				return kESFilmTypeNegative;
			}
		}
	}
	break;

	case kESFunctionalUnitFlatbed:
	case kESFunctionalUnitDocumentFeeder:
	default:
		return 0;
	}

	return 0;
}

ESErrorCode CESCI2Accessor::SetFilmType(ESNumber nFilmType)
{
	ESStringA strKey1, strKey2;

	switch (GetFunctionalUnitType())
	{
	case kESFunctionalUnitTransparent:

		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_NEGATIVE);
		{
			ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());

			if (pStrSet != NULL)
			{
				if (nFilmType == kESFilmTypeNegative)
				{
					if (pStrSet->find(strKey2) == pStrSet->end())
					{
						pStrSet->insert(strKey2);
					}
				}
			}
			else
			{
				pStrSet->erase(strKey2);
			}
			return kESErrorNoError;
		}
		return kESErrorFatalError;
		break;
	case kESFunctionalUnitDocumentFeeder:
	case kESFunctionalUnitFlatbed:
	default:
		break;
	}
	return kESErrorNoError;
}

bool CESCI2Accessor::IsSupportedScanningMode()
{
	ESStringA strKey1, strKey2;
	switch (GetFunctionalUnitType())
	{

	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_SCANSPEED_SELECTION);
		break;

	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_SCANSPEED_SELECTION);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_SCANSPEED_SELECTION);
		break;

	default:
		return false;
	}

	bool *pValue = SafeKeyDicInKeysDataPtr<bool>(m_dicCapabilities, strKey1.c_str(), strKey2.c_str());
	return (pValue ? *pValue : false);
}

ESNumber CESCI2Accessor::GetScanningMode()
{
	switch (GetFunctionalUnitType())
	{

	case kESFunctionalUnitDocumentFeeder:
	{
		ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_ADF).c_str());
		if (pValue != NULL)
		{
			if (pValue->find(FCCSTR(CAP_ADF_SCANSPEED_SELECTION)) != pValue->end())
			{
				return kESScanningModeHighSpeed;
			}
		}
	}
	break;

	case kESFunctionalUnitFlatbed:
	{

		ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_FB).c_str());
		if (pValue != NULL)
		{
			if (pValue->find(FCCSTR(CAP_FB_SCANSPEED_SELECTION)) != pValue->end())
			{
				return kESScanningModeHighSpeed;
			}
		}
	}
	break;

	case kESFunctionalUnitTransparent:
	{

		ESStringSetA *pValue = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, FCCSTR(CAP_TPU).c_str());

		if (pValue != NULL)
		{
			if (pValue->find(FCCSTR(CAP_TPU_SCANSPEED_SELECTION)) != pValue->end())
			{
				return kESScanningModeHighSpeed;
			}
		}
	}
	break;

	default:
		return kESScanningModeNormal;
	}

	return kESScanningModeNormal;
}

ESErrorCode CESCI2Accessor::SetScanningMode(ESNumber nScanningMode)
{

	ESStringA strKey1, strKey2;

	switch (GetFunctionalUnitType())
	{

	case kESFunctionalUnitDocumentFeeder:
		strKey1 = FCCSTR(CAP_ADF);
		strKey2 = FCCSTR(CAP_ADF_SCANSPEED_SELECTION);
		break;

	case kESFunctionalUnitFlatbed:
		strKey1 = FCCSTR(CAP_FB);
		strKey2 = FCCSTR(CAP_FB_SCANSPEED_SELECTION);
		break;

	case kESFunctionalUnitTransparent:
		strKey1 = FCCSTR(CAP_TPU);
		strKey2 = FCCSTR(CAP_TPU_SCANSPEED_SELECTION);
		break;

	default:
		return kESErrorInvalidParameter;
	}

	ESStringSetA *pStrSet = SafeKeysDataPtr<ESStringSetA>(m_dicParameters, strKey1.c_str());
	if (pStrSet != NULL)
	{
		if (nScanningMode == kESScanningModeHighSpeed)
		{
			if (pStrSet->find(strKey2) == pStrSet->end())
			{
				pStrSet->insert(strKey2);
			}
		}
		else
		{
			pStrSet->erase(strKey2);
		}
		return kESErrorNoError;
	}

	return kESErrorFatalError;
}
