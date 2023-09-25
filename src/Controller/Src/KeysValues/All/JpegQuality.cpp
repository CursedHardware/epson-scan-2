#include "JpegQuality.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void JpegQuality::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void JpegQuality::Reset()
{
	current_  = 85;
}

void JpegQuality::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void JpegQuality::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
    capability.supportLevel = kSDISupportLevelAvailable;
	capability.minValue = 0;
	capability.maxValue = 100;
	capability.allMinValue = 0;
	capability.allMaxValue = 100;
}

ESDictionary JpegQuality::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
