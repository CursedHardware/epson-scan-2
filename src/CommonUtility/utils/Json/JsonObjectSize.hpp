#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// ST_ES_SIZE_S32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_SIZE_S32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_SIZE; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_SIZE_S32& value )
			{
				writer.StartObject();
				writer.String( LB_SIZE_X );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.cx );
				writer.String( LB_SIZE_Y );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.cy );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_SIZE_S32& value )
			{
				if ( json.IsObject() && json.HasMember( LB_SIZE_X ) && json.HasMember( LB_SIZE_Y ) ){
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_SIZE_X], value.cx );
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_SIZE_Y], value.cy );
					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_SIZE_UN32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_SIZE_UN32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_SIZE_U; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_SIZE_UN32& value )
			{
				writer.StartObject();
				writer.String( LB_SIZE_X );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.cx );
				writer.String( LB_SIZE_Y );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.cy );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_SIZE_UN32& value )
			{
				if ( json.IsObject() && json.HasMember( LB_SIZE_X ) && json.HasMember( LB_SIZE_Y ) ){
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_SIZE_X], value.cx );
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_SIZE_Y], value.cy );
					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_SIZE_F型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_SIZE_F > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_SIZE_F; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_SIZE_F& value )
			{
				writer.StartObject();
				writer.String( LB_SIZE_X );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.cx );
				writer.String( LB_SIZE_Y );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.cy );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_SIZE_F& value )
			{
				if ( json.IsObject() && json.HasMember( LB_SIZE_X ) && json.HasMember( LB_SIZE_Y ) ){
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_SIZE_X], value.cx );
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_SIZE_Y], value.cy );
					return 0;
				} else {
					return 1;
				}
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
