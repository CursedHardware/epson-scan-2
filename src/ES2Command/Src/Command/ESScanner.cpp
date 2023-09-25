////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScanner.cpp
//!
//! @brif     スキャナーコマンド用ベースクラスの実装
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "CommonUtility/utils/ESJsonUtils.h"
#include "CommonUtility/utils/ESBuffer.h"
using namespace ES_CMN_FUNCS::BUFFER;

#include "ESScanner.h"
#include "ESScannerDefs.h"

#include "ESCI/ESCIScanner.h"
#include "ESCI2/ESCI2Scanner.h"


CESScanner* CESScanner::CreateInstance(ESCommandType eCommandType)
{
	switch ( eCommandType ) {
	case kESCommandTypeESCI:
		return new CESCIScanner;
	case kESCommandTypeESCI2:
		return new CESCI2Scanner;
	default:
		ES_LOG_INVALID_COMMAND_TYPE_PARAM();
		throw std::invalid_argument("Invalid Command Type");
	}
	return NULL;
}

//===============================================================
//!
//! インスタンスの破壊
//!
//===============================================================
void CESScanner::DestroyInstance()
{
	ES_LOG_TRACE_FUNC();

	try {
		delete this;
	} catch (...){
		ES_LOG_EXCEPTION_ERROR();
	}
}

//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESScanner::CESScanner()
{
	PROPERTY_RW( kESWorkFolder,            CESScanner, ESString,        GetWorkFolder,           ESString,        SetWorkFolder            ); // rw   ESString
	PROPERTY_R ( kESErrorStatus,           CESScanner, ESNumber,        GetErrorStatus                                                     ); // r    ESNumber (ESErrorCode)
	PROPERTY_R ( kESAuthentication,        CESScanner, bool,            IsAuthenticationSupported                                          ); // rw    bool
	PROPERTY_RW( kESAuthenticationEnabled, CESScanner, bool,            IsAuthenticationEnabled, bool,            SetAuthenticationEnabled ); // r    bool
	PROPERTY_RW( kESAuthUsername,          CESScanner, ESString,        GetAuthUserName,         ESString,        SetAuthUserName          ); // rw   ESString
	PROPERTY_RW( kESAuthPassword,          CESScanner, ESString,        GetAuthPassword,         ESString,        SetAuthPassword          ); // rw   ESString
	PROPERTY_RW( kESFunctionalUnitType,    CESScanner, ESNumber,        GetFunctionalUnitType,   ESNumber,        SetFunctionalUnitType    ); // rw   ESNumber (ESFunctionalUnitType)
	PROPERTY_RW( kESXResolution,           CESScanner, ESNumber,        GetXResolution,          ESNumber,        SetXResolution           ); // rw   ESNumber
	PROPERTY_RW( kESYResolution,           CESScanner, ESNumber,        GetYResolution,          ESNumber,        SetYResolution           ); // rw   ESNumber
	PROPERTY_R ( kESOpticalResolution,     CESScanner, ESNumber,        GetOpticalResolution                                               ); // r   ESNumber
	PROPERTY_R ( kESMaxScanSize,           CESScanner, ST_ES_SIZE_F,    GetMaxScanSize                                                     ); // r   ST_ES_SIZE_F
	PROPERTY_RW( kESScanArea,              CESScanner, ST_ES_RECT_F,    GetScanArea,             ST_ES_RECT_F,    SetScanArea              ); // rw   ST_ES_RECT_F
	PROPERTY_RW( kESScanAreaInPixel,       CESScanner, ST_ES_RECT_UN32, GetScanAreaInPixel,      ST_ES_RECT_UN32, SetScanAreaInPixel       ); // rw   ST_ES_RECT_UN32
	PROPERTY_RW( kESScanSize,              CESScanner, ST_ES_SIZE_F,    GetScanSize,             ST_ES_SIZE_F,    SetScanSize              ); // rw   ST_ES_SIZE_F
	PROPERTY_RW( kESScanSizeInPixel,       CESScanner, ST_ES_SIZE_UN32, GetScanSizeInPixel,      ST_ES_SIZE_UN32, SetScanSizeInPixel       ); // rw   ST_ES_SIZE_UN32
	PROPERTY_RW( kESXOffsetMargin,         CESScanner, ESFloat,         GetXOffsetMargin,        ESFloat,         SetXOffsetMargin         ); // rw   ESFloat
	PROPERTY_RW( kESYOffsetMargin,         CESScanner, ESFloat,         GetYOffsetMargin,        ESFloat,         SetYOffsetMargin         ); // rw   ESFloat
	PROPERTY_RW( kESColorFormat,           CESScanner, ESNumber,        GetColorFormat,          ESNumber,        SetColorFormat           ); // rw   UInt32 (ESColorFormat)
	PROPERTY_RW( kESColorMatrix,           CESScanner, ESAny,           GetColorMatrix,          ESFloatArray,    SetColorMatrix           ); // rw ESFloatArray(3 x 3 Floats) or nullptr(get only)
	PROPERTY_R ( kESBitsPerPixel,          CESScanner, ESNumber,        GetBitsPerPixel                                                    ); // r   ESNumber
	PROPERTY_RW( kESGammaMode,             CESScanner, ESNumber,        GetGammaMode,            ESNumber,        SetGammaMode             ); // rw   ESNumber (ESGammaMode)
	PROPERTY_RW( kESGammaTableMono,        CESScanner, ESAny,           GetGammaTableMono,       ESIndexArray,    SetGammaTableMono        ); // ESIndexArray(256 bytes) or nullptr(get only)
	PROPERTY_RW( kESGammaTableRed,         CESScanner, ESAny,           GetGammaTableRed,        ESIndexArray,    SetGammaTableRed         ); // ESIndexArray(256 bytes) or nullptr(get only)
	PROPERTY_RW( kESGammaTableGreen,       CESScanner, ESAny,           GetGammaTableGreen,      ESIndexArray,    SetGammaTableGreen       ); // ESIndexArray(256 bytes) or nullptr(get only)
	PROPERTY_RW( kESGammaTableBlue,        CESScanner, ESAny,           GetGammaTableBlue,       ESIndexArray,    SetGammaTableBlue        ); // ESIndexArray(256 bytes) or nullptr(get only)
	PROPERTY_RW( kESThreshold,             CESScanner, ESNumber,        GetThreshold,            ESNumber,        SetThreshold             ); // rw   ESNumber
	PROPERTY_RW( kESQuietMode,             CESScanner, ESNumber,        GetQuietMode,            ESNumber,        SetQuietMode             ); // rw   ESNumber (ESQuietMode)
	PROPERTY_RW( kESBufferSize,            CESScanner, ESNumber,        GetBufferSize,           ESNumber,        SetBufferSize            ); // rw   ESNumber
	PROPERTY_R ( kESDuplexType,            CESScanner, ESNumber,        GetDuplexType                                                      ); // r   ESNumber (ESDuplexType)
	PROPERTY_RW( kESDuplex,                CESScanner, bool,            IsDuplexEnabled,         bool,            SetDuplexEnabled         ); // rw   bool
	PROPERTY_RW( kESDoubleFeedDetection,   CESScanner, ESNumber,        GetDoubleFeedDetection,  ESNumber,        SetDoubleFeedDetection   ); // rw   ESNumber (ESDoubleFeedDetection)
	PROPERTY_RW( kESPagesToBeScanned,      CESScanner, ESNumber,        GetPagesToBeScanned,     ESNumber,        SetPagesToBeScanned      ); // rw   ESNumber
	PROPERTY_R ( kESDocumentLoaded,        CESScanner, bool,            IsDocumentLoaded                                                   ); // r   bool
	PROPERTY_R ( kESCleaning,              CESScanner, bool,            IsCleaningSupported                                                ); // r   bool
	PROPERTY_R ( kESCalibration,           CESScanner, bool,            IsCalibrationSupported                                             ); // r   bool

	this->GetAllKeys();
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESScanner::~CESScanner()
{
}

//===============================================================
//!
//! 初期化処理
//!
//===============================================================
ESErrorCode CESScanner::Initialize()
{
	return kESErrorNoError;
}

//===============================================================
//!
//! 接続設定を行う
//!
//===============================================================
ESErrorCode CESScanner::SetConnection( ES_JSON_CPTR pszJSON )
{
	ES_LOG_TRACE_FUNC();

	IInterface* pInterface = nullptr;
	ESErrorCode err = CreateInterface(pszJSON, &pInterface);
	if (!pInterface){
		ES_LOG_INVALID_INPUT_PARAM();
		return err;
	}
	return SetDeviceInterface(pInterface);
}

//===============================================================
//!
//! 指定したキーの指定した能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetTargetCapabilityForKey( ES_CHAR_CPTR pszKey, ES_CHAR_CPTR pszTarget, ESDictionary& dicResult )
{
	if ( pszKey == NULL || pszKey[0] == ES_STRING('\0') || pszTarget == NULL || pszTarget[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}
	ESDictionary dicCapability;
	ESErrorCode err = GetCapabilityForKey( pszKey, dicCapability );
	if ( IS_ERR_CODE( err ) ){
		ES_ERROR_LOG( ES_STRING("Failed get capability for key. Key:%s, Target:%s"), pszKey, pszTarget);
		return err;
	}
	if ( dicCapability.count( pszTarget ) != 0 ){
		dicResult[ pszKey ] = dicCapability[ pszTarget ];;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! 指定したキーのデフォルト値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetDefaultValueForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult )
{
	ES_LOG_TRACE_FUNC();
	return GetTargetCapabilityForKey( pszKey, ES_CAPABILITY_KEY_DEFAULT, dicResult );
}

//===============================================================
//!
//! 指定したキーでセット可能なすべての値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAllValuesForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult )
{
	ES_LOG_TRACE_FUNC();
	return GetTargetCapabilityForKey( pszKey, ES_CAPABILITY_KEY_ALLVALUES, dicResult );
}

//===============================================================
//!
//! 指定したキーに現在セット可能なすべての値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAvailableValuesForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult )
{
	ES_LOG_TRACE_FUNC();
	return GetTargetCapabilityForKey( pszKey, ES_CAPABILITY_KEY_AVAILABLEVALUES, dicResult );
}

//===============================================================
//!
//!  全キーに対する、セット可能なすべての値を取得する
//!
//===============================================================
ESDictionary CESScanner::GetAllValuesDictionary()
{
	ES_LOG_TRACE_FUNC();

	ESDictionary dicResult;
	const ESStringArray& arKeyList = GetAllKeys();
	for(const ESString& strKey : arKeyList){
		GetAllValuesForKey( strKey.c_str(), dicResult );
	}
	return dicResult;
}

//===============================================================
//!
//!  全キーに対する、セット可能なすべての値を取得する
//!
//===============================================================
ESDictionary CESScanner::GetAvailableDictionary()
{
	ES_LOG_TRACE_FUNC();

	ESDictionary dicResult;
	const ESStringArray& arKeyList = GetAllKeys();
	for(const ESString& strKey : arKeyList){
		GetAvailableValuesForKey( strKey.c_str(), dicResult );
	}
	return dicResult;
}

//===============================================================
//!
//! 指定したキーの設定値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetValueForKey( ES_CHAR_CPTR pszKey, ESAny& anyValue )
{
	ES_LOG_TRACE_FUNC();

	if ( pszKey == NULL || pszKey[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	ES_INFO_LOG( ES_STRING("Key = %s"), pszKey );
	ESErrorCode err = kESErrorNoError;
	if ( m_mapAccessor.count( pszKey ) != 0 ){
		err = m_mapAccessor[ pszKey ].GetValue( anyValue );
	} else {
		err = GetValueForUndefinedKey( pszKey, anyValue );
	}
	return err;
}

//===============================================================
//!
//! 指定したキーの値を設定する
//!
//===============================================================
ESErrorCode CESScanner::SetValueForKey( ES_CHAR_CPTR pszKey, ESAny anyValue )
{
	ES_LOG_TRACE_FUNC();

	if ( pszKey == NULL || pszKey[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	ES_INFO_LOG( ES_STRING("Key = %s"), pszKey );
	ESErrorCode err = kESErrorNoError;
	if ( m_mapAccessor.count( pszKey ) != 0 ){
		err = m_mapAccessor[ pszKey ].SetValue( anyValue );
		if ( IS_ERR_CODE( err ) ){
			ES_ERROR_LOG( ES_STRING("Failed set value for %s key."), pszKey );
		}
	} else {
		err = SetValueForUndefinedKey( pszKey, anyValue );
	}
	return err;
}

//===============================================================
//!
//! 優先するキーを取得
//!
//===============================================================
ESStringArray CESScanner::GetPriorKeys()
{
	const ES_CHAR_CPTR KeyList[] ={
		kESFunctionalUnitType,
		kESXResolution,
		kESYResolution,
		kESXOffsetMargin,
		kESYOffsetMargin,
		kESColorFormat,
		NULL
	};
	static ESStringArray s_arPriorKeys(&KeyList[0], &KeyList[ _countof(KeyList)-1 ] );
	return s_arPriorKeys;
}

//===============================================================
//!
//! Dictionary形式で値をセットする
//!
//===============================================================
ESErrorCode CESScanner::SetValuesForKeysWithDictionary( ESDictionary& dicKeysValues )
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;
	ESDictionary dicValues = dicKeysValues;	// コピー

	//
	// PriorKeyを優先登録
	//-----------------------------------------
	const ESStringArray& arKeyList = GetPriorKeys();
	for(const ESString& strKey : arKeyList){
		if ( dicKeysValues.count( strKey.c_str() ) ){
			ESErrorCode errRet = SetValueForKey( strKey.c_str(), dicValues[ strKey.c_str() ] );
			if ( errRet != kESErrorNoError ){
				err = errRet;
			}
			dicValues.erase( strKey.c_str() );
		}
	}

	//
	// 残りを登録
	//-----------------------------------------
	ESDictionary::const_iterator itDic;
	for ( itDic = dicValues.begin(); itDic != dicValues.end() ; ++itDic ){
		ESErrorCode errRet = SetValueForKey( itDic->first.c_str(), itDic->second );
		if ( errRet != kESErrorNoError ){
			err = errRet;
		}
	}
	return err;
}

//===============================================================
//!
//! 登録されていないキーの値の取得処理
//!
//===============================================================
ESErrorCode CESScanner::GetValueForUndefinedKey( ES_CHAR_CPTR pszKey, ESAny& anyValue )
{
	ES_WARM_LOG( ES_STRING("%s key is not defined."), pszKey);
	anyValue = nullptr;
	return kESErrorInvalidParameter;
}

//===============================================================
//!
//! 登録されていないキーのセット処理
//!
//===============================================================
ESErrorCode CESScanner::SetValueForUndefinedKey( ES_CHAR_CPTR pszKey, ESAny anyValue )
{
	ES_WARM_LOG( ES_STRING("%s key is not defined."), pszKey);
	return kESErrorInvalidParameter;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// JSON
//

//===============================================================
//!
//! すべての設定可能なキーをJSONで取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAllKeys( IESResultString* pstrResult )
{
	if ( pstrResult == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	const ESStringArray& arKeys = GetAllKeys();

	// JSON文字列作成
	ESDictionary dicSrc;
	ESString strJSON;
	dicSrc[ ES_JSON_KEY_ALLKEYS ] =  arKeys;
	UInt32 ret = ES_CMN_FUNCS::JSON::DictionaryToJSON( dicSrc, strJSON );
	assert( ret == 0 );

	// バッファにセット
	return ( pstrResult->Set(strJSON.c_str()) ? kESErrorNoError : kESErrorFatalError );
}

//===============================================================
//!
//! 指定したキーの指定した能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )をJSONで取得する
//!
//===============================================================
ESErrorCode CESScanner::GetTargetCapabilityForKey( ES_CHAR_CPTR pszKey, ES_CHAR_CPTR pszTarget, IESResultString* pstrResult )
{
	if ( pstrResult == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	ESDictionary dicValues;
	ESErrorCode err = GetTargetCapabilityForKey( pszKey, pszTarget, dicValues );
	if ( IS_ERR_CODE( err ) ){
		ES_ERROR_LOG( ES_STRING("Failed get target capability for keys. key:%s, target:%s"), pszKey, pszTarget );
		return err;
	}
	if ( dicValues.empty() ){
		return kESErrorNoError;
	}

	// JSON文字列作成
	ESString strJSON;
	UInt32 ret = ES_CMN_FUNCS::JSON::DictionaryToJSON( dicValues, strJSON );
	assert( ret == 0 );

	// バッファにセット
	return ( pstrResult->Set(strJSON.c_str()) ? kESErrorNoError : kESErrorFatalError );
}

//===============================================================
//!
//! 指定したキーのデフォルト値をJSONで取得する
//!
//===============================================================
ESErrorCode CESScanner::GetDefaultValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	return GetTargetCapabilityForKey( pszKey, ES_CAPABILITY_KEY_DEFAULT, pstrResult );
}

//===============================================================
//!
//! 指定したキーにセット可能なすべての値をJSONで取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAllValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	return GetTargetCapabilityForKey( pszKey, ES_CAPABILITY_KEY_ALLVALUES, pstrResult );
}

//===============================================================
//!
//! 指定したキーに現在セット可能なすべての値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAvailableValuesForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	return GetTargetCapabilityForKey( pszKey, ES_CAPABILITY_KEY_AVAILABLEVALUES, pstrResult );
}

//===============================================================
//!
//! 指定したキーの設定値をJSONで取得する
//!
//===============================================================
ESErrorCode CESScanner::GetValueForKey( ES_CHAR_CPTR pszKey, IESResultString* pstrResult )
{
	if ( pstrResult == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	ESAny anyValue;
	ESErrorCode err = GetValueForKey( pszKey, anyValue );
	if ( IS_ERR_CODE( err ) ){
		ES_ERROR_LOG( ES_STRING("Failed get value for key.") );
		return err;
	}
	ESDictionary dicResult;
	dicResult[ pszKey ] = anyValue;

	// JSON文字列作成
	ESString strJSON;
	UInt32 ret = ES_CMN_FUNCS::JSON::DictionaryToJSON( dicResult, strJSON );
	assert( ret == 0 );

	// バッファにセット
	return ( pstrResult->Set(strJSON.c_str()) ? kESErrorNoError : kESErrorFatalError );
}

//===============================================================
//!
//! 指定したキーの値をJSONで設定する
//!
//===============================================================
ESErrorCode CESScanner::SetValueForKey( ES_CHAR_CPTR pszKey, ES_JSON_CPTR pszJSON )
{
	if ( pszKey == NULL || pszKey[0] == ES_STRING('\0') || pszJSON == NULL || pszJSON[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}
	// Json読み込み
	ESDictionary dicValue;
	UInt32 un32ErrCounter = ES_CMN_FUNCS::JSON::JSONtoDictionary( pszJSON, dicValue );
	assert( un32ErrCounter == 0 );

	// Dictionaryから値を取り出しセットする
	if ( dicValue.count( pszKey ) == 0 ){
		ES_LOG_INVALID_JSON_FORMAT( pszJSON );
		return kESErrorFatalError;
	}
	return SetValueForKey( pszKey, dicValue[ pszKey ] );
}


//===============================================================
//!
//! JSON形式で値をセットする
//!
//===============================================================
ESErrorCode CESScanner::SetValuesWithJSON( ES_JSON_CPTR pszJSON )
{
	if ( pszJSON == NULL || pszJSON[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	// Json読み込み
	ESDictionary dicValue;
	UInt32 un32ErrCounter = ES_CMN_FUNCS::JSON::JSONtoDictionary( pszJSON, dicValue );
	assert( un32ErrCounter == 0 );

	// 値をセットする
	return SetValuesForKeysWithDictionary( dicValue );
}

//===============================================================
//!
//!  全キーに対する、セット可能なすべての値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAllValues( IESResultString* pstrResult )
{
	if ( pstrResult == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	ESDictionary dicValue = GetAllValuesDictionary();

	// JSON文字列作成
	ESString strJSON;
	UInt32 ret = ES_CMN_FUNCS::JSON::DictionaryToJSON( dicValue, strJSON );
	assert( ret == 0 );

	// バッファにセット
	return ( pstrResult->Set(strJSON.c_str()) ? kESErrorNoError : kESErrorFatalError );
}

//===============================================================
//!
//!  全キーに対する、セット可能なすべての値を取得する
//!
//===============================================================
ESErrorCode CESScanner::GetAllAvailableValues( IESResultString* pstrResult )
{
	if ( pstrResult == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	ESDictionary dicValue = GetAvailableDictionary();

	// JSON文字列作成
	ESString strJSON;
	UInt32 ret = ES_CMN_FUNCS::JSON::DictionaryToJSON( dicValue, strJSON );
	assert( ret == 0 );

	// バッファにセット
	return ( pstrResult->Set(strJSON.c_str()) ? kESErrorNoError : kESErrorFatalError );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// 別スレッド スキャン
//
void* CESScanner::EnterScannerThread(void* vpContext)
{
	ES_LOG_TRACE_FUNC();
	if(vpContext)
	{
		CESScanner* pcThis =(CESScanner*) vpContext;
		pcThis->Scan();
	}
	pthread_exit(0);
	return (void*)NULL;
}

//===============================================================
//!
//! 別スレッドでスキャンを開始する。
//!
//===============================================================
ESErrorCode CESScanner::ScanInBackground()
{
	ES_LOG_TRACE_FUNC();

	((CESScanner*)this)->SetScanning( true );
	((CESScanner*)this)->SetCancelled( false );

	pthread_t thread;
	if (pthread_create(&thread, NULL, &CESScanner::EnterScannerThread, this) == 0)
	{
		pthread_detach(thread);
		return kESErrorNoError;
	}
	
	return kESErrorFatalError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// その他
//

//===============================================================
//!
//! オートフォーカスを要求する。
//!
//===============================================================
ESErrorCode CESScanner::DoAutoFocus( ESFloat* pfOutFocus )
{
	return kESErrorNoError;
}

BOOL CESScanner::IsScannableDeviceConfig()
{
	return TRUE;
}
