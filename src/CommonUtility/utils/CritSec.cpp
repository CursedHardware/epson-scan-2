////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     CritSec.cpp
//!
//! @brif     CRITICAL_SECTIONのWrapperクラスの定義
//! @note
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CritSec.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// CRITICAL_SECTIONのWrapperクラス
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
CritSec::InitializeCriticalSectionExProc CritSec::s_fpInitProc = NULL;
void CritSec::InitializeCriticalSectionEx(CRITICAL_SECTION& section)
{
	DWORDLONG dwlConditionMask = 0;
	OSVERSIONINFOEX osVer = {0 };
	osVer.dwMajorVersion = 6;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);

	if(s_fpInitProc == NULL && ::VerifyVersionInfo(&osVer, VER_MAJORVERSION, dwlConditionMask)){
		//Windows vista or later...
		HMODULE hInst =  ::LoadLibrary(TEXT("kernel32.dll"));
		if(hInst){
			s_fpInitProc = (InitializeCriticalSectionExProc)GetProcAddress(hInst, "InitializeCriticalSectionEx");
		}
	}

	if(s_fpInitProc){
		//For Win Vista or later version, critical section leaks debug info memory.
		//So, we use InitializeCriticalSectionEx
		(*s_fpInitProc)(&section, 4000, CRITICAL_SECTION_NO_DEBUG_INFO);
	}
	else{
		InitializeCriticalSection( &section );
	}
}
#endif
