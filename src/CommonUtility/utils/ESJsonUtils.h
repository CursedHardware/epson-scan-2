
#pragma once

#include "../ESCommonTypedef.h"

namespace ES_CMN_FUNCS
{
	namespace JSON
	{
		STATIC_CONST_KEY TYPE_NO_DATA			= ES_STRING("no_data");			// データーなし
		STATIC_CONST_KEY TYPE_INT				= ES_STRING("int");				// 数値
		STATIC_CONST_KEY TYPE_INT_U				= ES_STRING("int_u");			// 数値 unsigned
		STATIC_CONST_KEY TYPE_FLOAT				= ES_STRING("float");			// 小数
		STATIC_CONST_KEY TYPE_BOOLEAN			= ES_STRING("boolean");			// ブール
		STATIC_CONST_KEY TYPE_INDEX				= ES_STRING("index");			// インデックス
		STATIC_CONST_KEY TYPE_ARRAY				= ES_STRING("array");			// 配列
		STATIC_CONST_KEY TYPE_ARRAY_U			= ES_STRING("array_u");			// 配列 unsigned
		STATIC_CONST_KEY TYPE_ARRAY_F			= ES_STRING("array_f");			// 配列 float
		STATIC_CONST_KEY TYPE_ARRAY_STR			= ES_STRING("array_str");		// 配列 文字列
		STATIC_CONST_KEY TYPE_ARRAY_BYTE		= ES_STRING("array_byte");		// 配列 バイト
		STATIC_CONST_KEY TYPE_MULTI_ARRAY		= ES_STRING("multi_array");		// 多次元配列
		STATIC_CONST_KEY TYPE_MULTI_ARRAY_U		= ES_STRING("multi_array_u");	// 多次元配列 unsigned
		STATIC_CONST_KEY TYPE_MULTI_ARRAY_F		= ES_STRING("multi_array_f");	// 多次元配列 float
		STATIC_CONST_KEY TYPE_MULTI_ARRAY_STR	= ES_STRING("multi_array_str");	// 多次元配列 文字列
		STATIC_CONST_KEY TYPE_STRING			= ES_STRING("string");			// 文字列
		STATIC_CONST_KEY TYPE_RANGE				= ES_STRING("range");			// 範囲
		STATIC_CONST_KEY TYPE_RANGE_U			= ES_STRING("range_u");			// 範囲 unsigned
		STATIC_CONST_KEY TYPE_RANGE_F			= ES_STRING("range_f");			// 範囲 float
		STATIC_CONST_KEY TYPE_SIZE				= ES_STRING("size");			// サイズ
		STATIC_CONST_KEY TYPE_SIZE_U			= ES_STRING("size_u");			// サイズ unsigned
		STATIC_CONST_KEY TYPE_SIZE_F			= ES_STRING("size_f");			// サイズ float
		STATIC_CONST_KEY TYPE_RECT				= ES_STRING("rect");			// 矩形
		STATIC_CONST_KEY TYPE_RECT_U			= ES_STRING("rect_u");			// 矩形 unsigned
		STATIC_CONST_KEY TYPE_RECT_F			= ES_STRING("rect_f");			// 矩形 float
		STATIC_CONST_KEY TYPE_POINT				= ES_STRING("point");			// ポイント
		STATIC_CONST_KEY TYPE_POINT_U			= ES_STRING("point_u");			// ポイント unsigned
		STATIC_CONST_KEY TYPE_POINT_F			= ES_STRING("point_f");			// ポイント float


		STATIC_CONST_KEY LB_RANGE_MIN			= ES_STRING("min");				// 範囲 min
		STATIC_CONST_KEY LB_RANGE_MAX			= ES_STRING("max");				// 範囲 max
		STATIC_CONST_KEY LB_RANGE_STEP			= ES_STRING("step");			// 範囲 step

		STATIC_CONST_KEY LB_SIZE_X				= ES_STRING("x");				// サイズ x
		STATIC_CONST_KEY LB_SIZE_Y				= ES_STRING("y");				// サイズ y

		STATIC_CONST_KEY LB_RECT_X				= ES_STRING("x");				// 矩形 x
		STATIC_CONST_KEY LB_RECT_Y				= ES_STRING("y");				// 矩形 y
		STATIC_CONST_KEY LB_RECT_W				= ES_STRING("w");				// 矩形 w
		STATIC_CONST_KEY LB_RECT_H				= ES_STRING("h");				// 矩形 h

		STATIC_CONST_KEY LB_POINT_X				= ES_STRING("x");				// ポイント x
		STATIC_CONST_KEY LB_POINT_Y				= ES_STRING("y");				// ポイント y

		const UInt32 ES_JSON_INVALID_ERR		= (UInt32)-1;		// 例外エラーが発生した時に返す値


		//!
		//! DictionaryデーターをJSON文字列に書き出します。
		//!
		//! @return 変換に失敗した要素数
		//!
		UInt32 DictionaryToJSON( const ESDictionary& dicSrc, ESString& strDstJSON );


		//!
		//! JSON文字列からDictionaryデーターを作成します。
		//!
		//! @return 変換に失敗した要素数
		//!
		UInt32 JSONtoDictionary( const ESString& strSrcJSON, ESDictionary& dicDst );


		//!
		//! DictionaryデーターからをJSONファイル作成します。
		//!
		//! @return 変換に失敗した要素数
		//!          ファイル作成に失敗もしくは例外エラーの場合は、ES_JSON_INVALID_ERRを返す
		//!
		UInt32 DictionaryToJSONFile( const ESDictionary& dicSrc, const ESString& strDstFile );


		//!
		//! JSONファイルからDictionaryデーターを作成します。
		//!
		//! @return 変換に失敗した要素数
		//!
		UInt32 JSONFiletoDictionary( const ESString& strSrcFile, ESDictionary& dicDst );

	};
};
