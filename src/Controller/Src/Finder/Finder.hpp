#pragma once
#include "EpsonScan2.h"
#include <memory>
#include <thread>
#include <vector>

namespace epsonscan
{
	
	class UsbFinder;
	class NetFinder;
	class Finder
	{
		public:
				Finder();
				void Start(bool searchSecondDevice = false);
				void Stop();
				SDIDeviceInfo* GetDevicesPointer();
				int32_t        GetDevicesCount() const;
		private:	
				std::shared_ptr<UsbFinder> usbFinder_;
				std::shared_ptr<NetFinder> netFinder_;		
				std::shared_ptr<std::thread> notifyThread_;
				std::vector<SDIDeviceInfo> devices_;
	};
}
