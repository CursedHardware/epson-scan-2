#pragma once

#include "../Include/es2CommnoType.h"
#include <assert.h>
class CESReferenceCounter
{
public:
	CESReferenceCounter(void){
		m_lRefCounter = 0;
	}
	virtual ~CESReferenceCounter(void){

	}

	LONG IncrementReference(){

#ifdef WIN32
		return InterlockedIncrement(&m_lRefCounter);
#else
		CBlockCriticalSection sec(m_sec);
		m_lRefCounter++;
		return m_lRefCounter;
#endif
	}

	LONG DecrementReference(){
#ifdef WIN32
		return  InterlockedDecrement(&m_lRefCounter);
#else
		CBlockCriticalSection sec(m_sec);
		m_lRefCounter--;
		return m_lRefCounter;
#endif
	}
	LONG GetReferenceCounter(){
		return m_lRefCounter;
	}

private:
	LONG m_lRefCounter;
#ifndef WIN32
	CritSec m_sec;
#endif
};
