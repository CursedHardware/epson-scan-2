#include "DoubleFeedDetection.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void DoubleFeedDetection::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void DoubleFeedDetection::Reset()
{
	current_  = true;
}

void DoubleFeedDetection::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void DoubleFeedDetection::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
  	capability.supportLevel = kSDISupportLevelNone;
	ESIndexSet available;

	SDICapability levelCapability;

	GetCapabilityWithKey(std::string(kSDIDoubleFeedDetectionLevelKey), levelCapability);

	if (levelCapability.supportLevel != kSDISupportLevelNone)
	{
		AddList(capability, true);
		AddList(capability, false);

		AddAllList(capability, true);
		AddAllList(capability, false);

		capability.supportLevel = kSDISupportLevelAvailable;
	}else{
		ResetList(capability);
	}

	//
	// Functional Unit
	// 
	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
	{
		if (levelCapability.supportLevel == kSDISupportLevelAvailable || levelCapability.supportLevel == kSDISupportLevelUnavailable)
		{
			capability.supportLevel = kSDISupportLevelUnavailable;
		}
		if (levelCapability.supportLevel == kSDISupportLevelNone)
		{
			capability.supportLevel = kSDISupportLevelNone;
		}
		ResetList(capability);
	}
}

ESDictionary DoubleFeedDetection::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan