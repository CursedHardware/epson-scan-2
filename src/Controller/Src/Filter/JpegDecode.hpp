#pragma once
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
    class JpegDecode : public Filter
    {
        public:            

           JpegDecode(std::shared_ptr<KeyMgr> keyMgr,
                      std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
            {   

            }

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
            {
                				// Decode Jpeg Data
				if (ES_IMAGE_INFO::GetESImageDataType(imageInfo) == (ESNumber)kESImageFormatJPEG)
				{
                    FFManager ffMgr = FFManager::GetInstance();

					imageInfo[ES_IMAGE_INFO::kESImageInfoDataTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageDataTypeJpeg;

					DumpImage(imageInfo, inDataBuf, "sourceJpeg");

                    bool decodeOnlyValid = false;

                    if (keyMgr_->GetCommandValueAsBool(kESPaperEndDetection))
                    {
                        decodeOnlyValid = true;
                    }

					if(ffMgr.DecodeJpeg(inDataBuf,
                                       (ESNumber)ES_IMAGE_INFO::GetESImageHeight(imageInfo),
                                       (ESNumber)ES_IMAGE_INFO::GetESImageSize(imageInfo),
                                       decodeOnlyValid
                                        ) == false)
					{
						SDI_TRACE_LOG("DecodeJpeg fails for page = %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
						throw std::bad_alloc();
					}
					imageInfo[ES_IMAGE_INFO::kESImageInfoDataTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageDataTypeRaw;	
				}
            }


            std::string Name() override
            {
                return "JpegDecode";
            }
    };
}