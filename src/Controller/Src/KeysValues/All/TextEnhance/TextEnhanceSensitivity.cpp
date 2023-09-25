#include "TextEnhanceSensitivity.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void TextEnhanceSensitivity::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void TextEnhanceSensitivity::Reset()
{
	current_  = 0;
}

void TextEnhanceSensitivity::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void TextEnhanceSensitivity::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	capability.supportLevel= kSDISupportLevelAvailable;
}

ESDictionary TextEnhanceSensitivity::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	
	return dict;
}
	
}//namespace epsonscan
