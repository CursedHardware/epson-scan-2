#include "TextEnhance.hpp"
#include "Scanner.hpp"
#include  "DDEFilter.hpp"
#include <ES2Command/ES2CommandAPI.h>
#include <utils/PathUtils.h>
#include <utils/ESFile.h>

namespace epsonscan {


void TextEnhance::SetValue(const SDIInt& intVal)
{
	if (GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1)
	{
		if (intVal == 2)
		{
			current_ = 1;
		}else{
			current_ = intVal;
		}
	}else{
		current_ = intVal;
	}
}

void TextEnhance::Reset()
{
	current_  = kSDITextEnhanceLevelNone;
}

void TextEnhance::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void TextEnhance::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	ESIndexSet availableColorTypes;
  	capability.supportLevel = kSDISupportLevelNone;
	capability.countOfList = 0;

	ESString ddeName;
	ESString imfName;
		
	ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();

	ESNumber isTETSupport = kPrvHRD_TextEnhancementNotSupport_Support;
	if(dataProvider_->GetModelInfo()->GetValue(kPrvHRD_TextEnhancementNotSupport, isTETSupport))
	{
		if(isTETSupport == kPrvHRD_TextEnhancementNotSupport_NotSupport)
		{
			capability.supportLevel = kSDISupportLevelNone;
			return;
		}
	}

    ESNumber kind = 0;
   	if(dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind))
	{
		if(kind == kPrvHRD_Kind_MF_LFP || kind == kPrvHRD_Kind_MF_Consumer || kind == kPrvHRD_Kind_MF_Business)
		{
			capability.supportLevel = kSDISupportLevelNone;
			return;
		}
	} 

		
	modelInfo->GetValue(kPrvHRD_DDEName, ddeName);
	modelInfo->GetValue(kPrvHRD_ImageFilterName, imfName);

	if (DDEFilter::IsDDEAvailable())
	{
		capability.supportLevel = kSDISupportLevelAvailable;
		capability.countOfAllList = 3;
		capability.allList[0] = kSDITextEnhanceLevelNone;
		capability.allList[1] = kSDITextEnhanceLevelStandard;
		capability.allList[2] = kSDITextEnhanceLevelHigh;

		capability.countOfList = 3;
		capability.list[0] = kSDITextEnhanceLevelNone;
		capability.list[1] = kSDITextEnhanceLevelStandard;
		capability.list[2] = kSDITextEnhanceLevelHigh;
	}
}

ESDictionary TextEnhance::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}
	
}//namespace epsonscan
