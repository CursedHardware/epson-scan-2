#include "Contrast.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void Contrast::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void Contrast::Reset()
{
	current_  = 0;

	ESNumber currentContast;
	Scanner* scannerSupportContrast = dataProvider_->GetScanner().get();
	if(scannerSupportContrast &&  scannerSupportContrast->GetValueForKey(kESContrast, currentContast)) {
		isSupportContarastCommand_ = true;
	}else{
		isSupportContarastCommand_ = false;
	}
}

void Contrast::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void Contrast::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);

	capability.supportLevel = kSDISupportLevelAvailable;

	if (GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1)
	{
		capability.minValue = 0;
		capability.maxValue = 0;
		capability.supportLevel= kSDISupportLevelUnavailable;
	}else
	{
		capability.minValue = 0;
		capability.maxValue = 100;
		capability.supportLevel= kSDISupportLevelAvailable;
	}
	
	capability.allMinValue = 0;
	capability.allMaxValue = 100;


}

ESDictionary Contrast::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	if(isSupportContarastCommand_) {
		dict[kESContrast] = (ESNumber)current_;
	}

	return dict;
}

bool Contrast::IsSupportContrastCommand()
{
	return isSupportContarastCommand_;
}
	
}//namespace epsonscan
