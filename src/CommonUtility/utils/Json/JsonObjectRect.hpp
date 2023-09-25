#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// ST_ES_RECT_S32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_RECT_S32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_RECT; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_RECT_S32& value )
			{
				writer.StartObject();
				writer.String( LB_RECT_X );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.left );
				writer.String( LB_RECT_Y );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.top );
				writer.String( LB_RECT_W );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.right - value.left );
				writer.String( LB_RECT_H );
				CJsonObject<ESNumber>::Write<Writer>(writer, value.bottom - value.top );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_RECT_S32& value )
			{
				if ( json.IsObject() &&
					 json.HasMember( LB_RECT_X ) &&
					 json.HasMember( LB_RECT_Y ) &&
					 json.HasMember( LB_RECT_W ) &&
					 json.HasMember( LB_RECT_H ) ){

					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RECT_X], value.left );
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RECT_Y], value.top );
					ESNumber w;
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RECT_W], w );
					value.right = value.left + w;
					ESNumber h;
					CJsonObject<ESNumber>::Read<JsonValue>(json[LB_RECT_H], h );
					value.bottom = value.top + h;

					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_RECT_UN32型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_RECT_UN32 > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_RECT_U; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_RECT_UN32& value )
			{
				writer.StartObject();
				writer.String( LB_RECT_X );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.left );
				writer.String( LB_RECT_Y );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.top );
				writer.String( LB_RECT_W );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.right - value.left );
				writer.String( LB_RECT_H );
				CJsonObject<ESUnsigned>::Write<Writer>(writer, value.bottom - value.top );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_RECT_UN32& value )
			{
				if ( json.IsObject() &&
					 json.HasMember( LB_RECT_X ) &&
					 json.HasMember( LB_RECT_Y ) &&
					 json.HasMember( LB_RECT_W ) &&
					 json.HasMember( LB_RECT_H ) ){

					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RECT_X], value.left );
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RECT_Y], value.top );
					ESUnsigned w;
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RECT_W], w );
					value.right = value.left + w;
					ESUnsigned h;
					CJsonObject<ESUnsigned>::Read<JsonValue>(json[LB_RECT_H], h );
					value.bottom = value.top + h;

					return 0;
				} else {
					return 1;
				}
			}
		};

		//
		// ST_ES_RECT_F型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ST_ES_RECT_F > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_RECT_F; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ST_ES_RECT_F& value )
			{
				writer.StartObject();
				writer.String( LB_RECT_X );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.left );
				writer.String( LB_RECT_Y );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.top );
				writer.String( LB_RECT_W );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.right - value.left );
				writer.String( LB_RECT_H );
				CJsonObject<ESFloat>::Write<Writer>(writer, value.bottom - value.top );
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ST_ES_RECT_F& value )
			{
				if ( json.IsObject() &&
					 json.HasMember( LB_RECT_X ) &&
					 json.HasMember( LB_RECT_Y ) &&
					 json.HasMember( LB_RECT_W ) &&
					 json.HasMember( LB_RECT_H ) ){

					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RECT_X], value.left );
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RECT_Y], value.top );
					ESFloat w;
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RECT_W], w );
					value.right = value.left + w;
					ESFloat h;
					CJsonObject<ESFloat>::Read<JsonValue>(json[LB_RECT_H], h );
					value.bottom = value.top + h;

					return 0;
				} else {
					return 1;
				}
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
