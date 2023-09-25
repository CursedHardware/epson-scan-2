#include "ScanAreaHeight.hpp"
#include "Scanner.hpp"
#include "Resolution.hpp"
#include "AutoSize.hpp"
#include "PaperDeskew.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void ScanAreaHeight::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ScanAreaHeight::Reset()
{
	current_  = 11.4 * 200;
}

void ScanAreaHeight::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ScanAreaHeight::GetCapability(SDICapability &capability)
{
	SetDefaultRangeCapability(capability);
}

void ScanAreaHeight::UpdateScanAreaWithDetectedSize(ESFunctionalUnitType funcUnit, ESDictionary& dict)
{
	ST_ES_SIZE_F detectedSize = {0};
	dataProvider_->GetScanner()->GetValueForKey(kESDetectedDocumentSize, detectedSize, funcUnit);

	  //fails to detect
	  if (detectedSize.cx < 1 && detectedSize.cy < 1)
	  {
			dataProvider_->GetScanner()->GetValueForKey(kESMaxScanSize, detectedSize, funcUnit);
	  }
	  dict[kESScanSize] = detectedSize;
}


bool ScanAreaHeight::IsOverScanForCroppingAvailable()
{
	ESIndexSet	availaleValues;
	Scanner* scanner = dataProvider_->GetScanner().get();
	scanner->GetAvailableValueForKey(kESOverScanForCropping, availaleValues, kESFunctionalUnitDocumentFeeder);
	if (availaleValues.find(true) != availaleValues.end()) {
		SDI_TRACE_LOG("Enter true");
		return true;
	}
	else
	{
		SDI_TRACE_LOG("Leave false");
		return false;
	}
}

ESDictionary ScanAreaHeight::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	SDIInt outputResolution = GetValueInt(kSDIResolutionKey);

	SDIInt scanResolutionX  = dataProvider_->GetCommandValueAsNumber(kESXResolution);
	SDIInt scanResolutionY  = dataProvider_->GetCommandValueAsNumber(kESYResolution);


	// ESC/I機種ではガイドポジションを外部からセットする必要がある
	ESNumber adfOffset = kPrvHRD_ADFOffset_Left;
	if (dataProvider_->GetModelInfo()->GetValue(kPrvHRD_ADFOffset, adfOffset))
	{
		if (adfOffset == kPrvHRD_ADFOffset_Right_Manual)
		{
			dataProvider_->GetScanner()->SetValueForKey(kESGuidePositionADF, (ESNumber)kESGuidePositionRight);
		}
		if (adfOffset == kPrvHRD_ADFOffset_Left)
		{
			dataProvider_->GetScanner()->SetValueForKey(kESGuidePositionADF, (ESNumber)kESGuidePositionLeft);
		}
		if (adfOffset == kPrvHRD_ADFOffset_Center_Auto || adfOffset == kPrvHRD_ADFOffset_Center_Manual)
		{
			dataProvider_->GetScanner()->SetValueForKey(kESGuidePositionADF, (ESNumber)kESGuidePositionCenter);
		}
	}


	SDIInt offsetX = GetValueInt(kSDIScanAreaOffsetXKey) * scanResolutionX / outputResolution;
	SDIInt offsetY = GetValueInt(kSDIScanAreaOffsetYKey) * scanResolutionY / outputResolution;
	SDIInt width = GetValueInt(kSDIScanAreaWidthKey) * scanResolutionX / outputResolution;
	SDIInt height = GetValueInt(kSDIScanAreaHeightKey) * scanResolutionY / outputResolution;

	SDIFunctionalUnit funcUnit = (SDIFunctionalUnit)GetValueInt(kSDIFunctionalUnitKey);
	auto autoSize = (AutoSize*)(dataProvider_->GetKeyInstance(kSDIAutoSizeKey).get());
	auto paperDeskew = (PaperDeskew*)dataProvider_->GetKeyInstance(kSDIPaperDeskewKey).get();

	/*
		For Flatbed scanning
	*/
	if (funcUnit == kSDIFunctionalUnitFlatbed)
	{		
		/*	
			Flatbed Hardware Auto cropping mode
		*/
		if (autoSize->IsDetectedDocumentSizeAvailable(kESFunctionalUnitFlatbed) && GetValueInt(kSDIAutoSizeKey) == kSDIAutoSizeStandard)
		{	
			/*
				Set MaxSize
			*/
			UpdateScanAreaWithDetectedSize(kESFunctionalUnitFlatbed, dict);
		}
		/*
			Flatbed sotware Auto cropping mode
		*/
		else if (autoSize->IsAdvancedSotwareAutoCropEnabled())
		{
			  ST_ES_SIZE_F maxSize = {0};
			  dataProvider_->GetScanner()->GetValueForKey(kESMaxScanSize, maxSize, kESFunctionalUnitFlatbed);
			  dict[kESScanSize] = maxSize;
		}
		/*
			Flabed designated scanning area
		*/
		else{
			dict[kESScanAreaInPixel] = MakeRect<UInt32>(offsetX,
														offsetY,
														offsetX + width,
														offsetY + height);
		}
	}
	else
	{
		/*
			For ADF scanning
		*/

		/*
			Standard cropping
		*/
		if (GetValueInt(kSDIAutoSizeKey) == kSDIAutoSizeStandard)
		{

			/*
			   ESIMP2 cropping
			*/
			if (autoSize->IsAdvancedSotwareAutoCropEnabled())
			{
				dict[kESScanSizeInPixel] = MakeSize<UInt32>(GetValueInt(kSDIMaxScanAreaWidthKey) / 100.0 *  scanResolutionX,
														GetValueInt(kSDIMaxScanAreaHeightKey) / 100.0 * scanResolutionY);

				if (IsOverScanForCroppingAvailable())
				{
					dict[kESOverScanForCropping] =  (bool)true;
				}
			}
			/*
				CRP cropping
			*/
			 else if (autoSize->IsCRPAvailable())
			 {
				dict[kESScanSizeInPixel] = MakeSize<UInt32>(GetValueInt(kSDIMaxScanAreaWidthKey) / 100.0 *  scanResolutionX,
														GetValueInt(kSDIMaxScanAreaHeightKey) / 100.0 * scanResolutionY);
				dict[kESPaperEndDetection] = (bool)false;
			 }
			 /*
				MFP detected document size scanning
			 */
			 else if (autoSize->IsDetectedDocumentSizeAvailable(kESFunctionalUnitDocumentFeeder))
			 {
				 UpdateScanAreaWithDetectedSize(kESFunctionalUnitDocumentFeeder, dict);
				 dict[kESPaperEndDetection] = (bool)true;
			 }else{
				/*
					DTR Software cropping
				*/
				dict[kESScanSizeInPixel] = MakeSize<UInt32>(GetValueInt(kSDIMaxScanAreaWidthKey) / 100.0 *  scanResolutionX,													
															GetValueInt(kSDIMaxScanAreaHeightKey) / 100.0 * scanResolutionY);
														
				dict[kESPaperEndDetection] = (bool)true;
				dict[kESOverScan] = (bool)true;
			 }
			return dict;
		}else if (GetValueInt(kSDIAutoSizeKey) == kSDIAutoSizeLongPaper)
		{
			/*
				For LongPaper Auto cropping mode
			*/
			int  heightMax  =  std::min(120* scanResolutionY, (int)(GetValueInt(kSDIMaxScanAreaHeightInLongPaperKey) / 100.0 * scanResolutionY));
			dict[kESScanSizeInPixel] = MakeSize<UInt32>(GetValueInt(kSDIMaxScanAreaWidthKey) / 100.0 *  scanResolutionX, heightMax);														

			dict[kESPaperEndDetection] = (bool)true;
			dict[kESOverScan] = (bool)true;
			return dict;
		}else if (GetValueInt(kSDIPaperEndDetectionKey) == 1)
		{

			/*
				For paper end detection mode
			*/
			int maxHeight = std::min((int)(GetValueInt(kSDIMaxScanAreaHeightInLongPaperKey)/ 100.0 * scanResolutionY), 215 * 300);
			dict[kESScanSizeInPixel] = MakeSize<UInt32>(width, maxHeight);
														
			dict[kESPaperEndDetection] = (bool)true;
			return dict;
		}


		/*
			If software paper deskew enabled
		*/
		if (paperDeskew->IsAutoDeskewForWhiteBGEnabled())
		{
			dict[kESScanSizeInPixel] = MakeSize<UInt32>(GetValueInt(kSDIMaxScanAreaWidthKey) / 100.0 *  scanResolutionX,
														GetValueInt(kSDIMaxScanAreaHeightKey) / 100.0 * scanResolutionY);


			if (IsOverScanForCroppingAvailable())
			{
				dict[kESOverScanForCropping] =  (bool)true;
			}
			return dict;
		}else{
			if (offsetX == 0 && offsetY== 0 )
			{
				dict[kESScanSizeInPixel] = MakeSize<UInt32>(width,
															height);
			}else{
				dict[kESScanAreaInPixel] = MakeRect<UInt32>(offsetX,
															offsetY,
															offsetX + width,
															offsetY + height);
			}
			dict[kESPaperEndDetection] = (bool)false;
		}
	}

	return dict;
}
	
}//namespace epsonscan
