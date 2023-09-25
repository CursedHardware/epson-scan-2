#include "ScanAreaWidth.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanAreaWidth::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ScanAreaWidth::Reset()
{
	current_  = 11.4 * 200;
}

void ScanAreaWidth::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ScanAreaWidth::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

ESDictionary ScanAreaWidth::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	return dict;
}
	
}//namespace epsonscan
