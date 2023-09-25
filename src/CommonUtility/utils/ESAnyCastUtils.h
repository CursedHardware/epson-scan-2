#pragma once

#ifndef __ES_ANY_CAST_UTILS__
#define __ES_ANY_CAST_UTILS__

#include "../ESCommonTypedef.h"
#include "stringutils.h"
#include "../DbgLog.h"
#include "../ESproperty.h"

/**
マクロ一覧

定義されている関数を直接CALLすると、ログ出力されるエラー発生箇所がこのソースとなり、
Cast失敗箇所の特定が困難なため、以下のマクロを経由して呼び出すことを強く推奨する。

■Anyから値のポインタを取得する際に使用。
	SAFE_ANY_DATA_PTR
	SAFE_ANY_DATA_CPTR					//Const Version

■Dictionaryから指定キーの値に対するポインタを取得する
	SAFE_KEYS_DATA_PTR
	SAFE_KEYS_DATA_CPTR					//Const Version

■Dictionary内のDictionaryから値のポインタを取得する
	SAFE_KEY_DIC_IN_KEYS_DATA_PTR
	SAFE_KEY_DIC_IN_KEYS_DATA_CPTR		//Const Version

※※※※ここから下は、値コピーなので、Dictionaryなどの取得には極力使わないこと！！

■Anyから値を取得する。
	SAFE_ANY_VALUE

■Dictionaryの特定のキーの値を取得する。
	SAFE_ANY_VALUE_FROM_DICT
	SAFE_ANY_VALUE_FROM_DICT2		//成功失敗を取得可能

■PROPERTY_MAPから特定のキーの値を取得する。
	SAFE_ANY_VALUE_FROM_PMAP
	SAFE_ANY_VALUE_FROM_PMAP2		//成功失敗を取得可能

■PropertyMapからAnyのまま取得する。
	SAFE_GET_ANY_FROM_PMAP

*/

#ifdef WIN32
	#ifndef __ES_FILE__
		#define __ES_FILE__			__FILEW__
	#endif
	#ifndef __ES_FUNCTION__
		#define __ES_FUNCTION__		__FUNCTION__
	#endif
#else
	#ifndef __ES_FILE__
		#define __ES_FILE__			__FILE__
	#endif
	#ifndef __ES_FUNCTION__
		#define __ES_FUNCTION__		__FUNCTION__
	#endif
#endif

template < typename T >
static void SafeBoostAnyCast(ESAny anyIn, T& outputValue ,BOOL* pbResult = NULL)
{
	if( anyIn.empty() || (anyIn.type() != typeid(T)) ){
		if(anyIn.empty()){
			ES_Warning_Log2(__ES_FUNCTION__, ES_STRING("Boost Any Cast Warning Empty!!"));
		} else {
#ifndef __AFX_H__
			ES_Error_Log2(__ES_FUNCTION__, ES_STRING("Boost Any Cast Errror[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
			ES_Error_Log2(__ES_FUNCTION__, ES_STRING("Boost Any Cast Errror"));
#endif
		}
		if(pbResult){
			*pbResult = FALSE;
		}
	}else{
		try{
			outputValue = boost::any_cast<T>(anyIn);
			if(pbResult!= NULL){
				*pbResult = TRUE;
			}
		}catch(...){
#ifndef __AFX_H__
			ES_Error_Log2(__ES_FUNCTION__, ES_STRING("Boost Any Cast Errror[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
			ES_Error_Log2(__ES_FUNCTION__,ES_STRING("Boost Any Cast Errror"));
#endif
			if(pbResult){
				*pbResult = FALSE;
			}
		}
	}
}

template < typename T>
static T SafeBoostAnyCastWithLog(ESAny anyIn,T defaultValue, BOOL* pbResult, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	T tValue = defaultValue;

	if( anyIn.empty() || (anyIn.type() != typeid(T)) ){
		if(anyIn.empty()){
			ES_Warning_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Warning Empty!!"));
		}else if(anyIn.type() == typeid(nullptr)){
			ES_Warning_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Warning nullptr!!"));
		}else{
#ifndef __AFX_H__
			ES_Error_Log3(__ES_FUNCTION__, lpszFile, nLine,ES_STRING("Boost Any Cast Errror[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
			ES_Error_Log3(__ES_FUNCTION__, lpszFile, nLine,ES_STRING("Boost Any Cast Errror"));
#endif
		}
		if(pbResult){
			*pbResult = FALSE;
		}
		return defaultValue;
	}
	try{
		tValue = boost::any_cast<T>(anyIn);
		if(pbResult!= NULL){
			*pbResult = TRUE;
		}
	}catch(...){
#ifndef __AFX_H__
		ES_Error_Log3(__ES_FUNCTION__, lpszFile, nLine,ES_STRING("Boost Any Cast Errror[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
		ES_Error_Log3(__ES_FUNCTION__, lpszFile, nLine,ES_STRING("Boost Any Cast Errror"));
#endif
		if(pbResult){
			*pbResult = FALSE;
		}
	}
	return tValue;
}

// Anyが格納しているデーターを直接受け取る
template< typename T >
T* SafeAnyDataPtr( ESAny& anyIn )
{
	if( anyIn.empty() || (anyIn.type() != typeid(T)) ){
		return NULL;
	}
	try {
		return &(boost::any_cast< T& >(anyIn));
	} catch(...){
	}
	return NULL;
}
template< typename T >
const T* SafeAnyDataCPtr( const ESAny& anyIn )
{
	if( anyIn.empty() || (anyIn.type() != typeid(T)) ){
		return NULL;
	}
	try {
		return &(boost::any_cast< const T& >(anyIn));
	} catch(...){
	}
	return NULL;
}

template< typename T >
T* SafeAnyDataPtr_WithLog( ESAny& anyIn, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	if( anyIn.empty() || (anyIn.type() != typeid(T)) ){
		if(anyIn.empty()){
			ES_Warning_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Warning Empty!!"));
		} else {
#ifndef __AFX_H__
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error"));
#endif
		}
		return NULL;
	}
	try {
		return &(boost::any_cast< T& >(anyIn));
	} catch(...){
#ifndef __AFX_H__
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error"));
#endif
	}
	return NULL;
}

template< typename T >
const T* SafeAnyDataCPtr_WithLog( const ESAny& anyIn, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	if( anyIn.empty() || (anyIn.type() != typeid(T)) ){
		if(anyIn.empty()){
			ES_Warning_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Warning Empty!!"));
		} else {
#ifndef __AFX_H__
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error"));
#endif
		}
		return NULL;
	}
	try {
		return &(boost::any_cast< const T& >(anyIn));
	} catch(...){
#ifndef __AFX_H__
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error[%s]->[%s]"), ES_CMN_FUNCS::STRING::ToESString(anyIn.type().name()).c_str(), ES_CMN_FUNCS::STRING::ToESString(typeid(T).name()).c_str());
#else
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Boost Any Cast Error"));
#endif
	}
	return NULL;
}

// ESDictionaryの対応したキーが持つデーターを直接受け取る
template< typename T, typename DicT, typename KeyT >
T* SafeKeysDataPtr( DicT& dic, KeyT strKeyName )
{
	try {
		if ( dic.count( strKeyName ) != 0 ){
			return SafeAnyDataPtr< T >( dic.at(strKeyName) );
		}
	} catch(...){
	}
	return NULL;
}
template< typename T, typename DicT, typename KeyT >
const T* SafeKeysDataCPtr( const DicT& dic, KeyT strKeyName )
{
	try {
		if ( dic.count( strKeyName ) != 0 ){
			return SafeAnyDataCPtr< T >( dic.at(strKeyName) );
		}
	} catch(...){
	}
	return NULL;
}
template< typename T, typename DicT, typename KeyT >
T* SafeKeysDataPtr_WithLog( DicT& dic, KeyT strKeyName, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	try {
		if ( dic.count( strKeyName ) != 0 ){
			return SafeAnyDataPtr_WithLog< T >( dic.at(strKeyName), lpszFile, nLine );
		} else {
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is Empty."), ES_CMN_FUNCS::STRING::ToESString(strKeyName).c_str());
		}
	} catch(...){
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Key[%s] is unknown fail."), ES_CMN_FUNCS::STRING::ToESString(strKeyName).c_str());
	}
	return NULL;
}
template< typename T, typename DicT, typename KeyT >
const T* SafeKeysDataCPtr_WithLog( const DicT& dic, KeyT strKeyName, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	try {
		if ( dic.count( strKeyName ) != 0 ){
			return SafeAnyDataCPtr_WithLog< T >( dic.at(strKeyName), lpszFile, nLine );
		} else {
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is Empty."), ES_CMN_FUNCS::STRING::ToESString(strKeyName).c_str());
		}
	} catch(...){
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Key[%s] is unknown fail."), ES_CMN_FUNCS::STRING::ToESString(strKeyName).c_str());
	}
	return NULL;
}

// Dictionary内のDictionaryが持つキーデーターを直接受け取る
template< typename T, typename DicT, typename KeyT1, typename KeyT2 >
T* SafeKeyDicInKeysDataPtr( DicT& dic, KeyT1 strDicKeyName, KeyT2 strValuesKeyName )
{
	try {
		if ( dic.count( strDicKeyName ) != 0 ){
			DicT* pDic = SafeKeysDataPtr< DicT >( dic, strDicKeyName );
			if ( pDic && (pDic->count(strValuesKeyName) != 0) ){
				return SafeAnyDataPtr<T>( pDic->at(strValuesKeyName) );
			}
		}
	} catch(...){
	}
	return NULL;
}
template< typename T, typename DicT, typename KeyT1, typename KeyT2 >
const T* SafeKeyDicInKeysDataCPtr( const DicT& dic, KeyT1 strDicKeyName, KeyT2 strValuesKeyName )
{
	try {
		if ( dic.count( strDicKeyName ) != 0 ){
			const DicT* pDic = SafeKeysDataCPtr< DicT >( dic, strDicKeyName );
			if ( pDic && (pDic->count(strValuesKeyName) != 0) ){
				return SafeAnyDataCPtr<T>( pDic->at(strValuesKeyName) );
			}
		}
	} catch(...){
	}
	return NULL;
}
template< typename T, typename DicT, typename KeyT1, typename KeyT2 >
T* SafeKeyDicInKeysDataPtr_WithLog( DicT& dic, KeyT1 strDicKeyName, KeyT2 strValuesKeyName, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	try {
		if ( dic.count( strDicKeyName ) != 0 ){
			DicT* pDic = SafeKeysDataPtr_WithLog< DicT >( dic, strDicKeyName, lpszFile, nLine );
			if ( pDic ){
				if ( pDic->count(strValuesKeyName) != 0 ){
					return SafeAnyDataPtr_WithLog<T>( pDic->at(strValuesKeyName), lpszFile, nLine );
				} else {
					ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Key[%s, %s] is unknown fail."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str(), ES_CMN_FUNCS::STRING::ToESString(strValuesKeyName).c_str());
				}
			} else {
				ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is not dictionary."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str());
			}
		} else {
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is Empty."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str());
		}
	} catch(...){
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Key[%s, %s] is unknown fail."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str(), ES_CMN_FUNCS::STRING::ToESString(strValuesKeyName).c_str());
	}
	return NULL;
}
template< typename T, typename DicT, typename KeyT1, typename KeyT2 >
const T* SafeKeyDicInKeysDataCPtr_WithLog( const DicT& dic, KeyT1 strDicKeyName, KeyT2 strValuesKeyName, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__ )
{
	try {
		if ( dic.count( strDicKeyName ) != 0 ){
			const DicT* pDic = SafeKeysDataCPtr_WithLog< DicT >( dic, strDicKeyName, lpszFile, nLine );
			if ( pDic ){
				if ( pDic->count(strValuesKeyName) != 0 ){
					return SafeAnyDataCPtr_WithLog<T>( pDic->at(strValuesKeyName), lpszFile, nLine );
				} else {
					ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Key[%s, %s] is unknown fail."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str(), ES_CMN_FUNCS::STRING::ToESString(strValuesKeyName).c_str());
				}
			} else {
				ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is not dictionary."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str());
			}
		} else {
			ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is Empty."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str());
		}
	} catch(...){
		ES_Error_Log3( __ES_FUNCTION__, lpszFile, nLine, ES_STRING("Key[%s, %s] is unknown fail."), ES_CMN_FUNCS::STRING::ToESString(strDicKeyName).c_str(), ES_CMN_FUNCS::STRING::ToESString(strValuesKeyName).c_str());
	}
	return NULL;
}


#define SAFE_ANY_DATA_PTR( any, ValueType )													SafeAnyDataPtr_WithLog<ValueType>( any, __ES_FILE__, __LINE__ )
#define SAFE_ANY_DATA_CPTR( any, ValueType )												SafeAnyDataCPtr_WithLog<ValueType>( any, __ES_FILE__, __LINE__ )

#define SAFE_KEYS_DATA_PTR( dic, pszKeyName, ValueType )									SafeKeysDataPtr_WithLog<ValueType>( dic, pszKeyName, __ES_FILE__, __LINE__ )
#define SAFE_KEYS_DATA_CPTR( dic, pszKeyName, ValueType )									SafeKeysDataCPtr_WithLog<ValueType>( dic, pszKeyName, __ES_FILE__, __LINE__ )
#define SAFE_KEY_DIC_IN_KEYS_DATA_PTR( dic, pszDicKeyName, pszValuesKeyName, ValueType )	SafeKeyDicInKeysDataPtr_WithLog<ValueType>( dic, pszDicKeyName, pszValuesKeyName, __ES_FILE__, __LINE__ )
#define SAFE_KEY_DIC_IN_KEYS_DATA_CPTR( dic, pszDicKeyName, pszValuesKeyName, ValueType )	SafeKeyDicInKeysDataCPtr_WithLog<ValueType>( dic, pszDicKeyName, pszValuesKeyName, __ES_FILE__, __LINE__ )


template < typename T>
static T SafeGetValueForKeyWithLog(const ESDictionary& dict, ES_CHAR_CPTR pszKeyName, T defaultValue, BOOL* pbResult = NULL, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__)
{
	if(pbResult){
		*pbResult = FALSE;
	}
	if(SafeKeysDataCPtr_WithLog<T, ESDictionary, ES_CHAR_CPTR>(dict,pszKeyName,lpszFile,nLine)){
		if(pbResult){
			*pbResult = TRUE;
		}
		//Copy Value
		return *SafeKeysDataCPtr_WithLog<T, ESDictionary, ES_CHAR_CPTR>(dict,pszKeyName,lpszFile,nLine);
	}
	return defaultValue;
}

template < typename T>
static T SafeGetValueForKeyFromPropetyMapWithLog(PROPERTY_MAP& map, ES_CHAR_CPTR pszKeyName, T defaultValue, BOOL* pbResult = NULL, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__)
{
	T value = defaultValue;
	if(map.count(pszKeyName) > 0){
		const ESAny& anyData = map[pszKeyName].GetValue();
		if(!anyData.empty()){
			return SafeBoostAnyCastWithLog<T>(anyData,defaultValue,pbResult,lpszFile,nLine);
		}else{
			ES_Warning_Log(__ES_FUNCTION__, lpszFile, nLine, ES_STRING("Value of [%s] is Empty. SafeGetValueForKeyFromPropetyMap"), pszKeyName);
			if(pbResult){
				*pbResult = FALSE;
			}
		}
	}else{
		ES_Error_Log3(__ES_FUNCTION__, lpszFile, nLine, ES_STRING("SafeGetValueForKeyCast Key[%s] is unknown fail"), pszKeyName);
		if(pbResult){
			*pbResult = FALSE;
		}
	}
	return value;
}

static ESAny SafeGetAnyValueForKeyFromPropetyMapWithLog(PROPERTY_MAP& map, ES_CHAR_CPTR pszKeyName, BOOL* pbResult = NULL, ES_CHAR_CPTR lpszFile = __ES_FILE__, const int nLine = __LINE__)
{
	ESAny anyOut;

	if(map.count(pszKeyName) > 0){
		if(pbResult){
			*pbResult = TRUE;
		}
		return map[pszKeyName].GetValue();
	}
	return anyOut;
}

//値取り出し、値のCopyが発生するため、Dictionaryの取り出し等には使用しないこと。
#define SAFE_ANY_VALUE( ValueType, any, DefaultValue )								SafeBoostAnyCastWithLog                   <ValueType>( any, DefaultValue, NULL, __ES_FILE__, __LINE__ )
#define SAFE_ANY_VALUE_FROM_DICT( dict, key, ValueType, DefaultValue )				SafeGetValueForKeyWithLog                 <ValueType>( dict, key, DefaultValue, NULL, __ES_FILE__, __LINE__ )
#define SAFE_ANY_VALUE_FROM_DICT2( dict, key, ValueType, DefaultValue, pbResult )	SafeGetValueForKeyWithLog                 <ValueType>( dict, key, DefaultValue, pbResult, __ES_FILE__, __LINE__ )
#define SAFE_ANY_VALUE_FROM_PMAP( map, key, ValueType, DefaultValue )				SafeGetValueForKeyFromPropetyMapWithLog   <ValueType>( map, key, DefaultValue, NULL, __ES_FILE__, __LINE__ )
#define SAFE_ANY_VALUE_FROM_PMAP2( map, key, ValueType, DefaultValue, pbResult)		SafeGetValueForKeyFromPropetyMapWithLog   <ValueType>( map, key, DefaultValue,pbResult, __ES_FILE__, __LINE__ )
#define SAFE_GET_ANY_FROM_PMAP( map, key )											SafeGetAnyValueForKeyFromPropetyMapWithLog<ValueType>( map, key, NULL, __ES_FILE__, __LINE__ )

#endif // __ES_ANY_CAST_UTILS__
