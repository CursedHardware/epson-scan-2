#include "ScanHeightMax.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanHeightMax::SetValue(const SDIInt& intVal)
{
}	
void ScanHeightMax::Reset()
{
}

SDIInt ScanHeightMax::ADFValue()
{
	ST_ES_SIZE_F stSize={0};

	Scanner* scanner = dataProvider_->GetScanner().get();

	scanner->GetValueForKey(kESMaxScanSize, stSize, kESFunctionalUnitDocumentFeeder);
				
	return (SDIInt)(stSize.cy * 100);
}

SDIInt ScanHeightMax::FlatbedValue()
{
	ST_ES_SIZE_F stSize={0};

	Scanner* scanner = dataProvider_->GetScanner().get();

	scanner->GetValueForKey(kESMaxScanSize, stSize, kESFunctionalUnitFlatbed);
				
	return (SDIInt)(stSize.cy * 100);
}

SDIInt ScanHeightMax::AutoValue()
{
	return std::max(ADFValue(), FlatbedValue());
}

void ScanHeightMax::GetValue(SDIInt& intVal)
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

void ScanHeightMax::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

ESDictionary ScanHeightMax::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

	
}//namespace epsonscan
