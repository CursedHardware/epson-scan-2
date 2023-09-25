#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"
#include "JsonObjectArray.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// ESIndexMultiArray型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESIndexMultiArray > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_MULTI_ARRAY; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESIndexMultiArray& value )
			{
				writer.StartArray();
				for( ESIndexMultiArray::const_iterator it = value.begin(); it != value.end(); ++it ){
					CJsonObject<ESIndexArray>::Write<Writer>(writer, *it );
				}
				writer.EndArray();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESIndexMultiArray& value )
			{
				UInt32 un32Ret = 0;
				if ( json.IsArray() ){
					for ( SizeType i = 0; i < json.Size(); i++){
						ESIndexArray ar;
						if ( CJsonObject<ESIndexArray>::Read<JsonValue>( json[i], ar ) == 0 ){
							value.push_back( ar );
						} else {
							un32Ret = 1;
						}
					}
				} else {
					return 1;
				}
				return un32Ret;
			}
		};

		//
		// ESUnsignedMultiArray型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESUnsignedMultiArray > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_MULTI_ARRAY_U; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESUnsignedMultiArray& value )
			{
				writer.StartArray();
				for( ESUnsignedMultiArray::const_iterator it = value.begin(); it != value.end(); ++it ){
					CJsonObject<ESUnsignedArray>::Write<Writer>(writer, *it );
				}
				writer.EndArray();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESUnsignedMultiArray& value )
			{
				UInt32 un32Ret = 0;
				if ( json.IsArray() ){
					for ( SizeType i = 0; i < json.Size(); i++){
						ESUnsignedArray ar;
						if ( CJsonObject<ESUnsignedArray>::Read<JsonValue>( json[i], ar ) == 0 ){
							value.push_back( ar );
						} else {
							un32Ret = 1;
						}
					}
				} else {
					return 1;
				}
				return un32Ret;
			}
		};

		//
		// ESFloatMultiArray型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESFloatMultiArray > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_MULTI_ARRAY_F; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESFloatMultiArray& value )
			{
				writer.StartArray();
				for( ESFloatMultiArray::const_iterator it = value.begin(); it != value.end(); ++it ){
					CJsonObject<ESFloatArray>::Write<Writer>(writer, *it );
				}
				writer.EndArray();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESFloatMultiArray& value )
			{
				UInt32 un32Ret = 0;
				if ( json.IsArray() ){
					for ( SizeType i = 0; i < json.Size(); i++){
						ESFloatArray ar;
						if ( CJsonObject<ESFloatArray>::Read<JsonValue>( json[i], ar ) == 0 ){
							value.push_back( ar );
						} else {
							un32Ret = 1;
						}
					}
				} else {
					return 1;
				}
				return un32Ret;
			}
		};

		//
		// ESStringMultiArray型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESStringMultiArray > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_MULTI_ARRAY_STR; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESStringMultiArray& value )
			{
				writer.StartArray();
				for( ESStringMultiArray::const_iterator it = value.begin(); it != value.end(); ++it ){
					CJsonObject<ESStringArray>::Write<Writer>(writer, *it );
				}
				writer.EndArray();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESStringMultiArray& value )
			{
				UInt32 un32Ret = 0;
				if ( json.IsArray() ){
					for ( SizeType i = 0; i < json.Size(); i++){
						ESStringArray ar;
						if ( CJsonObject<ESStringArray>::Read<JsonValue>( json[i], ar ) == 0 ){
							value.push_back( ar );
						} else {
							un32Ret = 1;
						}
					}
				} else {
					return 1;
				}
				return un32Ret;
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
