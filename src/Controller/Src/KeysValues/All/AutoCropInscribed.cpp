#include "AutoCropInscribed.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void AutoCropInscribed::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void AutoCropInscribed::Reset()
{

	SDICapability cap;
	GetCapability(cap);

	if (cap.supportLevel != kSDISupportLevelNone)
	{
		current_  = true;
	}else{
		current_ = false;
	}
	adf_ = nullptr;
}

void AutoCropInscribed::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void AutoCropInscribed::GetADFCapability(SDICapability &capability)
{
	if (adf_)
	{
		capability = *adf_;
		return;
	}
	std::unique_ptr<SDICapability> adfCap(new SDICapability());

	SetDefaultListCapability(*adfCap);

	adfCap->supportLevel = kSDISupportLevelNone;
	ESIndexSet available;

	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESAutoCroppingInscribed, available, kESFunctionalUnitDocumentFeeder)) {

		if (available.find(true) != available.end())
	   	{
			AddList(*adfCap, true);
			AddList(*adfCap, false);

			AddAllList(*adfCap, true);
			AddAllList(*adfCap, false);

			adfCap->supportLevel = kSDISupportLevelAvailable;
		}else{
			AddList(*adfCap, false);
			AddAllList(*adfCap, false);
		}
	}
	capability = *adfCap;
	adf_.swap(adfCap);
}

void AutoCropInscribed::GetCapability(SDICapability &capability)
{

	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder)
	{
		GetADFCapability(capability);
		return;
	}

	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
	{
		SDICapability adfCap;
		GetADFCapability(adfCap);
		if (adfCap.supportLevel != kSDISupportLevelNone)
		{
			capability.supportLevel = kSDISupportLevelUnavailable;
		}else{
			capability.supportLevel = kSDISupportLevelNone;
		}
		ResetList(capability);
		return;
	}
}

ESDictionary AutoCropInscribed::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
