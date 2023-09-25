#include "NoiseReductionLevel.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void NoiseReductionLevel::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void NoiseReductionLevel::Reset()
{
	current_  = 1;
}

void NoiseReductionLevel::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void NoiseReductionLevel::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	capability.supportLevel= kSDISupportLevelAvailable;
}

ESDictionary NoiseReductionLevel::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	
	return dict;
}
	
}//namespace epsonscan
