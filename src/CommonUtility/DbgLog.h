#pragma once

#ifdef AKBMODE
#define	LOG_FILE_NAME		ES_STRING("ScannerDriver.log")
#else
#define	LOG_FILE_NAME		ES_STRING("EpsonScan2.log")
#endif

#include "utils/CritSec.h"

#include "ESCommonTypedef.h"
#include "utils/ESBuffer.h"
#include "utils/stringutils.h"

#ifndef WIN32
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

/* Returns size of version info in bytes */
//TBD reservation
DWORD
GetFileVersionInfoSize(
        LPCSTR lptstrFilename, /* Filename of version stamped file */
        LPDWORD lpdwHandle       /* Information for use by GetFileVersionInfo */
        );
				/* Read version info into buffer */
BOOL
VerQueryValue(
				ESString& lplpBuffer,
				DWORD puLen
);

void
GetLocalTime(
    		LPSYSTEMTIME lpSystemTime
);

DWORD
GetCurrentThreadId(
    		void
);

DWORD
GetTickCount(
    		void
);
#endif //WIN32

namespace ENUM_LOG_LEVEL{
	enum enumLogLevel{
		LogLevelTrace = 1,
		LogLevelInfo,
		LogLevelNotice,
		LogLevelWarning,
		LogLevelError,

		LogDisable
	};
}
namespace ENUM_IMAGE_TYPE{
	enum enumImageType{
		LogImageRaw = 0,
		LogImageJpg ,
		LogImageTiff,
		LogImagePng,
		LogImagePdf,
		LogImageBmp,
		LogImageUnknown

	};
}

class CDbgLog
{
public:
	CDbgLog(const TCHAR *pszLogFileName = LOG_FILE_NAME);

	virtual ~CDbgLog(void);

	void InitializeInstanceHandle(HINSTANCE hInst)//Moduleのインスタンスハンドルを渡すこと。
	{
		m_hModuleInstanceHandle = hInst;
		BuildLogName();
	}

#if  defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)
	void DisalbeLog();
	void EnableLog();
#endif

	void WriteTwainLog(LPCTSTR pszFormat);

	void MessageLog(ENUM_LOG_LEVEL::enumLogLevel eLevel,LPCSTR pszSenderName,LPCTSTR pszFormat, ...);
	void MessageLog(ENUM_LOG_LEVEL::enumLogLevel eLevel,LPCSTR pszSenderName,LPCTSTR pszFileName,int nLine,LPCTSTR pszFormat, ...);
	void Dump(LPBYTE pMemory, int iMemSize, const ES_CHAR* fileName);

	void Dump(const ESDictionary& dictDump, const ES_CHAR* pwsFileName,bool bAddTimestampPrefilxToFileName = false);

	//InitImageを呼び出し、必要であれば、ヘッダを追加
	void DumpImage(const ES_CHAR* pwsFileNameWithoutExt,ENUM_IMAGE_TYPE::enumImageType eType,int nBitsPerPixel,const ES_CMN_FUNCS::BUFFER::IESBuffer& cBuf);
	void InitImage(const ES_CHAR* pwsFileNameWithoutExt,ENUM_IMAGE_TYPE::enumImageType eType,int nBitsPerPixel,int nWidth,int nHeight);
	void SaveDebugImageData(const ES_CHAR* pwsFileNameWithExt,const ES_CMN_FUNCS::BUFFER::IESBuffer& cBuf,BOOL bAddTimestampPrefix = FALSE);
	void MessageLog2(ENUM_LOG_LEVEL::enumLogLevel eLevel,LPCSTR pszSenderName,LPCTSTR pszFileName,int nLine,LPCTSTR pszFormat,va_list argList);

protected:

	void WriteLog(LPCTSTR pszMessage, BOOL bHeader = FALSE, LPCTSTR pszLogFilePath = nullptr);

	BOOL BuildLogName(const TCHAR *pszLogFileName = LOG_FILE_NAME);

	BOOL IsEnableThreadSafeLog(void);

public:

	BOOL IsEnableLogging(void);

	BOOL IsEanbleDumpJson(void)const;

	BOOL IsEnableDumpImage(void)const;

	BOOL IsEnableDumpCommand(void)const;

	BOOL IsEnableDumpTwainLog(void)const;

	ESString GetLogDirectory();

protected:
	ESString GetCommonAppPath();

	ESString m_csLogName;

	ENUM_LOG_LEVEL::enumLogLevel m_eTargetLogLevel;

	CritSec m_stCriticalSection;

	BOOL m_bUseThreadSafeLog ;

	BOOL m_bDumpJson;

	BOOL m_bDumpImage;

	BOOL m_bModuleIsorationLog;

	ESString m_strSubDir;

	ESString m_strRestLogFiles ;

	BOOL m_bDisableTimeStamp;

	BOOL m_bDumpCommand ;

	BOOL m_bDumpTwainLog;

	HINSTANCE m_hModuleInstanceHandle;

	ESString GetLevelName(ENUM_LOG_LEVEL::enumLogLevel eLevel);

	ESString GetHeader();

	void EnterLock(){
		if(m_bUseThreadSafeLog){
			m_stCriticalSection.Lock();
		}
	}

	void LeaveLock(){
		if(m_bUseThreadSafeLog){
			m_stCriticalSection.Unlock();
		}
	}

	ESString GetModuleVersion(ESString strModulePath);
	ESString GetArchitecture();

};

extern CDbgLog theLog;
CDbgLog *AfxGetLog(void);

// MFCでtypeid(T).name()がメモリリークをご検出するため処理を分ける
#ifndef __AFX_H__

#define ES_IfNotTrueBail(assertion, exceptionLabel, loglevel, sender, message, ... ) \
if(!(assertion)){ \
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), message, ##__VA_ARGS__ ); \
    goto exceptionLabel;\
}

#define ES_IfTrueBail(assertion, exceptionLabel, loglevel, sender, message, ... ) \
if(assertion){ \
	AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), message, ##__VA_ARGS__ ); \
	goto exceptionLabel;\
}

#define ES_Bail( exceptionLabel, loglevel, sender, message, ... ) \
{ \
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), message, ##__VA_ARGS__ );\
    goto exceptionLabel;\
}

#ifdef WIN32
#define ES_IfNilBail(assertion, exceptionLabel, loglevel, sender, nilClass ) \
if(assertion == NULL){\
    ESString className = ES_CMN_FUNCS::STRING::MBStringToUnicode( typeid(nilClass).name());\
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), ES_STRING("%s is nil\r\n") , className.c_str() ); \
    goto exceptionLabel;\
}
#else
#define ES_IfNilBail(assertion, exceptionLabel, loglevel, sender, nilClass ) \
if(assertion == NULL){\
    ESString className = typeid(nilClass).name();\
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), ES_STRING("%s is nil\r\n") , className.c_str() ); \
    goto exceptionLabel;\
}
#endif

#ifdef WIN32
#define ES_IfNilBailWithAction(assertion, exceptionLabel, loglevel, sender,action, nilClass ) \
if(assertion == NULL){\
    ESString className = ES_CMN_FUNCS::STRING::MBStringToUnicode( typeid(nilClass).name());\
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), ES_STRING("%s is nil\r\n") , className.c_str() ); \
    action;\
	goto exceptionLabel;\
}
#else
#define ES_IfNilBailWithAction(assertion, exceptionLabel, loglevel, sender,action, nilClass ) \
if(assertion == NULL){\
    ESString className = typeid(nilClass).name();\
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), ES_STRING("%s is nil\r\n") , className.c_str() ); \
    action;\
	goto exceptionLabel;\
}
#endif

#ifndef __FILET__
#ifdef WIN32
#define __FILET__	__FILEW__
#else
#define __FILET__	__FILE__
#endif
#endif

#define ES_ErrorBail(assertion, exceptionLabel, sender, message, ... ) \
if(!(assertion)){\
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError, typeid(sender).name(), __FILET__, __LINE__, message, ##__VA_ARGS__ );\
	goto exceptionLabel;\
}

#define ES_ErrorBailWithAction(assertion, exceptionLabel, sender, action, message, ... ) \
if(!(assertion)){\
    AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError, typeid(sender).name(), __FILET__, __LINE__, message, ##__VA_ARGS__ );\
    action;\
    goto exceptionLabel;\
}


#define ES_IfNotTruePrintLog(assertion, loglevel, sender, message, ... ) \
if(!(assertion)){\
    AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), message, ##__VA_ARGS__ );\
}

#ifdef WIN32
#define ES_IfNilPrintLog(assertion, loglevel, sender, nilClass ) \
if(!(assertion)){\
	ESString className = ES_CMN_FUNCS::STRING::MBStringToUnicode(typeid(nilClass).name());\
  AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), ES_STRING("%s is nil\r\n") , className.c_str() );\
}
#else
#define ES_IfNilPrintLog(assertion, loglevel, sender, nilClass ) \
if(!(assertion)){\
	ESString className = typeid(nilClass).name();\
  AfxGetLog()->MessageLog(loglevel, typeid(sender).name(), ES_STRING("%s is nil\r\n") , className.c_str() );\
}
#endif


#define ES_ASSERT_TRUE(assertion, sender, message, ...) \
if(!(assertion)) {\
    AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError, typeid(sender).name(), message, ##__VA_ARGS__ );\
    assert(true); \
}


#define ES_Error_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError,typeid(sender).name(),__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Warning_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelWarning,typeid(sender).name(),__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Info_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelInfo,typeid(sender).name(),__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Trace_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelTrace,typeid(sender).name(),__FILET__,__LINE__, message, ##__VA_ARGS__ )


#else // ifndef __AFX_H__

// MFCでtypeid(T).name()がメモリリークをご検出するため処理を分ける
//
// senderの代わりに__FUNCTION__を出力する。
//

#define ES_IfNotTrueBail(assertion, exceptionLabel, loglevel, sender, message, ... ) \
if(!(assertion)){ \
    AfxGetLog()->MessageLog(loglevel, __FUNCTION__, message, ##__VA_ARGS__ ); \
    goto exceptionLabel;\
}

#define ES_IfTrueBail(assertion, exceptionLabel, loglevel, sender, message, ... ) \
if(assertion){ \
	AfxGetLog()->MessageLog(loglevel, __FUNCTION__, message, ##__VA_ARGS__ ); \
	goto exceptionLabel;\
}

#define ES_Bail( exceptionLabel, loglevel, sender, message, ... ) \
{ \
    AfxGetLog()->MessageLog(loglevel, __FUNCTION__, message, ##__VA_ARGS__ );\
    goto exceptionLabel;\
}

#define ES_IfNilBail(assertion, exceptionLabel, loglevel, sender, nilClass ) \
if(assertion == NULL){\
	AfxGetLog()->MessageLog(loglevel, __FUNCTION__, ES_STRING("%s is nil\r\n") , L ## #nilClass ); \
    goto exceptionLabel;\
}

#define ES_IfNilBailWithAction(assertion, exceptionLabel, loglevel, sender,action, nilClass ) \
if(assertion == NULL){\
    AfxGetLog()->MessageLog(loglevel, __FUNCTION__, ES_STRING("%s is nil\r\n") , L ## #nilClass ); \
    action;\
	goto exceptionLabel;\
}

#define ES_ErrorBail(assertion, exceptionLabel, sender, message, ... ) \
if(!(assertion)){\
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError, __FUNCTION__, ES_STRING(__FILE__), __LINE__, message, ##__VA_ARGS__ );\
	goto exceptionLabel;\
}

#define ES_ErrorBailWithAction(assertion, exceptionLabel, sender, action, message, ... ) \
if(!(assertion)){\
    AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError, __FUNCTION__, ES_STRING(__FILE__), __LINE__, message, ##__VA_ARGS__ );\
    action;\
    goto exceptionLabel;\
}


#define ES_IfNotTruePrintLog(assertion, loglevel, sender, message, ... ) \
if(!(assertion)){\
    AfxGetLog()->MessageLog(loglevel, __FUNCTION__, message, ##__VA_ARGS__ );\
}

#define ES_IfNilPrintLog(assertion, loglevel, sender, nilClass ) \
if(!(assertion)){\
    AfxGetLog()->MessageLog(loglevel, __FUNCTION__, ES_STRING("%s is nil\r\n") , L ## #nilClass );\
}


#define ES_ASSERT_TRUE(assertion, sender, message, ...) \
if(!(assertion)) {\
    AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError, __FUNCTION__, message, ##__VA_ARGS__ );\
    assert(true); \
}


#define ES_Error_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError,__FUNCTION__,ES_STRING(__FILE__),__LINE__, message, ##__VA_ARGS__ )

#define ES_Warning_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelWarning,__FUNCTION__,ES_STRING((__FILE__),__LINE__, message, ##__VA_ARGS__ )

#define ES_Info_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelInfo,__FUNCTION__,ES_STRING((__FILE__),__LINE__, message, ##__VA_ARGS__ )

#define ES_Trace_Log(sender, message, ...) \
AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelTrace,__FUNCTION__,ES_STRING((__FILE__),__LINE__, message, ##__VA_ARGS__ )


#endif // ifndef __AFX_H__




#define ES_Error_Log2(senderName, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError,senderName,__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Error_Log3(senderName, lpszFile, nLine, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelError,senderName,lpszFile,nLine, message, ##__VA_ARGS__ )

#define ES_Warning_Log2(senderName, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelWarning,senderName,__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Warning_Log3(senderName, lpszFile, nLine, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelWarning,senderName,lpszFile,nLine, message, ##__VA_ARGS__ )

#define ES_Info_Log2(senderName, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelInfo,senderName,__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Info_Log3(senderName, lpszFile, nLine, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelInfo,senderName,lpszFile,nLine, message, ##__VA_ARGS__ )

#define ES_Trace_Log2(senderName, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelTrace,senderName,__FILET__,__LINE__, message, ##__VA_ARGS__ )

#define ES_Trace_Log3(senderName, lpszFile, nLine, message, ...) \
	AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelTrace,senderName,lpszFile,nLine, message, ##__VA_ARGS__ )

#define ES_Dump_Json(dictDump, pwsFileName) \
	AfxGetLog()->Dump(dictDump, pwsFileName)

#define ES_Dump_JsonWithTimestamp(dictDump, pwsFileName) \
	AfxGetLog()->Dump(dictDump, pwsFileName,true)
