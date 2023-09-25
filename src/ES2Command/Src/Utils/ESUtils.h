#pragma once

#include "CommonUtility/utils/Utilities.h" // for roundf


// スレッドが生きているか？
bool IsThreadAlive( HANDLE hThread );

//　dwLastTickCountからdwInterval_msだけ時間が経過しているか？
bool IsPassedInterval( DWORD dwLastTickCount, DWORD dwInterval_ms, DWORD* pdwPassedTime_ms );

// １ライン辺りのバイト数
int ESCIGetBytesPerRow(int nImageWidth, int nBitsPerPixel);

// 
void ESCIRoundColorCorrectionMatrix(int mult, double org_cct[], int rnd_cct[]);

//
ESIndexSet AnyArrayToIndexSet( ESAnyArray& anyArray );

// trim from start
ESStringA& LTrimSpace(ESStringA& s);
// trim from end
ESStringA& RTrimSpace(ESStringA& s);
// trim from both ends
ESStringA& TrimSpace(ESStringA& s);

#ifdef WIN32
ESErrorCode LoadJsonResouce( ESDictionary& dicOutput, const ESString& strResouceType, const ESString& strResouceName );

ESErrorCode RegQueryDWORDValue(HKEY hKeyRoot, LPCWSTR lpszSubKey, WCHAR *pszEntry, DWORD *pdwData);
#endif
