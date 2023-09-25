#include "ThresholdAdjustment.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ThresholdAdjustment::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ThresholdAdjustment::Reset()
{
	current_  = 0;
}

void ThresholdAdjustment::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ThresholdAdjustment::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	capability.supportLevel= kSDISupportLevelAvailable;
}

ESDictionary ThresholdAdjustment::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	
	return dict;
}
	
}//namespace epsonscan
