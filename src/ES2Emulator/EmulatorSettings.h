#pragma once
class EmulatorSettings
{
public:
	EmulatorSettings(void);
	virtual ~EmulatorSettings(void);

	void SetEmulatorControllerDir(ESString strCtrlerDir);
	ESString GetEmulatorControllerDir();
	ESString GetEmulatorSettingIniPath();

	//CreateDummyInfoModeの表示/非表示
	BOOL IsEnabledCreateDummyInfoMode();
	//スキャン画像の使い回しをするかしないか
	BOOL IsScanSpeedUp();
	//スキャン画像にページ番号等の情報を表示させるかしないか
	BOOL IsIndicatedPageNumber();
	BOOL IsShowErrorCode();
	BOOL IsShowNickNameImage();
	BOOL IsInitialLoadPaper();
	INT  GetInitialLoadPaperNum();
	BOOL IsEnableDecresingPaper();
	BOOL IsEnablePagePerMinute();

private:
	ESString m_strCtrlerDir;
	ESString m_strSettingIniPath;
};
