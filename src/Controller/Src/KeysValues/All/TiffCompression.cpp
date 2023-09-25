#include "TiffCompression.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void TiffCompression::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void TiffCompression::Reset()
{
	current_  = false;
}

void TiffCompression::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void TiffCompression::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	AddList(capability, kSDITiffCompressionNone);
	AddList(capability, kSDITiffCompressionCCITFAX4);

	AddAllList(capability, kSDITiffCompressionNone);
	AddAllList(capability, kSDITiffCompressionCCITFAX4);
}

ESDictionary TiffCompression::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
