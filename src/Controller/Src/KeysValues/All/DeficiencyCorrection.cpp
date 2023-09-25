#include "DeficiencyCorrection.hpp"
#include "Scanner.hpp"
#include "Lut.hpp"
#include "ESIMP2.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void DeficiencyCorrection::SetValue(const SDIInt& intVal)
{
	SDICapability cap;
	GetCapability(cap);
	if (cap.supportLevel != kSDISupportLevelNone)
	{		
		current_ = intVal;
	}
}	
void DeficiencyCorrection::Reset()
{
	current_ = 0;
}

void DeficiencyCorrection::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void DeficiencyCorrection::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	capability.supportLevel = kSDISupportLevelNone;


	ESNumber imageCorrectionSupport = (ESNumber)kPrvHRD_ImageDefectCorrection_NotSupport;
	dataProvider_->GetModelInfo()->GetValue(kPrvHRD_ImageDefectCorrection, imageCorrectionSupport);
	if(imageCorrectionSupport && ESIMP2::IsESIMP2Available())
	{
		AddList(capability, true);
		AddList(capability, false);

		AddAllList(capability, true);
		AddAllList(capability, false);

		capability.supportLevel = kSDISupportLevelAvailable;
	}else{
		capability.supportLevel = kSDISupportLevelNone;
	}
}

ESDictionary DeficiencyCorrection::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

bool DeficiencyCorrection::IsProcessDeficiencyCorrection()
{
	bool shouldProcessRmovePunchHole = false;
	SDICapability deficiencyCap = {0};
	GetCapabilityWithKey(kSDIDeficiencyCorrectionKey, deficiencyCap);

	if (deficiencyCap.supportLevel != kSDISupportLevelNone &&  GetValueInt(kSDIRemovePunchholeKey) )
	{
		shouldProcessRmovePunchHole = true;
	}

	return GetValueInt(kSDIDeficiencyCorrectionKey) || shouldProcessRmovePunchHole;
}

	
}//namespace epsonscan
