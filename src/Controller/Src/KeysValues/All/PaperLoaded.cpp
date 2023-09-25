#include "PaperLoaded.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void PaperLoaded::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void PaperLoaded::Reset()
{
	current_  = 0;
}

void PaperLoaded::GetValue(SDIInt& intVal)
{
	Scanner* scanner = dataProvider_->GetScanner().get();

	 bool isLoaded = 0;

	scanner->GetValueForKey(kESDocumentLoaded, isLoaded, kESFunctionalUnitDocumentFeeder);
	SDI_TRACE_LOG("isLoaded = %d", intVal);;

	current_ = isLoaded;

	intVal = current_;
}

void PaperLoaded::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
}

ESDictionary PaperLoaded::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
