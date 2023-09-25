#include "stdafx.h"
#include "Platform.h"
#include "PathUtils.h"
#include <vector>

#ifndef WIN32
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

int UnlinkFile(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}
#endif

#ifdef WIN32
#include <Shlwapi.h>
#include <strsafe.h>

#pragma comment(lib, "Shlwapi.lib")
#endif

#define ES_TRACE(x) ::OutputDebugString(x);

#ifndef WIN32
#include <libgen.h>

DWORD GetTempPath(DWORD nBufferLength, ESString& lpBuffer)
{
	DWORD dwLen = 4;
	lpBuffer = "/tmp";
	return dwLen;
}

BOOL GetTempFileName(ESString DirName, ESString Prefix, LPTSTR TempFile)
{

	size_t path_len = DirName.length() + Prefix.length() + strlen("XXXXXX");
	char *buf = (char *)malloc(path_len + 1);

	buf[path_len] = 0;
	ES_CMN_FUNCS::PATH::ES_AddBackYen(DirName);
  	strcpy(buf, DirName.c_str());
  	strcat(buf, Prefix.c_str());
	strcat(buf, "XXXXXX");

 	int fd = mkstemp(buf);
	if(!fd){
		free(buf);
	   	return FALSE;
	}
 	FILE* fp = fdopen(fd, "w");

	memcpy(TempFile, buf, path_len+1);
 	fclose(fp);
	free(buf);

 	return TRUE;
}
#endif

namespace ES_CMN_FUNCS
{
	namespace PATH
	{

		//====================================================================
		//!
		//! モジュールの実行パス名を取得する
		//!
		//====================================================================
		BOOL ES_GetModuleFileName( __in HMODULE hModule, __out ESString& strDst )
		{
			try {
				DWORD dwLen = _MAX_PATH;
				for (;;){
					std::vector<TCHAR> buf(dwLen, 0);
					// 起動されたアプリケーションのフルパス名を取得
#ifdef WIN32
					DWORD dwRet = ::GetModuleFileName(hModule, &buf[0], dwLen);
#else
					DWORD dwRet = GetModuleFileName(&buf[0], dwLen);
#endif
					if (dwRet == 0){
						strDst = ES_STRING("");
						return FALSE;
					}
					if (dwRet == dwLen){
						dwLen += _MAX_PATH;
						continue;
					}
					strDst = &buf[0];
					return TRUE;
				}
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetModuleFileName() : Exception Error"));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! モジュールの実行フォルダ名を取得する
		//!
		//====================================================================
		BOOL ES_GetModuleFolder( __in HMODULE hModule, __out ESString& strDst )
		{
			try {
				ESString strModulePath;
				if (!ES_GetModuleFileName(hModule, strModulePath)){
					strDst = ES_STRING("");
					return FALSE;
				}

				return ES_GetFolderPath(strDst, strModulePath);
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetModuleFolder() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! システムのTempディレクトリを取得する
		//!
		//====================================================================
		BOOL ES_GetSystemTempDir( __out ESString& strDst )
		{
			try {
				DWORD dwLen = _MAX_PATH;
				for (;;){
					// Tempフォルダのパス名を取得
#ifdef WIN32
					std::vector<TCHAR> buf(dwLen, 0);
					DWORD dwRet = ::GetTempPath(dwLen, &buf[0]);
#else
					ESString path;
					DWORD dwRet = GetTempPath(dwLen, path);
#endif
					if (dwRet == 0){
						strDst = ES_STRING("");
						return FALSE;
					}
					// GetTempPath() が返す値
					// 失敗:0
					// バッファ不足:終端NULLを含む必要な文字サイズ
					// 成功:終端NULLを含まないコピーした文字サイズ
					if (dwRet > dwLen){
						dwLen = dwRet;
						continue;
					}
#ifdef WIN32
					strDst = &buf[0];
#else
					strDst = path;
#endif
					return TRUE;
				}
			} catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetSystemTempDir() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! システムのTempフォルダに一時ファイルを作成します。
		//!
		//! @param strPrefix 作成されるTempファイルの接頭辞の最初の3文字
		//!
		//! @note 作成されたファイルは、呼び出し元で削除すること
		//!
		//====================================================================
		BOOL ES_GetTempFile( __out ESString& strCreateTempFilePath, __in const ESString& strPrefix )
		{
			ESString strTempPath = ES_STRING("");
			if( !ES_GetSystemTempDir( strTempPath ) ) {
				strCreateTempFilePath = ES_STRING("");
				return FALSE;
			}

			TCHAR buf[MAX_PATH+1] = { 0 };
#ifdef WIN32
			if ( ::GetTempFileName( strTempPath.c_str(), strPrefix.c_str(), 0, buf ) == 0 ){
#else
			if ( GetTempFileName( strTempPath, strPrefix, buf ) == 0 ){
#endif
				strCreateTempFilePath = ES_STRING("");
				return FALSE;
			}
			strCreateTempFilePath = buf;

			return TRUE;
		}

		//====================================================================
		//!
		//! ターゲットフォルダに一時ファイルを作成します。
		//!
		//! @param
		//!   strTargetFolder  : 出力先フォルダー（このフォルダが存在しない場合は失敗する）
		//!   strPrefix        : ファイル名の接頭辞の最初の 3文字
		//!
		//! @note 作成されたファイルは、呼び出し元で削除すること
		//!
		//====================================================================
		BOOL ES_GetTempFile( __out ESString& strCreateTempFilePath,
							 __in const ESString& strTargetFolder,
							 __in const ESString& strPrefix )
		{
			if( !ES_IsExistFolder( strTargetFolder ) ) {
				strCreateTempFilePath = ES_STRING("");
				return FALSE;
			}
			TCHAR buf[MAX_PATH+1] = { 0 };
#ifdef WIN32
			if ( ::GetTempFileName( strTargetFolder.c_str(), strPrefix.c_str(), 0, buf ) == 0 ){
#else
			if ( GetTempFileName( strTargetFolder, strPrefix, buf ) == 0 ){
#endif
				strCreateTempFilePath = ES_STRING("");
				return FALSE;
			}
			strCreateTempFilePath = buf;

			return TRUE;
		}


		//====================================================================
		//!
		//! 指定したフォルダが存在するか確認する
		//!
		//====================================================================
		BOOL ES_IsExistFolder( __in const ESString& strPath )
		{
#ifdef WIN32
			return ::PathIsDirectory(strPath.c_str());
#else
			return ES_IsExistFile(strPath, TRUE);
#endif
		}


		//====================================================================
		//!
		//! 指定したファイルが存在するか確認する
		//!
		//====================================================================
		BOOL ES_IsExistFile( __in const ESString& strPath, __in BOOL bIsFolderInclude )
		{
#ifdef WIN32
			if (bIsFolderInclude){
				return ::PathFileExists(strPath.c_str());
			}
			else {
				return (::PathFileExists(strPath.c_str()) && !::PathIsDirectory(strPath.c_str()));
			}
#else //TBD unfinished ->bIsFolderInclude
			struct stat path_stat;
		     if (::stat(strPath.c_str(), &path_stat)!= 0)
			 {
				if (errno == ENOENT || errno == ENOTDIR)
				{
					return FALSE;
				}

				return FALSE;
			 }

			 return TRUE;
#endif
		}

		//====================================================================
		//!
		//! パスを連結させる
		//!
		//====================================================================
		BOOL ES_CombinePath( __out ESString& strDst, __in const ESString& strBasePath, __in const ESString& strAddPath )
		{
			try {
				int nLen = (int)(strBasePath.length() + strAddPath.length() + 2); // 終端NULL + path間の'\\'分追加

				std::vector<TCHAR> buf(nLen, 0);
#ifdef WIN32
				::StringCchCopy(&buf[0], nLen, strBasePath.c_str());
				BOOL bRet = ::PathAppend(&buf[0], strAddPath.c_str());

				if (bRet){
					strDst = &buf[0];
				}else{
					strDst = ES_STRING("");
				}
				return bRet;
#else
				std::string tmp = strBasePath;

				if (strBasePath[strBasePath.length()- 1] != '/') { // Need to add a
					tmp +=  '/';                // path separator
				}
			
				strDst = tmp + strAddPath;
				
				return TRUE;
#endif
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_CombinePath() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! パスの最後に\を追加する
		//!
		//====================================================================
		BOOL ES_AddBackYen( __inout ESString& strPath )
		{
			if ( strPath.empty() || (strPath.at( strPath.length()-1) != SEPARATOR) ){
				strPath += SEPARATOR;
			}
			return TRUE;
		}

		//====================================================================
		//!
		//! パスの最後の\を削除する
		//!
		//====================================================================
		BOOL ES_RemoveBackYen( __inout ESString& strPath )
		{
			if ( !strPath.empty() && (strPath.at( strPath.length()-1) == SEPARATOR) ){
				strPath = strPath.substr(0, strPath.length() - 1);
			}
			return TRUE;
		}

		//====================================================================
		//!
		//! パスからドライブ部分を抜き出します
		//!
		//====================================================================
		BOOL ES_GetDrive( __out ESString& strDst, __in const ESString& strSrcPath )
		{
			if (strSrcPath.empty()){
				strDst = ES_STRING("");
				return FALSE;
			}
			try {
#ifdef WIN32
				TCHAR szDrive[_MAX_DRIVE + 1] = {0};

				// ファイルパスを分割
				if (::_tsplitpath_s(strSrcPath.c_str(), szDrive, _MAX_DRIVE, NULL, 0, NULL, 0, NULL, 0) != 0){
					strDst = ES_STRING("");
					return FALSE;
				}

				strDst = szDrive;
				return TRUE;
#else
				// nothing to do
				return TRUE;
#endif
			}
			catch (...) {
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetDrive() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! パスからフォルダ部分を抜き出します
		//!
		//====================================================================
		BOOL ES_GetFolderPath( __out ESString& strDst, __in const ESString& strSrcPath)
		{
			if (strSrcPath.empty()){
				strDst = ES_STRING("");
				return FALSE;
			}

			try {
#ifdef WIN32
				TCHAR szDrive[_MAX_DRIVE + 1];
				TCHAR szDir[_MAX_DIR + 1];
				memset(szDrive, 0, sizeof(szDrive));
				memset(szDir, 0, sizeof(szDir));

				// ファイルパスを分割
				if (::_tsplitpath_s(strSrcPath.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, NULL, 0, NULL, 0) != 0){
					strDst = ES_STRING("");
					return FALSE;
				}

				// フォルダパス = ( ドライブ名 + ディレクトリ名 )
				strDst = szDrive;
				strDst += szDir;
#else
				std::string srcPathCopy = strSrcPath;
				if (srcPathCopy.size() <= 1) //Make sure it's possible to check the last character.
				{
					strDst = ES_STRING("");
					return FALSE;
				}
				if (*(srcPathCopy.rbegin() + 1) == '/') //Remove trailing slash if it exists.
				{
					srcPathCopy.pop_back();
				}
				srcPathCopy.erase(std::find(srcPathCopy.rbegin(), srcPathCopy.rend(), '/').base(), srcPathCopy.end());
				strDst = srcPathCopy;
				if (strDst.empty())
				{
					strDst = ES_STRING("");
					return FALSE;
				}
#endif
				return TRUE;
			}
			catch (...) {
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetFolderPath() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! ファイルパスからファイル名部分を取得
		//!
		//====================================================================
		BOOL ES_GetFileName( __out ESString& strDst, __in const ESString& strSrcPath, __in BOOL bExt)
		{
			if (strSrcPath.empty()){
				strDst = ES_STRING("");
				return FALSE;
			}

			try {
#ifdef WIN32
				if (bExt){
					strDst = ::PathFindFileName(strSrcPath.c_str());
					return !strDst.empty();

				} else {

					TCHAR szDrive[_MAX_DRIVE + 1] = {0};
					TCHAR szDir  [_MAX_DIR + 1]   = {0};
					TCHAR szFName[_MAX_FNAME + 1] = {0};
					TCHAR szExt  [_MAX_EXT + 1]   = {0};

					// ファイルパスを分割
					if (::_tsplitpath_s(strSrcPath.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) != 0){
						strDst = ES_STRING("");
						return FALSE;
					}
					// ファイル名
					strDst = szFName;

					return TRUE;
				}
#else
				strDst =  basename((char*)strSrcPath.c_str());;
				if (strDst.empty())
				{
					return FALSE;
				}
#endif
			}
			catch (...) {
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetFileName() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}

			return TRUE;
		}

		//====================================================================
		//!
		//! ファイルパスから拡張子を取得
		//!
		//====================================================================
		BOOL ES_GetExtension( __out ESString& strDst, __in const ESString& strSrcPath )
		{

			if (strSrcPath.empty()){
				strDst = ES_STRING("");
				return FALSE;
			}
			try {
#ifdef WIN32
				strDst = ::PathFindExtension(strSrcPath.c_str());
				return !strDst.empty();
#else
				ESString fileName;
				if(ES_GetFileName(fileName, strSrcPath, TRUE) == FALSE)
				{
					return FALSE;
				}

				const char *dot = strrchr(fileName.c_str(), '.');
    			if(!dot || dot == fileName.c_str())
				{	
					return FALSE;
				}

			    strDst = ESString(dot + 1);
				return TRUE;
#endif
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_GetExtension() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		///====================================================================
		//!
		//! パスの拡張子を変更
		//!
		///====================================================================
		BOOL ES_RenameExt( __out ESString& strDst, __in const ESString& strSrcPath, __in const ESString& strExt )
		{
			try {
#ifdef WIN32
				int nLen = strSrcPath.length() + strExt.length() + 1;
				std::vector<TCHAR> buf(nLen, 0);
				::StringCchCopy(&buf[0], nLen, strSrcPath.c_str());

				BOOL bRet = ::PathRenameExtension(&buf[0], strExt.c_str());
				if (bRet){
					strDst = &buf[0];
				}else{
					strDst = ES_STRING("");
				}
				return bRet;
#else
				assert(false); //TODO IMPLEMENT
#endif
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_RenameExt() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! フルパスを求める
		//!
		//====================================================================
		BOOL ES_ConvertFullpath( __out ESString& strDst, __in const ESString& strSrcPath)
		{
			if (strSrcPath.empty()){
				strDst = ES_STRING("");
				return FALSE;
			}
			try {
#ifdef WIN32
				TCHAR szBuffer[_MAX_PATH + 1] = {0};

				if (!::PathSearchAndQualify(strSrcPath.c_str(), szBuffer, _MAX_PATH)){
					strDst = ES_STRING("");
					return FALSE;
				}
				strDst = szBuffer;
				return TRUE;
#else
				assert(false); //TODO IMPLEMENT
				return FALSE;;
#endif
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_ConvertFullpath() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		///====================================================================
		//!
		//! パスが相対パスかどうかを判定
		//!
		///====================================================================
		BOOL ES_IsRelative( __in const ESString& strPath )
		{
#ifdef WIN32
			return ::PathIsRelative(strPath.c_str());
#else
			assert(false); // TODO IMPLEMENT
			return FALSE;
#endif
		}

		///====================================================================
		//!
		//! パスを正常化しフルパスに変更する
		//!
		///====================================================================
		BOOL ES_NormalizeFilePath( __out ESString& strDst, __in const ESString& strCurrentPath, __in const ESString& strSrcPath )
		{
			if (strSrcPath.empty()){
				if (strCurrentPath.empty()){
					strDst = ES_STRING("");
					return FALSE;
				}
				else {
					strDst = strCurrentPath;
					return TRUE;
				}
			}
			try {
				strDst = strSrcPath;
				if (!strCurrentPath.empty() && ES_IsRelative(strSrcPath)){
					// 相対ファイルでカレントフォルダが指定されていればパスを結合
#ifdef WIN32
					if (!ES_CombinePath(strDst, strCurrentPath, strSrcPath)){
#else
					ESString tempPath;
					if(!ES_GetFileName(tempPath, strDst, FALSE)){
						return FALSE;
					}
					if (!ES_CombinePath(strDst, strCurrentPath, tempPath)){
#endif
#ifdef _DEBUG
						ES_TRACE(ES_STRING("ES_NormalizeFilePath() FAILED APPEND PATH"));
#endif
						return FALSE;
					}
				}

				//フルパスを求める
				if (!ES_ConvertFullpath(strDst, strDst)){
#ifdef _DEBUG
					ES_TRACE(ES_STRING("ES_NormalizeFilePath() FAILED CREATE FULL PATH"));
#endif
					return FALSE;
				}
				return TRUE;
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_NormalizeFilePath() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! 二つのパス間の相対パスを取得
		//!
		//====================================================================
		BOOL ES_RelativePathTo( __out ESString& strDst, __in const ESString& strFrom, __in DWORD dwAttrFrom, __in const ESString& strTo, __in DWORD dwAttrTo, __in BOOL bRemoveFirstDot )
		{
			try {
				strDst = ES_STRING("");
#ifdef WIN32
                TCHAR szBuffer[_MAX_PATH + 1] = {0};
				BOOL bRet = ::PathRelativePathTo(szBuffer, strFrom.c_str(), dwAttrFrom, strTo.c_str(), dwAttrTo);
				if (bRet){
					strDst = szBuffer;
					if (bRemoveFirstDot && strDst.at(0) == ES_STRING('.')){
						strDst.substr(1, strDst.length() - 1);
					}
				}
				else {
					strDst = ES_STRING("");
				}
				return bRet;
#else
				//TBD...
				assert(FALSE);
#endif
			}
			catch (...){
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_RelativePathTo() : Exception Error."));
#endif
				strDst = ES_STRING("");
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! フォルダを作成する
		//!
		//====================================================================
		BOOL ES_MakeFolder( __in const ESString& strPath )
		{
			if (strPath.empty()){
				return FALSE;
			}
			try {
				if (ES_IsExistFolder(strPath)){
					// 既に存在する
					return TRUE;
				}

				// パスの最後に\を追加する
				ESString strFolderPath = strPath;
				if (!ES_AddBackYen(strFolderPath)){
					return FALSE;
				}

#ifdef WIN32
				// ドライブ名
				ESString strDrive;
				if (!ES_GetDrive(strDrive, strFolderPath)){
					return FALSE;
				}
				if (strDrive.empty()){
					// ネットワーク上のフォルダか？
					int pos = strFolderPath.find(ES_STRING("\\\\"), 0);
					if ( pos == 0 ){
						pos = strFolderPath.find(ES_STRING("\\"), 3);
						strDrive = strFolderPath.substr(0,pos);
					}
				}
#endif
				// フォルダを階層ごとに存在確認し、作成する
				ESString	strMakePath = ES_STRING("");
#ifdef WIN32
				int	iIndex = strDrive.size();
#else
				int	iIndex = 0;
#endif
				BOOL bResult = FALSE;

				while ((strMakePath + SEPARATOR) != strFolderPath){
					// 作成するディレクトリ名を設定
					iIndex = (int)strFolderPath.find(SEPARATOR, iIndex + 1);
					strMakePath = strFolderPath.substr(0,iIndex);

#ifdef WIN32
					if (!ES_IsExistFolder(strMakePath) && strMakePath != strDrive){

						// フォルダが存在しておらず、パスがドライブを示していない場合
						// フォルダを作成
						if (!(bResult = ::CreateDirectory(strMakePath.c_str(), NULL))){
							break;
						}
					}
#else
					if (!ES_IsExistFolder(strMakePath)){
						if(mkdir(strMakePath.c_str(), 0755) != 0)
						{
							bResult = FALSE;
							break;
						}
						else
						{
							bResult = TRUE;
							break;
						}
					}
#endif
				}
				return bResult;
			} catch (...) {
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_MakeFolder() : Exception Error."));
#endif
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! フォルダを削除する
		//!
		//! @note Systemを残す処理等入っていないため、指定するフォルダには注意すること
		//!
		//====================================================================
		BOOL ES_DeleteFolder( __in const ESString& strPath, __in BOOL bRemoveReadOnly /*= FALSE*/)
		{
			if (strPath.empty()){
				return FALSE;
			}
			try {
#ifdef WIN32
				if (!ES_IsExistFolder(strPath)){
					// フォルダとして存在していない
					return TRUE;
				}

				ESString strSearch;
				if (!ES_CombinePath(strSearch, strPath, ES_STRING("*.*"))){
					return FALSE;
				}
				WIN32_FIND_DATA fd;
				HANDLE hFind = ::FindFirstFile(strSearch.c_str(), &fd);
				if (hFind == INVALID_HANDLE_VALUE){
#ifdef _DEBUG
					ES_TRACE(ES_STRING("ES_DeleteFolder() : FindFirstFile() Error."));
#endif
					return FALSE;
				}
				do {
					if (esscmp(fd.cFileName, ES_STRING(".")) == 0 ||
						esscmp(fd.cFileName, ES_STRING("..")) == 0){
						// カレントフォルダ「.」と親フォルダ「..」は、処理をスキップ
						continue;
					}

					// 検索されたファイルのフルパス名を取得する
					ESString strFoundFilePathName;
					if (!ES_CombinePath(strFoundFilePathName, strPath, fd.cFileName)){
#ifdef _DEBUG
						ES_TRACE(ES_STRING("ES_DeleteFolder() : CombinePath() Error."));
#endif
						::FindClose(hFind);
						return FALSE;
					}

					if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
						// ディレクトリの場合
						// 再帰呼び出し
						ES_DeleteFolder(strFoundFilePathName, bRemoveReadOnly);
					}
					else{
						if (bRemoveReadOnly && fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY){
							// 読み取り専用なら外しておく
							ES_RemoveReadOnlyAttribute(strFoundFilePathName);
						}
						// フォルダを削除
						if (!::DeleteFile(strFoundFilePathName.c_str()) ){
#ifdef _DEBUG
							ES_TRACE(ES_STRING("ES_DeleteFolder() : DeleteFile() Error."));
#endif
						}
					}
				} while (::FindNextFile(hFind, &fd));
				::FindClose(hFind);

				if (bRemoveReadOnly) {
					// 読み取り専用なら外しておく
					ES_RemoveReadOnlyAttribute(strPath);
				}

				// フォルダを削除
				return ::RemoveDirectory(strPath.c_str());
#else
#ifndef __ANDROID__
				nftw(strPath.c_str(), UnlinkFile, 64, FTW_DEPTH | FTW_PHYS);
#endif
#endif
				return TRUE;
			}
			catch (...) {
#ifdef _DEBUG
				ES_TRACE(ES_STRING("ES_DeleteFolder() failed. : Exception Error."));
#endif
				return FALSE;
			}
		}

		//====================================================================
		//!
		//! 読み取り専用属性を外す
		//!
		//====================================================================
		void ES_RemoveReadOnlyAttribute( __in const ESString& strPath )
		{
			if (strPath.empty()){
				return;
			}
#ifdef WIN32
			DWORD dwAttribute = ::GetFileAttributes(strPath.c_str());
			if (dwAttribute & FILE_ATTRIBUTE_READONLY){
				::SetFileAttributes(strPath.c_str(), dwAttribute & (~FILE_ATTRIBUTE_READONLY));
			}
#else
			assert(FALSE);
			//TBD...
#endif
		}

		//====================================================================
		//!
		//! フォルダに書き込み可能かどうかをチェックする
		//!
		//====================================================================
		BOOL ES_IsWritableFolder( __in const ESString& strFolderPath )
		{
			/* 指定されたフォルダが存在するか */
			if(ES_IsExistFolder(strFolderPath))
			{
#ifdef WIN32
				const ESString strPrefix(ES_STRING("tmp"));
#else
				const ESString strPrefix(ES_STRING("/tmp_file_"));
#endif
				ESString strTempFilePath;
				/* 一時ファイルを作成する */
				if(ES_GetTempFile(strTempFilePath, strFolderPath, strPrefix))
				{
					/* 作成した一時ファイルが存在する */
					if(ES_IsExistFile(strTempFilePath))
					{
#ifdef WIN32
						DeleteFileW(strTempFilePath.c_str());
						return TRUE;
#else
						if(ES_DeleteFolder(strTempFilePath.c_str(), FALSE)){
							return TRUE;
						}
#endif
					}
				}
			}
			return FALSE;
		}

		//====================================================================
		//!
		//! ドライブに書き込み可能かどうかチェックする
		//!
		//====================================================================
		BOOL ES_IsWritableDrive( __in const ESString& strDrivePath )
		{
#if 0
			ESString strDriveName;
			if(ES_GetDrive(strDriveName, strDrivePath ))
			{
				DWORD dwFlags = 0;
				if(GetVolumeInformationW(strDriveName.c_str(), NULL, 0, NULL, NULL, &dwFlags, NULL, 0))
				{
					return (dwFlags & FILE_READ_ONLY_VOLUME);
				}
			}
			return FALSE;
#else
			/* システムデフォルトの一時領域を取得する */
			ESString strSystemTmpDir;
			if(!ES_GetSystemTempDir(strSystemTmpDir))
			{
				return FALSE;	/* 取得できなければ × */
			}

#ifdef WIN32
			/* システムデフォルトの一時領域があるドライブ部分を取得する */
			ESString strSystemTmpDriveName;
			if(!ES_GetDrive(strSystemTmpDriveName, strSystemTmpDir))
			{
				return FALSE;
			}

			/* 指定されたドライブ文字列からドライブ部分を取得する */
			ESString strDriveName;
			if(!ES_GetDrive(strDriveName, strDrivePath ))
			{
				return FALSE;	/* 取得できなければ × */
			}

			/* 指定されたドライブが, システムデフォルトの一時領域と一致していたら, OK */
			if(strDriveName.compare(strSystemTmpDriveName) == 0)
			{
				return TRUE;
			}
#endif
			/* 指定されたドライブパスに一時ファイルを作成出来たら OK */
			return ES_IsWritableFolder(strDrivePath);
#endif
		}
#ifndef WIN32
		//====================================================================
		//!
		//!  指定ファイルのサイズを取得する
		//!
		//====================================================================
		DWORD ES_GetFileSize( __in const ESString& strPath )
		{
	   		 struct stat statbuf;
			if (stat(strPath.c_str(), &statbuf) == -1)
			{
				return 0;
			}

			return (DWORD)statbuf.st_size;
		}

		//====================================================================
		//!
		//! 	ディレクトリかどうかをかえす
		//!
		//====================================================================
		BOOL ES_IsDirectory(__in const ESString& strPath)
		{
	   		 struct stat statbuf;
			if (stat(strPath.c_str(), &statbuf) == -1)
			{
				return 0;
			}

			return S_ISDIR(statbuf.st_mode);
		}

		//====================================================================
		//!
		//! 	通常ファイルかどうかをかえす
		//!
		//====================================================================
		BOOL ES_IsRegularFile(__in const ESString& strPath)
		{
	   		 struct stat statbuf;
			if (stat(strPath.c_str(), &statbuf) == -1)
			{
				return 0;
			}

			return S_ISREG(statbuf.st_mode);
		}

		//====================================================================
		//!
		//! 	フォルダー内のファイルを列挙する
		//!
		//====================================================================
#ifndef __ANDROID__
#if !TARGET_OS_IPHONE && !defined(TARGET_OS_EMBEDDED)
		ESStringArray EnumFilesInDir(ESString strWildcard, ESString strDir)
		{
			ESStringArray arFiles;
			ES_CMN_FUNCS::PATH::ES_AddBackYen(strDir);
		#ifdef WIN32
			ESString strSearchTarget;
			strSearchTarget = strDir + strWildcard;
			HANDLE hFind = NULL;
			WIN32_FIND_DATA win32fd = {0};

			hFind = FindFirstFile(strSearchTarget.c_str(), &win32fd);
			if (hFind == INVALID_HANDLE_VALUE) {
				return arFiles;
			}

			do {
				if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				} else {
					arFiles.push_back(strDir + win32fd.cFileName);

				}
			} while (FindNextFile(hFind, &win32fd));

			FindClose(hFind);

		#else
		auto dirp = opendir(strDir.c_str());
		if (dirp)
		{
				struct dirent *dp = NULL;
				while ((dp = readdir(dirp)) != NULL)
				{
					std::string path;
				   ES_CMN_FUNCS::PATH::ES_CombinePath(path, strDir, dp->d_name);
					if (ES_IsRegularFile(path) || ES_IsDirectory(path))
					{
						arFiles.push_back(path);
					}
				}
				closedir(dirp);
		}
		#endif
			return arFiles;
		}
#endif
#endif
#endif

		std::string ES_GetWorkTempPath() 
		{
			pid_t c_pid;
			c_pid = getpid();

			std::string work_path = EPSON_WORK_PATH;
			work_path = work_path + "_";
			work_path = work_path + std::to_string(c_pid);
			work_path = work_path + "/";

			return work_path;
		}

	};	// namespace PATH
};	// namespace ES_CMN_FUNCS


#ifdef __in
#undef __in
#endif

#ifdef __out
#undef __out
#endif

#ifdef __inout
#undef __inout
#endif
