#ifndef DEFAULTSETTINGS_H
#define DEFAULTSETTINGS_H

#include "ESJsonUtils.h"
#include "supervisor.h"
#include "configuration.h"

class DefaultSettings
{
private:
    std::string SettingsFilePath;
public:
    DefaultSettings();

    bool CheckDefaultSettingsFile();
    void DeleteDefaultSettingsFile();
    bool ReadSettingsFile(SCANPARA* device_data);
    void SetDeviceData(std::string settings_file_path, SCANPARA* device_data);
};

#endif // DEFAULTSETTINGS_H
