#pragma once
#include <utils/ESBuffer.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "KeyMgr.hpp"
#include "ModelInfo.hpp"
#include "Filter.hpp"

namespace epsonscan
{

class Resize : public Filter
{
public:

    Resize(std::shared_ptr<KeyMgr> keyMgr,
           std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
    {   

    }

    virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
    {
        if ( ES_IMAGE_INFO::GetESImageResolutionX(imageInfo) == 0 ||  ES_IMAGE_INFO::GetESImageResolutionY(imageInfo) == 0)
        {
            return;
        }

    	if (ES_IMAGE_INFO::GetESImageResolutionX(imageInfo) != keyMgr_->GetValueInt(kSDIResolutionKey) ||
		    ES_IMAGE_INFO::GetESImageResolutionY(imageInfo) != keyMgr_->GetValueInt(kSDIResolutionKey))
		{
            double scaleX = (double)keyMgr_->GetValueInt(kSDIResolutionKey) / ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
            double scaleY = (double)keyMgr_->GetValueInt(kSDIResolutionKey) / ES_IMAGE_INFO::GetESImageResolutionY(imageInfo);

			UInt8* pInBytes = inDataBuf.GetConstBufferPtr();
			ESImageInfo outInfo = imageInfo;
			ES_CMN_FUNCS::BUFFER::CESHeapBuffer outDataBuf;
			{
				outInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] = (ESNumber)ES_IMAGE_INFO::GetESImageWidth(imageInfo) * scaleX;
				outInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] = (ESNumber)ES_IMAGE_INFO::GetESImageHeight(imageInfo) * scaleY;
                outInfo[ES_IMAGE_INFO::kESImageInfoResolutionXKey] = (ESNumber)keyMgr_->GetValueInt(kSDIResolutionKey);
                outInfo[ES_IMAGE_INFO::kESImageInfoResolutionYKey] = (ESNumber)keyMgr_->GetValueInt(kSDIResolutionKey);
                outInfo[ES_IMAGE_INFO::kESImageInfoOutputResolutionKey] = (ESNumber)keyMgr_->GetValueInt(kSDIResolutionKey);

				outDataBuf.AllocBuffer(ES_IMAGE_INFO::GetESImageSize(outInfo));
			}
			UInt8* pOutBytes = outDataBuf.GetBufferPtr();
			
			ProcessResize( 
					ES_IMAGE_INFO::GetESImageWidth(imageInfo),
					ES_IMAGE_INFO::GetESImageHeight(imageInfo),
					ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo),
					ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo),
					ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo),
					ES_IMAGE_INFO::GetESImageWidth(outInfo),
					ES_IMAGE_INFO::GetESImageHeight(outInfo),
					ES_IMAGE_INFO::GetESImageBytesPerRow(outInfo),
					(uint8_t*)pInBytes,
					(uint8_t*)pOutBytes);
			
			imageInfo = outInfo;
			inDataBuf.FreeBuffer();
			inDataBuf.Attach(outDataBuf);
        }
    }


    std::string Name() override
    {
    	 return "Resize";
    }
private:
    /**
        画像をリサイズする
          outBuffer : 出力画像のバッファー　使用側で確保する
    */
    static void ProcessResize(int32_t fromWidth,
                             int32_t fromHeight,
                             int32_t fromRowBytes,
                             int32_t bitsPerSample,
                             int32_t samplesPerPixel,
                             int32_t toWidth,
                             int32_t toHeight,
                            int32_t toRowBytes,
                            uint8_t *inBuffer,
                            uint8_t *outBuffer
                       );
};

}
    

    
    
