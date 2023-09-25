#include "Dropout.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void Dropout::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void Dropout::Reset()
{
	current_  = kSDIDropoutColorNone;
	hasCache_ = false;
}

void Dropout::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void Dropout::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	ESIndexSet availableColorTypes;
  	capability.supportLevel = kSDISupportLevelNone;
	//
	//  Check hardware
	// 
	if (hasCache_ == false)
	{
		isDropoutAvailable_ = false;
		Scanner* scanner = dataProvider_->GetScanner().get();
		if(scanner &&  scanner->GetAvailableValueForKey(kESColorFormat, availableColorTypes)) { 

			AddList(capability, kSDIDropoutColorNone);
			AddAllList(capability, kSDIDropoutColorNone);

			if (availableColorTypes.find(kESColorFormatMonoDropR8) != availableColorTypes.end() &&
				availableColorTypes.find(kESColorFormatMonoDropG8) != availableColorTypes.end() &&
				availableColorTypes.find(kESColorFormatMonoDropB8) != availableColorTypes.end())	
			{
				isDropoutAvailable_ = true;
			}
		}
		hasCache_ = true;
	}

	if (GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto ||
		GetValueInt(kSDIColorTypeKey) == kSDIColorTypeRGB24 ||  
		GetValueInt(kSDIColorTypeKey) == kSDIColorTypeRGB48)
	{
		if (isDropoutAvailable_)
		{
			capability.supportLevel = kSDISupportLevelUnavailable;
			AddAllList(capability, kSDIDropoutColorRed);
			AddAllList(capability, kSDIDropoutColorGreen);
			AddAllList(capability, kSDIDropoutColorBlue);
			AddList(capability, kSDIDropoutColorRed);
			AddList(capability, kSDIDropoutColorGreen);
			AddList(capability, kSDIDropoutColorBlue);
		}else{
			capability.supportLevel = kSDISupportLevelNone;
		}

		return;
	}		
	if (GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1  || 
	   	GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono16 || 
	    GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono8)
	{
		if (isDropoutAvailable_)
		{
			capability.supportLevel = kSDISupportLevelAvailable;
			AddAllList(capability, kSDIDropoutColorRed);
			AddAllList(capability, kSDIDropoutColorGreen);
			AddAllList(capability, kSDIDropoutColorBlue);
			AddList(capability, kSDIDropoutColorRed);
			AddList(capability, kSDIDropoutColorGreen);
			AddList(capability, kSDIDropoutColorBlue);
		}else{
			capability.supportLevel = kSDISupportLevelNone;
		}
	}

}

ESDictionary Dropout::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
