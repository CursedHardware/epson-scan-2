#include "ScanWidthMax.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanWidthMax::SetValue(const SDIInt& intVal)
{
}	
void ScanWidthMax::Reset()
{
}

SDIInt ScanWidthMax::ADFValue()
{
	ST_ES_SIZE_F stSize={0};

	Scanner* scanner = dataProvider_->GetScanner().get();

	scanner->GetValueForKey(kESMaxScanSize, stSize, kESFunctionalUnitDocumentFeeder);
				
	return (SDIInt)(stSize.cx * 100);
}

SDIInt ScanWidthMax::FlatbedValue()
{
	ST_ES_SIZE_F stSize={0};

	Scanner* scanner = dataProvider_->GetScanner().get();

	scanner->GetValueForKey(kESMaxScanSize, stSize, kESFunctionalUnitFlatbed);
				
	return (SDIInt)(stSize.cx * 100);
}

SDIInt ScanWidthMax::AutoValue()
{
	return std::max(ADFValue(), FlatbedValue());
}

void ScanWidthMax::GetValue(SDIInt& intVal)
{
	SDIFunctionalUnit func = (SDIFunctionalUnit)GetValueInt(kSDIFunctionalUnitKey);
	switch(func)
	{
		case kSDIFunctionalUnitFlatbed:
			intVal =  FlatbedValue();
			return;

		case kSDIFunctionalUnitDocumentFeeder:
		 	intVal = ADFValue();
			return;
	}
}

void ScanWidthMax::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

ESDictionary ScanWidthMax::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

	
}//namespace epsonscan
