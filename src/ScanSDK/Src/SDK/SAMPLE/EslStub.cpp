/***********************************************************************

	Copyright (c) Seiko Epson Corporation
 	All rights reserved.

	Date		Author			Reason
 ***********************************************************************/
//#include "stdafx.h"
#include "EslStub.h"
#include <iostream>

//SDK library input full path
#define	SCANSDK_DLL	"/usr/lib/x86_64-linux-gnu/epsonscan2/libepsonscansdk.so"

CEslStub::CEslStub(LPCTSTR pszLibModule) :
es2lib_(NULL),
m_peslInit(NULL),
m_peslFree(NULL),
m_peslOpenScanner(NULL),
m_peslCtrlParameter(NULL),
m_peslExecute(NULL),
m_peslCloseScanner(NULL),
m_peslGetDeviceStatus(NULL)
{
//	ES_TRACE_FUNC(TEXT("CEslStub::CEslStub"));

	std::string strDst = SCANSDK_DLL;

	es2lib_ = dlopen(strDst.c_str(), RTLD_LAZY);
	if(es2lib_ == NULL){
	    printf("dlerro = %s\n", dlerror());
	    assert(false);
	}

	m_peslInit = (tfeslInit*)dlsym(es2lib_, "eslInit");
	m_peslFree = (tfeslFree*)dlsym(es2lib_, "eslFree");
	m_peslOpenScanner = (tfeslOpenScanner*)dlsym(es2lib_, "eslOpenScanner");
	m_peslCtrlParameter = (tfeslCtrlParameter*)dlsym(es2lib_, "eslCtrlParameter");
	m_peslExecute = (tfeslExecute*)dlsym(es2lib_, "eslExecute");
	m_peslCloseScanner = (tfeslCloseScanner*)dlsym(es2lib_, "eslCloseScanner");
	m_peslGetDeviceStatus = (tfeslGetDeviceStatus*)dlsym(es2lib_, "eslGetDeviceStatus");

	if (m_peslInit && m_peslFree && m_peslOpenScanner && m_peslCtrlParameter
		&& m_peslExecute && m_peslCloseScanner && m_peslGetDeviceStatus) {
		std::cout << "[INFO]Required function pointers READY\n" << std::endl;
		return;
	} else {
		std::cout << "[ERROR]Required function pointers NOT READY\n" << std::endl;
		Exit();
	}
}

CEslStub::~CEslStub()
{
	Exit();
}

void CEslStub::Exit()
{ 
	if (es2lib_ != NULL) {
      	dlclose(es2lib_);
    	es2lib_ = NULL;

		m_peslInit = NULL;
		m_peslFree = NULL;
		m_peslOpenScanner = NULL;
		m_peslCtrlParameter = NULL;
		m_peslExecute = NULL;
		m_peslCloseScanner = NULL;
		m_peslGetDeviceStatus = NULL;
	}
}

BOOL CEslStub::eslInit(DWORD* pInstance, LPCSTR pszKey, DWORD* pErrCode)
{
	if (NULL == m_peslInit) {
		return FALSE;
	}
	return m_peslInit(pInstance, pszKey, pErrCode);
}

BOOL CEslStub::eslFree(const DWORD Instance, DWORD* pErrCode)
{
	if (NULL == m_peslFree) {
		return FALSE;
	}
	return m_peslFree(Instance, pErrCode);
}

BOOL CEslStub::eslOpenScanner(const DWORD Instance, const ConnectInfo* pScanner, DWORD* pErrCode)
{
	if (NULL == m_peslOpenScanner) {
		return FALSE;
	}
	return m_peslOpenScanner(Instance, pScanner, pErrCode);
}

BOOL CEslStub::eslCtrlParameter(const DWORD Instance, const eParamOpType OpType, EslParamHdr* pParam, DWORD* pErrCode)
{
	if (NULL == m_peslCtrlParameter) {
		return FALSE;
	}
	return m_peslCtrlParameter(Instance, OpType, pParam, pErrCode);
}

BOOL CEslStub::eslExecute(const DWORD Instance, const eExecType ExecType, peslCallback pCbFunc, PVOID pPrivate, DWORD* pErrCode)
{
	if (NULL == m_peslExecute) {
		return FALSE;
	}
	return m_peslExecute(Instance, ExecType, pCbFunc, pPrivate, pErrCode);
}

BOOL CEslStub::eslCloseScanner(const DWORD Instance, DWORD* pErrCode)
{
	if (NULL == m_peslCloseScanner) {
		return FALSE;
	}
	return m_peslCloseScanner(Instance, pErrCode);
}

BOOL CEslStub::eslGetDeviceStatus(const DWORD Instance, DWORD* pStatus, DWORD* pErrCode)
{
	if (NULL == m_peslGetDeviceStatus) {
		return FALSE;
	}
	return m_peslGetDeviceStatus(Instance, pStatus, pErrCode);
}

