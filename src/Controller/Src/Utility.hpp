#pragma once
#include <../CommonUtility/DbgLog.h>

#define SDI_TRACE_LOG(message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelTrace,__FUNCTION__,__FILE__,__LINE__, message, ##__VA_ARGS__ )


