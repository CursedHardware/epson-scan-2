#pragma once

#include "../ESCANLIB.h"
#include "ScanMgr.h"

class CInstanceMgr
{
public:
	CInstanceMgr(void);
	~CInstanceMgr(void);
	
	DWORD		createScanMgr(void);
	CScanMgr*	getScanMgr(DWORD Instance);
	BOOL		destroyScanMgr(DWORD Instance);

private:
	CScanMgr*	m_pScanMgr;
};
