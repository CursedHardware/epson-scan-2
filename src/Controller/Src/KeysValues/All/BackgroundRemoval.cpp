#include "BackgroundRemoval.hpp"
#include "Scanner.hpp"
#include "Lut.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void BackgroundRemoval::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void BackgroundRemoval::Reset()
{
	ESNumber kind = kPrvHRD_Kind_SF_Document;
	dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind);
	if(kind == kPrvHRD_Kind_MF_LFP)
	{
		current_  = kSDIBackgroundRemovalNormal;
	}else{
		current_  = kSDIBackgroundRemovalNone;
	}
}

void BackgroundRemoval::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void BackgroundRemoval::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	capability.supportLevel = kSDISupportLevelNone;
	ESIndexSet availableBackgroundRemovalTypes;

	Scanner* scanner = dataProvider_->GetScanner().get();
	
	if(scanner &&  scanner->GetAvailableValueForKey(kESBackgroundRemoval, availableBackgroundRemovalTypes)) {
		// Avilable values
		AddList(capability, kSDIBackgroundRemovalNone);
		AddList(capability, kSDIBackgroundRemovalNormal);
		AddList(capability, kSDIBackgroundRemovalHigh);
		capability.supportLevel = kSDISupportLevelAvailable;	

		// All values		
		AddAllList(capability, kSDIBackgroundRemovalNone);
		AddAllList(capability, kSDIBackgroundRemovalNormal);
		AddAllList(capability, kSDIBackgroundRemovalHigh);	
	}else if (Lut::isFitPluginAvailable())
	{
		ESIndexSet availableColorTypes;

		if (GetValueInt(kSDIColorTypeKey) != kSDIColorTypeMono1)
		{
			// Avilable values
			AddList(capability, kSDIBackgroundRemovalNone);
			AddList(capability, kSDIBackgroundRemovalNormal);
			AddList(capability, kSDIBackgroundRemovalHigh);
			capability.supportLevel = kSDISupportLevelAvailable;
		}else
		{
			capability.supportLevel = kSDISupportLevelUnavailable;
		}	

		// All values		
		AddAllList(capability, kSDIBackgroundRemovalNone);
		AddAllList(capability, kSDIBackgroundRemovalNormal);
		AddAllList(capability, kSDIBackgroundRemovalHigh);	
	}
}

ESDictionary BackgroundRemoval::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	ESIndexSet availableBackgroundRemovalTypes;
	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESBackgroundRemoval, availableBackgroundRemovalTypes)) {
		ENUM_ES_BACKGROUNDREMOVAL_LEVEL backgroundRemovalLevel = kESBackgroundRemovalLevelOff;
		if(current_ == kSDIBackgroundRemovalNone)
		{
			backgroundRemovalLevel = kESBackgroundRemovalLevelOff;
		}
		else if(current_ == kSDIBackgroundRemovalNormal)
		{
			backgroundRemovalLevel = kESBackgroundRemovalLevelLow;
		}
		else if(current_ == kSDIBackgroundRemovalHigh)
		{
			backgroundRemovalLevel = kESBackgroundRemovalLevelHigh;
		}

		dict[kESBackgroundRemoval] = (ESNumber)backgroundRemovalLevel;
	}
	return dict;
}
	
}//namespace epsonscan
