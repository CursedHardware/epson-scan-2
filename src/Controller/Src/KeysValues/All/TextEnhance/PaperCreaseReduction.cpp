#include "PaperCreaseReduction.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {
	 
void PaperCreaseReduction::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void PaperCreaseReduction::Reset()
{
	current_  = 1;
}

void PaperCreaseReduction::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void PaperCreaseReduction::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	capability.supportLevel= kSDISupportLevelAvailable;
}

ESDictionary PaperCreaseReduction::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	
	return dict;
}
	
}//namespace epsonscan
