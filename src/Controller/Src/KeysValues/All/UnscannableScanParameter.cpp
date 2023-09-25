#include "UnscannableScanParameter.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void UnscannableScanParameter::SetValue(const SDIInt& intVal)
{

}	
void UnscannableScanParameter::Reset()
{

}

void UnscannableScanParameter::GetValue(SDIInt& intVal)
{

	auto containsObject = [](const ESIndexArray &ar, ESNumber findValue) -> bool {
		return (std::find(std::begin(ar), std::end(ar), findValue) != std::end(ar));
	};

	ESIndexArray unscanParamStatus;
	
	Scanner* scanner = dataProvider_->GetScanner().get();
	scanner->GetValueForKey(kESUnavailableScanParameterStatus, unscanParamStatus, kESFunctionalUnitDocumentFeeder);

	if (containsObject(unscanParamStatus, kESUnavailableScanParameterStatusAll)) {
		intVal = 1;
		return;

	}else{
		intVal = 0;
	}
}

void UnscannableScanParameter::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

}

ESDictionary UnscannableScanParameter::GetEngineKeyValues()
{
	auto dict =  ESDictionary();


	return dict;
}

	
}//namespace epsonscan
