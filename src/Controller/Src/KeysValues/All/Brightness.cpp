#include "Brightness.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>



namespace epsonscan {

void Brightness::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void Brightness::Reset()
{
	current_  = 0;
	
	ESNumber currentBrightness;
	Scanner* scannerSupportBrightness = dataProvider_->GetScanner().get();
	if(scannerSupportBrightness &&  scannerSupportBrightness->GetValueForKey(kESBrightness, currentBrightness)) {
		isSupportBrigthenessCommand_ = true;
	}else{
		isSupportBrigthenessCommand_ = false;
	}
}

void Brightness::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void Brightness::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
 
	capability.supportLevel= kSDISupportLevelAvailable;

	ESIndexSet availableBrightnessTypes;
	Scanner* scanner = dataProvider_->GetScanner().get();

	if (GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1)
	{
		capability.minValue = 0;
		capability.maxValue = 0;
		capability.supportLevel = kSDISupportLevelUnavailable;
	}else
	{
		capability.minValue = 0;
		capability.maxValue = 100;
		capability.supportLevel= kSDISupportLevelAvailable;
	}

	capability.allMinValue = 0;
	capability.allMaxValue = 100;
}

ESDictionary Brightness::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	if(isSupportBrigthenessCommand_) {
		dict[kESBrightness] = (ESNumber)current_;
	}

	return dict;
}
	
bool Brightness::IsSupportBrightnessCommand()
{
	return isSupportBrigthenessCommand_;
}

}//namespace epsonscan
