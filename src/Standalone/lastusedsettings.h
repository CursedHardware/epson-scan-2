#ifndef LASTUSEDSETTINGS_H
#define LASTUSEDSETTINGS_H

#include "ESJsonUtils.h"
#include "supervisor.h"
#include "configuration.h"

class LastUsedSettings
{
private:
    std::string PreferredInfoFilePath;
    std::string SettingsFilePath;
public:
    LastUsedSettings();

    //前回接続デバイス関連処理
    bool CheckLastUsedDevice();
    bool ReadPreferredInfo();
    bool SelectUSBDevice(std::string display_name);
    bool SelectNetworkDevice(std::string ip);
    void CreatePreferredInfo(SDIDeviceInfo dev_info);
    void DeletePreferredInfo(void);

    //CommonSettingsファイル関連
    bool CheckCommonSettingsFile();
    CONFIGURATION ReadCommonSettingsFile();
    void CreateCommonSettingsFile(Configuration& config);
    void DeleteCommonSettingsFile();
    CONFIGURATION ReadMainWindowInfo();

    //前回設定の保存関連
    bool CreateSettingsFile(std::string model_id, const SCANPARA device_data);
    void CreateSettingsDict(std::string user_settngs_file, const SCANPARA device_data);
    bool ReadSettingsFile(std::string model_id, SCANPARA* device_data);

    bool ReadCmdSettingsFile(std::string settings_file_path, SCANPARA* device_data);
    void SetDeviceData(std::string settings_file_path, SCANPARA* device_data);
};

#endif // LASTUSEDSETTINGS_H
