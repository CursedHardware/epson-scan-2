#include "Environment.h"
#include <memory>

namespace epsonscan2 {
namespace es2command {


bool Environment::IsCommandRetry()
{
    return true;
}

bool Environment::IsAllowStatusAuthError()
{
    bool bRet = false;
#ifdef WIN32
    const DWORD ALLOW_AUTH_CODE = 1;

    DWORD dwAllowAuthError(0);
    ESErrorCode err = RegQueryDWORDValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\EPSON\\Epson Scan 2", L"AllowAuthError", &dwAllowAuthError);
    if ( IS_ERR_SUCCESS(err) && dwAllowAuthError == ALLOW_AUTH_CODE){
        bRet = true;
    }
#endif
    return bRet;
}


} // namespace es2command
} // namespace epsonscan2