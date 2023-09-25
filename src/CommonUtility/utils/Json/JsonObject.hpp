#pragma once

#include "JsonCommon.h"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		//
		// Jsonオブジェクト (このクラスを型毎に特殊化させる)
		//-----------------------------------
		template< typename ValueType >
		class CJsonObject {
		public:
			// [Label]
			static LPCTSTR Label(){ return NULL; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ValueType& value ){
				return 1;
			}
			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESNumber& value ){
				return 1;
			}
		};

		//
		// ESNumber型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESNumber > {
		public:
			// [Label]
			static LPCTSTR Label(){ return TYPE_INT; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESNumber& value )
			{
				writer.Int( (int)value );
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESNumber& value )
			{
				if ( json.IsInt() ){
					value = (ESNumber)json.GetInt();
				} else if ( json.IsString() ){
#ifdef WIN32
					value = (ESNumber)_wtoi( json.GetString() );
#else
					value = (ESNumber)atoi( json.GetString() );
#endif
				} else {
					return 1;
				}
				return 0;
			}
		};

		//
		// ESUnsigned型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESUnsigned > {
		public:
			// [Label]
	  		static LPCTSTR Label(){ return TYPE_INT_U; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESUnsigned& value )
			{
				writer.Uint( (unsigned int)value );
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESUnsigned& value )
			{
				if ( json.IsUint() ){
					value = (UInt32)json.GetUint();
				} else if ( json.IsInt() ){
					value = (UInt32)json.GetInt();
				} else if ( json.IsString() ){
#ifdef WIN32
					value = (UInt32)_wtoi( json.GetString() );
#else
					value = (UInt32)atoi( json.GetString() );
#endif
				} else {
					return 1;
				}
				return 0;
			}
		};

		//
		// ESFloat型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESFloat > {
		public:
			// [Label]
	  		static LPCTSTR Label(){ return TYPE_FLOAT; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESFloat& value )
			{
				//double d = atof( (boost::format("%f") % value).str().c_str() );
				char buffer[32] ={};
				sprintf_s( buffer, 32, "%f", value );
				double d = atof(buffer);
				//ESstringstreamA ss;
				//ss << value;
				//double d = atof( ss.str().c_str(), NULL );
				writer.Double( d );
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESFloat& value )
			{
				if ( json.IsDouble() ){
					value = (ESFloat)json.GetDouble();
				} else if ( json.IsInt() ){
					value = (ESFloat)json.GetInt();
				} else if ( json.IsUint() ){
					value = (ESFloat)json.GetUint();
				} else if ( json.IsString() ){
#ifdef WIN32
					value = (ESFloat)_wtof( json.GetString() );
#else
					value = (ESFloat)atof( json.GetString() );
#endif
				} else {
					return 1;
				}
				return 0;
			}
		};

		//
		// bool型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< bool > {
		public:
			// [Label]
	  		static LPCTSTR Label(){ return TYPE_BOOLEAN; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const bool& value )
			{
				writer.Bool( value );
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, bool& value )
			{
				if ( json.IsBool() ){
					value = json.GetBool();
				} else if ( json.IsString() ){
					ESString s = json.GetString();
					if ( s == ES_STRING("true") ){
						value = true;
					} else if ( s == ES_STRING("false") ){
						value = false;
					} else {
						return 1;
					}
				} else {
					return 1;
				}
				return 0;
			}
		};

		//
		// ESString型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ESString > {
		public:
			// [Label]
	  		static LPCTSTR Label(){ return TYPE_STRING; }

			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ESString& value )
			{
				writer.String( value.c_str() );
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ESString& value )
			{
				if ( json.IsString() ){
					value = json.GetString();
				} else {
					return 1;
				}
				return 0;
			}
		};
		//
		// ES_CHAR_CPTR型 Jsonオブジェクト
		//-----------------------------------
		template<> class CJsonObject< ES_CHAR_CPTR > {
		public:
			// [Label]
	  		static LPCTSTR Label(){ return TYPE_STRING; }
#
			// [Write]
			template< typename Writer >
			static UInt32 Write( Writer& writer, const ES_CHAR_CPTR& value )
			{
				writer.String( value );
				return 0;
			}

			// [Read]
			template< typename JsonValue >
			static UInt32 Read( const JsonValue& json, ES_CHAR_CPTR& value )
			{
				return 1; // ESStringを使う
			}
		};

	}; // JSON
}; // ES_CNM_FUNCS
