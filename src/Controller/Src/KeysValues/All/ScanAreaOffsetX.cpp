#include "ScanAreaOffsetX.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanAreaOffsetX::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ScanAreaOffsetX::Reset()
{
	current_  = 0;
}

void ScanAreaOffsetX::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ScanAreaOffsetX::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

ESDictionary ScanAreaOffsetX::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

	
}//namespace epsonscan
