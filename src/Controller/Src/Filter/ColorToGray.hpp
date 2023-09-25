#pragma once
#include <utils/ESBuffer.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "KeyMgr.hpp"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "../FileFormat/FFManager.hpp"
#include "ColorToGray.hpp"
#include "ColorType.hpp"

namespace epsonscan
{
    class ColorToGray : public Filter
    {
        public:            

           ColorToGray(std::shared_ptr<KeyMgr> keyMgr,
                      std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo), autoJudgedColor_(true)
            {   

            }

            void SetAutoJudgedColor(bool autoJudgedColor)
            {
                autoJudgedColor_ = autoJudgedColor;
            }

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
            {
			    if (ES_IMAGE_INFO::GetESImageColorType(imageInfo) == ES_IMAGE_INFO::kESImageColorTypeRGB &&
						(keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1 ||
						 keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono8 ||
                         (keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto && autoJudgedColor_ == false)))
                {

                        UInt8* pInBytes = inDataBuf.GetConstBufferPtr();
                        ESImageInfo outInfo = imageInfo;
                        ES_CMN_FUNCS::BUFFER::CESHeapBuffer outDataBuf;
                        {
                            outInfo[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey] = 1;
                            if(outDataBuf.AllocBuffer(ES_IMAGE_INFO::GetESImageSize(outInfo)) == false)
                            {                                            
                                SDI_TRACE_LOG("AllocBuffer fails");
                                throw std::bad_alloc();           
                            }
                        }
                        UInt8* pOutBytes = outDataBuf.GetBufferPtr();

                        {
                            ESNumber nImageHeight = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
                            ESNumber nImageWidth  =  ES_IMAGE_INFO::GetESImageWidth(imageInfo);

                            ESNumber nInBytesPerRow      = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
                            ESNumber nInSamplesPerPixel  = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
                            ESNumber nOutBytesPerRow     = ES_IMAGE_INFO::GetESImageBytesPerRow(outInfo);
                            ESNumber nOutSamplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(outInfo);

                            SDIDropoutColor dropOut = kSDIDropoutColorNone;

                            ColorType*  colorType = (ColorType*)(keyMgr_->GetKeyInstance(kSDIColorTypeKey).get());
                            if (colorType)
                            {
                                if (colorType->IsSoftwareDropout())
                                {
                                    dropOut = (SDIDropoutColor)(keyMgr_->GetValueInt(kSDIDropoutColorKey));
                                }
                            }


                            for( ESNumber i = 0; i < nImageHeight; i ++ ) {
                                for( ESNumber j = 0; j < nImageWidth;  j++ ) {
                                ESNumber nInIndex  = i * nInBytesPerRow  + j * nInSamplesPerPixel;
                                ESNumber nOutIndex = i * nOutBytesPerRow + j * nOutSamplesPerPixel;

                                    if (dropOut == kSDIDropoutColorNone)
                                    {
                                        pOutBytes[nOutIndex] =	std::min(255,
                                                                            static_cast<UInt8>(0.3 * pInBytes[nInIndex]		) +
                                                                            static_cast<UInt8>(0.6 * pInBytes[nInIndex  + 1]) +
                                                                            static_cast<UInt8>(0.1 * pInBytes[nInIndex  + 2]));
                                    }

                                    if (dropOut == kSDIDropoutColorRed)
                                    {
                                        pOutBytes[nOutIndex] = pInBytes[nInIndex];
                                    }

                                   if (dropOut == kSDIDropoutColorGreen)
                                    {
                                        pOutBytes[nOutIndex] =	 pInBytes[nInIndex  + 1];
                                    }

                                    if (dropOut == kSDIDropoutColorBlue)
                                    {
                                        pOutBytes[nOutIndex] =	 pInBytes[nInIndex  + 2];
                                    }
                                }
                            }
                        }
                        {
                            inDataBuf.FreeBuffer();
                            inDataBuf.Attach(outDataBuf);
                            imageInfo = outInfo;
                        }

                }
            }

            std::string Name() override
            {
                return "ColorToGray";
            }

            private:
                bool autoJudgedColor_;
    };
}