#pragma once

#include "../ESCommonTypedef.h"

#ifdef WIN32
#include <Windows.h>	// Mutex利用のため
#endif

class CESMutex
{

private:
	HANDLE m_hMutex;

public:
	CESMutex(void);
	virtual ~CESMutex(void);

	BOOL InitMutex(LPCTSTR strMutexName,BOOL* pbAlreadyExist = NULL);		//Mutexを生成する（所有権は取得しない）

	static BOOL IsExistMutex(LPCTSTR strMutexName);	//Mutexが存在するか確認する

	BOOL EnterMutex();						//所有権を取得する
	void Release();								//所有権を解放する
	void CloseMutex();
};


class CBlockMutex
{
public:
	explicit CBlockMutex(CESMutex& mutex)
	{
		m_pMutex = &mutex;
		m_pMutex->EnterMutex();
	}
	~CBlockMutex()
	{
		m_pMutex->Release();
	}
private:
	CESMutex* m_pMutex;
};
