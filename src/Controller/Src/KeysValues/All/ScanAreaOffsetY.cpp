#include "ScanAreaOffsetY.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanAreaOffsetY::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ScanAreaOffsetY::Reset()
{
	current_  = 0;
}

void ScanAreaOffsetY::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ScanAreaOffsetY::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

ESDictionary ScanAreaOffsetY::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	return dict;
}
	
}//namespace epsonscan
