#include <utils/ESFile.h>
#include "TransferMgr.hpp"
#include "JpegDecode.hpp"
#include "DTR.hpp"
#include "GrayToMono.hpp"
#include "ColorToGray.hpp"
#include "Lut.hpp"
#include "ProcOrientation.hpp"
#include "MonoToGray.hpp"
#include "Saving.hpp"
#include "Resize.hpp"
#include "GetOrientation.hpp"
#include "Error.hpp"
#include "Padding.hpp"
#include "DDEFilter.hpp"
#include "Brightness.hpp"
#include "Contrast.hpp"
#include "Gamma.hpp"
#include "SkipBlankPageOnOff.hpp"
#include "ESIMP2.hpp"
#include "Gamma10To18.hpp"

namespace epsonscan 
{
	int Image::threadCount_ = 0;
	CritSec Image::m_ThreadCriticSec;

	bool Image::IsHardwareImageProcessSupported(std::shared_ptr<KeyMgr> keyMgr)
	{
		Brightness* brightness = (Brightness*)(keyMgr->GetKeyInstance(kSDIBrightnessKey).get());
		Contrast* contrast = (Contrast*)(keyMgr->GetKeyInstance(kSDIContrastKey).get());
		Gamma* gamma = (Gamma*)(keyMgr->GetKeyInstance(kSDIGammaKey).get());
		if(brightness || contrast || gamma)
		{
			if(brightness->IsSupportBrightnessCommand() || contrast->IsSupportContrastCommand() || gamma->IsSupportGammaCommand())
			{
				return true;
			}
		}

		return false;
	}

	SDIError Image::applyImageProc(std::shared_ptr<KeyMgr> keyMgr, std::shared_ptr<ModelInfo> modelInfo)
	{
			imageProcThread_.reset(new std::thread([this, keyMgr, modelInfo]
			{
				try	
				{
				WaitUntilTheradLimit(keyMgr->GetCommandValueAsNumber(kESXResolution));
				AddThreadCount();

				SDI_TRACE_LOG("applyImageProc start %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo_));

				imageInfo_[ES_IMAGE_INFO::kESImageInfoResolutionXKey]	 = (ESNumber)keyMgr->GetCommandValueAsNumber(kESXResolution);
				imageInfo_[ES_IMAGE_INFO::kESImageInfoResolutionYKey]	 = (ESNumber)keyMgr->GetCommandValueAsNumber(kESYResolution);
				imageInfo_[ES_IMAGE_INFO::kESImageInfoOutputResolutionKey] = (ESNumber)keyMgr->GetCommandValueAsNumber(kESXResolution);

				JpegDecode jpegDecode(keyMgr, modelInfo);
				ColorToGray colorToGray(keyMgr, modelInfo);
				GrayToMono	grayToMono(keyMgr, modelInfo);
				DTR dtr(keyMgr, modelInfo);
				Lut lut(keyMgr, modelInfo);
				MonoToGray monoToGray(keyMgr, modelInfo);
				Resize resize(keyMgr, modelInfo);
				ProcOrientation procOrientation(keyMgr, modelInfo);
				Saving saving(keyMgr, modelInfo);
				GetOrientation getOrientation(keyMgr, modelInfo);
				Padding padding(keyMgr, modelInfo);
				ESIMP2 esimp2(keyMgr, modelInfo);
				Gamma10To18 gamma10To18(keyMgr, modelInfo);
				DDEFilter ddeFilter(keyMgr, modelInfo);

			
				SDIAutoColorPixelType autoColorPixelType = kSDIAutoColorPixelTypeMono;

				bool autoJudgedColor = true;
				
				ESNumber whitePageSkipType = kPrvHRD_WhitePageSkipColorType_Gray;
				modelInfo->GetValue(kPrvHRD_WhitePageSkipColorType, whitePageSkipType);
	
				{
					jpegDecode.Process(imageInfo_, dataBuf_);
				}
				{
					padding.Process(imageInfo_, dataBuf_);
				}		
				{
					esimp2.Detect(imageInfo_, dataBuf_);
				}
				{
					gamma10To18.Process(imageInfo_, dataBuf_);
				}
				{
					esimp2.DoDeficiencyCorrection(imageInfo_, dataBuf_);
				}
				{	
					if (imageInfo_[ES_IMAGE_INFO::kESImageInfoUpsideDownKey])
					{
						procOrientation.SetOrientation(kSDIOrientation180);
						procOrientation.Process(imageInfo_, dataBuf_);
					}
				}
				{
					dtr.SetActionType(kActionTypeGetDocumentLocation);
					dtr.Process(imageInfo_, dataBuf_);
				}
				{
					resize.Process(imageInfo_, dataBuf_);
				}
				{
					esimp2.Crop(imageInfo_, dataBuf_);
				}
				{
					ColorType* colorTypeAuto = (ColorType*)(keyMgr->GetKeyInstance(kSDIColorTypeKey).get()); 
					if(colorTypeAuto && colorTypeAuto->IsSupportHardwareColorTypeAuto())
					{
						if(ES_IMAGE_INFO::GetESImageMonoPage(imageInfo_) && keyMgr->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto)
						{
							autoJudgedColor = false;
							autoColorPixelType = (SDIAutoColorPixelType)keyMgr->GetValueInt(kSDIAutoColorPixelTypeKey);
						}
					} else {
						dtr.SetActionType(kActionTypeJudgeColorType);
						dtr.Process(imageInfo_, dataBuf_);
						autoJudgedColor = dtr.IsColorPage();
						autoColorPixelType = dtr.GetAutoColorPixelType();
					}
				}
				{				
					dtr.SetActionType(kActionTypeRemovePunchHole);
					dtr.Process(imageInfo_, dataBuf_);
				}
				{				
					if (whitePageSkipType == kPrvHRD_WhitePageSkipColorType_Color)
					{
						SkipBlankPageOnOff* skipBlankPageOnOff = (SkipBlankPageOnOff*)(keyMgr->GetKeyInstance(kSDIBlankPageSkipKey).get());
						if(skipBlankPageOnOff && skipBlankPageOnOff->IsSupportHardwareBlankPageOnOff())
						{
							if(ES_IMAGE_INFO::GetESImageBlankPageSkip(imageInfo_))
							{
								isBlankPage_ = true;
								dataBuf_.FreeBuffer();
								SDI_TRACE_LOG("machine blank page found  %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo_));
								SubThreadCount();
								return;
							} else {
								isBlankPage_ = false;
							}
						} else {
							dtr.SetActionType(kActionTypeJudgeBlankPage);
							dtr.Process(imageInfo_, dataBuf_);
							isBlankPage_ = dtr.IsBlankPage();
							if (isBlankPage_)
							{
								dataBuf_.FreeBuffer();
								SDI_TRACE_LOG("blank page found  %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo_));
								SubThreadCount();
								return;
							}
						}
					}
				}
				{	
					if(IsHardwareImageProcessSupported(keyMgr) == false)
					{
						lut.SetAutoJudgedColor(autoJudgedColor || autoColorPixelType == kSDIAutoColorPixelTypeGray);
						lut.Process(imageInfo_, dataBuf_);		
					}								
				}
				{
					if (whitePageSkipType == kPrvHRD_WhitePageSkipColorType_Gray)
					{
						SkipBlankPageOnOff* skipBlankPageOnOff = (SkipBlankPageOnOff*)(keyMgr->GetKeyInstance(kSDIBlankPageSkipKey).get());
						if(skipBlankPageOnOff && skipBlankPageOnOff->IsSupportHardwareBlankPageOnOff())
						{
							if(ES_IMAGE_INFO::GetESImageBlankPageSkip(imageInfo_))
							{
								isBlankPage_ = true;
								dataBuf_.FreeBuffer();
								SDI_TRACE_LOG("machine blank page found  %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo_));
								SubThreadCount();
								return;
							} else {
								isBlankPage_ = false;
							}
						} else {
							dtr.SetActionType(kActionTypeJudgeBlankPage);
							dtr.Process(imageInfo_, dataBuf_);
							isBlankPage_ = dtr.IsBlankPage();
							if (isBlankPage_)
							{
								dataBuf_.FreeBuffer();
								SDI_TRACE_LOG("blank page found  %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo_));
								SubThreadCount();
								return;
							}
						}
					}
				}
				{
					colorToGray.SetAutoJudgedColor(autoJudgedColor);
					colorToGray.Process(imageInfo_, dataBuf_);
					if (keyMgr->GetValueInt(kSDITextEnhanceKey) != kSDITextEnhanceLevelNone)
					{
						if(keyMgr->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1
							|| (keyMgr->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto && autoJudgedColor == false && autoColorPixelType == kSDIAutoColorPixelTypeMono))
						{
							ddeFilter.SetActionType(kActionTypeBin);
							ddeFilter.Process(imageInfo_, dataBuf_);
						}
						else{
							ddeFilter.SetActionType(kActionTypeADE);
							ddeFilter.Process(imageInfo_, dataBuf_);
						}
					}else
					{
						grayToMono.SetAutoJudgedColor(autoJudgedColor || autoColorPixelType == kSDIAutoColorPixelTypeGray);
						grayToMono.Process(imageInfo_, dataBuf_);				
					}
				}
				{
					if (keyMgr->GetValueInt(kSDIOrientationKey) == kSDIOrientationAuto)
					{
						getOrientation.Process(imageInfo_, dataBuf_);
						if (getOrientation.GetJudgeResult() != kSDIOrientationNone)
						{
							procOrientation.SetOrientation(getOrientation.GetJudgeResult());
							procOrientation.Process(imageInfo_, dataBuf_);
						}
					}
					else
					{	
							procOrientation.SetOrientation((SDIOrientation)keyMgr->GetValueInt(kSDIOrientationKey));
							procOrientation.Process(imageInfo_, dataBuf_);
					}
				}
				{
					monoToGray.Process(imageInfo_, dataBuf_);
				}
				{
					saving.Process(imageInfo_, dataBuf_);
				}

				filePath_ = saving.GetOutputFileName();				
				dataBuf_.FreeBuffer();

				SDI_TRACE_LOG("applyImageProc end %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo_));

				}catch(std::bad_alloc& memerror)
				{
					SDI_TRACE_LOG("memory  error");
					lastError_ = kSDIErrorNoMemory;	
				}catch(RuntimeError& runtimeError)
				{
					SDI_TRACE_LOG("runtime error = %s", runtimeError.what());
					lastError_ = runtimeError.GetCode();
				}
				catch(...)
				{
					SDI_TRACE_LOG("unknown  error");
					lastError_ = kSDIErrorUnknownError;
				}

				SubThreadCount();
			}));


			return kSDIErrorNone;
	}
}//namespace epsonscan
