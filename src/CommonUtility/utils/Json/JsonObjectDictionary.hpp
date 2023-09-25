#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// ESDictionary型 Jsonオブジェクト
		//-----------------------------------
		class CJsonDictionaryObject {
		public:
			// [Label]
			static LPCTSTR Label(){ return NULL; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESAny& value );

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESDictionary& dicDst );
		};

		//
		// ESDicArray型 Jsonオブジェクト
		//-----------------------------------
		class CJsonDicArrayObject {
		public:
			// [Label]
			static LPCTSTR Label(){ return NULL; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESAny& value )
			{
				UInt32 un32ErrCounter = 0;
				if ( const ESDicArray* pDicArray = boost::any_cast<ESDicArray>( &value ) ){

					writer.StartArray();
					{
						writer.StartArray();
						{
							for( ESDicArray::const_iterator it = pDicArray->begin(); it != pDicArray->end(); ++it ){
								un32ErrCounter += CJsonDictionaryObject::Write<Writer>( writer, *it );
							}
						}
						writer.EndArray();
					}
					writer.EndArray();

					return un32ErrCounter;
				} else {
					writer.StartObject();
					writer.EndObject();
					return 1;
				}
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESDicArray& arDic )
			{
				UInt32 un32ErrCounter = 0;
				for ( SizeType i = 0; i < json.Size(); i++){
					if ( !json[i].IsArray() ){
						un32ErrCounter++;
						continue;
					}
					for ( SizeType j = 0; j < json[i].Size(); j++){
						ESDictionary dicSub;
						arDic.push_back(dicSub);
						un32ErrCounter += CJsonDictionaryObject::Read<JsonValue>( json[i][j], arDic.back() );
					}
				}
				return un32ErrCounter;
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
