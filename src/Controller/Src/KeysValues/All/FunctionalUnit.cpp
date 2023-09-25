#include "FunctionalUnit.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>

namespace epsonscan {

void FunctionalUnit::SetValue(const SDIInt& intVal)
{	

	if (intVal == kSDIFunctionalUnitDocumentFeeder || intVal == kSDIFunctionalUnitCarrierSheet) 
	{
		if (ADFAvailable())
		{
			current_  = intVal;
			return;
		}
	}
	if (intVal == kSDIFunctionalUnitFlatbed)
	{
		if (FlatbedAvailable())
		{
			current_  = intVal;
			return;
		}
	}
}	

void FunctionalUnit::Reset()
{

	if (FlatbedAvailable())
	{
		current_  = kSDIFunctionalUnitFlatbed;
		return;
	}

	if (ADFAvailable())
	{
		current_  = kSDIFunctionalUnitDocumentFeeder;
		return;
	}
}

void FunctionalUnit::GetValue(SDIInt& intVal)
{
	intVal = current_;
}


bool FunctionalUnit::TransparentAvailable()
{
	ESIndexSet availableFunctionalUnit;
	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESFunctionalUnitType, availableFunctionalUnit)) {

		if (availableFunctionalUnit.find(kESFunctionalUnitTransparent) != availableFunctionalUnit.end()) {
			return true;
		}
	}

	return false;
}

bool FunctionalUnit::TransarentAreaGuideAvailable()
{
	ESIndexSet availableFunctionalUnit;
	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESFunctionalUnitType, availableFunctionalUnit)) {

		if (availableFunctionalUnit.find(kESFunctionalUnitTPUAreaGuide) != availableFunctionalUnit.end()) {
			return true;
		}
	}

	return false;
}


bool FunctionalUnit::ADFAvailable()
{
	ESIndexSet availableFunctionalUnit;
	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESFunctionalUnitType, availableFunctionalUnit)) {

		if (availableFunctionalUnit.find(kESFunctionalUnitDocumentFeeder) != availableFunctionalUnit.end()) {
			return true;
		}
	}

	return false;
}


bool FunctionalUnit::FlatbedAvailable()
{
	ESIndexSet availableFunctionalUnit;
	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESFunctionalUnitType, availableFunctionalUnit)) {

		if (availableFunctionalUnit.find(kESFunctionalUnitFlatbed) != availableFunctionalUnit.end()) {
			return true;
		}
	}

	return false;
}
	
void FunctionalUnit::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
    capability.supportLevel = kSDISupportLevelAvailable;


	if (FlatbedAvailable()) {
		AddList(capability, kSDIFunctionalUnitFlatbed);	
		AddAllList(capability, kSDIFunctionalUnitFlatbed);
	}

	if (ADFAvailable()) {
		AddList(capability, kSDIFunctionalUnitDocumentFeeder);	
		AddAllList(capability, kSDIFunctionalUnitDocumentFeeder);
	}
	
}

ESDictionary FunctionalUnit::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	// At first switch functional uint to support 2in1
	Scanner* scanner = dataProvider_->GetScanner().get();

	if (current_ == kSDIFunctionalUnitDocumentFeeder)
	{		
		dict[kESFunctionalUnitType] = (ESNumber)kESFunctionalUnitDocumentFeeder;

		scanner->SetValueForKey(kESFunctionalUnitType, (ESNumber)kESFunctionalUnitDocumentFeeder);
	}
	if (current_ == kSDIFunctionalUnitFlatbed)
	{
		SDIDocumentType documentType = (SDIDocumentType)GetValueInt(kSDIDocumentTypeKey);
		if (documentType == kSDIDocumentTypeReflective)
		{
			dict[kESFunctionalUnitType] = (ESNumber)kESFunctionalUnitFlatbed;
			scanner->SetValueForKey(kESFunctionalUnitType, (ESNumber)kESFunctionalUnitFlatbed);
		}else{		
			bool isFilmAreaGuide = (bool)GetValueInt(kSDIFilmAreaGuideKey);
			if (isFilmAreaGuide)
			{
				dict[kESFunctionalUnitType] = (ESNumber)kESFunctionalUnitTPUAreaGuide;
				scanner->SetValueForKey(kESFunctionalUnitType, (ESNumber)kESFunctionalUnitTPUAreaGuide);
			}else{
				dict[kESFunctionalUnitType] = (ESNumber)kESFunctionalUnitTransparent;
				scanner->SetValueForKey(kESFunctionalUnitType, (ESNumber)kESFunctionalUnitTransparent);
			}
		}
	}

	return dict;
}

}//namespace epsonscan
