#include "PaperDeskew.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>
#include "ESIMP2.hpp"

namespace epsonscan {

void PaperDeskew::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void PaperDeskew::Reset()
{
	current_  = true;
	adf_ = nullptr;
}

void PaperDeskew::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

bool PaperDeskew::IsAutoDeskewForWhiteBGEnabled()
{

	if ( ESIMP2::IsESIMP2Available() == false)
	{
		return false;
	}

	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder && 
		GetValueInt(kSDIBackgroundColorKey) == kSDIBackgroundColorWhite)
	{

		double maxScanAreaInch = GetValueInt(kSDIMaxScanAreaHeightKey) / 100.0;
		double scanAreaInch = GetValueInt(kSDIScanAreaHeightKey) / GetValueInt(kSDIResolutionKey);

		// long paper
		if (scanAreaInch > maxScanAreaInch || GetValueInt(kSDIPaperEndDetectionKey))
		{
			return false;
		}

		return current_;
	}


	return false;
}



void PaperDeskew::GetADFCapability(SDICapability &capability)
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
	if(scanner &&  scanner->GetAvailableValueForKey(kESSkewCorrection, available, kESFunctionalUnitDocumentFeeder)) {

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

void PaperDeskew::GetCapability(SDICapability &capability)
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

ESDictionary PaperDeskew::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	dict[kESSkewCorrection] = (bool)current_;

	return dict;
}
	
}//namespace epsonscan
