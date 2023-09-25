#include "Finder.hpp"
#include "NetFinder.hpp"
#include "UsbFinder.hpp"
#include "Utility.hpp"
#include <functional>
#include <memory>
#include <iostream>

namespace epsonscan {
		
		Finder::Finder() : 
		usbFinder_(std::make_shared<UsbFinder>()), netFinder_(std::make_shared<NetFinder>())
		{

		}
		
		void Finder::Start(bool searchSecondDevice /*= false*/)
		{		
			usbFinder_.get()->Start(devices_, searchSecondDevice);
			if (searchSecondDevice == false)
			{
				netFinder_.get()->Reset();					
				notifyThread_ = std::make_shared<std::thread>([this]() {	

				NetFinderCallBack finderCallBack = [this](SDIDeviceInfo& device) {
					SDI_TRACE_LOG("network device found ip = %s modelID = %s displayname = %s", device.ipAddress, device.modelID, device.displayName);

					devices_.push_back(device);
				};
					netFinder_.get()->Start(finderCallBack);
				});
			}
		}

		void Finder::Stop() {
			if (notifyThread_) {
				netFinder_.get()->Stop();	
				notifyThread_.get()->join();
				notifyThread_ = nullptr;	
			}	
		}
		SDIDeviceInfo* Finder::GetDevicesPointer()
		{
			return &devices_[0];
		}
	
		int32_t  Finder::GetDevicesCount() const
		{
			return static_cast<int32_t>(devices_.size());
		}
}
