#include "DuplexType.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void DuplexType::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void DuplexType::Reset()
{
	current_  = false;
	hasCache_ = false;
	isDuplexAvailable_ = false;

	if (hasCache_ == false)
	{
		isDuplexAvailable_= false;
		ESIndexSet available;
		Scanner* scanner = dataProvider_->GetScanner().get();
		if(scanner &&  scanner->GetAvailableValueForKey(kESDuplex, available, kESFunctionalUnitDocumentFeeder)) {

			if (available.find(true) != available.end())
			{
				isDuplexAvailable_ = true;
				
				SDIDuplexType duplexType  = (SDIDuplexType)GetValueInt(kSDIADFDuplexTypeKey);
				SDIScannerKind kind = (SDIScannerKind)GetValueInt(KSDIScannerKindKey);
				if (kind == kSDIKindSFDocument && duplexType == kSDIADFDuplexType1Pass)
				{
					current_ = true;
				}else{
					current_ = false;
				}

			}
		}
		hasCache_ = true;
	}
}

void DuplexType::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void DuplexType::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
  	capability.supportLevel = kSDISupportLevelNone;


	//
	// Functional Unit
	// 
	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder)
	{
		if (isDuplexAvailable_)
		{
			AddList(capability, true);
			AddList(capability, false);

			AddAllList(capability, true);
			AddAllList(capability, false);

			capability.supportLevel = kSDISupportLevelAvailable;
		}else
		{
			capability.supportLevel = kSDISupportLevelNone;
		}
	}
	//
	// Functional Unit
	// 
	if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
	{
		if (isDuplexAvailable_)
		{
			AddAllList(capability, true);
			AddAllList(capability, false);
			capability.supportLevel = kSDISupportLevelUnavailable;
		}else{
			capability.supportLevel = kSDISupportLevelNone;
		}
	}
}

ESDictionary DuplexType::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	SDICapability cap;
	GetCapability(cap);

	if (cap.supportLevel == kSDISupportLevelUnavailable)
	{
		dict[kESDuplex] = (bool)false;
		return dict;
	}
	
	dict[kESDuplex] = (bool)current_;
	return dict;
}
	
}//namespace epsonscan
