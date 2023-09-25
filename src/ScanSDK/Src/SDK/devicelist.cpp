#include "devicelist.h"

std::list<SDIDeviceInfo> DeviceList::device_list;
std::list<SDIDeviceInfo> DeviceList::manu_network_device_list;
int DeviceList::select_device;
int DeviceList::select_manu_ip_address;

DeviceList::DeviceList()
{
}

DeviceList::~DeviceList()
{
    if(DeviceList::device_list.size() != 0){
        DeviceList::device_list.clear();
    }
    if(DeviceList::manu_network_device_list.size() != 0){
        DeviceList::manu_network_device_list.clear();
    }
}

SDIDeviceInfo DeviceList::list(const char* search_device, bool use_manual_ip)
{
    SDIDeviceInfo target_device_info;
    memset(&target_device_info, 0, sizeof(target_device_info));
    Supervisor* sv = new Supervisor();
    if(sv == NULL){
        memcpy(target_device_info.displayName, "", strlen(""));
        return target_device_info;
    }

    sv->SetUp();

    SDIDeviceFinder* finder = nullptr;
    sv->SDIDeviceFinder_CreatePtr_(&finder);
    sv->SDIDeviceFinder_StartDiscoveryPtr_(finder, nullptr, nullptr);
    sleep(DEVICE_SEARCH_TIME);
    sv->SDIDeviceFinder_StopDiscoveryPtr_(finder);
    SDIDeviceInfo* devices = nullptr;
    SDIInt count = 0;
    sv->SDIDeviceFinder_GetDevicesPtr_(finder, &devices, &count);

    if(use_manual_ip){
        if(search_device){
            //手動ＩＰアドレス追加機種の場合
            SDIDeviceInfo devInfo;
            memset(devInfo.ipAddress, 0, MAX_IP_ADDR);
            if(strlen(search_device) <= MAX_IP_ADDR){
                memcpy(devInfo.ipAddress, search_device, strlen(search_device));
            }
            sv->SANEManuNetfinder();
            if(!sv->DeviceInfoResolve(&devInfo)){
                std::cout << " Device is not found..." << std::endl;
                std::cout << "" << std::endl;
                memcpy(target_device_info.displayName, "", strlen(""));
            }else{
                target_device_info = devInfo;
                target_device_info.productID = 0;
                target_device_info.version = 0;
            }
        }
    }else {
        for (int i = 0; i < count; i++) {
            SDIDeviceInfo devInfo = devices[i];
            //std::cout << " Device found :"  <<  devInfo.productID << " " << search_device << " " << devInfo.displayName <<  std::endl;

            if(i == 0){
                if(search_device){
                    memcpy(target_device_info.displayName, "", strlen(""));
                }else {
                    //デバイスの指定がない場合は最初に見つかったデバイスを確保する
                    target_device_info = devInfo;
                }
            }
            if(search_device){
                if(devInfo.ipAddress[0] == '\0'){
                    //USB connection
                    if(strncmp(devInfo.displayName, search_device, strlen(search_device)) == 0){
                        target_device_info = devInfo;
                    }
                }else {
                    //Network connection
                    if(strcmp(devInfo.ipAddress, search_device) == 0){
                        target_device_info = devInfo;
                    }
                }
            }
        }
    }
    sv->SDIDeviceFinder_DisposePtr_(finder);
    finder = nullptr;

    sv->Terminate();
    delete sv;
    sv = nullptr;

    return target_device_info;
}

void DeviceList::show_list(void)
{
    Supervisor* sv = new Supervisor();
    if(sv == NULL){
        return;
    }
    sv->SetUp();

    SDIDeviceFinder* finder = nullptr;
    sv->SDIDeviceFinder_CreatePtr_(&finder);

    sv->SDIDeviceFinder_StartDiscoveryPtr_(finder, nullptr, nullptr);
    sleep(DEVICE_SEARCH_TIME);
    sv->SDIDeviceFinder_StopDiscoveryPtr_(finder);

    SDIDeviceInfo* devices = nullptr;
    SDIInt count = 0;
    sv->SDIDeviceFinder_GetDevicesPtr_(finder, &devices, &count);

    if(count == 0){
        std::cout << " Device is not found..." << std::endl;
        std::cout << "" 	<< std::endl;
    }else {
        std::cout << " === List of available devices ==" << std::endl;
        for (int i = 0; i < count; i++) {
            SDIDeviceInfo devInfo = devices[i];

            if(devInfo.ipAddress[0] == '\0'){
                //USB connection
                std::cout << "device ID :" << devInfo.displayName << std::endl;
                std::cout << "ModelID:"		 << devInfo.modelID 	<< std::endl;
                std::cout << "" 	<< std::endl;
            }else {
               //Network connection
                std::cout << "ipAdder :"     << devInfo.ipAddress   << std::endl;
                //std::cout << "productID :"   << devInfo.productID   << std::endl;
                std::cout << "ModelID:"		 << devInfo.modelID 	<< std::endl;
                std::cout << "" 	<< std::endl;
            }
        }
    }

    sv->SDIDeviceFinder_DisposePtr_(finder);
    finder = nullptr;

    sv->Terminate();
    delete sv;
    sv = nullptr;
}
