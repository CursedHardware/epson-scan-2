#ifndef COMMANDLINEMODE_H
#define COMMANDLINEMODE_H

#include "supervisor.h"
#ifndef NO_QTMODE
#include "networkdeviceselectdialog.h"
#else
#include <fstream>
#endif


class CommandLineMode
{
public:
    CommandLineMode();

    bool SelectMode(int option_num, const char** option);
    bool SelectEditMode(int option_num, const char** option);

    int IsGetStatus();

private:

    int m_DeviceStatus;

    void ShowHelp();
    void ListDevices();
    void GetStatus(const char* target_device);
    void SetIPAddress(const char* ip_address);
    void ScanStart(const char* target_device, const char* settings_file_path);
    bool ValidateScanOption(int option_num, const char** option);
    bool ValidateGetStatusOption(int option_num, const char** option);
    void ErrorMessage(SDIError outError);
    bool ValidateEditOption(int option_num, const char** option);

    static void cancel_sig_handler(int sig);
    void UpdateScanArea(SCAN_AREA_VALUE& scan_area_parameter, int resolution);
    double calculate_pixel_size(double value, int res);
    bool search_item_list(int item, int32_t item_list[], int32_t list_size);

    void CreateDefaultSettingFile(const char* target_device, const char* settings_file_path);
};

#endif // COMMANDLINEMODE_H
