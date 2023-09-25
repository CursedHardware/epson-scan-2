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
    class MonoToGray : public Filter
    {
        public:            

           MonoToGray(std::shared_ptr<KeyMgr> keyMgr,
                      std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
            {   

            }

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
            {
                  if (ES_IMAGE_INFO::GetESImageColorType(imageInfo) == ES_IMAGE_INFO::kESImageColorTypeMono &&
					  (SDIImageFormat)keyMgr_->GetValueInt(kSDIImageFormatKey) == kSDIImageFormatJPEG)
					{
                      		ES_CMN_FUNCS::BUFFER::CESHeapBuffer cDest;

                            ESNumber nWidth			= ES_IMAGE_INFO::GetESImageWidth(imageInfo);
                            ESNumber nBytesPerRow	= ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
                            ESNumber nHeight		=  inDataBuf.GetLength() / nBytesPerRow;

                            if(cDest.AllocBuffer(nWidth * nHeight) == false)
                            {                                                
                                SDI_TRACE_LOG("AllocBuffer fails");
                                throw std::bad_alloc();           
                            }
                                                    
                            UInt8* pOutBytes = cDest.GetBufferPtr();
                            if(pOutBytes != NULL){
                                UInt8* pInBytes = inDataBuf.GetConstBufferPtr();

                                for(ESNumber h = 0;h<nHeight;h++){
                                    for (ESNumber j = 0; j < nWidth; j++) {
                                        if (GetBit(pInBytes, j)) {
                                            pOutBytes[j] = 255;
                                        }else{
                                            pOutBytes[j] = 0;
                                        }
                                    }
                                    pInBytes  += nBytesPerRow;
                                    pOutBytes += nWidth;
                                }
                            }
                        
                            {
                                ESImageInfo outInfo = imageInfo;
                                outInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey] = 8;
                                inDataBuf.FreeBuffer();
                                inDataBuf.Attach(cDest);
                                imageInfo = outInfo;
                            }	  
					}
            }

            static BOOL GetBit (uint8_t* pData, uint32_t nBitIndex)
	        {
        	    uint32_t nOctet = nBitIndex / 8;
    	        uint8_t  un8Mask = 1 << (7 - (nBitIndex % 8));

        	    if( pData[nOctet] & un8Mask ){
                	return TRUE;
                }

                return FALSE;
    	    }

            std::string Name() override
            {
                return "MonoToGray";
            }
    };
}//epsonscan