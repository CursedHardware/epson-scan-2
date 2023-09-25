#include "stdafx.h"
#include "Platform.h"
#include "stdarg.h"

#include "DbgLog.h"
#include "utils/ESJsonUtils.h"
#include <locale.h>

#ifdef WIN32
#include <strsafe.h>
#include <share.h>

//shobjidl.hで出るワーニング対策
#pragma warning (disable: 6387)

#include <Shlobj.h>
#else
#ifdef __ANDROID__
#include <android/log.h>
#endif

#if TARGET_OS_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <sys/syscall.h>
#include <sys/time.h>
#include <boost/range/iterator_range.hpp>
//#include <boost/regex.hpp>

#endif //WIN32

#include "utils/PathUtils.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"


#if TARGET_OS_IPHONE
#if __cplusplus
extern "C" {
#endif
    void NSLog(CFStringRef format, ...);
    void NSLogv(CFStringRef format, va_list args);
#if __cplusplus
}
#endif
#endif

#ifndef WIN32
DWORD //TBD reservation
GetFileVersionInfoSize( LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
	return 5;
}

BOOL //TBD reservation
VerQueryValue(ESString& lplpBuffer, DWORD puLen)
{
  lplpBuffer = "1.0.0";
	return TRUE;
}

void
GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
  struct timeval myTime;
  struct tm *local;

  gettimeofday(&myTime, NULL);

  /* 現在時刻を取得 */	

  local = localtime(&myTime.tv_sec); /* 地方時に変換 */

  lpSystemTime->wYear = local->tm_year + 1900;
  lpSystemTime->wMonth = local->tm_mon + 1;
  lpSystemTime->wDayOfWeek = local->tm_wday;
  lpSystemTime->wDay = local->tm_mday;
  lpSystemTime->wHour = local->tm_hour;
  lpSystemTime->wMinute = local->tm_min;
  lpSystemTime->wSecond = local->tm_sec;
  lpSystemTime->wMilliseconds = myTime.tv_usec / 1000;
}

DWORD
GetCurrentThreadId(void)
{
#if TARGET_OS_IPHONE
    return (DWORD)pthread_mach_thread_np(pthread_self());
#else
	return syscall(SYS_gettid);
#endif
}

DWORD
GetTickCount(void)
{
	struct timeval tv;

	if(gettimeofday(&tv, NULL) != 0)
		return 0;

	return (DWORD)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif //WIN32


ESString FormatMessage( LPCTSTR pszFormat, va_list argList )
{
	if ( pszFormat == NULL ){
		return ES_STRING("");
	}
	try {
		size_t size = esslen(pszFormat) * 2;

#ifndef WIN32
        size = 1024; // Linux環境ではvsnprintfが失敗しないため、ログが途中で切れてしまう問題対応
#endif
		std::vector< TCHAR > buff;
		buff.resize( size );

		while ( 1 ){

#ifdef WIN32
			if ( _vsntprintf_s( buff.data(), size, _TRUNCATE, pszFormat, argList ) != -1 ){
#else
			if ( vsnprintf( buff.data(), size, pszFormat, argList ) != -1 ){
#endif
				break;
			} else {
				size *= 2;
				buff.resize( size );
			}

		}

#ifdef _UNICODE
		return ESString( buff.data() );
#else
#ifdef WIN32
		ESStringA strA( buff.data() );
		return ES_CMN_FUNCS::STRING::MBStringToUnicode(strA);
#else
		return ESString( buff.data() );
#endif // WIN32
#endif // _UNICODE
	} catch(...){
		return ES_STRING("");
	}
}

CDbgLog::CDbgLog(const TCHAR *pszLogFileName)
{

	m_eTargetLogLevel =(ENUM_LOG_LEVEL::enumLogLevel) 0;
	m_bUseThreadSafeLog = FALSE;
	m_bDumpJson = FALSE;
	m_bDumpImage = FALSE;
	m_bModuleIsorationLog = FALSE;
	m_hModuleInstanceHandle = NULL;
	m_bDisableTimeStamp = FALSE;
	m_bDumpCommand = FALSE;
	m_bDumpTwainLog= FALSE;
	BuildLogName((pszLogFileName) ? (pszLogFileName) : LOG_FILE_NAME);
}

CDbgLog::~CDbgLog()
{
	//fprintf (stderr, " CDbgLog::~CDbgLog() \n");
}

#ifdef WIN32
//GetPrivateProfileStringのラッパー関数
ESString GetIniString(ESString csFilePath,ESString csSectionName,ESString csKeyName){


#define INI_READ	1024

	INT nSize = 0;

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cReadBuf;

	ESString strReaded;

	while(1){

		nSize +=INI_READ;

		cReadBuf.AllocBuffer(nSize);

		DWORD dwRet = GetPrivateProfileString( csSectionName.c_str(),
			csKeyName.c_str(),
			ES_STRING(""),
			(LPWSTR)cReadBuf.GetBufferPtr(),
			nSize,
			csFilePath.c_str() );


		if((dwRet == nSize -2)||(dwRet == nSize -1)){
			continue;

		}else{
			strReaded = (LPTSTR)cReadBuf.GetBufferPtr();
			break;
		}

	}
	return strReaded;
}
#endif //WIN32

#if  defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)
void CDbgLog::DisalbeLog()
{
	m_eTargetLogLevel	= ENUM_LOG_LEVEL::LogDisable;
	m_bDumpCommand			= FALSE;
}


void CDbgLog::EnableLog()
{
	m_eTargetLogLevel	= ENUM_LOG_LEVEL::LogLevelTrace;
	m_bDumpCommand			= TRUE;
}
#endif

BOOL CDbgLog::IsEnableLogging(void)
{
#if 1
// レジ検査はせず、ログファイルが所定位置に存在しているかどうかで判断。

#if defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)
	m_bUseThreadSafeLog = FALSE;
	m_bDumpJson				= FALSE;
	m_bDumpImage			= FALSE;
	m_bModuleIsorationLog	= FALSE;
	m_bDisableTimeStamp		= FALSE;
	m_strRestLogFiles		= ES_STRING(".+(.json|.p.?m|.bmp|.jpg|.log)$");

	if (m_eTargetLogLevel == ENUM_LOG_LEVEL::LogDisable)
	{
		return false;
	}
	return true;
#elif WIN32
	ESString strSettingPath = GetCommonAppPath();

	if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strSettingPath) == FALSE){
		return FALSE;
	}


	strSettingPath += ES_STRING("\\LogSetting.ini");

	ESString strSectionName;

	if(m_hModuleInstanceHandle != NULL){
		ESString strModulePath;
		ES_CMN_FUNCS::PATH::ES_GetModuleFileName(m_hModuleInstanceHandle,strModulePath);
		ES_CMN_FUNCS::PATH::ES_GetFileName(strSectionName,strModulePath,FALSE);

	}

	BOOL bSubDir = FALSE;
	if(!strSectionName.empty()){
		m_eTargetLogLevel	= (ENUM_LOG_LEVEL::enumLogLevel) GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("LogLevel"),-1,strSettingPath.c_str());
		m_bUseThreadSafeLog = (ENUM_LOG_LEVEL::enumLogLevel) GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("ThreadSafeMode"),(int)-1,strSettingPath.c_str());
		m_bDumpJson				= GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("DumpJson"),(int)-1,strSettingPath.c_str());
		m_bDumpImage			= GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("DumpImage"),(int)-1,strSettingPath.c_str());
		m_bModuleIsorationLog	= GetPrivateProfileInt(strSectionName.c_str(),_T("ModuleIsoration"),(int)-1,strSettingPath.c_str());
		bSubDir					= GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("ModuleSubDir"),(int)-1,strSettingPath.c_str());
		m_bDisableTimeStamp		= GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("DisableTimestampPrefix"),(int)-1,strSettingPath.c_str());
		m_bDumpCommand			= GetPrivateProfileInt(strSectionName.c_str(),ES_STRING("DumpCommand"),(int)-1,strSettingPath.c_str());
		m_strRestLogFiles		= GetIniString(strSettingPath,strSectionName,ES_STRING("ResetLog"));
	}

	if(strSectionName.empty() || m_eTargetLogLevel == -1){
		m_eTargetLogLevel	= (ENUM_LOG_LEVEL::enumLogLevel) GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("LogLevel"),(int)ENUM_LOG_LEVEL::LogDisable,strSettingPath.c_str());
	}
	if(strSectionName.empty() ||m_bUseThreadSafeLog == -1){
		m_bUseThreadSafeLog = (ENUM_LOG_LEVEL::enumLogLevel) GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("ThreadSafeMode"),(int)FALSE,strSettingPath.c_str());
	}
	if(strSectionName.empty() ||m_bDumpJson == -1){
		m_bDumpJson = GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("DumpJson"),(int)FALSE,strSettingPath.c_str());
	}
	if(strSectionName.empty() ||m_bDumpImage == -1){
		m_bDumpImage= GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("DumpImage"),(int)FALSE,strSettingPath.c_str());
	}
	if(strSectionName.empty() ||m_bModuleIsorationLog == -1){
		m_bModuleIsorationLog	= GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("ModuleIsoration"),(int)FALSE,strSettingPath.c_str());
	}
	if(strSectionName.empty() || bSubDir == -1){
		bSubDir					= GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("ModuleSubDir"),(int)FALSE,strSettingPath.c_str());
	}
	if((bSubDir == 1)&&(m_bModuleIsorationLog == 1)){
		m_strSubDir = strSectionName;
	}
	if(strSectionName.empty() ||m_bDisableTimeStamp == -1){
		m_bDisableTimeStamp		= GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("DisableTimestampPrefix"),(int)FALSE,strSettingPath.c_str());
	}
	if(strSectionName.empty() ||m_bDumpCommand == -1){
		m_bDumpCommand = GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("DumpCommand"),(int)FALSE,strSettingPath.c_str());
	}
	if(strSectionName.empty() ||m_strRestLogFiles.empty() ){
		m_strRestLogFiles = GetIniString(strSettingPath,ES_STRING("Setting"),ES_STRING("ResetLog"));
	}
	m_bDumpTwainLog		= GetPrivateProfileInt(ES_STRING("Setting"),ES_STRING("DumpTwain"),(int)0,strSettingPath.c_str());
#else

	ESString strSectionName;
	ESString strModulePath;
	ES_CMN_FUNCS::PATH::ES_GetModuleFileName(m_hModuleInstanceHandle,strModulePath);
	ES_CMN_FUNCS::PATH::ES_GetFileName(strSectionName,strModulePath,FALSE);

	BOOL bSubDir = FALSE;

	//TBD Fixed value Tentatively
#ifdef AKBMODE
	std::string dir_name = "/tmp/";
    dir_name = dir_name + V_NAME;
	if(ES_CMN_FUNCS::PATH::ES_IsExistFile(dir_name.c_str()))
#else
	if(ES_CMN_FUNCS::PATH::ES_IsExistFile("/tmp/epson"))
#endif
	{
		m_eTargetLogLevel	= ENUM_LOG_LEVEL::LogLevelTrace;
	}else
	{
		m_eTargetLogLevel	= ENUM_LOG_LEVEL::LogDisable;
	}

	m_bUseThreadSafeLog = FALSE;
	m_bDumpJson				= TRUE;
	m_bDumpImage			= TRUE;
	m_bModuleIsorationLog	= TRUE;
	bSubDir					= TRUE;
	m_bDisableTimeStamp		= FALSE;
	m_bDumpCommand			= TRUE;
	m_strRestLogFiles		= ES_STRING("");

	if((bSubDir == TRUE)&&(m_bModuleIsorationLog == TRUE)){
		m_strSubDir = strSectionName;
	}
#endif //WIN32
	return m_eTargetLogLevel != ENUM_LOG_LEVEL::LogDisable;
#else
	//TODO 本番リリース時には別途条件を設けるか要検討
#endif
}

BOOL CDbgLog::IsEnableThreadSafeLog(void){

	//レジストリ等で設定する。
	return m_bUseThreadSafeLog;
}

BOOL CDbgLog::IsEanbleDumpJson(void)const
{
	return m_bDumpJson;
}
BOOL CDbgLog::IsEnableDumpImage(void)const
{
 return m_bDumpImage;
}

BOOL CDbgLog::IsEnableDumpCommand(void)const
{
 return m_bDumpCommand;
}
BOOL CDbgLog::IsEnableDumpTwainLog(void)const
{
 return m_bDumpTwainLog;
}

ESString CDbgLog::GetModuleVersion(ESString strModulePath){
#ifdef WIN32
	#pragma comment(lib, "version.lib")
#endif

	ESString	szVersion;

#ifdef WIN32
	DWORD	dwSize = GetFileVersionInfoSizeW(strModulePath.c_str(), 0);
#else
	DWORD	dwSize = GetFileVersionInfoSize(strModulePath.c_str(), 0);
#endif
	if (dwSize) {
		try {
			TCHAR	*pVersionInfo = new TCHAR[dwSize];

#ifdef WIN32
			if (GetFileVersionInfoW(strModulePath.c_str(), 0, dwSize, pVersionInfo)) {
				void	*pvVersion;
				UINT	VersionLen;
				// ニュートラル固定、ファイルバージョンとします
				if (VerQueryValueW(pVersionInfo, ES_STRING("\\StringFileInfo\\000004b0\\FileVersion"), &pvVersion, &VersionLen)){
					szVersion = pvVersion;
				}
			}
#else
			ESString pvVersion;
			if (VerQueryValue(pvVersion, dwSize)){
				szVersion = pvVersion;
			}
#endif
			delete[] pVersionInfo;
		}
		catch (std::bad_alloc) {
		}
	}
	return szVersion;
}

ESString CDbgLog::GetArchitecture(){

#ifdef WIN32

#if _WIN64
	return ES_STRING("x64");
#else	// _WIN64

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE hProcess, PBOOL pbWow64Process);
	HMODULE hMod = ::GetModuleHandle(ES_STRING("kernel32"));
	if(hMod){
		LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(hMod, "IsWow64Process");

		BOOL bIsWow64 = FALSE;

		if (fnIsWow64Process != NULL) {
			if (fnIsWow64Process(::GetCurrentProcess(), &bIsWow64) == FALSE) {
				// handle error
			}
		}
		if(bIsWow64){
			return ES_STRING("x64");
		}else{
			return ES_STRING("x86");
		}
	}else{
		return ES_STRING("x86");
	}
#endif	// _WIN64

#else

//TBD reservation
	int bIsWow64 = true;

	if(bIsWow64){
		return ES_STRING("x64");
	}else{
		return ES_STRING("x86");
	}
#endif	// WIN32


	}

#if defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)

BOOL CDbgLog::BuildLogName(const TCHAR *pszLogFileName)
{
	m_csLogName = "mobile";
	IsEnableLogging();
	return true;
}

#else
 BOOL CDbgLog::BuildLogName(const TCHAR *pszLogFileName)
{

	m_csLogName.clear();

	if (IsEnableLogging() == TRUE) {

		ESString csLogDirectory = GetCommonAppPath();

		ESString strName;
		ESString strModulePath;
#ifdef WIN32
		if(m_hModuleInstanceHandle){
			ES_CMN_FUNCS::PATH::ES_GetModuleFileName(m_hModuleInstanceHandle,strModulePath);
			ES_CMN_FUNCS::PATH::ES_GetFileName(strName,strModulePath,FALSE);
		}
#else
		ES_CMN_FUNCS::PATH::ES_GetModuleFileName(m_hModuleInstanceHandle,strModulePath);
		ES_CMN_FUNCS::PATH::ES_GetFileName(strName,strModulePath,FALSE);
#endif

		if (csLogDirectory.length() != 0) {
			m_csLogName = csLogDirectory;
			ES_CMN_FUNCS::PATH::ES_AddBackYen(m_csLogName);

#ifdef WIN32
			if(m_bModuleIsorationLog&&m_hModuleInstanceHandle){
#else
			if(m_bModuleIsorationLog){
#endif
				if(!m_strSubDir.empty()){
					if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(m_csLogName)){
						ES_CMN_FUNCS::PATH::ES_CombinePath(m_csLogName, m_csLogName, m_strSubDir + SEPARATOR);
						ES_CMN_FUNCS::PATH::ES_MakeFolder(m_csLogName);
					}else {
						return false;
					}
				}
#ifndef WIN32
				ES_CMN_FUNCS::PATH::ES_AddBackYen(m_csLogName);
#endif
				strName +=  ES_STRING(".log");
				ES_CMN_FUNCS::PATH::ES_CombinePath(m_csLogName, m_csLogName, strName);
			}else{
#ifndef WIN32
				ES_CMN_FUNCS::PATH::ES_AddBackYen(m_csLogName);
#endif
				ES_CMN_FUNCS::PATH::ES_CombinePath(m_csLogName, m_csLogName, pszLogFileName);
			}

			if(!m_strRestLogFiles.empty()){
#ifdef WIN32
				if(m_hModuleInstanceHandle){
#else
				if(m_bModuleIsorationLog){ //TBD
#endif
					std::transform(strName.begin(), strName.end(), strName.begin(), tolower);

					if((m_bModuleIsorationLog && !m_strSubDir.empty())
						||(strName == ES_STRING("es2controller")))
					{

						ESString strDir;

						if(ES_CMN_FUNCS::PATH::ES_GetFolderPath(strDir,m_csLogName+m_strSubDir)){
#ifdef WIN32
							ESStringArray arFileTypes;
							boost::algorithm::split(arFileTypes,m_strRestLogFiles,boost::is_any_of(ES_STRING(",")) );
							for(INT nExt = 0;nExt < arFileTypes.size();nExt++){

								ESStringArray straAllFiles = ES_CMN_FUNCS::PATH::EnumFilesInDir(arFileTypes[nExt],strDir);

								for(int n=0;n<straAllFiles.size();n++){
									::DeleteFile(straAllFiles[n].c_str());
								}
							}
#else
							ESStringArray straAllFiles = ES_CMN_FUNCS::PATH::EnumFilesInDir(m_strRestLogFiles,strDir);

							for(int n=0;n<straAllFiles.size();n++){
								DeleteFile(straAllFiles[n].c_str());
							}
#endif
						}

#ifdef WIN32
						OSVERSIONINFOEX osv;
						memset(&osv, 0, sizeof(osv));
						osv.dwOSVersionInfoSize = sizeof(osv);
						GetVersionEx((LPOSVERSIONINFO)&osv);
						ES_Error_Log(this,ES_STRING("Environment OSVER:%d.%d %s Build:%d SP:%d.%d"),osv.dwMajorVersion,osv.dwMinorVersion,GetArchitecture().c_str(),osv.dwBuildNumber,osv.wServicePackMajor,osv.wServicePackMinor);
#endif
						}
				}
			}

		}

		if(!strModulePath.empty()){
			ES_Error_Log(this,ES_STRING("LoadModule Path:[%s] Ver:[%s]"),strModulePath.c_str(),GetModuleVersion(strModulePath).c_str());
		}
	}
	return ((m_csLogName.length() != 0) ? TRUE : FALSE);
}

#endif

ESString CDbgLog::GetLogDirectory()
{
	ESString strBasePath = GetCommonAppPath() + SEPARATOR;

 if(IsEnableLogging()){

	 if(m_bModuleIsorationLog&&!m_strSubDir.empty()){
		 if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strBasePath)){
			ES_CMN_FUNCS::PATH::ES_CombinePath(strBasePath, strBasePath, m_strSubDir + SEPARATOR);
			ES_CMN_FUNCS::PATH::ES_MakeFolder(strBasePath);
		 }
	 }
	}

 return strBasePath;

}

ESString CDbgLog::GetCommonAppPath(void)
{
#ifdef WIN32

#ifndef CSIDL_COMMON_APPDATA
#define	CSIDL_COMMON_APPDATA		0x0023
#endif	// CSIDL_COMMON_APPDATA

	ESString strDirectory;

	size_t nBaseLenI = MAX_PATH + 1;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;
	cBuf.AllocBuffer(nBaseLenI* sizeof(TCHAR));
	if (cBuf.GetBufferPtr() != NULL) {
		size_t nBaseLen = 0L;

		int nFolderID = CSIDL_COMMON_APPDATA;
		if (::SHGetSpecialFolderPath(NULL,(LPWSTR) cBuf.GetBufferPtr(), nFolderID, TRUE) == TRUE) {
			HRESULT hr = StringCchLength((LPWSTR)cBuf.GetBufferPtr(), nBaseLenI, &nBaseLen);
			if ((hr != S_OK) || (nBaseLen == 0)) {
				cBuf.FreeBuffer();
			}
		}

		if (cBuf.GetBufferPtr() != 0) {
			strDirectory = (LPWSTR)cBuf.GetBufferPtr();

			cBuf.FreeBuffer();

		}
	}

	if (strDirectory.length() != 0) {
		strDirectory += ES_STRING("\\EPSON\\EPSONSCAN2");
	}
#else
	ESString strDirectory;
	//TBD reservation
#ifdef AKBMODE
	std::string dir_name = "/tmp/";
    dir_name = dir_name + V_NAME;
	strDirectory = ES_STRING(dir_name.c_str());
#else
	strDirectory = ES_STRING("/tmp/epson");
#endif
#endif

	return strDirectory;

}

void CDbgLog::WriteTwainLog(LPCTSTR pszFormat){
	if((m_csLogName.length() == 0) || (IsEnableDumpTwainLog() == FALSE)){
		return;
	}

	WriteLog(pszFormat);
}

void CDbgLog::MessageLog(ENUM_LOG_LEVEL::enumLogLevel eLevel,LPCSTR pszSenderName,LPCTSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);

	MessageLog2( eLevel, pszSenderName, NULL, 0, pszFormat, argList);

	va_end(argList);
}

void CDbgLog::MessageLog(ENUM_LOG_LEVEL::enumLogLevel eLevel,LPCSTR pszSenderName,LPCTSTR pszFileName,int nLine,LPCTSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);

	MessageLog2( eLevel, pszSenderName, pszFileName, nLine, pszFormat, argList);

	va_end(argList);
}

void CDbgLog::MessageLog2(ENUM_LOG_LEVEL::enumLogLevel eLevel,LPCSTR pszSenderName,LPCTSTR pszFileName,int nLine,LPCTSTR pszFormat,va_list argList)
{
	if((eLevel < m_eTargetLogLevel)||(m_csLogName.length() == 0)){
		return;
	}
	EnterLock();

	ESString strType	= GetLevelName(eLevel);
	ESString strTimeHdr = GetHeader();
#ifdef WIN32
	ESString strSender = ES_CMN_FUNCS::STRING::MBStringToUnicode(pszSenderName);
#else
	ESString strSender = pszSenderName;
#endif

	ESString strHeader;
	if ( pszFileName == NULL ){
		strHeader = (esformat( ES_STRING("%s[---------------------------------------------;----][%-45s]%s ") ) % strTimeHdr % strSender % strType ).str();
	} else {
		ESString strPath	= pszFileName;
		ESString strFileName;
		ES_CMN_FUNCS::PATH::ES_GetFileName(strFileName,strPath,TRUE);
		strHeader = (esformat( ES_STRING("%s[%-45s;%4d][%-45s]%s ")) % strTimeHdr % strFileName % nLine % strSender % strType ).str();
	}

	ESString strMessage = FormatMessage( pszFormat, argList);
	BOOL bNeedCrLf = (strMessage.rfind(ES_STRING("\r\n")) != strMessage.length() -2);
	if ( bNeedCrLf ){
		strMessage += ES_STRING("\n");
	}

#ifdef _DEBUG
	::OutputDebugString( strHeader.c_str()  );
	::OutputDebugString( strMessage.c_str() );
#endif
	if ( (eLevel >= ENUM_LOG_LEVEL::LogLevelTrace) && (m_csLogName.length() != 0) ){


#if defined(__ANDROID__)
		__android_log_print(ANDROID_LOG_INFO,"EScan2", ES_STRING("%s"), strHeader.c_str());
		__android_log_print(ANDROID_LOG_INFO,"EScan2", ES_STRING("%s"), strMessage.c_str());
#elif TARGET_OS_IPHONE
        
        CFStringRef header = CFStringCreateWithCString(kCFAllocatorDefault, strHeader.c_str(), kCFStringEncodingUTF8);
        NSLog(header);
        CFRelease(header);
        
        CFStringRef message = CFStringCreateWithCString(kCFAllocatorDefault, strHeader.c_str(), kCFStringEncodingUTF8);
        NSLog(message);
        CFRelease(message);

#elif defined(TARGET_OS_EMBEDDED)

        fprintf( stderr,ES_STRING("%s"), strHeader.c_str());
		fprintf( stderr,ES_STRING("%s"), strMessage.c_str());
#else

#ifdef WIN32
		FILE *fp = _tfsopen( m_csLogName.c_str(), ES_STRING("a+,ccs=UTF-8"),_SH_DENYWR);
#else
		FILE *fp = fopen( m_csLogName.c_str(), ES_STRING("a+"));
#endif

		if (fp != NULL) {
#ifdef WIN32
			_ftprintf(fp, strHeader.c_str()  );
			_ftprintf(fp, strMessage.c_str() );
#else
			fprintf(fp, ES_STRING("%s"), strHeader.c_str()  );
			fprintf(fp, ES_STRING("%s"), strMessage.c_str() );

			fprintf(stderr, ES_STRING("%s"), strHeader.c_str()  );
			fprintf(stderr, ES_STRING("%s"), strMessage.c_str() );
#endif
			fclose(fp);
		}
#endif
	}
	LeaveLock();
}


void CDbgLog::WriteLog(LPCTSTR pszMessage, BOOL bHeader, LPCTSTR pszLogFilePath)
{
	EnterLock();

	ESString logPath;
	if (pszLogFilePath == nullptr)
	{
		logPath = m_csLogName;
	}else{
		logPath = pszLogFilePath;
	}

#if defined(__ANDROID__)

	__android_log_print(ANDROID_LOG_INFO,"EScan2", ES_STRING("%08ld: "), (unsigned long)GetTickCount());
	__android_log_print(ANDROID_LOG_INFO,"EScan2", ES_STRING("%s"), pszMessage);

#elif TARGET_OS_IPHONE
    CFStringRef header = CFStringCreateWithCString(kCFAllocatorDefault, ES_STRING("%08ld: "), kCFStringEncodingUTF8);
    NSLog(header);
    CFRelease(header);
    
    CFStringRef message = CFStringCreateWithCString(kCFAllocatorDefault, pszMessage, kCFStringEncodingUTF8);
    NSLog(message);
    CFRelease(message);

#elif defined(TARGET_OS_EMBEDDED)

    fprintf( stderr,ES_STRING("%s"), "%08ld: ");
    fprintf( stderr,ES_STRING("%s"), pszMessage);
#else
	if (logPath.length() != 0) {
#ifdef WIN32
		FILE *fp = _tfsopen( m_csLogName.c_str(), ES_STRING("a+,ccs=UTF-8"),_SH_DENYWR);
#else
		FILE *fp = fopen( logPath.c_str(), ES_STRING("a+"));
#endif

		if (fp != NULL) {
			if (bHeader == TRUE) {
#ifdef WIN32
				_ftprintf(fp, ES_STRING("%08ld: "), ::GetTickCount());
			}
			_ftprintf(fp, pszMessage);
#else
				fprintf(fp, ES_STRING("%08ld: "), (unsigned long)GetTickCount());
			}
			fprintf(fp, ES_STRING("%s"), pszMessage);
			fprintf(stderr, ES_STRING("%s"), pszMessage);
#endif
			fclose(fp);
		}
	}

#endif
	LeaveLock();
}

void CDbgLog::Dump(LPBYTE pMemory, int iMemSize, const ES_CHAR* fileName)
{

	if( !this->IsEanbleDumpJson())
	{
		return;
	}

	ESString strBasePath = GetCommonAppPath() + SEPARATOR;
	if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strBasePath) == FALSE){
		return;
	}
	if (fileName == nullptr) {
		return ;
	}
	if (pMemory == NULL) {
		return ;
	}
	if (iMemSize < 0) {
		return ;
	}
	EnterLock();

	ESString dumpPath = strBasePath + fileName;

	//ES_CMN_FUNCS::BUFFER::CESHeapBuffer cMem;

	size_t nLineXSize = MAX_PATH * sizeof(TCHAR);
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cpszLineX;
	cpszLineX.AllocBuffer((UInt32)nLineXSize);

	size_t nLineASize = MAX_PATH * sizeof(TCHAR);

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cpszLineA;
	cpszLineA.AllocBuffer((UInt32)nLineASize);

	size_t nLineBSize = MAX_PATH * sizeof(TCHAR);

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cpszLineB;
	cpszLineB.AllocBuffer((UInt32)nLineBSize);

	size_t nMemSize = 16 * sizeof(TCHAR);
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cpszMem;
	cpszMem.AllocBuffer((UInt32)nMemSize);

	size_t nDumpSize = (MAX_PATH * 3) * sizeof(TCHAR);
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cpszDump;
	cpszDump.AllocBuffer((UInt32)nDumpSize);

	if ((cpszLineX.GetBufferPtr() != NULL) && (cpszLineA.GetBufferPtr() != NULL) && (cpszLineB.GetBufferPtr() != NULL) && (cpszMem.GetBufferPtr() != NULL) && (cpszDump.GetBufferPtr() != NULL)) {

		#define	DUMP_HEADER		ES_STRING("Addr     : 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F : 0123456789ABCDEF\n")
		#define	DUMP_HEADER2	ES_STRING("---------+-------------------------------------------------+-----------------\n")
		WriteLog(ES_STRING("\n"), FALSE, dumpPath.c_str());
		WriteLog(DUMP_HEADER, FALSE, dumpPath.c_str());
		WriteLog(DUMP_HEADER2, FALSE, dumpPath.c_str());

		HRESULT hr = S_OK;
		errno_t err = 0;
		int iCount = 0;
		for (int nn = 0; nn < iMemSize; nn += iCount) {
			memset(cpszLineX.GetBufferPtr(), 0, nLineXSize);
#ifdef WIN32
			hr = StringCchPrintf((LPTSTR)cpszLineX.GetBufferPtr(), nLineXSize/sizeof(TCHAR), ES_STRING("%08X : "), nn);
#else
			hr = snprintf((LPTSTR)cpszLineX.GetBufferPtr(), nLineXSize/sizeof(TCHAR), ES_STRING("%08X : "), nn);
#endif
			WriteLog((LPTSTR)cpszLineX.GetBufferPtr(), FALSE, dumpPath.c_str());

			memset(cpszLineA.GetBufferPtr(), 0, nLineASize);
			memset(cpszLineB.GetBufferPtr(), 0, nLineBSize);

			for (iCount = 0; iCount < 16; iCount ++) {
				if ((nn + iCount) >= iMemSize) {
					// 表示桁合わせをする。
					for (int ex = iCount; ex < 16; ex ++) {
						err = _tcscat_s((LPTSTR)cpszLineA.GetBufferPtr(), nLineASize/sizeof(TCHAR), ES_STRING("   "));
						if (err != 0) {
							break;
						}
					}
					break;
				}

				// 数値ダンプ
				memset(cpszMem.GetBufferPtr(), 0, nMemSize);
#ifdef WIN32
				hr = StringCchPrintf((LPTSTR)cpszMem.GetBufferPtr(), nMemSize/sizeof(TCHAR), ES_STRING("%02X "), (BYTE)(pMemory[nn+iCount]));
#else
				hr = snprintf((LPTSTR)cpszMem.GetBufferPtr(), nMemSize/sizeof(TCHAR), ES_STRING("%02X "), (BYTE)(pMemory[nn+iCount]));
#endif
				err = _tcscat_s((LPTSTR)cpszLineA.GetBufferPtr(), nLineASize/sizeof(TCHAR), (LPTSTR)cpszMem.GetBufferPtr());
				if (err != 0) {
					break;
				}

				// キャラクタダンプ
				memset(cpszMem.GetBufferPtr(), 0, nMemSize);
				if ((0x20 <= pMemory[nn+iCount]) && (pMemory[nn+iCount] <= 0x7e)) {
#ifdef WIN32
					hr = StringCchPrintf((LPTSTR)cpszMem.GetBufferPtr(), nMemSize/sizeof(TCHAR), ES_STRING("%c"), pMemory[nn+iCount]);
				} else {
					hr = StringCchPrintf((LPTSTR)cpszMem.GetBufferPtr(), nMemSize/sizeof(TCHAR), ES_STRING("."));
				}
#else
					hr = snprintf((LPTSTR)cpszMem.GetBufferPtr(), nMemSize/sizeof(TCHAR), ES_STRING("%c"), pMemory[nn+iCount]);
				} else {
					hr = snprintf((LPTSTR)cpszMem.GetBufferPtr(), nMemSize/sizeof(TCHAR), ES_STRING("."));
				}
#endif
				err = _tcscat_s((LPTSTR)cpszLineB.GetBufferPtr(), nLineBSize/sizeof(TCHAR), (LPTSTR)cpszMem.GetBufferPtr());
				if (err != 0) {
					break;
				}
			}

			memset(cpszDump.GetBufferPtr(), 0, nDumpSize);
			err = _tcscat_s((LPTSTR)cpszDump.GetBufferPtr(), nDumpSize/sizeof(TCHAR),(LPTSTR) cpszLineA.GetBufferPtr());
			if (err != 0) {
				break;
			}
			err = _tcscat_s((LPTSTR)cpszDump.GetBufferPtr(), nDumpSize/sizeof(TCHAR), ES_STRING(": "));
			if (err != 0) {
				break;
			}
			err = _tcscat_s((LPTSTR)cpszDump.GetBufferPtr(), nDumpSize/sizeof(TCHAR),(LPTSTR) cpszLineB.GetBufferPtr());
			if (err != 0) {
				break;
			}
			err = _tcscat_s((LPTSTR)cpszDump.GetBufferPtr(), nDumpSize/sizeof(TCHAR), ES_STRING("\n"));
			if (err != 0) {
				break;
			}
			WriteLog((LPTSTR)cpszDump.GetBufferPtr(), FALSE, dumpPath.c_str());
		}
		WriteLog(ES_STRING("\n"), FALSE, dumpPath.c_str());
	}
	LeaveLock();
}

ESString GetTimestampPrefix(){
		SYSTEMTIME st;
		memset(&st, 0, sizeof(st));
		::GetLocalTime(&st);
		ESString strTime;
		strTime = (esformat(ES_STRING("[%04d%02d%02d_%02d%02d%02d_%03d]")) % st.wYear % st.wMonth % st.wDay % st.wHour % st.wMinute % st.wSecond % st.wMilliseconds ).str();

		return strTime;
}

void CDbgLog::Dump(const ESDictionary& dictDump, const ES_CHAR* pwsFileName,bool bAddTimeStamp)
{
	if( !this->IsEanbleDumpJson())
	{
		return;
	}

	if(!pwsFileName)
	{
		return;
	}
	EnterLock();
	ESString strFileName;

	if(bAddTimeStamp && !m_bDisableTimeStamp){
		strFileName = GetTimestampPrefix();
	}

	strFileName += ESString(pwsFileName) ;

	if(strFileName.find(ES_STRING(".json")) == -1){
		strFileName += ESString(ES_STRING(".json"));
	}

	ESString strFilePath;
	ESString strBasePath = GetCommonAppPath() + SEPARATOR;

	if(m_bModuleIsorationLog&&!m_strSubDir.empty()){
		if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strBasePath)){
			ES_CMN_FUNCS::PATH::ES_CombinePath(strBasePath, strBasePath, m_strSubDir + SEPARATOR);
			ES_CMN_FUNCS::PATH::ES_MakeFolder(strBasePath);
		}else {
			LeaveLock();
			return;
		}
	}

	ES_CMN_FUNCS::PATH::ES_CombinePath(strFilePath, strBasePath, strFileName);

	ES_CMN_FUNCS::JSON::DictionaryToJSONFile(dictDump, strFilePath);
	LeaveLock();
}


ESString GetFileNameWithBitsPerPixel(const ES_CHAR* pwsFileNameWithoutExt,ENUM_IMAGE_TYPE::enumImageType eType,int nBitsPerPixel){
	ESString strFileName;

	if(eType == ENUM_IMAGE_TYPE::LogImageJpg){
		strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".jpg"));
	}	if(eType == ENUM_IMAGE_TYPE::LogImageBmp){
		strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".bmp"));
	}	if(eType == ENUM_IMAGE_TYPE::LogImagePdf){
		strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".pdf"));
	}	if(eType == ENUM_IMAGE_TYPE::LogImagePng){
		strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".png"));
	}	if(eType == ENUM_IMAGE_TYPE::LogImageTiff){
		strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".tiff"));
	}	if(eType == ENUM_IMAGE_TYPE::LogImageUnknown){
		strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".bin"));
	}else if(eType == ENUM_IMAGE_TYPE::LogImageRaw){
		switch(nBitsPerPixel){
		case 1:	//MONO
			strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".pbm"));
			break;
		case 8: //Gray
		case 16:
			strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".pgm"));
			break;
		case 24://Color
		case 48:
			strFileName = pwsFileNameWithoutExt + ESString(ES_STRING(".ppm"));
			break;
		}

	}
	return strFileName;
}

ESStringA GetImageFileHeader(int nBitsPerSample,int nWidth,int nHeight){

	ESStringA strHeader;

	switch(nBitsPerSample){
	case 1:	//MONO
		strHeader = "P4\n";
		break;
	case 8: //Gray
	case 16:
		strHeader = "P5\n";
		break;
	case 24://Color
	case 48:
		strHeader = "P6\n";
		break;
	}

	strHeader += (boost::format("%d %d\n") % nWidth % nHeight).str();

	if (nBitsPerSample == 48 || nBitsPerSample == 16)
	{
		strHeader += "65535\n";
	}
	else if(nBitsPerSample != 1){
		strHeader += "255\n";
	}
	return strHeader;
}
void CDbgLog:: DumpImage(const ES_CHAR* pwsFileNameWituoutExt,ENUM_IMAGE_TYPE::enumImageType eType,int nBitsPerPixel,const ES_CMN_FUNCS::BUFFER::IESBuffer& cBuf){

	if(IsEnableDumpImage() == FALSE){
		return;
	}

	ESString strFileName = GetFileNameWithBitsPerPixel(pwsFileNameWituoutExt,eType,nBitsPerPixel);
	ESString strBasePath = GetCommonAppPath() + SEPARATOR;

	if(m_bModuleIsorationLog&&m_hModuleInstanceHandle&&!m_strSubDir.empty()){
		if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strBasePath)){
			ES_CMN_FUNCS::PATH::ES_CombinePath(strBasePath, strBasePath, m_strSubDir + SEPARATOR);
			ES_CMN_FUNCS::PATH::ES_MakeFolder(strBasePath);
		}else {
			return;
		}
	}

	ESString strPath = strBasePath + strFileName;

	FILE *fp=NULL;

#ifdef WIN32
	fp = _tfsopen( strPath.c_str(), ES_STRING("ab+"),_SH_DENYWR);
#else
	fp = fopen( strPath.c_str(), ES_STRING("ab+"));
#endif

	if(fp){


		if(nBitsPerPixel == 1){

			const UInt8* pbySrc = cBuf.GetConstBufferPtr();

			for(UInt32 n = 0;n<cBuf.GetLength();n++){

				//モノクロは反転してDumpする。pbmフォーマットに準拠するため
				UInt8 un8 = ~pbySrc[n];

				fwrite(&un8,1,1,fp);
			}


		}else{
			fwrite(cBuf.GetConstBufferPtr(),sizeof(BYTE),cBuf.GetLength(),fp);
		}
		fclose(fp);
	}

}
void CDbgLog::InitImage(const ES_CHAR* pwsFileNameWithoutExt,ENUM_IMAGE_TYPE::enumImageType eType,int nBitsPerSample,int nWidth,int nHeight){

	if(IsEnableDumpImage() == FALSE){
		return;
	}

	ESString strFileName = GetFileNameWithBitsPerPixel(pwsFileNameWithoutExt,eType,nBitsPerSample);
	ESString strBasePath = GetCommonAppPath() + SEPARATOR;

	if(m_bModuleIsorationLog&&m_hModuleInstanceHandle&&!m_strSubDir.empty()){
		if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strBasePath)){
			ES_CMN_FUNCS::PATH::ES_CombinePath(strBasePath, strBasePath, m_strSubDir + SEPARATOR);
			ES_CMN_FUNCS::PATH::ES_MakeFolder(strBasePath);
		}else {
			return;
		}
	}

	ESString strPath;// = strBasePath + strFileName;
	ES_CMN_FUNCS::PATH::ES_CombinePath(strPath, strBasePath, strFileName);

	FILE *fp=NULL;

#ifdef WIN32
	fp = _tfsopen( strPath.c_str(), ES_STRING("wb"),_SH_DENYWR);
#else
	fp = fopen( strPath.c_str(), ES_STRING("wb"));
#endif

	if(fp){

		if (eType == ENUM_IMAGE_TYPE::LogImageRaw){

			ESStringA strHeader = GetImageFileHeader(nBitsPerSample,nWidth,nHeight);
			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;
			cBuf.CopyBuffer((const UInt8*)strHeader.c_str(), (UInt32)strHeader.length());
			fwrite(cBuf.GetConstBufferPtr(),1,cBuf.GetLength(),fp);
		}

		fclose(fp);
	}


}

void CDbgLog::SaveDebugImageData(const ES_CHAR* pwsFileNameWithExt,const ES_CMN_FUNCS::BUFFER::IESBuffer& cBuf,BOOL bAddTimestampPrefix){
	if(IsEnableDumpImage() == FALSE){
		return;
	}
	ESString strBasePath = GetCommonAppPath() + SEPARATOR;

	if(!m_bDisableTimeStamp && bAddTimestampPrefix){
		strBasePath += GetTimestampPrefix();
	}

	if(m_bModuleIsorationLog&&m_hModuleInstanceHandle&&!m_strSubDir.empty()){
		if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(strBasePath)){
			ES_CMN_FUNCS::PATH::ES_CombinePath(strBasePath, strBasePath, m_strSubDir + SEPARATOR);
			ES_CMN_FUNCS::PATH::ES_MakeFolder(strBasePath);
		}else {
			return;
		}
	}

	ESString strPath = strBasePath + pwsFileNameWithExt;

	FILE *fp=NULL;

#ifdef WIN32
	fp = _tfsopen( strPath.c_str(), ES_STRING("wb"),_SH_DENYWR);
#else
	fp = fopen( strPath.c_str(), ES_STRING("wb"));
#endif

	if(fp){
		fwrite(cBuf.GetConstBufferPtr(),1,cBuf.GetLength(),fp);

		fclose(fp);
	}
}

// 唯一のログフレーム
CDbgLog theLog;

CDbgLog *AfxGetLog(void)
{
	return &theLog;
}

ESString CDbgLog::GetLevelName(ENUM_LOG_LEVEL::enumLogLevel eLevel){

		ESString strType;

		switch(eLevel){
		case ENUM_LOG_LEVEL::LogLevelError:
			strType=ES_STRING("[-ERROR-]");
			break;
		case ENUM_LOG_LEVEL::LogLevelWarning:
			strType=ES_STRING("[WARNING]");
			break;
		case ENUM_LOG_LEVEL::LogLevelNotice:
			strType=ES_STRING("[NOTICE-]");
			break;
		case ENUM_LOG_LEVEL::LogLevelInfo:
			strType=ES_STRING("[-INFO.-]");
			break;
		case ENUM_LOG_LEVEL::LogLevelTrace:
			strType=ES_STRING("[-TRACE-]");
			break;
        default:
            strType = ES_STRING("");
            break;
		}
		ESString strThreadId;
		strThreadId = (esformat(ES_STRING("[%08X]")) %  GetCurrentThreadId()).str();

		return strThreadId + strType;
	}
ESString CDbgLog::GetHeader(){
		SYSTEMTIME st;
		memset(&st, 0, sizeof(st));
		::GetLocalTime(&st);
		ESString strTime;
		ESString strName;
#ifdef WIN32
		if(m_hModuleInstanceHandle){
			ESString strModulePath;
			ES_CMN_FUNCS::PATH::ES_GetModuleFileName(m_hModuleInstanceHandle,strModulePath);
			ES_CMN_FUNCS::PATH::ES_GetFileName(strName,strModulePath,FALSE);
		}else{
			strName = ES_STRING("----------------");
		}
#else
		ESString strModulePath;
		ES_CMN_FUNCS::PATH::ES_GetModuleFileName(m_hModuleInstanceHandle,strModulePath);
		ES_CMN_FUNCS::PATH::ES_GetFileName(strName,strModulePath,FALSE);
#endif
		strTime = (esformat(ES_STRING("[%04d/%02d/%02d %02d:%02d:%02d.%03d][%-16s]")) % st.wYear % st.wMonth % st.wDay % st.wHour % st.wMinute % st.wSecond % st.wMilliseconds % strName.c_str()).str();
		return strTime;
	}
