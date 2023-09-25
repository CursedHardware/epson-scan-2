#include "PagesTobeScanned.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void PagesTobeScanned::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void PagesTobeScanned::Reset()
{
	current_  = 0;
}

void PagesTobeScanned::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void PagesTobeScanned::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
	capability.minValue = 0;
	capability.maxValue = 9999;

	capability.allMinValue = 0;
	capability.allMaxValue = 9999;
}

ESDictionary PagesTobeScanned::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	dict[kESPagesToBeScanned] = (ESNumber)current_;

	return dict;
}
	
}//namespace epsonscan
