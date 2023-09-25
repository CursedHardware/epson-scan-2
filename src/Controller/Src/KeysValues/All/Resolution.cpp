#include "Resolution.hpp"
#include "ModelInfo.hpp"
#include <ES2Command/ES2CommandAPI.h>
#include <utils/PathUtils.h>
#include "Utility.hpp"
#include <ESJsonUtils.h>
#include <utils/ESAnyCastUtils.h>

namespace epsonscan {

template <typename T>
static bool SafeBoostAnyCast_Local2(const ESAny &anyIn, T &outputValue)
{
	try
	{
		outputValue = boost::any_cast<T>(anyIn);
		return true;
	}
	catch (...)
	{
	}
	return false;
}


ESString  GetTableFromParamTable(ESDictionary &paramTable, const ES_CHAR *tableKey, const ES_CHAR *key, ESDictionary *requireCondKeysValues)
{
	ESString	value ;

	if (tableKey && paramTable.find(tableKey) != paramTable.end()) {

		ESDicArray	tableValues = boost::any_cast<ESDicArray>(paramTable[tableKey]);

		for (auto tableValue = tableValues.begin(); tableValue != tableValues.end(); tableValue ++) {
			try {
				if ((*tableValue).find(kConvertTableKey_IN) != (*tableValue).end()) {

					ESDictionary	cond = boost::any_cast<ESDictionary>((*tableValue)[kConvertTableKey_IN]);

					if (requireCondKeysValues != NULL) {

						for (auto condKey = cond.begin(); condKey != cond.end(); condKey++) {

							for (auto requireKey = requireCondKeysValues->begin(); requireKey != requireCondKeysValues->end(); requireKey++) {

								if (strcmp(condKey->first.c_str(), requireKey->first.c_str()) == 0) {

									ESIndexArray	conditions = boost::any_cast<ESIndexArray>(condKey->second);
									ESIndexArray	requireConditions = boost::any_cast<ESIndexArray>(requireKey->second);

									for (auto number = requireConditions.begin(); number != requireConditions.end(); number++) {
										if (std::find(conditions.begin(), conditions.end(), *number) == conditions.end()) {
											goto NEXT_TABLE;
										}
									}
								}
							}
						}
					}
				}
			}
			catch (boost::bad_any_cast) {
			}

			if ((*tableValue).find(kConvertTableKry_OUT) != (*tableValue).end()) {
				ESDictionary	dictOUT = boost::any_cast<ESDictionary>((*tableValue)[kConvertTableKry_OUT]);
				value = boost::any_cast<ESString>(dictOUT[key]);
			}

		NEXT_TABLE:;
		}
	}

	return value;
}

ESDictionary  Resolution::GetSeolutionTable(bool isXResolution)
{
	ESDictionary resTable;

	ESDictionary	dictParamTableAll;
	if (  dataProvider_->GetModelInfo()->GetConvertTable(dictParamTableAll)) {

		try {
			ESDictionary	paramTable;
			if (dictParamTableAll.find(kConvertTableKey_ParamTable) != dictParamTableAll.end()) {
				paramTable = boost::any_cast<ESDictionary>(dictParamTableAll[kConvertTableKey_ParamTable]);
			}

			SDIFunctionalUnit funcUnit  = (SDIFunctionalUnit)GetValueInt(kSDIFunctionalUnitKey);
			ESColorFormat	  colorFormat = (ESColorFormat)(dataProvider_->GetCommandValueAsNumber(kESColorFormat));

			ParamTable_Condition_FunctionalUnit condFunc = kParamTable_Condition_FunctionalUnit_ADF;
			ParamTable_Condition_Direction   condDire = kParamTable_Condition_Direction_Main;
			ParamTable_Condition_ImageType  imgType =  kParamTable_Condition_ImageType_Color;

			if (isXResolution)
			{
				condDire = kParamTable_Condition_Direction_Main;
			}
			else
			{
				condDire = kParamTable_Condition_Direction_Sub;
			}

			if (colorFormat == kESColorFormatMonoDropR1 ||
				colorFormat == kESColorFormatMonoDropG1 ||
				colorFormat == kESColorFormatMonoDropB1 ||
				colorFormat == kESColorFormatMono1)
			{
				imgType = kParamTable_Condition_ImageType_Mono;
			}

			if (colorFormat == kESColorFormatMonoDropR8 ||
				colorFormat == kESColorFormatMonoDropG8 ||
				colorFormat == kESColorFormatMonoDropB8 ||
				colorFormat == kESColorFormatMono8)
			{
				imgType = kParamTable_Condition_ImageType_Gray;
			}


			if (colorFormat == kESColorFormatRGB3||
				colorFormat == kESColorFormatRGB24 ||
				colorFormat == kESColorFormatRGB48)
			{
				imgType = kParamTable_Condition_ImageType_Color;
			}

			if (funcUnit == kSDIFunctionalUnitDocumentFeeder||
				funcUnit == kSDIFunctionalUnitCarrierSheet)
			{
				condFunc = kParamTable_Condition_FunctionalUnit_ADF;
			}else{

				SDIDocumentType documentType = (SDIDocumentType)GetValueInt(kSDIDocumentTypeKey);
				if (documentType != kSDIDocumentTypeReflective)
				{
					condFunc = kParamTable_Condition_FunctionalUnit_TPU;
				}else{
					condFunc = kParamTable_Condition_FunctionalUnit_Flatbled;
				}
			}

			ESDictionary tableCondition;
			tableCondition[kParamTable_Condition_FunctionalUnit] = ESIndexArray(1, condFunc);
			tableCondition[kParamTable_Condition_ImageType] = ESIndexArray(1, imgType);
			tableCondition[kParamTable_Condition_Direction] = ESIndexArray(1, condDire);

			auto resTableName = GetTableFromParamTable(paramTable, kTable_Info_KeyResolution, kTable_Key_ResolutionTable, &tableCondition);
			SDI_TRACE_LOG("resTable = %s", resTableName.c_str());

			ESString resTablePath;
			dataProvider_->GetModelInfo()->GetTablesPath(resTablePath);
			ES_CMN_FUNCS::PATH::ES_CombinePath(resTablePath, resTablePath, ES_STRING("Resolution/"));
			ES_CMN_FUNCS::PATH::ES_CombinePath(resTablePath, resTablePath, resTableName + ES_STRING(".json"));

			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(resTablePath))
			{
				ES_CMN_FUNCS::JSON::JSONFiletoDictionary(resTablePath, resTable);
			}else
			{
				SDI_TRACE_LOG("resolution faile does not found");
			}
		}
		catch (boost::bad_any_cast) {
		}
	}

	return resTable;
}

static ESNumber kSDEModelInfoResolutionLinear = 0;

ESNumber Resolution::ScanResolutionForDestinationResolution(ESNumber nTargetResolution, ESDictionary& dictResolutionTables)
{
	ESNumber nScanResolution = 0;
	ESNumber nPrevisouInputResolution = 0;

	ESDictionary* resTable = SAFE_KEYS_DATA_PTR(dictResolutionTables, "Resolution",ESDictionary);

	ESIndexArray* parInputResolutions = SAFE_KEYS_DATA_PTR(*resTable,"Input",ESIndexArray);
	ESIndexArray* parScanResolutions  = SAFE_KEYS_DATA_PTR(*resTable,"Output",ESIndexArray);

	ES_ErrorBail(parInputResolutions && parScanResolutions, BAIL, this, "return resolution invalid");

	for(ESNumber nIndex = 0; nIndex < (ESNumber)parInputResolutions->size(); ++nIndex)
	{
		if(	(nPrevisouInputResolution < nTargetResolution) && ((ESNumber)parInputResolutions->at(nIndex) >= nTargetResolution)	)
		{
			nScanResolution = parScanResolutions->at(nIndex);

			if(nScanResolution == kSDEModelInfoResolutionLinear)
			{
				nScanResolution = nTargetResolution;
			}
			break;
		}
		nPrevisouInputResolution = parInputResolutions->at(nIndex);
    }

	ES_ErrorBail(nScanResolution, BAIL, this, "return resolution invalid");
    
BAIL:;
    
    return nScanResolution;
}

void Resolution::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}
	
void Resolution::Reset()
{
	ESNumber kind = kPrvHRD_Kind_SF_Document;
	dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind);
	if(kind == kPrvHRD_Kind_MF_LFP)
	{
		current_ = 300;
	}else{
		current_ = 200;
	}
}

void Resolution::GetValue(SDIInt& intVal)
{
	intVal = current_;

}

void Resolution::GetCapability(SDICapability &capability)
{	
	SetDefaultRangeCapability(capability);

	ESNumber kind = kPrvHRD_Kind_SF_Document;
	dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind);

	if (kind == kPrvHRD_Kind_SF_Photo)
	{
		capability.minValue = 50;
		capability.allMinValue = 50;

		ESDictionary resInfo;
		dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Resolution, resInfo);

		ESNumber maxResolution = 4800;
		if(SafeBoostAnyCast_Local2( resInfo["Max"], maxResolution))
		{
			capability.maxValue = maxResolution * 2;
			capability.allMaxValue = maxResolution * 2;	
		}

	}else if(kind == kPrvHRD_Kind_MF_LFP)
	{
		capability.minValue = 50;
		capability.maxValue = 600;
		capability.allMinValue = 50;
		capability.allMaxValue = 600;
	}else{
		capability.minValue = 50;
		capability.maxValue = 1200;
		capability.allMinValue = 50;
		capability.allMaxValue = 1200;
	}
}

ESDictionary Resolution::GetEngineKeyValues()
{
	auto dict = ESDictionary();

	dict[kESXResolution] = GetScanResolution(current_, true);
	dict[kESYResolution] = GetScanResolution(current_, false);

	dict[kESJPEGQuality] = (ESNumber)85;
	if ( GetScanResolution(current_, true) < 600)
	{
		dict[kESJPEGQuality] = (ESNumber)90;
	}else if ( GetScanResolution(current_, true) < 300)
	{
		dict[kESJPEGQuality] = (ESNumber)95;
	}

	return dict;
}

SDIInt Resolution::GetScanResolution(SDIInt inResolution, bool isXResolution)
{
	auto resTable = GetSeolutionTable(isXResolution);
	auto resolution = ScanResolutionForDestinationResolution(inResolution, resTable);
	SDI_TRACE_LOG("scan resolution for isXResolution = %d  %d ", isXResolution, resolution);
	return resolution;
}




}//namespace epsonscan
