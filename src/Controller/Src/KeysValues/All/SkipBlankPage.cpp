#include "SkipBlankPage.hpp"
#include "Scanner.hpp"
#include "DTR.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void SkipBlankPage::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	

void SkipBlankPage::Reset()
{
	current_  = 10;

	ESNumber currentBlankPageLevel;
	Scanner* scannerSupportBlankPageLevel = dataProvider_->GetScanner().get();
	if(scannerSupportBlankPageLevel &&  scannerSupportBlankPageLevel->GetValueForKey(kESDetectBlankPageLevel, currentBlankPageLevel)) {
		isSupportHardwareBlankPageLevel_ = true;
	}else{
		isSupportHardwareBlankPageLevel_ = false;
	}
}

void SkipBlankPage::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void SkipBlankPage::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);

	capability.minValue = 0;
	capability.maxValue = 30;

	capability.allMinValue = 0;
	capability.allMaxValue = 30;

	bool hasAdf = false;

	ESIndexSet functionalUnits = dataProvider_->GetScanner().get()->GetSupportedFunctionalUnit();
	if (functionalUnits.find(kESFunctionalUnitDocumentFeeder) != functionalUnits.end())
	{
		hasAdf = true;
	}

	if ((DTR::IsDTRAvailable() || isSupportHardwareBlankPageLevel_ == true) && hasAdf)
	{
		capability.supportLevel = kSDISupportLevelAvailable;
		//
		// Functional Unit
		// 
		if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
		{
			capability.supportLevel = kSDISupportLevelUnavailable;
			ResetRange(capability);
		}
	}else{
		capability.supportLevel = kSDISupportLevelNone;
	}
}

ESDictionary SkipBlankPage::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	if(isSupportHardwareBlankPageLevel_ == true && GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder
		&& GetValueInt(kSDIBlankPageSkipKey))
	{
		dict[kESDetectBlankPageLevel] = (ESNumber)current_;
	}
	return dict;
}	

bool SkipBlankPage::IsSupportBlankPageLevel()
{
	return isSupportHardwareBlankPageLevel_;
}
	
}//namespace epsonscan
