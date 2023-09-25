#include "Orientation.hpp"
#include "Scanner.hpp"
#include "GetOrientation.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void Orientation::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void Orientation::Reset()
{
	current_  = kSDIOrientationNone;
}

void Orientation::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void Orientation::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);

	AddList(capability, kSDIOrientationNone);
	AddAllList(capability, kSDIOrientationNone);

	AddList(capability, kSDIOrientation90);
	AddAllList(capability, kSDIOrientation90);

	AddList(capability, kSDIOrientation180);
	AddAllList(capability, kSDIOrientation180);

	AddList(capability, kSDIOrientation270);
	AddAllList(capability, kSDIOrientation270);

	if (GetOrientation::IsPluginAvailable())
	{
		ESNumber kind = kPrvHRD_Kind_SF_Document;
		dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind);
		if(kind != kPrvHRD_Kind_MF_LFP)
		{
			AddList(capability, kSDIOrientationAuto);
			AddAllList(capability, kSDIOrientationAuto);
		}
	}
}

ESDictionary Orientation::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
