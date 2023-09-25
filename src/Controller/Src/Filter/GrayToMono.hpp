#include <utils/ESBuffer.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "KeyMgr.hpp"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "../FileFormat/FFManager.hpp"

namespace epsonscan
{
    class GrayToMono : public Filter
    {
        public:            

           GrayToMono(std::shared_ptr<KeyMgr> keyMgr,
                      std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo), autoJudgedColor_(true)
            {   

            }

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
            {
                if (ES_IMAGE_INFO::GetESImageColorType(imageInfo) == ES_IMAGE_INFO::kESImageColorTypeGray &&
					(keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1
                     || (keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto && autoJudgedColor_ == false))
                    )
				{
                	UInt8* pInBytes = inDataBuf.GetConstBufferPtr();
                    ESImageInfo outInfo = imageInfo;
                    ES_CMN_FUNCS::BUFFER::CESHeapBuffer outDataBuf;
                    {
                        outInfo[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey] = 1;
                        outInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey] = 1;
                        if(outDataBuf.AllocBuffer(ES_IMAGE_INFO::GetESImageSize(outInfo)) == false)
                        {                                      
                                SDI_TRACE_LOG("AllocBuffer fails");
                                throw std::bad_alloc();                        
                        }
                    }
                    UInt8* pOutBytes = outDataBuf.GetBufferPtr();

                    ESNumber nImageHeight 		 = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
                    ESNumber nImageWidth  		 = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
                    ESNumber nInBytesPerRow      = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
                    ESNumber nInSamplesPerPixel  = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
                    ESNumber nOutBytesPerRow     = ES_IMAGE_INFO::GetESImageBytesPerRow(outInfo);
                    ESNumber nOutSamplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(outInfo);

                    ESNumber threshold =  keyMgr_->GetValueInt(kSDIThresholdKey);

                    for(ESNumber i=0;i<nImageHeight;i++){
                            ESNumber inImagePos = 0;
                            for(ESNumber h=0;h<nOutBytesPerRow;h++){
                                    ESNumber outByte = 0;
                                    for(ESNumber bitIndex = 0; bitIndex < 8 && inImagePos < nImageWidth; bitIndex++) {

                                            if(pInBytes[i * nInBytesPerRow + inImagePos] >= threshold) {
                                                    ESNumber bit = 7 - (bitIndex % 8);
                                                    outByte     |= (1<< bit);
                                            }
                                            inImagePos++;
                                    }
                                    pOutBytes[i * nOutBytesPerRow + h] = outByte;
                            }
                    }

                    {
                        inDataBuf.FreeBuffer();
                        inDataBuf.Attach(outDataBuf);
                        imageInfo = outInfo;
                    }
                }
            }
            void SetAutoJudgedColor(bool autoJudgedColor)
            {
                autoJudgedColor_ = autoJudgedColor;
            }
            std::string Name() override
            {
                return "GrayToMono";
            }

            private:
                bool autoJudgedColor_;
    };
}