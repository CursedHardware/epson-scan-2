#pragma once

#include "JsonCommon.h"
#include "JsonObject.hpp"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// データーなし Jsonオブジェクト
		//-----------------------------------
		class CJsonNoDataObject {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_NO_DATA; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESAny& value )
			{
				writer.StartObject();
				{
					writer.String( Label() );
					writer.Null();
				}
				writer.EndObject();
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESAny& anyDst )
			{
				anyDst = nullptr;
				return 0;
			}
		};

	}; // JSON
}; // ES_CMN_FUNCS
