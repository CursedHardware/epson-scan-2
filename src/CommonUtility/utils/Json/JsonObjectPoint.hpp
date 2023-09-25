#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// ST_ES_POINT_S32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_POINT_S32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_POINT; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_POINT_S32& value )
			{
				writer.StartObject();
				writer.String( LB_POINT_X );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.x );
				writer.String( LB_POINT_Y );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.y );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_POINT_S32& value )
			{
				if ( json.IsObject() && json.HasMember( LB_POINT_X ) && json.HasMember( LB_POINT_Y ) ){
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_POINT_X], value.x );
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_POINT_Y], value.y );
					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_POINT_UN32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_POINT_UN32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_POINT_U; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_POINT_UN32& value )
			{
				writer.StartObject();
				writer.String( LB_POINT_X );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.x );
				writer.String( LB_POINT_Y );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.y );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_POINT_UN32& value )
			{
				if ( json.IsObject() && json.HasMember( LB_POINT_X ) && json.HasMember( LB_POINT_Y ) ){
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_POINT_X], value.x );
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_POINT_Y], value.y );
					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_POINT_F型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_POINT_F > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_POINT_F; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_POINT_F& value )
			{
				writer.StartObject();
				writer.String( LB_POINT_X );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.x );
				writer.String( LB_POINT_Y );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.y );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_POINT_F& value )
			{
				if ( json.IsObject() && json.HasMember( LB_POINT_X ) &&  json.HasMember( LB_POINT_Y ) ){
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_POINT_X], value.x );
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_POINT_Y], value.y );
					return 0;
				} else {
					return 1;
				}
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
