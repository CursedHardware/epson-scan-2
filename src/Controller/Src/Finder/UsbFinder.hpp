#pragma once
#include "EpsonScan2.h"
#include "Finder.hpp"
#include <vector>

namespace epsonscan {

class UsbFinder
{
public:	

	void Start(std::vector<SDIDeviceInfo>& devices, bool searchSecondDevice = false);
};

} // namespace usbfinder

