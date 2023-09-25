#ifndef DEVICELIST_H
#define DEVICELIST_H

#include "supervisor.h"

class DeviceList
{
public:
    DeviceList();
    ~DeviceList();

    static std::list<SDIDeviceInfo> device_list;
    static std::list<SDIDeviceInfo> manu_network_device_list;
    static int select_device;
    static int select_manu_ip_address;

    SDIDeviceInfo list(const char* search_device, bool use_manual_ip=false);
    void show_list(void);
};

#endif // DEVICELIST_H
