#include "DocumentType.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include "FunctionalUnit.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void DocumentType::SetValue(const SDIInt& intVal)
{
	SDI_TRACE_LOG("SetValue = %d", intVal);;

	SDICapability cap = {};

	GetCapability(cap);

	if (cap.supportLevel == kSDISupportLevelNone)
	{
		current_ =  kSDIDocumentTypeReflective;
	}else{

		current_ = intVal;
	}

}	
void DocumentType::Reset()
{
	current_ = kSDIDocumentTypeReflective;
}

void DocumentType::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void DocumentType::GetCapability(SDICapability &capability)
{
	if (cache_)
	{
		capability = *cache_;
		return;
	}
	
	std::unique_ptr<SDICapability> cap(new SDICapability());
	cap->capabilityType = kSDICapabilitTypeList;
	cap->supportLevel = kSDISupportLevelNone;
	ESIndexSet available;

	auto functionalUnit = (FunctionalUnit*)dataProvider_->GetKeyInstance(kSDIDocumentTypeKey).get();

	if (functionalUnit->TransparentAvailable() == false)
	{
		cap->supportLevel = kSDISupportLevelNone;
	}else{
		cap->supportLevel = kSDISupportLevelAvailable;

		AddList(*cap, kSDIDocumentTypeReflective);	
		AddAllList(*cap, kSDIDocumentTypeReflective);

		AddList(*cap, kSDIDocumentTypePositiveFilm);	
		AddAllList(*cap, kSDIDocumentTypePositiveFilm);

		AddList(*cap, kSDIDocumentTypeColorNegativeFilm);	
		AddAllList(*cap, kSDIDocumentTypeColorNegativeFilm);
		
		AddList(*cap, kSDIDocumentTypeMonoNegativeFilm);	
		AddAllList(*cap, kSDIDocumentTypeMonoNegativeFilm);
	}

	capability = *cap;
	cache_.swap(cap);
}

ESDictionary DocumentType::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	if (current_ == kSDIDocumentTypePositiveFilm || current_ == kSDIDocumentTypeMonoNegativeFilm)
	{
		dict[kESFilmType] = (ESNumber)kESFilmTypePositive;
	}
	if (current_ == kSDIDocumentTypeColorNegativeFilm)
	{
		dict[kESFilmType] =  (ESNumber)kESFilmTypeNegative;
	}

	return dict;
}
	
}//namespace epsonscan
