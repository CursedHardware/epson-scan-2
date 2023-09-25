#include "LibHelper.h"

void SetErrorCode(DWORD* pErrCode, DWORD ErrCode)
{
	if (pErrCode) {
		*pErrCode = ErrCode;
	}
}
