#include "stdafx.h"
#include "EmulatorSettings.h"


EmulatorSettings::EmulatorSettings(void)
{
}


EmulatorSettings::~EmulatorSettings(void)
{
}


//EmulatorControllerのあるディレクトリ
void EmulatorSettings::SetEmulatorControllerDir(ESString strCtrlerDir)
{
	m_strCtrlerDir = strCtrlerDir;
}

//EmulatorControllerのあるディレクトリ
ESString EmulatorSettings::GetEmulatorControllerDir()
{
	return m_strCtrlerDir;
}

#ifdef WIN32
//EmulatorSetting.iniのフルパス
ESString EmulatorSettings::GetEmulatorSettingIniPath()
{
	if (!ES_CMN_FUNCS::PATH::ES_IsExistFile(m_strSettingIniPath))
	{
		ES_CMN_FUNCS::PATH::ES_CombinePath(m_strSettingIniPath, GetEmulatorControllerDir(), ES_STRING("DummyInfo"));
		ES_CMN_FUNCS::PATH::ES_CombinePath(m_strSettingIniPath, m_strSettingIniPath, ES_STRING("EmulatorSetting.ini"));
	}

	return m_strSettingIniPath;
}

/*!
 * @brief	CreateDummyInfoModeの表示/非表示
 * @defalut	0(非表示)
 * @return	TRUE：表示 FALSE：非表示
 */
BOOL EmulatorSettings::IsEnabledCreateDummyInfoMode()
{
	ESString strINIfilePath = GetEmulatorSettingIniPath();
	UINT nEnabledDummyInfo = GetPrivateProfileInt(ES_STRING("function"), ES_STRING("EnableCreateDummyInfo"), 0, strINIfilePath.c_str());

	return nEnabledDummyInfo != 0;
}

/*!
 * @brief	ScanImageCreatorで作った画像を使い回すかどうか
 * @defalut	0(使い回さない)
 * @return	TRUE：使い回す FALSE：使い回さない
 */
BOOL EmulatorSettings::IsScanSpeedUp()
{
	ESString strINIfilePath = GetEmulatorSettingIniPath();
	UINT nScanSpeedUp = GetPrivateProfileIntW(ES_STRING("scan"), ES_STRING("ScanSpeedUp "), 0, strINIfilePath.c_str());

	return nScanSpeedUp != 0;
}

/*!
 * @brief	ScanImageCreatorで作った画像にページ番号等の情報を付加させるか
 * @defalut	1(付加させる)
 * @return	TRUE：付加させる FALSE：付加させない
 */
BOOL EmulatorSettings::IsIndicatedPageNumber()
{
	ESString strINIfilePath = GetEmulatorSettingIniPath();
	UINT nIndicate = GetPrivateProfileIntW(ES_STRING("scan"), ES_STRING("IndicatedPageNumber "), 1, strINIfilePath.c_str());

	return nIndicate != 0;
}

/*!
 * @brief	ErrorCode機能の表示/非表示
 * @defalut	0(非表示)
 * @return	TRUE：表示 FALSE：非表示
 */
BOOL EmulatorSettings::IsShowErrorCode()
{
	ESString strPath = GetEmulatorSettingIniPath();
	UINT nErrCode = GetPrivateProfileIntW(ES_STRING("function"), ES_STRING("errcode"), 0, strPath.c_str());

	if (nErrCode != 0)
	{
		return TRUE;
	}
	return FALSE;
}

/*!
 * @brief	NickName画像の表示/非表示
 * @defalut	0(非表示)
 * @return	TRUE：表示 FALSE：非表示
 */
BOOL EmulatorSettings::IsShowNickNameImage()
{
	ESString strPath = GetEmulatorSettingIniPath();
	UINT nNickName = GetPrivateProfileIntW(ES_STRING("function"), ES_STRING("nickname"), 0, strPath.c_str());

	if (nNickName != 0)
	{
		return TRUE;
	}
	return FALSE;
}

/*!
 * @brief	起動時にPaperLoadedとするかどうか（ADFのみ）
 * @defalut	0(Loadしない)
 * @return	TRUE：Loadする FALSE：Loadしない
 */
BOOL EmulatorSettings::IsInitialLoadPaper()
{
	ESString strPath = GetEmulatorSettingIniPath();
	UINT nLoad = GetPrivateProfileIntW(ES_STRING("paperLoading"), ES_STRING("LoadingPaper"), 0, strPath.c_str());

	return nLoad != 0;
}

/*!
 * @brief	起動時のLoad枚数（ADFのみ）
 * @Range	0～65535 [枚]
 * @return	Load枚数
 */
INT  EmulatorSettings::GetInitialLoadPaperNum()
{
	ESString strPath = GetEmulatorSettingIniPath();
	UINT nLoadNum = GetPrivateProfileIntW(ES_STRING("paperLoading"), ES_STRING("LoadingPaperNum"), 0, strPath.c_str());

	return nLoadNum;
}

/*!
 * @brief	Scanする毎に給紙枚数を減らすかどうか（ADFのみ）
 * @defalut	0(減らす)
 * @return	TRUE：減らさない FALSE：減らす
 */
BOOL EmulatorSettings::IsEnableDecresingPaper()
{
	ESString strPath = GetEmulatorSettingIniPath();
	UINT nDisableDecresing = GetPrivateProfileIntW(ES_STRING("paperLoading"), ES_STRING("DisableDecreseLoadingPaper "), 0, strPath.c_str());

	return nDisableDecresing == 0;
}

/*!
 * @brief	PPM設定パラメータ機能の表示/非表示
 * @defalut	0(非表示)
 * @return	TRUE：表示 FALSE：非表示
 */
BOOL EmulatorSettings::IsEnablePagePerMinute()
{
	ESString strPath = GetEmulatorSettingIniPath();
	UINT nPPM = GetPrivateProfileIntW(ES_STRING("function"), ES_STRING("PagePerMinute"), 0, strPath.c_str());

	return nPPM != 0;
}
#endif
