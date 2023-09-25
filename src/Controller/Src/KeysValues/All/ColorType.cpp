#include "ColorType.hpp"
#include "Scanner.hpp"
#include "DTR.hpp"
#include "AutoSize.hpp"
#include "DocumentType.hpp"
#include "Lut.hpp"
#include <ES2Command/ES2CommandAPI.h>

namespace epsonscan {

void ColorType::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ColorType::Reset()
{
	bool currentColorTypeOnOff;
	Scanner* scannerSupportColorTypeAuto = dataProvider_->GetScanner().get();
	if(scannerSupportColorTypeAuto && scannerSupportColorTypeAuto->GetValueForKey(kESDetectColorTypeEnabled, currentColorTypeOnOff))
	{
		isSupportHardwareColorTypeAuto_ = true;
	}else{
		isSupportHardwareColorTypeAuto_ = false;
	}

	ESNumber kind = kPrvHRD_Kind_SF_Document;
	dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind);
	if(kind == kPrvHRD_Kind_MF_LFP)
	{
		current_ = kSDIColorTypeRGB24;
		return;
	} else {
		current_  = kSDIColorTypeMono1;
	}	
}

void ColorType::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ColorType::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	capability.supportLevel = kSDISupportLevelAvailable;

	ESIndexSet availableColorTypes;
  
	Scanner* scanner = dataProvider_->GetScanner().get();
	if(scanner &&  scanner->GetAvailableValueForKey(kESColorFormat, availableColorTypes)) {

		AddAllList(capability, kSDIColorTypeRGB24);	
		AddList(capability, kSDIColorTypeRGB24);

		AddAllList(capability, kSDIColorTypeMono8);
		AddList(capability, kSDIColorTypeMono8);

		AddAllList(capability, kSDIColorTypeMono1);
		AddList(capability, kSDIColorTypeMono1);

		if (DTR::IsDTRAvailable() || isSupportHardwareColorTypeAuto_ == true)
		{
			ESNumber kind = kPrvHRD_Kind_SF_Document;
			dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Kind, kind);
			if(kind != kPrvHRD_Kind_MF_LFP)
			{
				AddAllList(capability, kSDIColorTypeAuto);	
				AddList(capability, kSDIColorTypeAuto);
			}
		}


		if (availableColorTypes.find(kESColorFormatRGB48) != availableColorTypes.end())
	   	{
			AddList(capability, kSDIColorTypeRGB48);
			AddAllList(capability, kSDIColorTypeRGB48);
		}

		if (availableColorTypes.find(kESColorFormatMono16) != availableColorTypes.end())
	   	{
			AddList(capability, kSDIColorTypeMono16);
			AddAllList(capability, kSDIColorTypeMono16);
		}		
	}	
	
}

bool ColorType::IsHardawaDropout()
{
	SDIDropoutColor dropout = (SDIDropoutColor)GetValueInt(kSDIDropoutColorKey);
	if (dropout == kSDIDropoutColorNone)
	{
		return false;
	}

	if (GetValueInt(kSDIAutoSizeKey) == kSDIAutoSizeLongPaper)
	{
		return false;
	}

	return true;
}

bool ColorType::IsSoftwareDropout()
{
	return IsHardawaDropout() == false;
}

ESNumber ColorType::GetScanCounterColorType()
{
	ENUM_ES_COLORCOUNTER_TYPE counterType = 0;

	if (current_ == kSDIColorTypeMono1)
	{
		counterType = kESColorCounterTypeMono;
	}

	if (current_ == kSDIColorTypeMono8)
	{
		counterType = kESColorCounterTypeMono;
	}

	if (current_ == kSDIColorTypeRGB24)
	{
		counterType = kESColorCounterTypeColor;
	}

	if (current_ == kSDIColorTypeAuto)
	{
		counterType = kESColorCounterTypeColor;
	}

	if (current_ == kSDIColorTypeRGB48)
	{
		counterType = kESColorCounterTypeColor;
	}

	if (current_ == kSDIColorTypeMono16)
	{
		counterType = kESColorCounterTypeMono;
	}

	return (ESNumber)counterType;
}



ESNumber ColorType::GetColorType()
{
	Scanner* scanner = dataProvider_->GetScanner().get();
	ESIndexSet availableColorTypes;
	ESColorFormat colorFormat = kESColorFormatRGB24;

	if (current_ == kSDIColorTypeMono1)
	{
		colorFormat = kESColorFormatMono8;
	}

	if (current_ == kSDIColorTypeMono8)
	{
		colorFormat = kESColorFormatMono8;
	}

	if (current_ == kSDIColorTypeRGB24)
	{
		colorFormat = kESColorFormatRGB24;
	}

	if (current_ == kSDIColorTypeAuto)
	{
		colorFormat = kESColorFormatRGB24;
	}

	if (current_ == kSDIColorTypeRGB48)
	{
		colorFormat = kESColorFormatRGB48;
	}

		if (current_ == kSDIColorTypeMono16)
	{
		colorFormat = kESColorFormatMono16;
	}

	SDIDropoutColor dropout = (SDIDropoutColor)GetValueInt(kSDIDropoutColorKey);
	if (IsHardawaDropout())
	{
		if (dropout == kSDIDropoutColorRed)
		{
			if (colorFormat == kESColorFormatMono8)
			{
				colorFormat = kESColorFormatMonoDropR8;
			}

			if (colorFormat == kESColorFormatMono1)
			{
				colorFormat = kESColorFormatMonoDropR1;
			}
		}

		if (dropout == kSDIDropoutColorGreen)
		{
			if (colorFormat == kESColorFormatMono8)
			{
				colorFormat = kESColorFormatMonoDropG8;
			}

			if (colorFormat == kESColorFormatMono1)
			{
				colorFormat = kESColorFormatMonoDropG1;
			}
		}

		if (dropout == kSDIDropoutColorBlue)
		{
			if (colorFormat == kESColorFormatMono8)
			{
				colorFormat = kESColorFormatMonoDropB8;
			}

			if (colorFormat == kESColorFormatMono1)
			{
				colorFormat = kESColorFormatMonoDropB1;
			}
		}
	}

	SDIRemovePunchhole removePunchHole = (SDIRemovePunchhole)GetValueInt(kSDIRemovePunchholeKey);
	if (removePunchHole != kSDIRemovePunchHoleNone)
	{
		colorFormat = kESColorFormatRGB24;
	}

	bool isDefificencyCorrectionEnabled = GetValueInt(kSDIDeficiencyCorrectionKey);
	if (isDefificencyCorrectionEnabled)
	{
		colorFormat = kESColorFormatRGB24;
	}


	return colorFormat;
}


void ColorType::GetGammaTable(ESGammaTable& outGammaTable)
{
	ESIndexArray narRGamma;
	ESIndexArray narGGamma;
	ESIndexArray narBGamma;

	auto documentType = (DocumentType*)dataProvider_->GetKeyInstance(kSDIDocumentTypeKey).get();
	ESColorFormat colorFormat = static_cast<ESColorFormat>(GetColorType());

	if (colorFormat == kESColorFormatRGB48 || colorFormat == kESColorFormatMono16)
	{
		unsigned char GammaR[256];
        unsigned char GammaG[256];
        unsigned char GammaB[256];

		for(ESNumber i = 0; i < kESGammaTableMax256; i++)
		{
			GammaR[i] = i;
			GammaG[i] = i;
			GammaB[i] = i;
		}

		if (documentType->IsNegativeFilm())
		{
			for(ESNumber i = 0; i < kESGammaTableMax256; i++)
			{
				GammaR[i] = 255 - GammaR[i];
				GammaG[i] = 255 - GammaG[i];
				GammaB[i] = 255 - GammaB[i];
			}
		}

		BuildGammaTable( GetValueInt(kSDIBrightnessKey),
						 GetValueInt(kSDIContrastKey), 
						 GetValueInt(kSDIGammaKey) * 10,
						 GammaR, 
						 GammaG, 
						 GammaB);

		for(ESNumber i = 0; i < kESGammaTableMax256; i++)
		{
			narRGamma.push_back(GammaR[i]);
			narGGamma.push_back(GammaG[i]);
			narBGamma.push_back(GammaB[i]);
		}
		
	}
	else
	{
		if (documentType->IsNegativeFilm())
		{
			for(ESNumber i = 0; i < kESGammaTableMax256; i++)
			{
				narRGamma.push_back(255 - i);
				narGGamma.push_back(255 - i);
				narBGamma.push_back(255 - i);
			}
		}else
		{
			for(ESNumber i = 0; i < kESGammaTableMax256; i++)
			{
				narRGamma.push_back(i);
				narGGamma.push_back(i);
				narBGamma.push_back(i);
			}
		}
	}
    
    outGammaTable[kESGammaTableMaxKey] = (ESNumber)kESGammaTableMax256;
    outGammaTable[kESGammaTableRedChannelKey]	 = narRGamma;
    outGammaTable[kESGammaTableGreenChannelKey]  = narGGamma;
    outGammaTable[kESGammaTableBlueChannelKey]	 = narBGamma;
    outGammaTable[kESGammaTableMonoChannelKey]	 = narGGamma;
}

ESDictionary ColorType::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	if(isSupportHardwareColorTypeAuto_ == true && current_ == kSDIColorTypeAuto)
	{
		dict[kESDetectColorTypeEnabled] = (bool)true;
	}
	Scanner* scanner = dataProvider_->GetScanner().get();
	ESIndexSet supprtedGammaMode;
	
	ESColorFormat colorFormat = static_cast<ESColorFormat>(GetColorType());
	dict[kESColorFormat] = (ESNumber)colorFormat;

	//
	//  Update GammaMode
	//
	if(scanner &&  scanner->GetAvailableValueForKey(kESGammaMode, supprtedGammaMode)) 
	{
		if ((colorFormat & kESColorFormatMaskBitDepth) == 8 || (colorFormat & kESColorFormatMaskBitDepth) == 16)
		{
			dict[kESGammaMode] = (ESNumber)kESGammaMode18;
			scanGammaMode_ = kESGammaMode18;
			saveGammaMode_ = kESGammaMode18;
		}else{
			dict[kESGammaMode] = (ESNumber)kESGammaMode10;		
			scanGammaMode_ = kESGammaMode10;
			saveGammaMode_ = kESGammaMode10;
		}

		
		auto autoSize = (AutoSize*)(dataProvider_->GetKeyInstance(kSDIAutoSizeKey).get());
		if (autoSize->IsAdvancedSotwareAutoCropEnabled())
		{
			scanGammaMode_ = kESGammaMode10;
			dict[kESGammaMode] = (ESNumber)kESGammaMode10;		
		}		
	}
	{ // Update Scan Counter
		ESIndexSet availableGammaTableRedTypes;

		if(scanner &&  scanner->GetAvailableValueForKey(kESColorCounterType, availableGammaTableRedTypes)) 
		{
			dict[kESColorCounterType] = GetScanCounterColorType();
		}
	}
	{// Update Gamma Table
		if(scanner &&  scanner->GetAvailableValueForKey(kESGammaMode, supprtedGammaMode)) {
			ESGammaTable gammaTable;
			GetGammaTable(gammaTable);
			if (colorFormat == kESColorFormatRGB24 || colorFormat == kESColorFormatRGB48)
			{
				dict[kESGammaTableRed] = gammaTable[kESGammaTableRedChannelKey];
				dict[kESGammaTableGreen] = gammaTable[kESGammaTableGreenChannelKey];
				dict[kESGammaTableBlue] = gammaTable[kESGammaTableBlueChannelKey];
				dict[kESColorMatrixMode] = (ESNumber)kESColorMatrixUnit;
			}else{
				dict[kESGammaTableMono] = gammaTable[kESGammaTableMonoChannelKey];
			}
			return dict;
		}
	}
	return dict;
}

bool ColorType::IsSupportHardwareColorTypeAuto()
{
	return isSupportHardwareColorTypeAuto_;
}
	
}//namespace epsonscan
