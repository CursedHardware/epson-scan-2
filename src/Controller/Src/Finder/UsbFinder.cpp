#include "UsbFinder.hpp"
#include "Error.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include "Utility.hpp"
extern "C" {                    // needed until libudev-150
#include <libusb-1.0/libusb.h>
}
#include <list>
#include <iostream>

namespace epsonscan {

static bool matchProductIDWithFlatbedOption(uint32_t productID)
{
		int j = 0;
		while(k2in1FlatbedOptionVID[j] != 0x0)
		{
			if(productID == k2in1FlatbedOptionVID[j])
			{	
				// 2 in 1 supported
                return true;
				break;
			}

			j++;
		}

    return false;
}

void UsbFinder::Start(std::vector<SDIDeviceInfo>& devices, bool searchSecondDevice)
{
	try {
    	
      libusb_context* ctx   = nullptr;
      bool has2in1SupportDevice = false;

	 if(libusb_init(&ctx) == 0)
     {
        libusb_device **haystack;
        ssize_t cnt = libusb_get_device_list(ctx, &haystack);

        for (ssize_t i = 0; i < cnt; i++){
                struct libusb_device_descriptor dd;
            if (libusb_get_device_descriptor (haystack[i], &dd) == 0)
            {
                SDI_TRACE_LOG("USB device found VID = %d PID = %d", dd.idVendor, dd.idProduct);
                if (dd.idVendor == kEpsonUSBVID)
                {
                    std::string modelID;
                    if ( (searchSecondDevice &&  matchProductIDWithFlatbedOption(dd.idProduct) ) ||
                         ModelInfo::GetModelIDFromProductID(dd.idProduct, modelID))
                    {

                        SDIDeviceInfo devInfo = {0};
                        devInfo.productID = dd.idProduct;

                        bool isValidDeviceInfo = false;
                        strncpy(const_cast<char*>(devInfo.modelID), modelID.c_str(), MAX_MODEL_ID - 1);


                        if (searchSecondDevice == false)
                        {
                            ModelInfo  modelInfo(devInfo);
                                    
                            ESNumber isSupported = 0;
                            modelInfo.GetValue(kPrvHRD_FlatbedOption, isSupported);
                            if (isSupported)
                            {
                                has2in1SupportDevice = true;
                            }
                        }

                        {
                            libusb_device_handle *devHandle = NULL;
                            uint8_t strDescWithAddress[300];
                            uint8_t busNumber = libusb_get_bus_number(haystack[i]);
                            uint8_t deviceNumber = libusb_get_device_address(haystack[i]);

                            if (dd.idProduct == 0x13A )
                            {
                                sprintf(const_cast<char*>(devInfo.displayName), "%s:%03d:%03d", "Perfection V600/GT-X820", busNumber, deviceNumber);
                                 isValidDeviceInfo = true;

                            }else if (dd.idProduct  == 0x13D)
                            {
                                 sprintf(const_cast<char*>(devInfo.displayName), "%s:%03d:%03d", "Perfection V39/GT-S650", busNumber, deviceNumber);
                                  isValidDeviceInfo = true;

                            }else if (dd.idProduct == 0x13C)
                            {
                                 sprintf(const_cast<char*>(devInfo.displayName), "%s:%03d:%03d", "Perfection V19", busNumber, deviceNumber);
                                 isValidDeviceInfo = true;
                            }else    
                            if (dd.idProduct == 0x146)
                            {
                                sprintf(const_cast<char*>(devInfo.displayName), "%s:%03d:%03d", "DS-50000/DS-60000/DS-70000", busNumber, deviceNumber);
                                isValidDeviceInfo = true;
                            }
                            else if (dd.idProduct == 0x145)
                            {
                                sprintf(const_cast<char*>(devInfo.displayName), "%s:%03d:%03d", "DS-5500/DS-6500/DS-7500", busNumber, deviceNumber);
                                isValidDeviceInfo = true;
                            }
                            else if(libusb_open (haystack[i], &devHandle) == 0) {

                                uint8_t strDesc[256];
                                //uint8_t strDesc2[256];

                                strncpy((char*)strDesc, "Scanner", 255);

                                libusb_get_string_descriptor_ascii(devHandle, dd.iProduct, strDesc, 256);
                                uint8_t busNumber = libusb_get_bus_number(haystack[i]);
                                uint8_t deviceNumber = libusb_get_device_address(haystack[i]);
                                //libusb_get_string_descriptor_ascii(devHandle, dd.iSerialNumber, strDesc2, 256);

                                //sprintf((char*)strDescWithAddress, "%s:%03d:%03d %s", strDesc, busNumber, deviceNumber, strDesc2);
                                sprintf((char*)strDescWithAddress, "%s:%03d:%03d", strDesc, busNumber, deviceNumber);

                                 strncpy(const_cast<char*>(devInfo.displayName),(const char*)strDescWithAddress, MAX_DISPLAYNAME - 1);

                                 if (strlen(devInfo.displayName) != 0 || searchSecondDevice)
                                 {
                                     isValidDeviceInfo = true;
                                 }

                                libusb_close (devHandle);
                                devHandle = nullptr;
                            }
                        }

                        if (isValidDeviceInfo)
                        {
                            devices.push_back(devInfo);
                        }
                    }
                }
            } 
        }
        libusb_free_device_list(haystack, 1);
     }else{
         SDI_TRACE_LOG("LibUSB Init fail");
     }
    
    //2in1 デバイスがある場合にはFlatbed Optionを消す    
    if (has2in1SupportDevice && searchSecondDevice == false)
    {
        std::vector<SDIDeviceInfo> newDevices = devices;
        devices.clear();

        for (SDIDeviceInfo devInfo2 : newDevices)
        {
            if(!matchProductIDWithFlatbedOption(devInfo2.productID))
            {
                devices.push_back(devInfo2);
            }
        }
    }

	libusb_exit(ctx);

	}catch(boost::exception& e) {
		std::cout << diagnostic_information(e) << std::endl; 
	}	
}

}
