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

class Padding : public Filter
{
public:

    Padding(std::shared_ptr<KeyMgr> keyMgr,
           std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
    {   

    }

    virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
    {
        if ( ES_IMAGE_INFO::GetESImageSize(imageInfo) != inDataBuf.GetLength())
        {
            SDI_TRACE_LOG("Padding will do process");

			UInt8* pInBytes = inDataBuf.GetConstBufferPtr();
    		ES_CMN_FUNCS::BUFFER::CESHeapBuffer outDataBuf;
			
			if (outDataBuf.AllocBuffer(ES_IMAGE_INFO::GetESImageSize(imageInfo)) == false)
            {
                SDI_TRACE_LOG("AllocBuffer fails");
                throw std::bad_alloc();
            }
			
			UInt8* pOutBytes = outDataBuf.GetBufferPtr();
            memcpy(pOutBytes, pInBytes, outDataBuf.GetLength());
		
			inDataBuf.FreeBuffer();
			inDataBuf.Attach(outDataBuf);

            SDI_TRACE_LOG("Padding end  process");
        }
    }


    std::string Name() override
    {
    	 return "Padding";
    }
private:
};

}
    

    
    
