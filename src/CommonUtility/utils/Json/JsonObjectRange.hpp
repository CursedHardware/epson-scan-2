#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// ST_ES_RANGE型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_RANGE > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_RANGE; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_RANGE& value )
			{
				writer.StartObject();
				writer.String( LB_RANGE_MIN );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.nMin );
				writer.String( LB_RANGE_MAX );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.nMax );
				writer.String( LB_RANGE_STEP );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.nStep );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_RANGE& value )
			{
				if ( json.IsObject() &&
					 json.HasMember( LB_RANGE_MIN ) &&
					 json.HasMember( LB_RANGE_MAX ) &&
					 json.HasMember( LB_RANGE_STEP ) ){

					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RANGE_MIN],  value.nMin );
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RANGE_MAX],  value.nMax );
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RANGE_STEP], value.nStep );
					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_RANGE_UN32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_RANGE_UN32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_RANGE_U; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_RANGE_UN32& value )
			{
				writer.StartObject();
				writer.String( LB_RANGE_MIN );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.un32Min );
				writer.String( LB_RANGE_MAX );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.un32Max );
				writer.String( LB_RANGE_STEP );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.un32Step );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_RANGE_UN32& value )
			{
				if ( json.IsObject() &&
					 json.HasMember( LB_RANGE_MIN ) &&
					 json.HasMember( LB_RANGE_MAX ) &&
					 json.HasMember( LB_RANGE_STEP ) ){

					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RANGE_MIN], value.un32Min );
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RANGE_MAX], value.un32Max );
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RANGE_STEP], value.un32Step );
					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_RANGE_F型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_RANGE_F > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_RANGE_F; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_RANGE_F& value )
			{
				writer.StartObject();
				writer.String( LB_RANGE_MIN );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.fMin );
				writer.String( LB_RANGE_MAX );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.fMax );
				writer.String( LB_RANGE_STEP );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.fStep );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_RANGE_F& value )
			{
				if ( json.IsObject() &&
					 json.HasMember( LB_RANGE_MIN ) &&
					 json.HasMember( LB_RANGE_MAX ) &&
					 json.HasMember( LB_RANGE_STEP ) ){

					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RANGE_MIN], value.fMin );
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RANGE_MAX], value.fMax );
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RANGE_STEP], value.fStep );
					return 0;
				} else {
					return 1;
				}
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
