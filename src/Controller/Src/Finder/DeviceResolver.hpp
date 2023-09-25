#pragma once
#include "EpsonScan2.h"
#include <memory>
#include <thread>
#include <vector>

namespace epsonscan
{
	class DeviceResolver
	{
		public:
				DeviceResolver()
                {

                }
                bool Resolve(SDIDeviceInfo& devInfo);
		private:	

	};
}
