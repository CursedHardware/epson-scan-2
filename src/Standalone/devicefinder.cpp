#include "devicefinder.h"
#include "devicelist.h"
#include "mainwindow.h"
#include "../Include/Config.h"
#include "loadnetworksetting.h"

#include <fstream>
#include <iostream>
#include "PathUtils.h"

DeviceFinder::DeviceFinder()
{
    wait_dialog = NULL;
}

void DeviceFinder::finder(QWidget *parent)
{
    Supervisor* sv = new Supervisor();
    if(sv == NULL){
        return;
    }
    sv->SetUp();

    SDIDeviceFinder* finder = nullptr;
    sv->SDIDeviceFinder_CreatePtr_(&finder);

    sv->SDIDeviceFinder_StartDiscoveryPtr_(finder, nullptr, nullptr);
    if(!wait_dialog){
        if(parent == 0){
            wait_dialog = new WaitingDialog((TranslationString::GetString().translation_str.LINUX_DEVICESELECT_003.toLocal8Bit()).constData(), DEVICE_SEARCH_TIME);
        }else {
            wait_dialog = new WaitingDialog((TranslationString::GetString().translation_str.LINUX_DEVICESELECT_003.toLocal8Bit()).constData(), DEVICE_SEARCH_TIME, parent);
        }
        delete wait_dialog;
    }
    sv->SDIDeviceFinder_StopDiscoveryPtr_(finder);

    SDIDeviceInfo* devices = nullptr;
    SDIInt count = 0;
    sv->SDIDeviceFinder_GetDevicesPtr_(finder, &devices, &count);

    //std::cout << " === dump devices start ==" << std::endl;

    DeviceList::device_list.clear();

    for (int i = 0; i < count; i++) {
        DeviceList::device_list.push_back(devices[i]);
      //  SDIDeviceInfo devInfo = devices[i];
      //  std::cout << "displayname :" << devInfo.displayName << std::endl;
       // std::cout << "ipAdder :"     << devInfo.ipAddress   << std::endl;
       // std::cout << "productID :"   << devInfo.productID   << std::endl;
      //  std::cout << "ModelID:"		 << devInfo.modelID 	<< std::endl;
    }

    //std::cout << " === dump devices fin ==" << std::endl;

    sv->SDIDeviceFinder_DisposePtr_(finder);
    finder = nullptr;

    sv->Terminate();

    delete sv;
    sv = nullptr;
}

bool DeviceFinder::ManuNetfinder()
{
    DeviceList::select_manu_ip_address = -1;
    GetNetworkDevcicesFromIni(DeviceList::manu_network_device_list);
    return true;
}
