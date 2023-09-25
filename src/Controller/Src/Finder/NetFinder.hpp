#pragma once
#include <functional>
#include <mutex>
#include "EpsonScan2.h"

namespace epsonscan {

		typedef std::function<void(SDIDeviceInfo&)> NetFinderCallBack;
 		class NetFinder 
		{
			public:
				NetFinder();	
				void Reset();
				void Start(NetFinderCallBack &didFindDeviceCallBack); 
				void Stop();

			private:				
				std::mutex mutex_;
				bool cancelled_;
				void SetCancelled(bool cancelled);
		};

} // mysample
