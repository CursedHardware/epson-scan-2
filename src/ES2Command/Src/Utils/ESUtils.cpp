#include "stdafx.h"
#include "ESUtils.h"
#include "CommonUtility/utils/ESJsonUtils.h"

#include <algorithm> 
#include <functional>
#include <cctype>

extern HINSTANCE g_hES2CommandModule;

// スレッドが生きているか？
bool IsThreadAlive( HANDLE hThread )
{
	if ( hThread == NULL ){
		return false;
	}
	DWORD dwExitCode = 0;
	if ( !GetExitCodeThread( hThread, &dwExitCode ) ){
		return false;
	}
	return ( dwExitCode == STILL_ACTIVE );
}

//　dwLastTickCountからdwInterval_msだけ時間が経過しているか？
bool IsPassedInterval( DWORD dwLastTickCount, DWORD dwInterval_ms, DWORD* pdwPassedTime_ms )
{
	DWORD dwPassedTime = 0;

	DWORD dwNowTickCount = GetTickCount(); // GetTikCount64()はXP対応コードでは利用しない
	if ( dwNowTickCount >= dwLastTickCount ){
		dwPassedTime = dwNowTickCount - dwLastTickCount;
	} else {
		UInt64 un64NowTickCount = dwNowTickCount + 0xffffffff;
		dwPassedTime = (DWORD)(un64NowTickCount - dwLastTickCount);
	}
	if ( pdwPassedTime_ms != NULL ){
		*pdwPassedTime_ms = dwPassedTime;
	}
	return ( dwPassedTime > dwInterval_ms ) ? true : false; 
}

int ESCIGetBytesPerRow(int nImageWidth, int nBitsPerPixel)
{
	switch (nBitsPerPixel) {
		case 48:
		case 24:
		case 16:
		case 8:
			return nImageWidth * (nBitsPerPixel / 8);
			
		case 1:
			return (nImageWidth + 7) / 8;
			
		default:
			break;
	}
	
	return 0;
}

static int get_roundup_index(double frac[], int n)
{
	int		i, index = -1;
	double	max_val = 0.0;

	for (i=0; i<n; i++) {
		if (frac[i]<0) continue;
		if (max_val<frac[i]) {
			index = i;
			max_val = frac[i];
		}
	}
	return index;
}

static int get_rounddown_index(double frac[], int n)
{
	int		i, index = -1;
	double	min_val = 1.0;

	for (i=0; i<n; i++) {
		if (frac[i]>0) continue;
		if (min_val>frac[i]) {
			index = i;
			min_val = frac[i];
		}
	}
	return index;
}

void ESCIRoundColorCorrectionMatrix(int mult, double org_cct[], int rnd_cct[])
{
	const int CCT_TABLE_SIZE = 9
;
	int		i, j, index;
	double	mult_cct[CCT_TABLE_SIZE], frac[CCT_TABLE_SIZE];
	int		sum[3];
	int		loop;

	for (i=0; i<CCT_TABLE_SIZE; i++) {
		mult_cct[i] = org_cct[i] * mult;
	}

	// round value multiplied by 'mult' off to integer.
	for (i=0; i<CCT_TABLE_SIZE; i++) {
		rnd_cct[i] = (int)floor(org_cct[i] * mult + 0.5);
	}

	loop=0;
	do {
		// If all element equal to 11, diagonal element is set to 10.
		for (i=0; i<3; i++) {
			if (	(rnd_cct[i*3]==11) &&
				(rnd_cct[i*3]==rnd_cct[i*3+1]) &&
				(rnd_cct[i*3]==rnd_cct[i*3+2])	) {
				rnd_cct[i*3+i] --;
				mult_cct[i*3+i] = rnd_cct[i*3+i];
			}
		}
		// calc. summation of each line.
		for (i=0; i<3; i++) {
			sum[i] = 0;
			for (j=0; j<3; j++) {
				sum[i] += rnd_cct[i*3+j];
			}
		}
		// calc. values rounded up or down.
		for (i=0; i<CCT_TABLE_SIZE; i++) {
			frac[i] = mult_cct[i] - rnd_cct[i];
		}

		// if summation does not equal to 'mult', adjust rounded up or down value.
		for (i=0; i<3; i++) {
			if (sum[i]<mult) {
				index = get_roundup_index(&frac[i*3], 3);
				if (index!=-1) {
					rnd_cct[i*3+index] ++;
					mult_cct[i*3+index] = rnd_cct[i*3+index];
					sum[i]++;
				}
			} else if (sum[i]>mult) {
				index = get_rounddown_index(&frac[i*3], 3);
				if (index!=-1) {
					rnd_cct[i*3+index] --;
					mult_cct[i*3+index] = rnd_cct[i*3+index];
					sum[i]--;
				}
			}
		}

	} while ((++loop<2)&&((sum[0]!=mult)||(sum[1]!=mult)||(sum[2]!=mult)));
}


ESIndexSet AnyArrayToIndexSet( ESAnyArray& anyArray )
{
	ESIndexSet index;
	for ( ESAnyArray::const_iterator it = anyArray.begin(); it != anyArray.end(); ++it ){
		const ESNumber* pNum = SafeAnyDataCPtr<ESNumber>( *it );
		if ( pNum ){
			index.insert( *pNum );
		}
	}
	return index;
}

// trim from start
ESStringA& LTrimSpace(ESStringA& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
ESStringA& RTrimSpace(ESStringA& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
ESStringA& TrimSpace(ESStringA& s)
{
	return LTrimSpace(RTrimSpace(s));
}

#ifdef WIN32
// リソースから文字列を取得する
ESErrorCode LoadStringFromResource( ESString& strOut, const ESString& strResouceType, const ESString& strResouceName )
{
	try {
		ESErrorCode err = kESErrorFatalError;

		if ( strResouceType.empty() || strResouceName.empty() || g_hES2CommandModule == NULL ){
			return err;
		}

		// リソースの検索
		HRSRC hResInfo = ::FindResourceW((HMODULE)g_hES2CommandModule, strResouceType.c_str(), strResouceName.c_str());
		if( hResInfo == NULL ){
			return err;
		}
		// サイズの取得
		DWORD dwSize = ::SizeofResource((HMODULE)g_hES2CommandModule, hResInfo);
		if( dwSize == 0 ){
			return err;
		}
		// リソースの取得
		HGLOBAL hResource = ::LoadResource((HMODULE)g_hES2CommandModule, hResInfo);
		if ( hResource == NULL ){
			return err;
		}
		{
			const char* pHead = (char*)LockResource(hResource);
			ESStringA strResouce( &pHead[0], &pHead[dwSize] );

			strOut = ES_CMN_FUNCS::STRING::UTF8ToUnincode(strResouce);

			err = kESErrorNoError;
		}
		return err;
	} catch(...){
		return kESErrorFatalError;
	}
}

ESErrorCode LoadJsonResouce( ESDictionary& dicOutput, const ESString& strResouceType, const ESString& strResouceName )
{
	ESString strJson;
	ESErrorCode err = LoadStringFromResource( strJson, strResouceType, strResouceName );
	if ( IS_ERR_CODE(err) ){
		return err;
	}

	UInt32 un32Count = ES_CMN_FUNCS::JSON::JSONtoDictionary( strJson, dicOutput );
	if ( un32Count != 0 ){
		return kESErrorFatalError;
	}
	return kESErrorNoError;
}
#endif

#ifdef WIN32
#define	SIZE_DWORD	9
ESErrorCode RegQueryDWORDValue(HKEY hKeyRoot, LPCWSTR lpszSubKey, WCHAR *pszEntry, DWORD *pdwData)
{
	ESErrorCode err = kESErrorNoError;

	HKEY hSubKey(NULL);
	LONG lRet = ::RegOpenKeyExW(hKeyRoot, lpszSubKey, 0, KEY_READ, &hSubKey);
	if (lRet == ERROR_SUCCESS){

		wchar_t	szGetData[SIZE_DWORD];
		DWORD	dwSize = SIZE_DWORD;
		DWORD	dwType = REG_SZ;

		lRet = RegQueryValueExW(hSubKey, pszEntry, NULL, &dwType, (LPBYTE)szGetData, &dwSize);
		if (lRet == ERROR_SUCCESS) {
			*pdwData = *((DWORD *)szGetData);
			err = kESErrorNoError;
		}
		else {
			err = kESErrorFatalError;
		}

		::RegCloseKey(hSubKey);
	} else {
		err = kESErrorFatalError;
	}
	return err;
}
#endif