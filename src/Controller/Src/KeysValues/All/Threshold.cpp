#include "Threshold.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void Threshold::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void Threshold::Reset()
{
	current_  = 110;
}

void Threshold::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void Threshold::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	capability.supportLevel= kSDISupportLevelAvailable;

	if (GetValueInt(kSDIColorTypeKey) != kSDIColorTypeMono1)
	{
		capability.minValue = 0;
		capability.maxValue = 0;
		capability.supportLevel= kSDISupportLevelUnavailable;
	}

	capability.allMinValue = 0;
	capability.allMaxValue = 255;
}

ESDictionary Threshold::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	
	return dict;
}
	
}//namespace epsonscan
