////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     Emulator.h
//!
//! @brif     Emulator クラス
//! @note
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2017/06/21
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#include <es2CommnoType.h>
#include <ESResultString.h>
#include <pthread.h>
#include "ESCommandProperties.h"
#include "ESPropertyKeys.h"
#include "IESScanner.h"
#include "utils/CritSec.h"
#include "Image/ESScannedImage.h"
#include "EmulatorSettings.h"


///////////////////////////////////////////////////////////////////
//
// 型定義
//
///////////////////////////////////////////////////////////////////
typedef ES_CHAR*		ES_JSON_PTR;	// JSON文字型ポインタ
typedef const ES_CHAR*	ES_JSON_CPTR;

typedef enum
{
	eLoadImgTypeDocument = 0,	//Document
	eLoadImgTypePhotoThumbnail,	//Photo thumbnail
	eLoadImgTypeNegaFilm,		//negaFilm
	eLoadImgTypePosiFilm,		//posiFilm
}VIRTUAL_ENUM_LOAD_IMAGE_TYPE;

class Emulator;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// [ IESScanner ]
//
// @brif スキャナ操作インターフェースクラス
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Emulator : public IESScanner
{
public:

	Emulator(void);
	~Emulator(void);

	pthread_t GetVirtualUIThread(){
		return m_pThread;
	}

	//===============================================================
	//!
	//! 初期化処理
	//!
	//===============================================================
	ESErrorCode Initialize()
	{
		return kESErrorNoError;
	}

	void SetScanCanceled(bool bCancel){
		m_ValueCritSec.Lock();
		m_bCancel = bCancel;
		m_ValueCritSec.Unlock();
	}
	bool GetScanCanceled(){
		return m_bCancel;
	}

	bool GetScanning(){
		return m_bScanning;
	}

	ESString GetModelName(){
		return m_strModelName;
	}

	void SetInterrupted(bool bInterrupted){
		m_ValueCritSec.Lock();
		m_bIsInterrupted = bInterrupted;
		m_ValueCritSec.Unlock();
	}

	bool IsPaperLoaded(){
		return m_bIsPaperLoaded;
	}

	void SetCurrentUnitType(ESString strCurUnitType){
		m_strCurUnitType = strCurUnitType;
	}
	ESString GetCurrentUnitType(){
		return m_strCurUnitType;
	}

	void SetAllCurrentValues(ESDictionary& dictAllCurrentValues){
		m_dictAllCurrentValues = dictAllCurrentValues;
	}

	void SetAllValues(ESDictionary& dictAllValues){
		m_dictAllValues = dictAllValues;
	}

	void SetAllAvailableValues(ESDictionary& dictAllAvailableValues){
		m_dictAllAvailableValues = dictAllAvailableValues;
	}

	int GetErrorCodeNum(){
		return m_nErrorCodeNum;
	}
	void SetErrorCodeNum(int nErrorCodeNum){
		m_ValueCritSec.Lock();
		m_nErrorCodeNum = nErrorCodeNum;
		m_ValueCritSec.Unlock();
	}

	//PPM
	int GetPagePerMinute(){
		return m_nPPM;
	}
	void SetPagePerMinute(int nPPM){
		m_nPPM = nPPM;
	}

	int GetLoadingPaperNum(){
		return m_nLoadingPaperNum;
	}
	void SetLoadingPaperNum(int nLoadingPapernum){
		m_ValueCritSec.Lock();
		m_nLoadingPaperNum = nLoadingPapernum;
		m_ValueCritSec.Unlock();
	}

	ESNumber GetPreviousPageNum(){
		return m_nPreviousPageNum;
	}
	void SetPreviousPageNum(ESNumber nPrePageNum){
		m_nPreviousPageNum = nPrePageNum;
	}

	template<typename T>
	T GetScanParamForKey_Internal(ES_CHAR_CPTR pszKey,T defaultValue)
	{
		ES_LOG_TRACE_FUNC();
		CBlockCriticalSection cValueBlock(m_ValueCritSec);

		return SAFE_ANY_VALUE_FROM_DICT(m_dictScanParameter,pszKey,T,defaultValue);

	}
	template<typename T>
	T GetCurrentValueForKey_Internal(ES_CHAR_CPTR pszKey,T defaultValue)
	{
		ES_LOG_TRACE_FUNC();
		CBlockCriticalSection cValueBlock(m_ValueCritSec);

		return SAFE_ANY_VALUE_FROM_DICT(m_dictAllCurrentValues,pszKey,T,defaultValue);

	}
	template<typename T>
	void SetCurrentValueForKey_Internal(ES_CHAR_CPTR pszKey,T Value)
	{
		ES_LOG_TRACE_FUNC();
		CBlockCriticalSection cValueBlock(m_ValueCritSec);

		m_dictAllCurrentValues[pszKey] = Value;

	}


	template<typename T>
	T GetAvailableValueForKey_Internal(ES_CHAR_CPTR pszKey,T defaultValue){
		ES_LOG_TRACE_FUNC();
		CBlockCriticalSection cValueBlock(m_ValueCritSec);

		return SAFE_ANY_VALUE_FROM_DICT(m_dictAllAvailableValues,pszKey,T,defaultValue);

	}
	template<typename T>
	T GetAllValueForKey_Internal(ES_CHAR_CPTR pszKey,T defaultValue){

		return SAFE_ANY_VALUE_FROM_DICT(m_dictAllValues,pszKey,T,defaultValue);
	}

	//==============================================================================================
	//
	// 初期化・終了処理
	//
	//==============================================================================================

	//
	// スキャン時のプロセス処理を行うDelegateを設定する
	//
	virtual void SetDelegate( IESScannerDelegate* pDelegate );

	//
	// スキャナとの接続設定を行う
	//  設定値は下記のJSONフォーマット文字列を渡す
	//
	// 【 ローカル接続  kESConnectionLocal 】
	//
	// {
	//   "ConnectionSetting" : [
	//        {
	//           "ConnectType" :{
	//             "int" : 0,
	//           },
	//           "InternalName" : {
	//      	   "string" : "デバイス名(DeviceInternalName)"
	//           }
	//       }
	//    ]
	// }
	//
	// 【 ネットワーク接続  kESConnectionNetworkIS | kESConnectionNetworkNT 】
	//
	// {
	//   "ConnectionSetting" : [
	//        {
	//           "ConnectType" :{
	//             "int" : 1     // ISプロトコルの場合
	//           }
	//           "Address" : {
	//      	   "string" : "IPアドレス" または"ホスト名"
	//           }
	//           "ConnectionTimeout" : {  // 接続タイムアウト時間(秒）[オプション]
	//      	   "int" : 30
	//           }
	//           "CommunicationTimeout" : {  // 通信タイムアウト時間(秒）[オプション]
	//      	   "int" : 30
	//           }
	//       }
	//    ]
	// }
	//
	//
	virtual ESErrorCode SetConnection( ES_JSON_CPTR pszJSON );


	//
	// クラスインスタンスを破壊します。
	//
	virtual ESErrorCode DestroyInstance();


	//==============================================================================================
	//
	// オープン・クローズ
	//
	//==============================================================================================

	//
	// スキャナとの通信を開始する。
	//
	virtual ESErrorCode Open();

	//
	// スキャナとの通信を終了する
	//
	virtual ESErrorCode Close();

	//
	// スキャナがオープンしているか？
	//
	virtual bool IsOpened() const;

	//==============================================================================================
	//
	// Capture/release scanner to control mutual exclusive access for the same scanner
	//
	//==============================================================================================

	virtual ESErrorCode CaptureScanner();

	virtual ESErrorCode ReleaseScanner();

	//==============================================================================================
	//
	// スキャン操作
	//
	//==============================================================================================

	//
	// 現在のスキャン設定でスキャンを開始する。
	//
	virtual ESErrorCode Scan();

	//
	// 別スレッドでスキャンを開始する。
	//
	virtual ESErrorCode ScanInBackground();

	//
	// スキャン処理をキャンセルする。
	// キャンセル成功時には、DidCompleteScanningWithErrorにて通知される
	//
	virtual ESErrorCode Cancel();

	//
	//
	//
	virtual ESErrorCode Abort();

	//
	// スキャン中か？
	//
	virtual bool IsScanning() const;

	//==============================================================================================
	//
	// メンテナンス操作
	//
	//==============================================================================================

	//
	// スキャナセンサーのクリーニング動作を要求する。
	//
	virtual ESErrorCode DoCleaning();

	//
	// スキャナセンサーのキャリブレーションを要求する。
	//
	virtual ESErrorCode DoCalibration();

	//==============================================================================================
	//
	// AutoFeedingMode操作
	//
	//==============================================================================================

	//
	// 中断中か？
	//
	virtual bool IsInterrupted() const;

	//
	// AFM中か？
	//
	virtual bool IsAfmEnabled() const;

	//
	// Auto Feeding Mode 開始
	//
	virtual ESErrorCode StartScanningInAutoFeedingMode();

	//
	// Auto Feeding Mode 終了
	//
	virtual ESErrorCode StopScanningInAutoFeedingMode();

	// Auto Feeding Mode タイムアウトチェック開始
	//
	virtual ESErrorCode ScheduleAutoFeedingModeTimeout()
	{
		return kESErrorNoError;
	}

	//==============================================================================================
	//
	// ジョブリクエスト
	//
	//==============================================================================================

	//
	// スキャンジョブの開始
	//
	virtual ESErrorCode StartJobInMode( ESJobMode eJobMode );

	//
	// スキャンジョブの終了
	//
	virtual ESErrorCode StopJobInMode( ESJobMode eJobMode );


	//==============================================================================================
	//
	// その他
	//
	//==============================================================================================

	//
	// オートフォーカスを要求する。
	// @param pfOutFocus オートフォーカスの結果として焦点位置を受け取るバッファへのポインタ。単位はmm。
	// @return エラーコード。
	//
	virtual ESErrorCode DoAutoFocus( ESFloat* pfOutFocus );

	//
	// スキャナーパネルのプッシュスキャン可能状態を設定する
	//
	virtual ESErrorCode SetPanelToPushScanReady( BOOL bPushScanReady );


	//==============================================================================================
	//
	// スキャナ情報
	//
	//==============================================================================================

	//
	// 設定をリセットする
	//
	virtual ESErrorCode Reset();

	//
	//  すべての設定可能なキーを取得する
	//
	//  @param
	//   ppszJSON : 結果のJSON文字列を受け取るポインタ
	//
	//  取得した文字列は呼び出し側でESDeleteJSON()を利用して削除すること
	//
	// {
	//    AllKeys : {
	//       "array_str" : [
	//          "< key name1 >" ,
	//          "< key name2 >" ,
	//       ]
	//    }
	// }
	//
	virtual ESErrorCode GetAllKeys( IESResultString* pstrResult );

	//
	//  指定したキーのデフォルト値を取得する
	//
	//  @param
	//   pszKey   : キー名
	//   ppszJSON : 結果のJSON文字列を受け取るポインタ
	//
	// {
	//    "< key name >" : 値データ ※別資料参照
	// }
	//
	virtual ESErrorCode GetDefaultValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult );


	ESAny GetValue(ES_CHAR_CPTR pszKey, ESDictionary& dict);

	//
	//  指定したキーの設定値を取得する
	//
	//  @param
	//   pszKey   : キー名
	//   ppszJSON : 結果のJSON文字列を受け取るポインタ
	//
	// {
	//    "< key name >" : 値データ ※別資料参照
	// }
	//
	virtual ESErrorCode GetValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult );

	//
	//  指定したキーに値を設定する
	//
	//  @param
	//   pszKey   : キー名
	//   ppszJSON : 設定値を記述したJSON文字列ポインタ
	//
	// {
	//    "< key name >" : 値データ ※別資料参照
	// }
	//
	virtual ESErrorCode SetValueForKey( ES_CHAR_CPTR pszKey, ES_JSON_CPTR pszJSON );

	//
	// JSONでまとめて値を設定する
	//
	// {
	//    "< key name1 >" : 値データ ※別資料参照
	//    "< key name2 >" : 値データ ※別資料参照
	//   ...
	// }
	//
	virtual ESErrorCode SetValuesWithJSON( ES_JSON_CPTR pszJSON );

	//
	//  指定したキーにセット可能なすべての値を取得する
	//
	//  param
	//   pszKey   : キー名
	//   ppszJSON : 結果のJSON文字列を受け取るポインタ
	//
	// {
	//    "< key name >" : 値データ ※別資料参照
	// }
	//
	virtual ESErrorCode GetAllValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult );

	//
	//  全キーに対する、セット可能なすべての値を取得する
	//
	//  param
	//   ppszJSON : 結果のJSON文字列を受け取るポインタ
	//
	// {
	//    "< key name1 >" : 値データ ※別資料参照 ,
	//    "< key name2 >" : 値データ ※別資料参照 ,
	//   ...
	// }
	//
	virtual ESErrorCode GetAllValues( IESResultString* pstrResult );


	//
	//  指定したキーにセット可能なすべての値を取得する
	//
	//  param
	//   pszKey   : キー名
	//   ppszJSON : 結果のJSON文字列を受け取るポインタ
	//
	// {
	//    "< key name >" : 値データ ※別資料参照
	// }
	//
	virtual ESErrorCode GetAvailableValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult );

	//
	//  全キーに対する、セット可能なすべての値を取得する
	//
	// {
	//    "< key name1 >" : 値データ ※別資料参照 ,
	//    "< key name2 >" : 値データ ※別資料参照 ,
	//   ...
	//   ...
	// }
	//
	virtual ESErrorCode GetAllAvailableValues( IESResultString* pstrResult );

	BOOL IsDuplex();
	ESNumber GetFunctionalUnitNameTypeFromDict(ESDictionary& dict);
	ESString GetDummyInfoFolderPath();
	// * @brief	DummyModuleDirectoryの取得
	ESString GetDummyModelInfoDir();

private:
	pthread_t m_pThread;
	IESScannerDelegate* m_pcDelegate;
	bool m_bOpen;
	bool m_bScanning;
	bool m_bCancel;
	bool m_bAfmEnabled;
	bool m_bIsPaperLoaded;
	bool m_bIsInterrupted;

	EmulatorSettings m_cEmulatorSettings;

	CritSec m_ValueCritSec;

	ESString m_strModelName;
	pthread_t m_pScannerThread;
	pthread_t m_pcScanImageCreateThread;
	pthread_t m_pMSSThread;

	int m_nLoadingPaperNum;
	ESNumber m_nPreviousPageNum;
	int m_nErrorCodeNum;
	int m_nPPM;

	ESString m_strCtrlerDirCache;
	ESString m_strCmdModulePath;

	ESString m_strCurUnitType;

	ESString m_csImagePath;

	ESDictionary m_dictAllCurrentValues;
	ESDictionary m_dictAllValues;
	ESDictionary m_dictAllAvailableValues;
	ESDictionary m_dictScanParameter;

	void ScannerThread();
	static void* EnterScannerThread(void* vpContext);
	void MSSThread();
	static void* EnterMSSThread(void* vpContext);

	ESString GetFunctionalUnitTypeFromDictionary();
	ESString GetFirstUnitType();
	BOOL IsChangedFunctionalUnit();
	ESString GetDummyDBPath();
	void GetScanSizeInPixel(ESNumber& nPixelX, ESNumber& nPixelY,BOOL& bArea ,ESNumber& nOffsetX,ESNumber& nOffsetY);
	void GetResolution(ESNumber& nResX, ESNumber& nResY);
	void GetColorSample(ESNumber& nBitsPerSample, ESNumber& nSamplesPerPixel);
	ESString GetEmulatorCtrlerDir();
	ESString GetModelInfoPath();
	void InitScanImage(CESScannedImage* pcImage,INT nPageNumber);
	ESString CreateImageWithImageCreator(CESScannedImage* pcImage);
	ESString MakeImageCreateCommand(CESScannedImage* pcImage,ESString& csImagePath);

	void DoScan(ESNumber nLoadingPaperNum, ESNumber& nPreviousPageNum);

	void LoadDummyInfo(ESString strDummyInfoPath, LPCTSTR strJsonFileName, ESDictionary& dictDummyInfo);
	void LoadDummyInfoFromDB(ESString strDummyInfoDBPath,LPCTSTR strFunctionalUnitName,LPCTSTR strJsonName,ESDictionary& dictDummyInfo);

	ESErrorCode InitializeDummyInfo(ESString strFunctionalUnit);
};
