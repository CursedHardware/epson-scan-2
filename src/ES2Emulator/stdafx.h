// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once


#include "ESCommonTypedef.h"
#include "ESCommandProperties.h"
#include "DbgLog.h"
#include "utils/ESAnyCastUtils.h"
#include "utils/PathUtils.h"
#include "utils/ESJsonUtils.h"
#include "utils/ESFile.h"
#include "boost/format.hpp"


// TRACE
#define ES_TRACE_LOG( msg, ... )					ES_Trace_Log2( __FUNCTION__, msg, ##__VA_ARGS__ );

#define ES_LOG_TRACE_FUNC()							ES_TRACE_LOG( ES_STRING("ENTER : %s"), __FUNCTION__ )
#define ES_LOG_LEAVE_FUNC()							ES_TRACE_LOG( ES_STRING("LEAVE : %s"), __FUNCTION__ )

/*#define STR_REGISTRY_SUBKEY_EPSON	ES_STRING("SOFTWARE\\EPSON")
#define STR_REGISTRY_MODULE			ES_STRING("Epson Scan 2")
#define STR_REGISTRY_DUMMYMODULE	ES_STRING("VirtualScanner")
#define STR_REGISTRY_VALUE_VIRTUALSCANNERPATH	ES_STRING("VirtualScannerPath")

#define	STR_REGISTRY_ENTRY_BASEMODULTPATH		ES_STRING("BaseModulePath")		//!< モジュールのベースディレクトリ
#define	STR_REGISTRY_ENTRY_BASEDATAPATH			ES_STRING("BaseDataPath")			//!< データベースのルートディレクトリ
*/
inline bool IsEnableDumpCommand(){
	return false;
}
#define ES_INFO_LOG( msg, ... )	printf
