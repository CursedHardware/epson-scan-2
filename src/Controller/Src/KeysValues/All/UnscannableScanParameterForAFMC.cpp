#include "UnscannableScanParameterForAFMC.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void UnscannableScanParameterForAFMC::SetValue(const SDIInt& intVal)
{

}	
void UnscannableScanParameterForAFMC::Reset()
{

}

void UnscannableScanParameterForAFMC::GetValue(SDIInt& intVal)
{

	auto containsObject = [](const ESIndexArray &ar, ESNumber findValue) -> bool {
		return (std::find(std::begin(ar), std::end(ar), findValue) != std::end(ar));
	};

	ESIndexArray unscanParamStatus;
	
	Scanner* scanner = dataProvider_->GetScanner().get();
	scanner->GetValueForKey(kESUnavailableScanParameterStatus, unscanParamStatus, kESFunctionalUnitDocumentFeeder);

	if (containsObject(unscanParamStatus, kESUnavailableScanParameterStatusAFMC)) {
		intVal = 1;
		return;
	}else{
		intVal = 0;
	}
}

void UnscannableScanParameterForAFMC::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

}

ESDictionary UnscannableScanParameterForAFMC::GetEngineKeyValues()
{
	auto dict =  ESDictionary();


	return dict;
}

	
}//namespace epsonscan
