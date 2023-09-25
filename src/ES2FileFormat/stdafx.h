// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
#include <windows.h>
#endif

#ifndef WIN32
#include "Platform.h"
#endif
// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include "ESCommonTypedef.h"
#include "ESproperty.h"
#include "ESBuffer.h"
#include "ESJsonUtils.h"
#include "ESImageInfo.h"
#include "FileFormatKitErrors.h"

#include "DbgLog.h"



#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)

	#define safeNew DBG_NEW

#else
#define safeNew new (std::nothrow)
#endif
