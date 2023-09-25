#pragma once

#include "../ESCommonTypedef.h"

#ifndef WIN32

DWORD
GetTempPath(
    DWORD nBufferLength,
    ESString& lpBuffer
    );
#endif

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __inout
#define __inout

BOOL GetTempFileName(
    ESString lpPathName,
    ESString lpPrefixString,
    LPTSTR lpTempFileName
    );

BOOL PathIsDirectory(LPCSTR pszPath);
BOOL PathFileExists(LPCSTR pszPath);
BOOL PathAppend(LPSTR pszPath, LPCSTR pszMore);
#endif

namespace ES_CMN_FUNCS
{
	namespace PATH
	{

		// 現在のモジュールのあるフォルダのパスを取得する
		BOOL ES_GetModuleFileName( __in HMODULE hModule, __out ESString& strDst );

		// モジュールの実行フォルダ名を取得する
		BOOL ES_GetModuleFolder( __in HMODULE hModule, __out ESString& strDst );

		// システムのTempディレクトリを取得する
		BOOL ES_GetSystemTempDir( __out ESString& strDst );

    // システムのTempフォルダに一時ファイルを作成します。
		// strPrefixは、ファイル名の接頭辞の最初の 3文字
		// 作成されたファイルは、呼び出し元で削除すること
		BOOL ES_GetTempFile( __out ESString& strCreateTempFilePath, __in const ESString& strPrefix );

    // ターゲットフォルダに一時ファイルを作成します。
		// strPrefixは、ファイル名の接頭辞の最初の 3文字
		// 作成されたファイルは、呼び出し元で削除すること
		BOOL ES_GetTempFile( __out ESString& strCreateTempFilePath, __in const ESString& strTargetFolder, __in const ESString& strPrefix );

		// 指定したフォルダが存在するか確認する
		BOOL ES_IsExistFolder( __in const ESString& strPath );

		// 指定したファイルが存在するか確認する
		BOOL ES_IsExistFile( __in const ESString& strPath, __in BOOL bIsFolderInclude = FALSE );

		// パスを連結させる
		BOOL ES_CombinePath( __out ESString& strDst, __in const ESString& strBasePath, __in const ESString& strAddPath );

		// パスの最後に\を追加する
		BOOL ES_AddBackYen( __inout ESString& strPath );

		// パスの最後の\を削除する
		BOOL ES_RemoveBackYen( __inout ESString& strPath );

		// パスからドライブ部分を抜き出します
		BOOL ES_GetDrive( __out ESString& strDst, __in const ESString& strSrcPath );

		// パスからフォルダ部分を抜き出します
		BOOL ES_GetFolderPath( __out ESString& strDst, __in const ESString& strSrcPath );

		// ファイルパスからファイル名部分を取得
		BOOL ES_GetFileName( __out ESString& strDst, __in const ESString& strSrcPath, __in BOOL bExt );

		// ファイルパスから拡張子を取得
		BOOL ES_GetExtension( __out ESString& strDst, __in const ESString& strSrcPath );

		// パスの拡張子を変更
		BOOL ES_RenameExt( __out ESString& strDst, __in const ESString& strSrcPath, __in const ESString& strExt);

		// フルパスを求める
		BOOL ES_ConvertFullpath( __out ESString& strDst, __in const ESString& strSrcPath);

		// パスが相対パスかどうかを判定
		BOOL ES_IsRelative( __in const ESString& strPath );

		// パスを正常化しフルパスに変更する
		BOOL ES_NormalizeFilePath( __out ESString& strDst, __in const ESString& strCurrentPath, __in const ESString& strSrcPath );

    // 二つのパス間の相対パスを取得
		// Attribute : ファイルを指すかディレクトリを指すか指定する
		//   FILE_ATTRIBUTE_DIRECTORY
		//   FILE_ATTRIBUTE_NORMAL
		BOOL ES_RelativePathTo( __out ESString& strDst, __in const ESString& strFrom, __in DWORD dwAttrFrom, __in const ESString& strTo, __in DWORD dwAttrTo, __in BOOL bRemoveFirstDot = FALSE );

		// フォルダを作成する
		BOOL ES_MakeFolder( __in const ESString& strPath );

    // フォルダを内部のファイル毎削除する
		// Systemを残す処理等入っていないため、指定するフォルダには注意すること
		BOOL ES_DeleteFolder( __in const ESString& strPath, __in BOOL bRemoveReadOnly = FALSE );

		// 読み取り専用属性を外す
		void ES_RemoveReadOnlyAttribute( __in const ESString& strPath );

		// フォルダに書き込み可能かどうかをチェックする
		BOOL ES_IsWritableFolder( __in const ESString& strFolderPath );

		// ドライブに書き込み可能かどうかチェックする
		BOOL ES_IsWritableDrive( __in const ESString& strDrivePath );

		// フォルダー内のファイルを列挙する
		ESStringArray EnumFilesInDir(ESString strWildcard, ESString strDir);

		// 作業フォルダのパスを返す
		std::string ES_GetWorkTempPath();

		#ifndef WIN32
		
		// ファイルサイズを取得する
		DWORD ES_GetFileSize( __in const ESString& strPath );

		// ディレクトリかどうかをかえす
		BOOL ES_IsDirectory(__in const ESString& strPath);

		// 通常ファイルかどうかをかえす
		BOOL ES_IsRegularFile(__in const ESString&);

		#endif

	}; // namespace PATH
}; // namespace CMN_FUNCS
