////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     Emulator.cpp
//!
//! @brif     Emulator クラス
//! @note
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2017/06/21
//!
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Emulator.h"


Emulator::Emulator(void)
{
	m_pcDelegate = NULL;

	m_bOpen = false;
	m_bScanning = false;
	m_bCancel = false;
	m_bAfmEnabled = false;
	m_bIsInterrupted = false;

	m_nErrorCodeNum = kESErrorNoError;
}


Emulator::~Emulator(void)
{

}


void Emulator::SetDelegate( IESScannerDelegate* pDelegate )
{
	m_pcDelegate = pDelegate;
}


ESErrorCode Emulator::SetConnection( ES_JSON_CPTR pszJSON )
{
	ES_LOG_TRACE_FUNC();
	ESDictionary dictConnect;
	ES_CMN_FUNCS::JSON::JSONtoDictionary(pszJSON,dictConnect);
	try{
		ESDictionary dictSetting = boost::any_cast<ESDictionary>(dictConnect[ES_CNCT_KEY_ROOT]);

		if(dictSetting.find(ES_STRING("modelID")) != dictSetting.end()){
			ESNumber nModelID = boost::any_cast<ESNumber>(dictSetting[ES_STRING("modelID")]);
			m_strModelName = (boost::format(ES_STRING("ES%04X")) % nModelID).str();
		}

	}catch(...){

	}
	ES_LOG_LEAVE_FUNC();
	return kESErrorNoError;
}


ESErrorCode Emulator::InitializeDummyInfo(ESString strFunctionalUnit){

	ESErrorCode eError = kESErrorNoError;
	BOOL bLoaded = FALSE;

	//Debug用DummyInfoがあるならばそれを使う
	if(GetDummyModelInfoDir().empty() == FALSE){

		ESString strDummyInfo;
		ES_CMN_FUNCS::PATH::ES_CombinePath(strDummyInfo, GetDummyModelInfoDir(), strFunctionalUnit);

		if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strDummyInfo)){

			ESDictionary dictAllCurrentValues;
			LoadDummyInfo(strDummyInfo, ES_STRING("AllCurrentValues.json")	, dictAllCurrentValues);
			SetAllCurrentValues(dictAllCurrentValues);

			ESDictionary dictAllValues;
			LoadDummyInfo(strDummyInfo, ES_STRING("AllValues.json")			, dictAllValues);
			SetAllValues(dictAllValues);

			ESDictionary dictAllAvailableValues;
			LoadDummyInfo(strDummyInfo, ES_STRING("AllAvailableValues.json")	, dictAllAvailableValues);
			SetAllAvailableValues(dictAllAvailableValues);

			bLoaded = TRUE;
		}

	}

	if(bLoaded == FALSE){

		//ModelフォルダのDummyInfoを探す
		ESString strDBName = GetDummyDBPath();

		if(strDBName.empty() == FALSE){

			ESDictionary dictAllCurrentValues;
			LoadDummyInfoFromDB(strDBName	,strFunctionalUnit.c_str(), ES_STRING("AllCurrentValues.json")	, dictAllCurrentValues);
			SetAllCurrentValues(dictAllCurrentValues);

			ESDictionary dictAllValues;
			LoadDummyInfoFromDB(strDBName	,strFunctionalUnit.c_str(), ES_STRING("AllValues.json")	, dictAllValues);
			SetAllValues(dictAllValues);

			ESDictionary dictAllAvailableValues;
			LoadDummyInfoFromDB(strDBName	,strFunctionalUnit.c_str(), ES_STRING("AllAvailableValues.json")	, dictAllAvailableValues);
			SetAllAvailableValues(dictAllAvailableValues);

			bLoaded = TRUE;
		}

	}

	if(bLoaded == FALSE){
		eError = kESErrorDeviceOpenError;
	}

	return eError;
}


ESErrorCode Emulator::DestroyInstance()
{
	delete this;
	return kESErrorNoError;
}


ESErrorCode Emulator::Open()
{
	ES_LOG_TRACE_FUNC();
	ESErrorCode eError = kESErrorNoError;

	//Initialize
	m_cEmulatorSettings.SetEmulatorControllerDir(GetEmulatorCtrlerDir());

	SetCurrentUnitType(GetFirstUnitType());
	eError = InitializeDummyInfo(GetFirstUnitType());

	if(eError == kESErrorNoError)
	{
		m_bOpen = true;
	}
	else
	{
		ES_Error_Log(this, ES_STRING("InitializeDummyInfo Error."));
		eError = kESErrorDeviceOpenError;
	}

	ES_LOG_LEAVE_FUNC();
	return eError;
}


ESErrorCode Emulator::Close()
{
	ES_LOG_TRACE_FUNC();
	m_bOpen = false;

	ES_LOG_LEAVE_FUNC();
	return kESErrorNoError;
}



bool Emulator::IsOpened() const
{
	return m_bOpen;
}


ESErrorCode Emulator::CaptureScanner()
{
	return kESErrorNoError;
}


ESErrorCode Emulator::ReleaseScanner()
{
	return kESErrorNoError;
}


ESErrorCode Emulator::Scan()
{
	ES_LOG_TRACE_FUNC();
	ES_LOG_LEAVE_FUNC();
	return kESErrorInvalidParameter;
}


//スキャンの実行
//@param	[in]		nLoadingPaperNum スキャンする枚数
//@param	[inout]	nPreviousPageNum これまでの通紙枚数
//@return	void
void Emulator::DoScan(ESNumber nLoadingPaperNum, ESNumber& nPreviousPageNum)
{
	//CStopWatch cTimer;
	ES_LOG_TRACE_FUNC();
	BOOL bDuplex = IsDuplex();
	if (bDuplex)
	{	//両面スキャンの場合はスキャン枚数を2倍にする
		nLoadingPaperNum = nLoadingPaperNum * 2;
		//PPMも2倍
		ESNumber nPPM = GetPagePerMinute();
		SetPagePerMinute(nPPM * 2);
	}
	//今までの通紙枚数と今回のスキャン枚数を合わせた枚数を合計枚数とする
	ESNumber nTotalPageNum = nPreviousPageNum + nLoadingPaperNum;

	ESNumber nError = kESErrorNoError;
	//BOOL bScanSpeedUp = m_cEmulatorSettings.IsScanSpeedUp();
	int nPageNum = nPreviousPageNum;
	for (; nPageNum < nTotalPageNum; nPageNum++)
	{
		CESScannedImage* pImage = new CESScannedImage();

		InitScanImage(pImage,nPageNum +1);

		//WillScan通知にて、ImageのDelegateをセットさせる
		m_pcDelegate->ScannerWillScanToScannedImage(this,pImage);
		/*if (m_cEmulatorSettings.IsEnablePagePerMinute())
		{	//TimerSet
			//m_ValueCritSec.Lock();
			//cTimer.Start();
			//m_ValueCritSec.Unlock();
		}*/
		//Beginを通知
		pImage->Open();

		nError = GetErrorCodeNum();

		if (GetScanCanceled()){
			//Cancel 通知
			m_pcDelegate->ScannerWillCancelScanning(this);
			m_pcDelegate->ScannerDidCancelScanning(this);
			ES_Info_Log(this, ES_STRING("Scan is Canceled -- ErrorCode : %d ."),GetErrorCodeNum());
			goto BAIL;
		}else if ( nError == kESErrorPaperJam || nError == kESErrorPaperDoubleFeed){
			if((nPageNum+1)%2 == 1){
				//紙ジャムor重送検知の場合はスキャンを中断する(Abort処理暫定対応：紙ジャム＆重送検知は奇数ページのみで発生するよう制限)
				SetInterrupted(true);
				nTotalPageNum = pImage->GetSerialNumber();	//中断時の枚数を設定
				pImage->Abort();
				nPreviousPageNum = nTotalPageNum - 1; //前回の通紙枚数を更新(中断の場合、Errorが発生したページから始める)
				goto BAIL;
			}else{
				//奇数ページは続行
				nError = kESErrorNoError;
			}
		}

		if (/*!bScanSpeedUp || */nPageNum == nPreviousPageNum)
		{
			m_csImagePath = ES_STRING("Resources/DummyInfo/test.raw");//TBD //CreateImageWithImageCreator(pImage);
		}

		CESFile cFile;
		UInt32 un32ReadedLen = 0;
		if(ES_CMN_FUNCS::PATH::ES_IsExistFile(m_csImagePath/*.GetString()*/) &&  cFile.Open(m_csImagePath/*.GetBuffer(0)*/))
		{
			while(!GetScanCanceled() && nError == kESErrorNoError)
			{
				ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;
				cFile.SeekToFileOffset(un32ReadedLen);

				//読み込むべきファイルの長さを計算する(50Line or 残りの長さ)
				UInt32 nReadDataLen = std::min<UInt32>(pImage->GetBytesPerRow() * 50, cFile.GetLength() - un32ReadedLen);
				INT nReaded = 0;
				if (pImage->GetImageFormat() == kESImageFormatJPEG)
				{
					//256KByte
					nReaded = cFile.ReadDataOfLength(256*1024, cBuf);
				}
				else
				{
					//一行分のbyte数
					nReaded = cFile.ReadDataOfLength(nReadDataLen, cBuf);
				}

				un32ReadedLen += nReaded;
				if(nReaded == 0){
					ES_Error_Log(this, ES_STRING("Read Data Length is 0."));
					break;
				}

				pImage->WriteData(cBuf);

				if(un32ReadedLen == cFile.GetLength()){
					ES_Error_Log(this, ES_STRING("un32ReadedLen [%d] == cFile.GetLength() [%d]."), un32ReadedLen, cFile.GetLength());
					break;
				}
			}
			cFile.CloseFile();
			/*if (!bScanSpeedUp || nPageNum == nTotalPageNum-1)
			{
				::DeleteFile(m_csImagePath);	//生成画像の削除
				m_csImagePath.Empty();
			}*/

		}else{
			m_pcDelegate->ScannerDidEncounterDeviceCommunicationError(this,kESErrorDataReceiveFailure);
		}

		//Did Receiveを通知
		pImage->Close();

		//Did 通知
		m_pcDelegate->ScannerDidScanToScannedImage(this,pImage);
		/*if (m_cEmulatorSettings.IsEnablePagePerMinute())
		{	//TimerStop
			//m_ValueCritSec.Lock();
			//DWORD dwTimerMSec = cTimer.Stop();
			//m_ValueCritSec.Unlock();
			DWORD dwSleepMSec = GetSleepCountMilliSec(dwTimerMSec);

			if (dwSleepMSec > 50)
			{
				Sleep(dwSleepMSec);
			}
		}*/
		// プログレスバーを1枚分進める、スキャン枚数を一枚分減らす
		/*GetVirtualUIThread()->m_pMainWnd->PostMessageW(WM_APP_SET_POS_PROGRESS);
		if (!(bDuplex && nPageNum%2==1))
		{
			GetVirtualUIThread()->m_pMainWnd->PostMessageW(WM_APP_DECREMENT_DOC);
		}*/
		//Backend側で 所有権取得されていなかったら削除
		//TBD if(pImage->IsRelegationOwner() == false){
			pImage->DeleteInstance();
		//}
	}
	nPreviousPageNum = nTotalPageNum; //前回の通紙枚数を更新
BAIL:
	ES_LOG_LEAVE_FUNC();
}

void Emulator::ScannerThread()
{
	ES_LOG_TRACE_FUNC();
	{
		m_ValueCritSec.Lock();
		m_bScanning = true;
		m_ValueCritSec.Unlock();
	}

	ESNumber nPrePageNum = 0;//前回の通紙枚数はゼロ
	if (IsInterrupted())
	{	//継続スキャン
		SetInterrupted(false);
		nPrePageNum = GetPreviousPageNum(); //前回までの通紙枚数を取得する
		SetPreviousPageNum(0);	//今までの通紙枚数をReset
	}

	m_pcDelegate->ScannerWillBeginContinuousScanning(this);

	int nLoadingNum = 1;//GetLoadingPaperNum(); //<-SetLoadingPaperNum TBD
	ESNumber nUnitType = GetFunctionalUnitNameTypeFromDict(m_dictScanParameter);
	if (nUnitType != kESFunctionalUnitDocumentFeeder)
	{	//原稿台は1枚だけスキャン
		nLoadingNum = 1;
	}else if(nLoadingNum == 0){
		//ADFにLoadされていないのに、Scan要求されるのはおかしい
		SetErrorCodeNum(kESErrorPaperEmpty);
	}

	ESNumber nToBeScanned = GetScanParamForKey_Internal(kESPagesToBeScanned, (ESNumber)0);
	if (nToBeScanned > 0)
	{
		nLoadingNum = 1;
	}

	//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SHOW_PROGRESS, nLoadingNum);
	//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SET_PPM);

	DoScan(nLoadingNum, nPrePageNum);

	m_pcDelegate->ScannerDidEndContinuousScanning(this);

	if (!IsInterrupted())
	{	//Scan 完了通知
		m_pcDelegate->ScannerDidCompleteScanningWithError(this,(ESErrorCode)GetErrorCodeNum());
	}
	else
	{	//Scan 中断
		m_pcDelegate->ScannerDidInterruptScanningWithError(this, (ESErrorCode)GetErrorCodeNum());
		ES_Info_Log(this, L"Scan is Interrupted -- ErrorCode : %d .",GetErrorCodeNum());
		SetPreviousPageNum(nPrePageNum); //現在通紙された枚数をメンバにSet
	}
	//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_HIDE_PROGRESS);
	//エラー状態をReset
	//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SET_NO_ERROR);

	{
		m_ValueCritSec.Lock();
		m_bScanning = false;
		m_ValueCritSec.Unlock();
	}
	SetScanCanceled(false);
	ES_LOG_LEAVE_FUNC();
}

void* Emulator::EnterScannerThread(void* vpContext)
{
	ES_LOG_TRACE_FUNC();
	if(vpContext)
	{
		Emulator* pcThis =(Emulator*) vpContext;
		pcThis->ScannerThread();
	}
	ES_LOG_LEAVE_FUNC();
	return (void*)NULL;
}

ESErrorCode Emulator::ScanInBackground()
{
	ES_LOG_TRACE_FUNC();

	//ProgramDataにDBがある状態で、ISISラッパー等経由で呼ばれた時対策
	//とりあえず、カバーオープンとしておく
	if(GetEmulatorCtrlerDir().empty()){
		m_pcDelegate->ScannerDidCompleteScanningWithError(this,kESErrorCoverOpen);
		return kESErrorFileReadError;
	}

	//Thread 開始し、画像を送る
	//dummyImage.jpeg
	//m_pScannerThread = ::AfxBeginThread(EnterScannerThread, this, THREAD_PRIORITY_NORMAL, 0, 0);
	pthread_create(&m_pScannerThread, NULL, &Emulator::EnterScannerThread, this);

	ES_LOG_LEAVE_FUNC();
	return kESErrorNoError;
}


ESErrorCode Emulator::Cancel()
{
	ES_LOG_TRACE_FUNC();
	SetScanCanceled(true);
	ES_LOG_LEAVE_FUNC();
	return kESErrorNoError;
}


ESErrorCode Emulator::Abort()
{
	ES_LOG_TRACE_FUNC();
	CBlockCriticalSection cValueBlock(m_ValueCritSec);
	ESErrorCode eErr = kESErrorNoError;

	assert(!IsScanning());

	if (IsInterrupted())
	{
		SetInterrupted(false);
		m_pcDelegate->ScannerDidCompleteScanningWithError(this, kESErrorNoError);
	}

	ES_LOG_LEAVE_FUNC();
	return eErr;
}


bool Emulator::IsScanning() const
{
	ES_LOG_TRACE_FUNC();
	bool bRet =false;
	m_ValueCritSec.Lock();
	bRet = m_bScanning;
	m_ValueCritSec.Unlock();
	ES_LOG_LEAVE_FUNC();
	return bRet;
}


ESErrorCode Emulator::DoCleaning()
{
	return kESErrorNoError;
}


ESErrorCode Emulator::DoCalibration()
{
	return kESErrorNoError;
}


bool Emulator::IsInterrupted() const
{
	return m_bIsInterrupted;
}


bool Emulator::IsAfmEnabled() const
{
	return m_bAfmEnabled;
}


void* Emulator::EnterMSSThread(void* vpContext)
{
	ES_LOG_TRACE_FUNC();
	if(vpContext)
	{
		Emulator* pcThis =(Emulator*) vpContext;
		pcThis->MSSThread();
	}
	ES_LOG_LEAVE_FUNC();

	return (void*)NULL;
}


void Emulator::MSSThread()
{
	ES_LOG_TRACE_FUNC();
	while (IsAfmEnabled() && !IsInterrupted())
	{
		if (IsPaperLoaded())
		{	//用紙が無くなるまでスキャンし続ける
			while (GetLoadingPaperNum() > 0 )
			{
				m_pcDelegate->ScannerWillBeginContinuousScanning(this);
				//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SHOW_PROGRESS, GetLoadingPaperNum());
				//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SET_PPM);

				ESNumber nPrePageNum = GetPreviousPageNum(); //前回までの通紙枚数を取得
				DoScan(GetLoadingPaperNum(), nPrePageNum);
				SetPreviousPageNum(nPrePageNum); //現在通紙された枚数をメンバにSet
				if (!IsInterrupted())
				{	//Scan 完了
					m_pcDelegate->ScannerDidEndContinuousScanning(this);
					//エラー状態をReset
					//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SET_NO_ERROR);
					//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_HIDE_PROGRESS);
				}
				else
				{	//Scan 中断
					m_pcDelegate->ScannerDidInterruptScanningWithError(this, (ESErrorCode)GetErrorCodeNum());
					ES_Info_Log(this, ES_STRING("Scan is Interrupted -- ErrorCode : %d ."),GetErrorCodeNum());
					//エラー状態をReset
					//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_SET_NO_ERROR);
					//GetVirtualUIThread()->m_pMainWnd->SendMessageW(WM_APP_HIDE_PROGRESS);
					break;
				}

				if (!IsPaperLoaded())
				{
					break;
				}
			}
		}
		sleep(1);
	}
	ES_LOG_LEAVE_FUNC();
}


//
// Auto Feeding Mode 開始
//
ESErrorCode Emulator::StartScanningInAutoFeedingMode()
{
	ES_LOG_TRACE_FUNC();
	//SetAfmEnabled(true);	//AFMモードSet
	if (IsInterrupted())
	{
		SetInterrupted(false);
	}

	//Threadの開始
	//m_pMSSThread = ::AfxBeginThread(EnterMSSThread, this, THREAD_PRIORITY_NORMAL, 0, 0);
	pthread_create(&m_pMSSThread, NULL, &Emulator::EnterMSSThread, NULL);

	ES_LOG_LEAVE_FUNC();
	return kESErrorNoError;
}


ESErrorCode Emulator::StopScanningInAutoFeedingMode()
{
	return kESErrorNoError;
}


ESErrorCode Emulator::StartJobInMode( ESJobMode eJobMode )
{
	ESErrorCode eErr = kESErrorNoError;
	if (eJobMode == kESJobModeAFM)
	{
		eErr = StartScanningInAutoFeedingMode();
	}
	return eErr;
}


ESErrorCode Emulator::StopJobInMode( ESJobMode eJobMode )
{
	return kESErrorNoError;
}


ESErrorCode Emulator::DoAutoFocus( ESFloat* pfOutFocus )
{
	return kESErrorNoError;
}


ESErrorCode Emulator::SetPanelToPushScanReady( BOOL bPushScanReady )
{
	return kESErrorNoError;
}


ESErrorCode Emulator::Reset()
{
	return kESErrorNoError;
}


ESErrorCode Emulator::GetAllKeys( IESResultString* pstrResult )
{
	return kESErrorInvalidParameter;
}


ESErrorCode Emulator::GetDefaultValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	return kESErrorInvalidParameter;
}


ESAny Emulator::GetValue(ES_CHAR_CPTR pszKey, ESDictionary& dict)
{
	ES_LOG_TRACE_FUNC();
	CBlockCriticalSection cValueBlock(m_ValueCritSec);

	ES_Trace_Log2(__FUNCTION__, ES_STRING("GetValue [%s]"), pszKey);

	ESAny anyValue;
	if(dict.count(pszKey))
	{
		anyValue = dict[pszKey];
	}
	ES_LOG_LEAVE_FUNC();
	return anyValue;
}


ESErrorCode Emulator::GetValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	ES_LOG_TRACE_FUNC();
	ESErrorCode eErr = kESErrorNoError;
	CBlockCriticalSection cValueBlock(m_ValueCritSec);
	if (m_dictAllCurrentValues.size() == 0)
	{
		ES_Error_Log(this, ES_STRING("Key:%s  m_dictAllCurrentValues is Null."), pszKey);
	}
	//AllCurrentValues.json
	ESAny any = GetValue(pszKey, m_dictAllCurrentValues);
	if(!any.empty())
	{
		ESDictionary dictRet;
		dictRet[pszKey] = any;

		ESString strJson;
		ES_CMN_FUNCS::JSON::DictionaryToJSON(dictRet,strJson);

		if(pstrResult){
			pstrResult->Set(strJson.c_str());
		}
	}
	ES_LOG_LEAVE_FUNC();
	return eErr;
}


ESErrorCode Emulator::SetValueForKey( ES_CHAR_CPTR pszKey, ES_JSON_CPTR pszJSON )
{
	ES_LOG_TRACE_FUNC();
	CBlockCriticalSection cValueBlock(m_ValueCritSec);

	ESErrorCode eErr = kESErrorInvalidParameter;

	if (pszJSON)
	{
		ESDictionary dict;
		ES_CMN_FUNCS::JSON::JSONtoDictionary(pszJSON, dict);
		if (!dict.empty())
		{
			if (m_dictAllCurrentValues.count(pszKey))
			{
				m_dictAllCurrentValues[pszKey] = dict.at(pszKey);
				eErr = kESErrorNoError;
			}
		}
	}

	ESString strKey = pszKey;
	if (strKey.compare(kESFunctionalUnitType) == 0)
	{
		//FunctionalUnitTypeが変更されたらキャッシュしなおす
		BOOL bChanged = IsChangedFunctionalUnit();

		if (bChanged)
		{
			//現在のFunctionalUnitTypeをSet
			SetCurrentUnitType(GetFunctionalUnitTypeFromDictionary());
			//特定のUnitTypeのJSONを取得する

			InitializeDummyInfo(GetCurrentUnitType());

			/*if( GetCurrentUnitType()  == ES_STRING("ADF/")){
				if(m_pThread&&m_pThread->GetMainWnd()){
					PostMessage(m_pThread->GetMainWnd()->GetSafeHwnd(),WM_APP_UPDATE_PAPER_LOADING,0,0);
				}
			}*/

		}
	}

	ES_LOG_LEAVE_FUNC();
	return eErr;
}


ESErrorCode Emulator::SetValuesWithJSON( ES_JSON_CPTR pszJSON )
{
	ES_LOG_TRACE_FUNC();
	CBlockCriticalSection cValueBlock(m_ValueCritSec);

	ESErrorCode eRet = kESErrorInvalidParameter;
	if (pszJSON)
	{
		ESDictionary dict;
		ES_CMN_FUNCS::JSON::JSONtoDictionary(pszJSON, dict);
		if (!dict.empty())
		{
			//ScanParameterのDictionaryをメンバとして別で持っておく -> ScanThreadで使う用
			//CurrentSettingsで見たい情報はこっち(m_dictScanParameter)
			m_dictScanParameter = dict;
			eRet = kESErrorNoError;
		}
	}
	ES_LOG_LEAVE_FUNC();
	return eRet;
}


ESErrorCode Emulator::GetAllValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	ES_LOG_TRACE_FUNC();
	ESErrorCode eErr = kESErrorNoError;
	CBlockCriticalSection cValueBlock(m_ValueCritSec);

	//AllValues.json
	ESAny any = GetValue(pszKey, m_dictAllValues);
	if(!any.empty())
	{
		ESDictionary dictRet;
		dictRet[pszKey] = any;

		ESString strJson;
		ES_CMN_FUNCS::JSON::DictionaryToJSON(dictRet,strJson);

		if(pstrResult){
			pstrResult->Set(strJson.c_str());
		}
	}
	ES_LOG_LEAVE_FUNC();
	return eErr;
}


ESErrorCode Emulator::GetAllValues( IESResultString* pstrResult )
{
	ES_LOG_TRACE_FUNC();
	CBlockCriticalSection cValueBlock(m_ValueCritSec);

	//AllValues.json
	ESString strJson;
	ES_CMN_FUNCS::JSON::DictionaryToJSON(m_dictAllValues,strJson);

	if(pstrResult){
		pstrResult->Set(strJson.c_str());
		return kESErrorNoError;
	}
	ES_LOG_LEAVE_FUNC();
	return kESErrorInvalidParameter;
}


ESErrorCode Emulator::GetAvailableValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	ES_LOG_TRACE_FUNC();
	ESErrorCode eErr = kESErrorNoError;
	CBlockCriticalSection cValueBlock(m_ValueCritSec);

	//AllAvailableValues.json
	ESAny any = GetValue(pszKey, m_dictAllAvailableValues);
	if(!any.empty())
	{
		ESDictionary dictRet;
		dictRet[pszKey] = any;

		ESString strJson;
		ES_CMN_FUNCS::JSON::DictionaryToJSON(dictRet,strJson);

		if(pstrResult){
			pstrResult->Set(strJson.c_str());
		}
	}

	ES_LOG_LEAVE_FUNC();
	return eErr;
}


ESErrorCode Emulator::GetAllAvailableValues( IESResultString* pstrResult )
{
	ES_LOG_TRACE_FUNC();
	CBlockCriticalSection cValueBlock(m_ValueCritSec);
	//AllAvailableValues.json
	ESString strJson;
	ES_CMN_FUNCS::JSON::DictionaryToJSON(m_dictAllAvailableValues, strJson);

	if(pstrResult){
		pstrResult->Set(strJson.c_str());
		return kESErrorNoError;
	}
	ES_LOG_LEAVE_FUNC();
	return kESErrorInvalidParameter;
}

//CurrentValueからFunctioalUnitTypeを取得する
ESString Emulator::GetFunctionalUnitTypeFromDictionary()
{
	ES_LOG_TRACE_FUNC();
	ESString strUnitType;
	ESNumber nUnitType = GetFunctionalUnitNameTypeFromDict(m_dictAllCurrentValues);

	switch(nUnitType)
	{
	case kESFunctionalUnitFlatbed:
		strUnitType = ES_STRING("Flatbed/");
		break;
	case kESFunctionalUnitDocumentFeeder:
		strUnitType = ES_STRING("ADF/");
		break;
	case kESFunctionalUnitTransparent:
		strUnitType = ES_STRING("Transparent/");
		break;
	case kESFunctionalUnitTPUAreaGuide:
		strUnitType = ES_STRING("AreaGuide/");
		break;
	default:
		break;
	}

	ES_LOG_LEAVE_FUNC();
	return strUnitType;
}

//最初に見つけたフォルダのFunctionalUnitTypeを取得
ESString Emulator::GetFirstUnitType()
{
	ES_LOG_TRACE_FUNC();
	ESString strFirstUnitType;
	ESString strDummyInfoPath = GetDummyModelInfoDir();

	if(strDummyInfoPath.empty() == FALSE){
		//TODO: 見るフォルダの優先順位を付ける
		if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strDummyInfoPath+ES_STRING("Flatbed/")))
		{
			strFirstUnitType = ES_STRING("Flatbed/");
		}
		else if (ES_CMN_FUNCS::PATH::ES_IsExistFolder(strDummyInfoPath+ES_STRING("ADF/")))
		{
			strFirstUnitType = ES_STRING("ADF/");
		}
		else
		{
			//No Action
			ES_Error_Log(this, ES_STRING("FunctionalUnit is No Folder."));
		}
	}/*else{

		ESString strDBPath = GetDummyDBPath();

		if(strDBPath.empty() == FALSE){
			ZipFile cZip;

			if(cZip.Open(strDBPath.c_str()) == 0)
			{
				if(cZip.Read("Flatbed/AllCurrentValues.json") == 0){
					strFirstUnitType = ES_STRING("Flatbed/");
				}
				else if(cZip.Read("ADF/AllCurrentValues.json") == 0){
					strFirstUnitType = ES_STRING("ADF/");
				}
				else
				{
					//No Action
					ES_Error_Log(this, ES_STRING("FunctionalUnit is No Folder."));
				}
			}
		}
	}*/

	ES_LOG_LEAVE_FUNC();
	return strFirstUnitType;
}

//FunctionalUnitTypeが変更されているかを確認
BOOL Emulator::IsChangedFunctionalUnit()
{
	ES_LOG_TRACE_FUNC();
	BOOL bChanged = FALSE;
	ESString strUnitType = GetFunctionalUnitTypeFromDictionary();

	if (GetCurrentUnitType() != strUnitType)
	{
		bChanged = TRUE;
	}

	ES_LOG_LEAVE_FUNC();
	return bChanged;
}

//両面スキャンかどうかを確認
BOOL Emulator::IsDuplex()
{
	ES_LOG_TRACE_FUNC();
	ES_LOG_LEAVE_FUNC();
	return GetScanParamForKey_Internal(kESDuplex, false);
}

//FunctionalUnitTypeを任意のDictionayから取得
ESNumber Emulator::GetFunctionalUnitNameTypeFromDict(ESDictionary& dict)
{
	ES_LOG_TRACE_FUNC();
	ESNumber nFunctionalUnitType = (ESNumber)kESFunctionalUnitFlatbed;
	nFunctionalUnitType = SAFE_ANY_VALUE_FROM_DICT( dict,kESFunctionalUnitType,ESNumber, nFunctionalUnitType);

	ES_LOG_LEAVE_FUNC();
	return nFunctionalUnitType;
}

/*!
 * @brief	DummyInfoフォルダのパスを取得
 * @return	DummyInfoフォルダのパス
 */
ESString Emulator::GetDummyInfoFolderPath()
{
	ESString strDummyInfoDir;
	strDummyInfoDir = GetEmulatorCtrlerDir();
	if (ES_CMN_FUNCS::PATH::ES_IsExistFolder(strDummyInfoDir))
	{
		ES_CMN_FUNCS::PATH::ES_CombinePath(strDummyInfoDir, strDummyInfoDir, ES_STRING("DummyInfo"));
	}

	return strDummyInfoDir;
}

/*!
 * @brief	DummyModuleDirectoryの取得
 * @return	DummyModuleDirectory
 */
ESString Emulator::GetDummyModelInfoDir()
{
	if(GetDummyInfoFolderPath().empty()){
		return ES_STRING("");
	}

	ESString strDummyInfoDir;
	ES_CMN_FUNCS::PATH::ES_CombinePath(strDummyInfoDir, GetDummyInfoFolderPath(), GetModelName());
	ES_CMN_FUNCS::PATH::ES_AddBackYen(strDummyInfoDir);

	if(!ES_CMN_FUNCS::PATH::ES_IsExistFolder(strDummyInfoDir)){
		return ES_STRING("");
	}

	return strDummyInfoDir;
}

ESString Emulator::GetDummyDBPath(){
		ESString strModelInfo = GetModelInfoPath();
		ESString strModelName = GetModelName();

		ESString strPath;
		ES_CMN_FUNCS::PATH::ES_CombinePath(strPath,strModelInfo,strModelName);

		ESString strDBName = strPath + ES_STRING("/") + strModelName+ ES_STRING("_HW.db");

		if(ES_CMN_FUNCS::PATH::ES_IsExistFile(strDBName)){
			return strDBName;
		}
		return ES_STRING("");
}

//DummyInfoのPath(VirtualScannerControllerが起動している場所)を取得
ESString Emulator::GetEmulatorCtrlerDir()
{
	ES_LOG_TRACE_FUNC();

	ESString strCtrlerDir ;
	if(m_strCtrlerDirCache.empty() == FALSE){
		return m_strCtrlerDirCache;
	}

	m_strCtrlerDirCache = ES_STRING("Resources");


	ES_LOG_LEAVE_FUNC();


	return m_strCtrlerDirCache;
}

ESString Emulator::GetModelInfoPath()
{
	ES_LOG_TRACE_FUNC();

	ESString strModelInfoPath;


	//ModuleFolderの上位にDataフォルダがあれば、そちらを参照する

	ESString strTempDataPath ;
	ES_CMN_FUNCS::PATH::ES_GetModuleFolder(NULL, strTempDataPath);

	strTempDataPath += ES_STRING("/Resources/Models/");

	if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strTempDataPath)){
		return strTempDataPath;
	}


	//ESString strSubKey;
	//ES_CMN_FUNCS::PATH::ES_CombinePath(strSubKey, STR_REGISTRY_SUBKEY_EPSON, STR_REGISTRY_MODULE);

	//Registry cReg;
	BOOL bRet = FALSE;//cReg.Open(HKEY_LOCAL_MACHINE, strSubKey.c_str(), KEY_READ);
	if (bRet)
	{
		//strModelInfoPath = cReg.GetString(STR_REGISTRY_ENTRY_BASEDATAPATH);

		strModelInfoPath += ES_STRING("/Models/");
	}
	else
	{


		//ES_Error_Log(this, ES_STRING("%s Open failed"), strSubKey);
	}

	ES_LOG_LEAVE_FUNC();


	return strModelInfoPath;
}
#ifdef WIN32
//ScanImageCreator.exeを実行してスキャン画像を生成する
//@param	[in]	pcImage(CESScannedImage*)	スキャン画像クラス
//@return	csImagePath(ESString)	生成画像のフルパス
ESString Emulator::CreateImageWithImageCreator(CESScannedImage* pcImage)
{
	ES_LOG_TRACE_FUNC();
	ESString csImagePath;
	ESString csCmdParam = MakeImageCreateCommand(pcImage,csImagePath);

	if(!csImagePath.IsEmpty() && !csCmdParam.IsEmpty())
	{
		ES_Info_Log(this,ES_STRING("CallImageCreator[%s]"),csCmdParam.GetString());

		/*ScanImageCreator.exeの起動*/
		/*
		ESString strImgCreatorPath;
		ES_CMN_FUNCS::PATH::ES_CombinePath(strImgCreatorPath, GetEmulatorCtrlerDir(), ES_STRING("ScanImageCreator"));

		SHELLEXECUTEINFOW seInfo;
		::ZeroMemory(&seInfo, sizeof(seInfo));
		seInfo.cbSize		 = sizeof(SHELLEXECUTEINFOW);
		seInfo.hwnd			 = NULL;
		seInfo.nShow		 = SW_SHOWNORMAL;
		seInfo.fMask		 = SEE_MASK_NOCLOSEPROCESS;
		seInfo.lpParameters	 = csCmdParam;
		seInfo.lpFile		 = strImgCreatorPath.c_str();

		BOOL bRet = ::ShellExecuteExW(&seInfo);
		if (!bRet)
		{
			ES_Error_Log(ES_STRING("ERROR : %s ShellExeCute Fail."), strImgCreatorPath.c_str());
			return ES_STRING("");
		}
		::ES2WaitForSingleObject(seInfo.hProcess, INFINITE);
*/
		if (pcImage->GetImageFormat() == kESImageFormatJPEG)
		{
			csImagePath += ES_STRING(".jpeg");
		}
	}
	//MakeImageCreateCommand()内で作成したGammaTable.jsonを消去する
	ESString strGammaJson;
	ES_CMN_FUNCS::PATH::ES_CombinePath(strGammaJson, GetEmulatorCtrlerDir(), ES_STRING("GammaTable.json"));
	DeleteFile(strGammaJson.c_str());

	ES_LOG_LEAVE_FUNC();
	return csImagePath;
}


//ScanImageCreator.exeに渡すコマンドラインパラメータの生成
//@param	[in]	pImage(ESScannedImage*)	スキャン画像クラス
//@param	[out]	csImagePath(ESString)	ScanImageCreatorで生成する画像ファイル名
//@return	csCmdparam(ESString)	生成したコマンドラインパラメータ
ESString Emulator::MakeImageCreateCommand(CESScannedImage* pImage,	ESString& csImagePath)
{
	ES_LOG_TRACE_FUNC();
	ESString csCmdParam;

	/* ModelName ---- "/MODEL" */
	csCmdParam.AppendFormat(ES_STRING(" /MODEL %s")	, m_strModelName.c_str());

	/* Pixel ---- "/PIXEL" */
	ESNumber nPixelX = 0;
	ESNumber nPixelY = 0;
	BOOL bArea = FALSE;
	ESNumber nOffsetX = 0;
	ESNumber nOffsetY = 0;
	GetScanSizeInPixel(nPixelX, nPixelY, bArea, nOffsetX, nOffsetY);

	csCmdParam.AppendFormat(ES_STRING(" /PIXEL %d %d")		, nPixelX, nPixelY);

	/* BitsPerSample ---- "/BITS" */
	/* SamplesPerPixel ---- "/SAMPLE" */
	ESNumber nBitsPerSample = 0;
	ESNumber nSamplesPerPixel = 0;
	GetColorSample(nBitsPerSample, nSamplesPerPixel);
	ESNumber nBytesPerRow = nPixelX * nSamplesPerPixel;
	csCmdParam.AppendFormat(ES_STRING(" /SAMPLE %d")	, nSamplesPerPixel);
	csCmdParam.AppendFormat(ES_STRING(" /BITS %d")	, nBitsPerSample);

	/* SerialNumber ---- "/PAGE" */
	csCmdParam.AppendFormat(ES_STRING(" /PAGE %d")	, pImage->GetSerialNumber());
	/* PaperSerialNumber ---- "/PAPER" */
	csCmdParam.AppendFormat(ES_STRING(" /PAPER %d")	, pImage->GetPaperSerialNumber());
	/* SurfaceType ---- "/SIDE" */
	csCmdParam.AppendFormat(ES_STRING(" /SIDE %d")	, pImage->GetSurfaceType());
	/* ImageFormat ---- "/FORM" */
	csCmdParam.AppendFormat(ES_STRING(" /FORM %d")	, pImage->GetImageFormat());
	/* DuplexType ---- "/UPSIDE_DOWN" */
	csCmdParam.AppendFormat(ES_STRING(" /UPSIDE_DOWN %d")	, (BOOL)pImage->IsUpsideDown());

	/* ImageFileName ---- "/FNAME" */
	csImagePath.Format(ES_STRING("%sImageTest_Page%d"), GetEmulatorCtrlerDir().c_str(), pImage->GetPaperSerialNumber());
	csCmdParam.AppendFormat(ES_STRING(" /FNAME \"%s\"")	, csImagePath);

	/* Resolution ---- "/RESOLUTION" */
	ESNumber nResX = 0;
	ESNumber nResY = 0;
	GetResolution(nResX, nResY);
	csCmdParam.AppendFormat(ES_STRING(" /RESOLUTION %d %d")	, nResX, nResY);

	/* ImageType ---- "/IMGTYPE" */
	ESNumber nImageType = GetLoadImageType();
	csCmdParam.AppendFormat(ES_STRING(" /IMGTYPE %d")	, nImageType);

	/* Offset ---- "/OFFSET" */
	if (nImageType == eLoadImgTypeDocument)
	{
		ST_ES_SIZE_F stfMaxScanSize = GetMaxScanSizeInPixel((ESFloat)nResX, (ESFloat)nResY);
		int nBackOffset = GetBackOffset((ESFloat)nResX, stfMaxScanSize.cx);
		if(!bArea)
		{
			nOffsetX = SetOffsetXInPixel(nPixelX, nBackOffset, stfMaxScanSize.cx);
		}
		else
		{
			nOffsetX += nBackOffset;
		}
	}
	csCmdParam.AppendFormat(ES_STRING(" /OFFSET %d %d")	, nOffsetX, nOffsetY);

	/* しきい値 ---- "/THRESHOLD" */
	ESNumber nThreshold = GetScanParamForKey_Internal(kESThreshold, 0);
	csCmdParam.AppendFormat(ES_STRING(" /THRESHOLD %d")	, nThreshold);

	/* Gammatable ---- "/GAMMA_TBL" */
	ESDictionary dictGammaTbl;
	if (pImage->GetBitsPerPixel() == 24 || pImage->GetBitsPerPixel() == 48)
	{	//Color
		dictGammaTbl[kESGammaTableRedChannelKey]   = GetScanParamForKey_Internal(kESGammaTableRed	,ESIndexArray());
		dictGammaTbl[kESGammaTableGreenChannelKey] = GetScanParamForKey_Internal(kESGammaTableGreen	,ESIndexArray());
		dictGammaTbl[kESGammaTableBlueChannelKey]  = GetScanParamForKey_Internal(kESGammaTableBlue	,ESIndexArray());
	}
	else
	{	//Gray or Mono
		dictGammaTbl[kESGammaTableMonoChannelKey]   = GetScanParamForKey_Internal(kESGammaTableMono,ESIndexArray());
	}

	ESString strGammaJson;
	ES_CMN_FUNCS::PATH::ES_CombinePath(strGammaJson, GetEmulatorCtrlerDir(), ES_STRING("GammaTable.json"));
	ES_CMN_FUNCS::JSON::DictionaryToJSONFile(dictGammaTbl,strGammaJson);
	csCmdParam.AppendFormat(ES_STRING(" /GAMMA_TBL \"%s\" ")	, strGammaJson.c_str());


	/* IsIndicatedPageNumber ---- "/DRAW_TEXT" */
	csCmdParam.AppendFormat(ES_STRING(" /DRAW_TEXT %d")	, m_cVirtualSettings.IsIndicatedPageNumber());

	return csCmdParam;
	ES_LOG_LEAVE_FUNC();
}
#endif

//現在のDummyInfoPathにあるJSONをDictionaryに変換
void Emulator::LoadDummyInfo(ESString strDummyInfoPath, LPCTSTR strJsonFileName, ESDictionary& dictDummyInfo)
{
	ES_LOG_TRACE_FUNC();
	strDummyInfoPath += strJsonFileName;
	ES_CMN_FUNCS::JSON::JSONFiletoDictionary(strDummyInfoPath,dictDummyInfo);
	ES_LOG_LEAVE_FUNC();
}

void Emulator::LoadDummyInfoFromDB(ESString strDummyInfoDBPath,LPCTSTR strFunctionalUnitName,LPCTSTR strJsonName,ESDictionary& dictDummyInfo){
	ES_LOG_TRACE_FUNC();

	ESString strPath = strFunctionalUnitName;
	strPath= strPath + strJsonName;

	//ES_CMN_FUNCS::ARCHIVE::ArchiveToDictionary(strDummyInfoDBPath,strPath,dictDummyInfo);

	ES_LOG_LEAVE_FUNC();
}


//スキャン画像クラス設定値の初期化
//@param	[in]		pImage スキャン画像クラス
//@param	[inout]	nPageNumber スキャンページ番号
//@return	void
void Emulator::InitScanImage(CESScannedImage* pImage,INT nPageNumber)
{
	ES_LOG_TRACE_FUNC();
	//Temp DirにImageHandle生成
	ESString strPath;
	ES_CMN_FUNCS::PATH::ES_GetSystemTempDir(strPath);

	/*Pixel & Offset*/
	ESNumber nPixelX = 0;
	ESNumber nPixelY = 0;
	BOOL bArea = FALSE;
	ESNumber nOffsetX = 0;
	ESNumber nOffsetY = 0;
	GetScanSizeInPixel(nPixelX, nPixelY, bArea, nOffsetX, nOffsetY);

	ESNumber nBitsPerSample = 0;
	ESNumber nSamplesPerPixel = 0;
	GetColorSample(nBitsPerSample, nSamplesPerPixel);
	ESNumber nBytesPerRow = nPixelX * nSamplesPerPixel;
	if (nBitsPerSample == 1)
	{
		nBytesPerRow = (nBytesPerRow+7)/8;
	}

	/*SerialNumber & PaperSerialNumber*/
	if(pImage->Init(nPixelX,nPixelY, nBytesPerRow, nBitsPerSample, nSamplesPerPixel, strPath)== kESErrorNoError)
	{	//情報初期化
		pImage->SetSerialNumber(nPageNumber);
		if (IsDuplex())
		{
			if ((nPageNumber)%2 == 0)
			{
				//両面スキャンは偶数が裏面処理
				pImage->SetSurfaceType(kESSurfaceTypeBackSide);
			}
			pImage->SetPaperSerialNumber((nPageNumber +1)/2);
		}
		else
		{
			pImage->SetPaperSerialNumber(nPageNumber);
		}
		/*転送方式*/
		ESAny anyImgFormat = GetValue(kESImageFormat, m_dictScanParameter);
		const ESNumber* pnImgFormat = NULL;
		pnImgFormat = SAFE_ANY_DATA_CPTR(anyImgFormat, ESNumber);
		if (pnImgFormat)
		{
			pImage->SetImageFormat((ESImageFormat)*pnImgFormat);
		}
	}

	//2pass && BackSideの時は 画像を反転させるFlagを立てる
	ESNumber nDuplexType = GetCurrentValueForKey_Internal(kESDuplexType, (ESNumber)kESDuplexType1Pass);
	if (nDuplexType == (ESNumber)kESDuplexType2Pass && pImage->GetSurfaceType() == kESSurfaceTypeBackSide)
	{	//画像反転フラグをTrue
		pImage->SetUpsideDown(true);
	}

	ES_LOG_LEAVE_FUNC();
}

//ScanSizeをPixelで取得
void Emulator::GetScanSizeInPixel(ESNumber& nPixelX, ESNumber& nPixelY,BOOL& bArea ,ESNumber& nOffsetX,ESNumber& nOffsetY)
{
	ES_LOG_TRACE_FUNC();
	bArea = FALSE;

	/*ScanSize - Pixel*/
	ESAny anySizeInPixel = GetValue(kESScanSizeInPixel, m_dictScanParameter);
	const ST_ES_SIZE_UN32* pnPixel = NULL;
	pnPixel = SAFE_ANY_DATA_CPTR(anySizeInPixel, ST_ES_SIZE_UN32);

	if (pnPixel)
	{
		nPixelX = pnPixel->cx;
		nPixelY = pnPixel->cy;
	}
	else
	{	/*ScanSize - Inch*/
		ESAny anySizeInch = GetValue(kESScanSize, m_dictScanParameter);
		const ST_ES_SIZE_F* pnInch = NULL;
		pnInch = SAFE_ANY_DATA_CPTR(anySizeInch, ST_ES_SIZE_F);
		if (pnInch)
		{
			ESNumber nResX = 0;
			ESNumber nResY = 0;
			GetResolution(nResX, nResY);

			nPixelX = (ESNumber)((pnInch->cx) * nResX);
			nPixelY = (ESNumber)((pnInch->cy) * nResY);
		}
		else
		{	/*ScanArea*/
			ESAny anyAreaInPixel = GetValue(kESScanAreaInPixel, m_dictScanParameter);
			const ST_ES_RECT_UN32* pnArea = NULL;
			pnArea = SAFE_ANY_DATA_CPTR(anyAreaInPixel, ST_ES_RECT_UN32);
			if (pnArea)
			{
				nPixelX = pnArea->right - pnArea->left;
				nPixelY = pnArea->bottom- pnArea->top;

				nOffsetX = pnArea->left;
				nOffsetY = pnArea->top;
				bArea = TRUE;
			}
		}
	}

	//幅[Pixel]と高さ[Pixel]が0の場合は適当な数値を入れとく
	if (nPixelX == 0)
	{
		nPixelX = 2482;
	}
	if (nPixelY == 0)
	{
		nPixelY = 3507;
	}
	ES_LOG_LEAVE_FUNC();
}

void Emulator::GetResolution(ESNumber& nResX, ESNumber& nResY)
{
	ES_LOG_TRACE_FUNC();
	nResX = GetScanParamForKey_Internal(kESXResolution, nResX);
	nResY = GetScanParamForKey_Internal(kESYResolution, nResY);
	ES_LOG_LEAVE_FUNC();
}

//BitsPerSampleとSamplesPerPixelを取得する
void Emulator::GetColorSample(ESNumber& nBitsPerSample, ESNumber& nSamplesPerPixel)
{
	ES_LOG_TRACE_FUNC();
	ESNumber nColor = GetScanParamForKey_Internal(kESColorFormat, (ESNumber)kESColorFormatRGB24);
	switch (nColor)
	{
	case kESColorFormatRGB48:
	case kESColorFormatRGB24:
		nSamplesPerPixel = 3;
		break;
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
		nSamplesPerPixel = 1;
		break;
	default:
		nSamplesPerPixel = 0;
		break;
	}

	switch (nColor)
	{
	case kESColorFormatRGB48:
	case kESColorFormatMono16:
	case kESColorFormatMonoDropR16:
	case kESColorFormatMonoDropG16:
	case kESColorFormatMonoDropB16:
		nBitsPerSample = 16;
		break;
	case kESColorFormatRGB24:
	case kESColorFormatMono8:
	case kESColorFormatMonoDropR8:
	case kESColorFormatMonoDropG8:
	case kESColorFormatMonoDropB8:
		nBitsPerSample = 8;
		break;
	case kESColorFormatMono1:
	case kESColorFormatMonoDropR1:
	case kESColorFormatMonoDropG1:
	case kESColorFormatMonoDropB1:
		nBitsPerSample = 1;
		break;
	default:
		nBitsPerSample = 0;
		break;
	}
	ES_LOG_LEAVE_FUNC();
}

#include "../Include/ES2Command/ES2CommandAPI.h"

//==============================================================================================
//!
//!  スキャナ操作インターフェースの作成
//!
//!  @param
//!   eCommandType : スキャナのコマンドタイプ
//!   ppScanner    : 作成されたスキャナーインターフェースを受け取るポインタへのポインタ
//!  @return
//!   エラーコード
//!    成功 : kESErrorNoError
//!    失敗 : その他
//!
//==============================================================================================
extern "C" ESSCANNER_API ESErrorCode ESCreateScanner( ESCommandType eCommandType, IESScanner** ppScanner )
{
	ES_LOG_TRACE_FUNC();
	Emulator* pScanner = NULL;

	if (ppScanner == NULL ){
		return kESErrorInvalidParameter;
	}
	*ppScanner = NULL;
	try {
		pScanner = new Emulator();

	} catch( std::bad_alloc ){
		return kESErrorMemoryError;
	} catch( std::invalid_argument ){
		return kESErrorFatalError;
	} catch(...){
		return kESErrorFatalError;
	}
	ESErrorCode ret = pScanner->Initialize();
	if ( ret == kESErrorNoError ){
		*ppScanner = dynamic_cast< IESScanner* >( pScanner );
	}
	ES_LOG_LEAVE_FUNC();
	return ret;
}