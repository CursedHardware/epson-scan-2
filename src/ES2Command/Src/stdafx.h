#pragma once

#include <sys/socket.h> 

#include <assert.h>
#include "Platform.h"
#include "CommonUtility/ESCommonTypedef.h"
#include "Include/ES2Command/ESCommandProperties.h"
#include "CommonUtility/utils/ESAnyCastUtils.h"
#include "Utils/ESDebugLog.h"

inline bool IS_ERR_SUCCESS( ESErrorCode err )
{
	return ( err == kESErrorNoError );
}
inline bool IS_ERR_CODE( ESErrorCode err )
{
	return ( err != kESErrorNoError );
}
inline bool IsEnableDumpCommand()
{
	return ( AfxGetLog()->IsEnableDumpCommand() ? true : false );
}

#ifndef WIN32

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __inout
#define __inout
#endif

#ifndef __in_opt
#define __in_opt
#endif

#ifndef __out_opt
#define __out_opt
#endif

#endif
