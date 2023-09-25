#include "stdafx.h"
#include "ESFile.h"

#include "PathUtils.h"

//=========================================================================
//!
//! コンストラクタ
//!
//=========================================================================
CESFile::CESFile(void)
{
#ifdef WIN32
	m_hFileHandle = INVALID_HANDLE_VALUE;
#else
	m_hFileHandle = nullptr;
	m_reOpenMode = ES_OPEN_MODE_READ;
#endif
}

//=========================================================================
//!
//!デストラクタ
//!
//=========================================================================
CESFile::~CESFile(void)
{
	CloseFile();
}

//=========================================================================
//!
//! CESFileインスタンス構築し、指定ファイルパス、モードでファイルを開く
//!
//=========================================================================
CESFile* CESFile::CreateFileInstanceWithPath(ESString strPath,ES_OPEN_MODE eOpenMode /*= ES_OPEN_MODE_READ*/){

	try {
		CESFile* pcFile = new CESFile();
		if( !pcFile->Open(strPath,eOpenMode) ){
			delete pcFile;
			pcFile = NULL;
		}
		return pcFile;
	} catch ( const std::bad_alloc& ){
		return NULL;
	}
}

//=========================================================================
//!
//! CESFileインスタンス構築し、指定PrefixのTempファイルを開く
//!
//=========================================================================
CESFile* CESFile::CreateTempFileInstanceWithPrefix( ESString strFolder, ESString strPrefix, ES_OPEN_MODE eOpenMode /*= ES_OPEN_MODE_WRITE_PLUS*/){

	try {
		CESFile* pcFile = new CESFile();
		if( !pcFile->CreateTempFile(strFolder, strPrefix, eOpenMode) ){
			delete pcFile;
			pcFile = NULL;
		}
		return pcFile;
	} catch ( const std::bad_alloc& ){
		return NULL;
	}
}

//=========================================================================
//!
//! ファイル名を取得する。
//!
//=========================================================================
ESString CESFile::GetFileName()
{
	return m_strFilePath;
}

//=========================================================================
//!
//! ファイルが開いているか確認する。（ハンドルの有効性確認）
//!
//=========================================================================
BOOL CESFile::IsOpen(){
#ifdef WIN32
	if((m_strFilePath.length() != 0)&&(m_hFileHandle != INVALID_HANDLE_VALUE)){
#else
	if((m_strFilePath.length() != 0)&&(m_hFileHandle != nullptr)){
#endif
		return TRUE;
	}
	return FALSE;

}

//=========================================================================
//!
//! 指定パスのファイルを開く。
//!
//=========================================================================
BOOL CESFile::Open(ESString strPath,ES_OPEN_MODE eOpenMode /*= ES_OPEN_MODE_READ*/){

	CloseFile();
	BOOL bRet = FALSE;

	try{

		DWORD dwAttr	 =0;
		DWORD dwAccess	 =0;
#ifdef WIN32
		DWORD dwCreation =0;
#else
		m_reOpenMode = eOpenMode;
#endif

#ifdef WIN32
		dwAttr = ::GetFileAttributes(strPath.c_str());
#else
		if(!ES_CMN_FUNCS::PATH::ES_IsExistFile(strPath.c_str(), FALSE)){
			dwAttr = 0xFFFFFFFF;
		}
#endif

		switch (eOpenMode) {
#ifdef WIN32
		case ES_OPEN_MODE_WRITE:
			dwAccess = GENERIC_WRITE;
			dwCreation = CREATE_ALWAYS;
			break;

		case ES_OPEN_MODE_APPEND:
			dwAccess = GENERIC_WRITE;

			if(dwAttr == 0xFFFFFFFF) {
				dwCreation = CREATE_NEW;
			}
			else {
				dwCreation = OPEN_ALWAYS;
			}
			break;

		case ES_OPEN_MODE_READ:
			dwAccess = GENERIC_READ;
			dwCreation = OPEN_EXISTING;
			break;

		case ES_OPEN_MODE_READ_PLUS:
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			dwCreation = OPEN_EXISTING;
			break;

		case ES_OPEN_MODE_WRITE_PLUS:
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			dwCreation = CREATE_ALWAYS;
			break;
#else
		case ES_OPEN_MODE_WRITE:
			dwAccess = std::ios_base::binary | std::ios_base::out|std::ios_base::trunc;
			break;

		case ES_OPEN_MODE_APPEND:
			if(dwAttr == 0xFFFFFFFF) {
				dwAccess = std::ios_base::binary | std::ios_base::out;
			}
			else {
				dwAccess =  std::ios_base::binary | std::ios_base::out|std::ios_base::app;
			}
			break;

		case ES_OPEN_MODE_READ:
			dwAccess =  std::ios_base::binary | std::ios_base::in;
			break;

		case ES_OPEN_MODE_READ_PLUS:
			dwAccess =  std::ios_base::binary  | std::ios_base::in|std::ios_base::out;
			break;

		case ES_OPEN_MODE_WRITE_PLUS:
			dwAccess =  std::ios_base::binary | std::ios_base::in|std::ios_base::out|std::ios_base::app;
			break;
#endif
		default:
			//Bad parameter
			throw;
			break;
		}

#ifdef WIN32
		m_hFileHandle = ::CreateFile(
				strPath.c_str(),
				dwAccess,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				dwCreation,
				FILE_ATTRIBUTE_NORMAL,
				NULL
		);

		if (m_hFileHandle == INVALID_HANDLE_VALUE) {
			throw;
		}else{
			bRet = TRUE;
			m_strFilePath = strPath;
		}

		if (eOpenMode == ES_OPEN_MODE_APPEND) {
			::SetFilePointer(m_hFileHandle, 0, NULL, FILE_END);
		}
#else
		m_hFileHandle = new std::fstream(strPath.c_str(), (std::ios_base::openmode)dwAccess);
		//std::fstream m_hFileHandle(strPath.c_str(), (std::ios_base::openmode)dwAccess);
		//m_hFileHandle = &inoutFile;
		if(!m_hFileHandle){
			bRet = FALSE;
		}else{
			bRet = TRUE;
			m_strFilePath = strPath;

			if (eOpenMode == ES_OPEN_MODE_APPEND) {
				m_hFileHandle->seekp(0, std::ios_base::end);
			}
		}
#endif
	}catch(...){
		//Error　TODO log
	}
	return bRet;
}

//=========================================================================
//!
//! Tempファイルを作成する。ファイル名はPrefrixのみ指定可能
//!
//=========================================================================
BOOL CESFile::CreateTempFile(ESString strFolder, ESString strPrefix, ES_OPEN_MODE eOpenMode /*= ES_OPEN_MODE_WRITE_PLUS*/){

	ESString strCreateTempFilePath;
	if ( !ES_CMN_FUNCS::PATH::ES_GetTempFile( strCreateTempFilePath, strFolder, strPrefix ) ){
		return FALSE;
	}
	return Open( strCreateTempFilePath, eOpenMode );
}

//=========================================================================
//!
//! ファイルにデータを書き込む。
//!
//=========================================================================
BOOL CESFile::WriteData(ES_CMN_FUNCS::BUFFER::IESBuffer& cBuffer){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bRet = FALSE;

	DWORD nBytesWritten=0;
	if (::WriteFile(m_hFileHandle, cBuffer.GetBufferPtr(), cBuffer.GetLength(), & nBytesWritten, NULL) ) {
		if(cBuffer.GetLength() == nBytesWritten){
			bRet = TRUE;
		}else{
			//misstake
		}
	}
#else
	if (m_hFileHandle == nullptr) {
		return FALSE;
	}

	BOOL bRet = TRUE;

	m_hFileHandle->write((char *)cBuffer.GetBufferPtr(), cBuffer.GetLength());
	if (m_hFileHandle->bad() || m_hFileHandle->fail())
	{
		bRet = FALSE;	
	}

#endif

	return bRet;

}

BOOL CESFile::WriteData(UInt32 un32Len,LPVOID pData){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bRet = FALSE;

	DWORD nBytesWritten=0;
	if (::WriteFile(m_hFileHandle, pData, un32Len, & nBytesWritten, NULL) ) {
		if(un32Len == nBytesWritten){
			bRet = TRUE;
		}else{
			//misstake
		}
	}
#else
	if (m_hFileHandle == nullptr) {
		return FALSE;
	}

	BOOL bRet = FALSE;

	if(m_hFileHandle->write((char *)pData, un32Len)){
		bRet = TRUE;
	}else {
		//misstake
	}
#endif

	return bRet;

}


//=========================================================================
//!
//! Streamのバッファをファイルに書き出す。
//!
//=========================================================================
void CESFile::Flush(void){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return;
	}

	if (!::FlushFileBuffers(m_hFileHandle)) {
		//Error
	}
#else
	if (m_hFileHandle == nullptr) {
		return;
	}

	if(!m_hFileHandle->flush()){
		return;
	}
#endif
}

//=========================================================================
//!
//! ファイルポインタから、最後まで読み出す。（ファイルポインタは移動しない）
//!
//=========================================================================
void CESFile::ReadAvailableData(ES_CMN_FUNCS::BUFFER::IESBuffer& cOutBuf){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return ;
	}

	DWORD dwFileSize = ::GetFileSize(m_hFileHandle,NULL);

	if(dwFileSize > 0){

		DWORD nCurrentPos = ::SetFilePointer(m_hFileHandle,0,NULL,FILE_CURRENT);

		DWORD nReadSize = dwFileSize - nCurrentPos;

		ReadDataOfLength(nReadSize,cOutBuf);

		//ファイルポインタを戻す
		::SetFilePointer(m_hFileHandle,nCurrentPos,NULL,FILE_BEGIN);

	}
#else
	if (m_hFileHandle == nullptr) {
		return;
	}

	DWORD dwFileSize =	ES_CMN_FUNCS::PATH::ES_GetFileSize(GetFileName());
	if(dwFileSize > 0){

		DWORD nCurrentPos = (DWORD)m_hFileHandle->tellp();

		DWORD nReadSize = dwFileSize - nCurrentPos;

		ReadDataOfLength(nReadSize,cOutBuf);

		//ファイルポインタを戻す
		m_hFileHandle->seekp(nCurrentPos,std::ios_base::beg);

	}
#endif
}

//=========================================================================
//!
//! ファイルポインタから、最後まで読み出す。（ファイルポインタは末尾へ移動する）
//!
//=========================================================================
void CESFile::ReadDataToEndOfFile(ES_CMN_FUNCS::BUFFER::IESBuffer& cOutBuf){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return ;
	}

	DWORD dwFileSize = ::GetFileSize(m_hFileHandle,NULL);

	if(dwFileSize > 0){

		DWORD nCurrentPos = ::SetFilePointer(m_hFileHandle,0,NULL,FILE_CURRENT);

		DWORD nReadSize =dwFileSize - nCurrentPos;

		ReadDataOfLength(nReadSize,cOutBuf);

	}
#else
	if (m_hFileHandle == nullptr) {
		return;
	}

	DWORD dwFileSize = ES_CMN_FUNCS::PATH::ES_GetFileSize(GetFileName());

	if(dwFileSize > 0){

		DWORD nCurrentPos = (DWORD)m_hFileHandle->tellp();

		DWORD nReadSize =dwFileSize - nCurrentPos;

		ReadDataOfLength(nReadSize,cOutBuf);

	}
	#endif
}


//=========================================================================
//!
//! ファイルポインタから、指定ファイルサイズを読み込む
//!
//=========================================================================
int CESFile::ReadDataOfLength(UInt32 un32Length,ES_CMN_FUNCS::BUFFER::IESBuffer& cOutBuf){
	BOOL bBufAvailable = FALSE;
	if(cOutBuf.GetLength() == un32Length){
		bBufAvailable = TRUE;
	}else{
		bBufAvailable = cOutBuf.AllocBuffer(un32Length);
	}

	if(bBufAvailable){
		return ReadDataOfLength(un32Length,cOutBuf.GetBufferPtr());
	}
	return 0;
}

int CESFile::ReadDataOfLength(UInt32 un32Length,LPVOID pBuf){

	DWORD dwReaded=0;
#ifdef WIN32
	if(::ReadFile(m_hFileHandle, pBuf, un32Length, &dwReaded, NULL) == FALSE){
		//fail
		return 0;
	}
#else
	try {
		m_hFileHandle->read((char *)pBuf, un32Length);
	}
	catch (...) {
		return 0;
	}
	dwReaded = (DWORD)m_hFileHandle->gcount();
#endif
	return dwReaded;
}

//=========================================================================
//!
//! ファイルポインタを先頭に移動する
//!
//=========================================================================
BOOL CESFile::SeekToHead(){
	return SeekToFileOffset(0);
}

//=========================================================================
//!
//!　ファイルポインタを指定位置に移動する
//!
//=========================================================================
BOOL CESFile::SeekToFileOffset(UInt32 un32Offset){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	DWORD dwRet = ::SetFilePointer(m_hFileHandle, un32Offset, NULL, FILE_BEGIN);

	if((dwRet == INVALID_SET_FILE_POINTER)&&(GetLastError() != NO_ERROR)){
		return false;
	}else{
		return true;
	}
#else
	if (m_hFileHandle == nullptr) {
		return FALSE;
	}

	try {
		m_hFileHandle->seekp(un32Offset);
	} catch (...){
		return FALSE;
	}
	return TRUE;
#endif
}

//=========================================================================
//!
//! ファイルポインタをファイルの末尾に移動する
//!
//=========================================================================
BOOL CESFile::SeekToEndOfFile(){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	::SetFilePointer(m_hFileHandle, 0, NULL, FILE_END);

	if(GetLastError() == NO_ERROR){
		return TRUE;
	}else{
		return FALSE;
	}
#else
	if (m_hFileHandle == nullptr) {
		return FALSE;
	}

	try {
		m_hFileHandle->seekp(0, std::ios_base::end);
	} catch (...) {
		return FALSE;
	}
	return TRUE;
#endif
}

UInt32 CESFile::GetLength(){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return 0;
	}

	DWORD dwFileSize = ::GetFileSize(m_hFileHandle,NULL);
#else
	if (m_hFileHandle == nullptr) {
		return 0;
	}

	DWORD dwFileSize = 0;
	try {
		INT nCurrentPos = (INT)m_hFileHandle->tellp();
		if (nCurrentPos < 0)
		{
			CloseFile();
			Open(m_strFilePath, m_reOpenMode);
		}

		dwFileSize = (DWORD)m_hFileHandle->seekg(0, std::ios_base::end).tellg();

		//ファイルポインタを戻す
		m_hFileHandle->seekp(nCurrentPos,std::ios_base::beg);
	} catch (...){
		return 0;
	}
#endif
	return dwFileSize;
}

//=========================================================================
//!
//! ファイルを閉じる
//!
//=========================================================================
BOOL CESFile::CloseFile(){
#ifdef WIN32
	if (m_hFileHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	::CloseHandle(m_hFileHandle);
	m_hFileHandle = INVALID_HANDLE_VALUE;

	m_strFilePath.clear();
#else
	if (m_hFileHandle == nullptr) {
		return FALSE;
	}

	m_hFileHandle->close();
	delete m_hFileHandle;
	m_hFileHandle = nullptr;

#endif
	return TRUE;
}


