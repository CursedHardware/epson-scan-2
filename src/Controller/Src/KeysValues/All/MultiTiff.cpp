#include "MultiTiff.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void MultiTiff::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void MultiTiff::Reset()
{
	current_  = false;
}

void MultiTiff::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void MultiTiff::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	
	AddList(capability, true);
	AddList(capability, false);

	AddAllList(capability, true);
	AddAllList(capability, false);

}

ESDictionary MultiTiff::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
