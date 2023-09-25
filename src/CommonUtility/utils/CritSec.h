////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     CritSec.h
//!
//! @brif     CRITICAL_SECTIONのWrapperクラスの定義
//! @note
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifdef WIN32
#include <Windows.h>	// CRITICAL_SECTION利用のため
#else
#include <mutex>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// CRITICAL_SECTIONのWrapperクラス
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CritSec
{
public:
#ifdef WIN32
	CritSec()
	{
		//Dynamic load version InitializeCriticalSectionEx
		InitializeCriticalSectionEx(m_CriticalSection);
	}

	~CritSec()
	{
		DeleteCriticalSection( &m_CriticalSection );
	}

	//_Acquires_lock_(m_CriticalSection) <- VS2012以降
	void Lock()
	{
		EnterCriticalSection(&m_CriticalSection);
	}

	//_Releases_lock_(m_CriticalSection) <- VS2012以降
	void Unlock()
	{
		LeaveCriticalSection(&m_CriticalSection);
	}
	typedef BOOL (WINAPI *InitializeCriticalSectionExProc)(
		LPCRITICAL_SECTION lpCriticalSection,
		DWORD dwSpinCount,
		DWORD Flags
	);

	static void InitializeCriticalSectionEx(CRITICAL_SECTION& section);
public:
	CRITICAL_SECTION m_CriticalSection;
private:
	static InitializeCriticalSectionExProc s_fpInitProc;

};
#else
	CritSec()
	{
	}

	~CritSec()
	{
	}

	void Unlock()
	{
		m_CriticalSection.unlock();
	}

	void Lock()
	{
		m_CriticalSection.lock();

	}
private:
	std::recursive_mutex m_CriticalSection;
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// CRITICAL_SECTIONの自動ロック、アンロック クラス
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBlockCriticalSection
{
public:
	//_Acquires_lock_(m_pCriticalSection) <- VS2012以降
	explicit CBlockCriticalSection(CritSec& crit)
	{
		m_pCriticalSection = &crit;
		m_pCriticalSection->Lock();
	}
	//_Releases_lock_(m_pCriticalSection) <- VS2012以降
	~CBlockCriticalSection()
	{
		m_pCriticalSection->Unlock();
	}
private:
	CritSec* m_pCriticalSection;
};
