#include <utils/ESBuffer.h>
#include <utils/ESFile.h>
#include <utils/PathUtils.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "KeyMgr.hpp"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "Error.hpp"
#include "../FileFormat/FFManager.hpp"

namespace epsonscan
{
    class Saving : public Filter
    {
        public:            

           Saving(std::shared_ptr<KeyMgr> keyMgr,
                      std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
            {   

            }

			static void Convert16To8(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
			{
				ESNumber imageHeight        = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
				ESNumber imageWidth         = ES_IMAGE_INFO::GetESImageWidth(imageInfo);

				ESNumber inBytesPerRow      = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
				ESNumber inSamplesPerPixel  = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
				ESNumber inBitsPerSample    = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
				
				if (inBitsPerSample == 16)
				{
					ESImageInfo outInfo = imageInfo;
					outInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey] = 8;
								
					ESNumber outBytesPerRow = ES_IMAGE_INFO::GetESImageBytesPerRow(outInfo);
					ESNumber outSamplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(outInfo);

					UInt8* pInBytes = inDataBuf.GetConstBufferPtr();
                    ES_CMN_FUNCS::BUFFER::CESHeapBuffer outDataBuf;
                    {
                        if(outDataBuf.AllocBuffer(ES_IMAGE_INFO::GetESImageSize(outInfo)) == false)
                        {                                      
                                SDI_TRACE_LOG("AllocBuffer fails");
                                throw std::bad_alloc();                        
                        }
                    }
                    UInt8* pOutBytes = outDataBuf.GetBufferPtr();

					for( ESNumber i = 0; i < imageHeight; i ++ ) {
						
						for( ESNumber j = 0; j < imageWidth; j++ )  {
							
							ESNumber inIndex  = i * inBytesPerRow  + j * inSamplesPerPixel * 2;//16bit Image
							ESNumber outIndex = i * outBytesPerRow + j * outSamplesPerPixel;//8bit Image
							
							pOutBytes[outIndex]  =  (uint16_t)(*((uint16_t*)&pInBytes[inIndex])) / 256;
							
							if (inSamplesPerPixel == 3) {
								pOutBytes[outIndex + 1] = (uint16_t)(*((uint16_t*)&pInBytes[inIndex + 2])) / 256;
								pOutBytes[outIndex + 2] = (uint16_t)(*((uint16_t*)&pInBytes[inIndex + 4])) / 256;
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

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
            {
                   FFManager ffMgr = FFManager::GetInstance();

                	CESFile* file = nullptr;
					std::stringstream prefixStream;
					prefixStream << ES_IMAGE_INFO::GetESImageWidth(imageInfo)
								 << "_"
								 << ES_IMAGE_INFO::GetESImageHeight(imageInfo)
								 << "_"
								 << ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo)
								 << "_"
								 << ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo)
								 << "_"
								 <<	ES_IMAGE_INFO::GetESImageResolutionX(imageInfo)
								 << "_";


					SDI_TRACE_LOG("will create file for page = %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));

					SDIImageFormat imgFormat = (SDIImageFormat)keyMgr_->GetValueInt(kSDIImageFormatKey);

					if (imgFormat == kSDIImageFormatBMP ||
						imgFormat == kSDIImageFormatJPEG ||
						imgFormat == kSDIImageFormatPNM ||
						imgFormat == kSDIImageFormatColorJpegMonoPNM ||
						imgFormat == kSDIImageFormatPDF ||
						imgFormat == kSDIImageFormatOFD
						)
					{
						Convert16To8(imageInfo, inDataBuf);
					}

					if (imgFormat == kSDIImageFormatRaw)
					{
					    file = CESFile::CreateTempFileInstanceWithPrefix(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath(),  prefixStream.str(), CESFile::ES_OPEN_MODE_WRITE_PLUS);
						if(file->WriteData(inDataBuf) == false)
						{
							delete file;
							file = nullptr;
							SDI_TRACE_LOG("ffMgr.Write fails %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
							throw RuntimeError("file write error", kSDIErrorWriteFile);
						}
						file->CloseFile();
					}else{
						

						if (imgFormat == kSDIImageFormatColorJpegMonoPNM && ES_IMAGE_INFO::GetESImageColorType(imageInfo) != ES_IMAGE_INFO::kESImageColorTypeMono)
						{
							imgFormat = kSDIImageFormatJPEG;
						}else if (imgFormat == kSDIImageFormatColorJpegMonoPNM && ES_IMAGE_INFO::GetESImageColorType(imageInfo) == ES_IMAGE_INFO::kESImageColorTypeMono)
						{
							imgFormat = kSDIImageFormatPNM;
						}

						file = CESFile::CreateTempFileInstanceWithPrefix(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath(), prefixStream.str());
						file->CloseFile();

						if(ffMgr.Write(inDataBuf, // input buffer  
										file->GetFileName(), // destination
										imgFormat,
										imageInfo,// imageInfo,
										*keyMgr_
										) == false)
						{
							delete file;
							file = nullptr;
							SDI_TRACE_LOG("ffMgr.Write fails %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
							throw RuntimeError("file write error", kSDIErrorWriteFile);
						}
					}

					filePath_ = file->GetFileName();	
					delete file;
					file = nullptr;
            }

            std::string GetOutputFileName()
            {
                return filePath_;
            }
            std::string Name() override
            {
                return "Saving";
            }
            private:
                std::string filePath_;

    };
}//epsonscan