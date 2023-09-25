#include "ScanHeightMaxInLongPaperResolutionTable.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanHeightMaxInLongPaperResolutionTable::SetValue(const std::string& intVal)
{
}	

void ScanHeightMaxInLongPaperResolutionTable::Reset()
{
}

void ScanHeightMaxInLongPaperResolutionTable::GetValue(std::string& intVal)
{
	Scanner* scanner = dataProvider_->GetScanner().get();
	if (scanner)
	{	
		ESDicArray dictArray;
		if (scanner->GetValueForKey(kESMaxLongLengthTable, dictArray) == false)
		{
			return;
		}

		ESString  outVal = "[";
		for(ESDictionary dic : dictArray)
		{
			ESString localVal;
			ES_CMN_FUNCS::JSON::DictionaryToJSON(dic, localVal);			
			outVal += localVal;
			outVal += ",";
		}
		outVal.pop_back();
		outVal += "]";

		intVal = outVal;
	}
}

void ScanHeightMaxInLongPaperResolutionTable::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
	Scanner* scanner = dataProvider_->GetScanner().get();
	if (scanner)
	{	
		ESDictionary dict;
		if(scanner->GetValueForKey(kESMaxLongLengthTable, dict))
		{
			capability.supportLevel = kSDISupportLevelAvailable;
		}else{
			capability.supportLevel = kSDISupportLevelNone;
		}
	} 
}

ESDictionary ScanHeightMaxInLongPaperResolutionTable::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

	
}//namespace epsonscan
