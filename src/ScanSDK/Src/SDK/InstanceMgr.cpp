#include "InstanceMgr.h"

CInstanceMgr::CInstanceMgr(void) :
m_pScanMgr(NULL)
{
}

CInstanceMgr::~CInstanceMgr(void)
{
	destroyScanMgr(1);
}

DWORD CInstanceMgr::createScanMgr(void)
{
	if (m_pScanMgr) {
		return 1;
	}
	m_pScanMgr = new CScanMgr();
	if (m_pScanMgr) {
		return 1;
	}

	return 0;
}

CScanMgr* CInstanceMgr::getScanMgr(DWORD Instance)
{
	return m_pScanMgr;
}

BOOL CInstanceMgr::destroyScanMgr(DWORD Instance)
{
	if (Instance != 1) {
		return FALSE;
	}

	if (m_pScanMgr) {
		delete m_pScanMgr;
		m_pScanMgr = NULL;
	}

	return TRUE;
}


