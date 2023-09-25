#include "ScanHeightMaxInLongPaper.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanHeightMaxInLongPaper::SetValue(const SDIInt& intVal)
{
}	
void ScanHeightMaxInLongPaper::Reset()
{
}

ESNumber ScanHeightMaxInLongPaper::ADFMaxScanAreaHeightInHRD()
{
	ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();
	if (modelInfo)
	{
		ESNumber maxSizeInMM = 0;
	
		if (GetValueInt(kSDIDuplexTypeKey))
		{
			modelInfo->GetValue(kPrvHRD_DuplexADFHeightMax, maxSizeInMM);
			if (maxSizeInMM == 0)
			{
				modelInfo->GetValue(kPrvHRD_ADFHeightMax, maxSizeInMM);
			}

		}else{
			modelInfo->GetValue(kPrvHRD_ADFHeightMax, maxSizeInMM);
		}

		return MMtoInch(maxSizeInMM) * 10;
	}

	return 0;
}


void ScanHeightMaxInLongPaper::GetValue(SDIInt& intVal)
{
	auto modelInfo = dataProvider_->GetModelInfo().get();

	ESNumber kind = 0;
	if (modelInfo->GetValue(kPrvHRD_Kind, kind))
	{
		if (kind == kPrvHRD_Kind_MF_Business || kind == kPrvHRD_Kind_MF_Consumer)
		{
			intVal = GetValueInt(kSDIMaxScanAreaHeightKey);
			return;
		}
	}


	if (ADFMaxScanAreaHeightInHRD() > 0)
	{
		intVal = ADFMaxScanAreaHeightInHRD();
		return;
	}

	Scanner* scanner = dataProvider_->GetScanner().get();
	if (scanner)
	{
		ST_ES_SIZE_F stSize = {0};

		Scanner* scanner = dataProvider_->GetScanner().get();

		scanner->GetValueForKey(kESMaxScanSizeInLongLength, stSize, kESFunctionalUnitDocumentFeeder);
					
		intVal = (SDIInt)(stSize.cy * 100);
	}	
}

void ScanHeightMaxInLongPaper::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

ESDictionary ScanHeightMaxInLongPaper::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

	
}//namespace epsonscan
