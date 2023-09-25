#include "DoubleFeedDetectionLevel.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>

namespace epsonscan {

void DoubleFeedDetectionLevel::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void DoubleFeedDetectionLevel::Reset()
{
	current_  = kSDIDoubleFeedDetectionLow;
	adf_ = nullptr;
}

void DoubleFeedDetectionLevel::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void DoubleFeedDetectionLevel::GetADFCapability(SDICapability &capability)
{	
	if (adf_)
	{
		capability = *adf_;
		return;
	}

	std::unique_ptr<SDICapability> adfCap(new SDICapability());
	adfCap->supportLevel = kSDISupportLevelNone;
	ESIndexSet available;

	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESDoubleFeedDetection, available, kESFunctionalUnitDocumentFeeder)) {

			if (available.find(kESDoubleFeedDetectionLow) != available.end())
			{
				AddList(*adfCap, kSDIDoubleFeedDetectionLow);
				AddAllList(*adfCap, kSDIDoubleFeedDetectionLow);
				adfCap->supportLevel = kSDISupportLevelAvailable;
			}

			if (available.find(kESDoubleFeedDetectionVeryLow) != available.end())
			{
				AddList(*adfCap, kESDoubleFeedDetectionVeryLow);
				AddAllList(*adfCap, kESDoubleFeedDetectionVeryLow);
				adfCap->supportLevel = kSDISupportLevelAvailable;
			}

			if (available.find(kESDoubleFeedDetectionHigh) != available.end())
			{
				AddList(*adfCap, kSDIDoubleFeedDetectionHigh);
				AddAllList(*adfCap, kSDIDoubleFeedDetectionHigh);
				adfCap->supportLevel = kSDISupportLevelAvailable;
			}
	}
	capability = *adfCap;
	adf_.swap(adfCap);
}

void DoubleFeedDetectionLevel::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder)
	{
		GetADFCapability(capability);
	}
	else if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
	{
		SDICapability adfCap;
		GetADFCapability(adfCap);

		if (adfCap.supportLevel == kSDISupportLevelAvailable)
		{
			capability.supportLevel = kSDISupportLevelUnavailable;
			ResetList(capability);
		}else{
			capability.supportLevel = kSDISupportLevelNone;
		}
	}else{
		return;
	}
}

ESDictionary DoubleFeedDetectionLevel::GetEngineKeyValues()
{
	auto dict =  ESDictionary();


	SDICapability cap;
	GetCapability(cap);

	if (cap.supportLevel == kSDISupportLevelUnavailable)
	{
		return dict;
	}

	dict[kESDoubleFeedDetection] = (ESNumber)kESDoubleFeedDetectionDisable;

	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder &&
		GetValueInt(kSDIDoubleFeedDetectionKey) == true)
	{	

		if (current_ == kSDIDoubleFeedDetectionVeryLow)
		{
			dict[kESDoubleFeedDetection] = (ESNumber)kESDoubleFeedDetectionVeryLow;
		}

		if (current_ == kSDIDoubleFeedDetectionLow)
		{
			dict[kESDoubleFeedDetection] = (ESNumber)kESDoubleFeedDetectionLow;
		}

		if (current_ == kSDIDoubleFeedDetectionHigh)
		{
			dict[kESDoubleFeedDetection] = (ESNumber)kESDoubleFeedDetectionHigh;
		}
	}

	return dict;
}
	
}//namespace epsonscan
