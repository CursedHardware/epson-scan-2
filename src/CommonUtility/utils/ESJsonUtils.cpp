#include "stdafx.h"
#include "Json/JsonCommon.h"
#include "Json/JsonObject.hpp"
#include "Json/JsonObjectPoint.hpp"
#include "Json/JsonObjectRect.hpp"
#include "Json/JsonObjectSize.hpp"
#include "Json/JsonObjectRange.hpp"
#include "Json/JsonObjectArray.hpp"
#include "Json/JsonObjectMultiArray.hpp"
#include "Json/JsonObjectDictionary.hpp"
#include "Json/JsonObjectNoData.hpp"
#include "Json/JsonObjectAny.hpp"

#ifdef _JSON_DEBUG
#include "debugstream.h"
#endif

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//==================================================================
		//!
		//! DictionaryデーターをJSON Writerに書き出します。
		//!
		//! @return 変換に失敗した要素数
		//!
		//==================================================================
		template < typename Writer >
		UInt32 DictionaryToJSONWriter( const ESDictionary& dicSrc, Writer& writer )
		{
			UInt32 un32ErrCounter = 0;
			try {
				writer.StartObject();
				{
					for ( ESDictionary::const_iterator it = dicSrc.begin(); it != dicSrc.end(); ++it ){
						const ESString& strKey = it->first;
						const ESAny& value = it->second;

						writer.String( strKey.c_str() );
						un32ErrCounter += CJsonObject<ESAny>::template Write<Writer>( writer, value );
					}
				}
				writer.EndObject();

				return un32ErrCounter;
			} catch(...){
				return ES_JSON_INVALID_ERR;
			}
		}

		//==================================================================
		//!
		//! DictionaryデーターをJSON文字列に書き出します。
		//!
		//! @return 変換に失敗した要素数
		//!
		//==================================================================
		UInt32 DictionaryToJSON( const ESDictionary& dicSrc, ESString& strDstJSON )
		{
#ifdef _JSON_DEBUG
			wdostream wdebug;
			ULONGLONG ullStart = GetTickCount64();
			wdebug << ES_STRING("DictionaryToJSON : ") << std::endl;
#endif

			UInt32 un32ErrCounter = 0;

			strDstJSON.clear();
			if ( dicSrc.empty()){
				goto BAIL;
			}

			try {
#ifdef WIN32
				StringBuffer16 buffer;
				BufferWriter16 writer(buffer);

				un32ErrCounter = DictionaryToJSONWriter< BufferWriter16 >( dicSrc, writer );
#else
				StringBuffer8 buffer;
				BufferWriter8 writer(buffer);

				un32ErrCounter = DictionaryToJSONWriter< BufferWriter8 >( dicSrc, writer );
#endif
				strDstJSON = buffer.GetString();

			} catch(...){
				un32ErrCounter = ES_JSON_INVALID_ERR;
			}

BAIL:
#ifdef _JSON_DEBUG
			wdebug << ES_STRING(" Time(ms) : ") << (GetTickCount64()-ullStart) << std::endl;
			wdebug << ES_STRING(" Ret :") << un32ErrCounter << std::endl;
#endif

			return un32ErrCounter;
		}

		//==================================================================
		//!
		//! DictionaryデーターからをJSONファイル作成します。
		//!
		//! @return 変換に失敗した要素数
		//!          ファイル作成に失敗もしくは例外エラーの場合は、ES_JSON_INVALID_ERRを返す
		//!
		//==================================================================
		UInt32 DictionaryToJSONFile( const ESDictionary& dicSrc, const ESString& strDstFile )
		{
#ifdef _JSON_DEBUG
			wdostream wdebug;
			ULONGLONG ullStart = GetTickCount64();
			wdebug << ES_STRING("DictionaryToJSONFile : ") << strDstFile << std::endl;
#endif

			FILE* fp = NULL;
			UInt32 un32ErrCounter = 0;

			if ( dicSrc.empty()){
				goto BAIL;
			}
			try {
				errno_t err = fopen_s( &fp, strDstFile.c_str(), ES_STRING("wb") );
				if ( err != 0 ){
					un32ErrCounter = ES_JSON_INVALID_ERR;
					goto BAIL;
				}
				char writeBuffer[512];
				FileWriteStream bos( fp, writeBuffer, sizeof(writeBuffer) );
#ifdef WIN32
				EncodeStream16 eos( bos, true );
				FileWriter16 writer(eos);

				un32ErrCounter = DictionaryToJSONWriter< FileWriter16 >( dicSrc, writer );
#else
				EncodeStream8 eos( bos, true );
				FileWriter8 writer(eos);

				un32ErrCounter = DictionaryToJSONWriter< FileWriter8 >( dicSrc, writer );
#endif
			} catch(...){
				un32ErrCounter = ES_JSON_INVALID_ERR;
			}
			try {
				if ( fp != NULL){
					fclose( fp );
				}
			}catch(...){
			}
BAIL:
#ifdef _JSON_DEBUG
			wdebug << ES_STRING(" Time(ms) : ") << (GetTickCount64()-ullStart) << std::endl;
			wdebug << ES_STRING(" Ret :") << un32ErrCounter << std::endl;
#endif

			return un32ErrCounter;
		}

		//==================================================================
		//!
		//! JSON StreamからDictionaryデーターを作成します。
		//!
		//! @return 変換に失敗した要素数
		//!
		//==================================================================
		template < typename Stream >
		UInt32 JSONStreamtoDictionary( Stream& s, ESDictionary& dicDst )
		{
			dicDst.clear();
#ifdef WIN32
			try {
				Document16 doc;
				doc.ParseStream< Stream >( s );
				if ( doc.HasParseError() ){
					return ES_JSON_INVALID_ERR;
				}

				UInt32 un32ErrCounter = 0;

				for ( Value16::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); itr++ ){
					un32ErrCounter += CJsonObject<ESAny>::Read<Value16>( itr->value, dicDst[ itr->name.GetString() ] );
				}
#else
			try {
				Document8 doc;
				doc.ParseStream< Stream >( s );
				if ( doc.HasParseError() ){
					return ES_JSON_INVALID_ERR;
				}

				UInt32 un32ErrCounter = 0;

				for ( Value8::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); itr++ ){
					un32ErrCounter += CJsonObject<ESAny>::Read<Value8>( itr->value, dicDst[ itr->name.GetString() ] );
				}
#endif
				return un32ErrCounter;
			} catch (...){
				return ES_JSON_INVALID_ERR;
			}
		}

		//==================================================================
		//!
		//! JSON文字列からDictionaryデーターを作成します。
		//!
		//! @return 変換に失敗した要素数
		//!
		//==================================================================
		UInt32 JSONtoDictionary( const ESString& strSrcJSON, ESDictionary& dicDst )
		{
#ifdef _JSON_DEBUG
			wdostream wdebug;
			ULONGLONG ullStart = GetTickCount64();
			wdebug << ES_STRING("JSONtoDictionary") << std::endl;
#endif

			UInt32 un32ErrCounter = 0;
			dicDst.clear();
			if ( strSrcJSON.empty() ){
				un32ErrCounter = 0;
				goto BAIL;
			}
			try {
#ifdef WIN32
				StringStream16 s( strSrcJSON.c_str() );
				un32ErrCounter = JSONStreamtoDictionary< StringStream16 >( s, dicDst );
#else
				StringStream8 s( strSrcJSON.c_str() );
				un32ErrCounter = JSONStreamtoDictionary< StringStream8 >( s, dicDst );
#endif
			} catch(...){
				un32ErrCounter = ES_JSON_INVALID_ERR;
			}
BAIL:
#ifdef _JSON_DEBUG
			wdebug << ES_STRING(" Time(ms) : ") << (GetTickCount64()-ullStart) << std::endl;
			wdebug << ES_STRING(" Ret :") << un32ErrCounter << std::endl;
#endif

			return un32ErrCounter;
		}

		//==================================================================
		//!
		//! JSONファイルからDictionaryデーターを作成します。
		//!
		//! @return 変換に失敗した要素数
		//!
		//==================================================================
		UInt32 JSONFiletoDictionary( const ESString& strSrcFile, ESDictionary& dicDst )
		{
#ifdef _JSON_DEBUG
			wdostream wdebug;
			ULONGLONG ullStart = GetTickCount64();
			wdebug << ES_STRING("JSONFiletoDictionary : ") << strSrcFile << std::endl;
#endif

			FILE* fp = NULL;
			UInt32 un32ErrCounter = 0;

			dicDst.clear();

			try {
				errno_t err = fopen_s( &fp, strSrcFile.c_str(), ES_STRING("rb") );
				if ( err != 0 ){
					un32ErrCounter = ES_JSON_INVALID_ERR;
					goto BAIL;
				}
				char readBuffer[256];
				FileReadStream bis( fp, readBuffer, sizeof(readBuffer) );
				AutoUTFInputStream<unsigned, FileReadStream> eis(bis);
				un32ErrCounter = JSONStreamtoDictionary< AutoUTFInputStream<unsigned, FileReadStream> >( eis, dicDst );
			} catch(...){
				un32ErrCounter = ES_JSON_INVALID_ERR;
			}
			try {
				if ( fp != NULL){
					fclose( fp );
				}
			}catch(...){
			}
BAIL:
#ifdef _JSON_DEBUG
			wdebug << ES_STRING(" Time(ms) : ") << (GetTickCount64()-ullStart) << std::endl;
			wdebug << ES_STRING(" Ret :") << un32ErrCounter << std::endl;
#endif

			return un32ErrCounter;
		}
	};
};
