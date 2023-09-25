#include "FFManager.hpp"
#include "ESManageFileFormatKit.h"
#include <utils/PathUtils.h>
#include <utils/ESFile.h>
#include "Utility.hpp"
#include "DbgLog.h"
#include "ofdplugin.h"

namespace epsonscan
{

        FFManager::FFManager() : initialized_(false), 
                                 writerForMulti_(nullptr),
                                 formatForMulti_(kSDIImageFormatPDF),
                                 pdf_(nullptr), 
                                 ofdPlugin_(nullptr),
                                 ofdPluginModule_(nullptr)
        {
		    m_pcManageFileFormatKit = new ::CESManageFileFormatKit();
            if (m_pcManageFileFormatKit == nullptr)
            {
              	SDI_TRACE_LOG("m_pcManageFileFormatKit is null");
                return;
            }

    		std::string strModulePath = EPSON_INSTALL_PATH;
            ES_CMN_FUNCS::PATH::ES_CombinePath(strModulePath, strModulePath, ES_STRING("libes2fileformat.so"));


            bool bLoadModule = m_pcManageFileFormatKit->LoadModule(strModulePath.c_str());
            if (bLoadModule == false)
            {
                SDI_TRACE_LOG("%s", dlerror());
                SDI_TRACE_LOG("%s LoadModule fails", strModulePath.c_str());
                return;
            }

          initialized_ = true;
        }

        bool FFManager::DecodeJpeg(ES_CMN_FUNCS::BUFFER::CESHeapBuffer&  dataBuf, 
                                   ESNumber expectedHeight,
                                   ESNumber expectedRawImageSize,
                                   bool decodeOnlyValid
                                   )
        {  
           	SDI_TRACE_LOG("Enter");
            
            ES_CMN_FUNCS::BUFFER::CESHeapBuffer outBuf;
            {
                if (initialized_ == false)
                {
                   	SDI_TRACE_LOG("FFManager not initialized");
                    return false;
                }

            }

            IFKDestination* destination = m_pcManageFileFormatKit->Call_CreateAndInitFKDestination(kFKDestinationTypeData, outBuf);
            if (destination == nullptr)
            {
                SDI_TRACE_LOG("destination create fails");
                return false;
            }        
        	IFKReader*		reader	    =  m_pcManageFileFormatKit->Call_CreateAndInitFKReaderInstance(kFKReaderTypeJpeg);;
            if (reader == nullptr)
            {
                SDI_TRACE_LOG("reader create fails");
                return false;
            }


            {
		         ENUM_FK_ERROR_CODE eLocalError = kFKNoError;
                 BOOL isSuccess = reader->OpenWithDestination(destination, eLocalError);
                 if (isSuccess == false)
                 {
                    SDI_TRACE_LOG("OpenWithDestination fails");
                     return false;
                 }
            }
           reader->SetValidImageHeight(expectedHeight,decodeOnlyValid);
            {
            		ENUM_FK_ERROR_CODE fkError = kFKNoError;
                    IFKSource* source = m_pcManageFileFormatKit->Call_CreateAndInitFKSource(kFKSourceTypeData,dataBuf , FALSE );
           			BOOL isSuccess = reader->AppendSource(source, fkError);
                    if (isSuccess == false)
                    {
                        SDI_TRACE_LOG("AppendSource fails");
                         return false;
                    }
                    source->Destroy();
             }
             {
		           ENUM_FK_ERROR_CODE fkError = kFKNoError;
                   BOOL bSuccess  = reader->CloseAndReturnError(fkError);
                    if (bSuccess == false)
                    {
                        SDI_TRACE_LOG("CloseAndReturnError fails");
                         return false;
                    }
             }
             {
                dataBuf.Attach(destination->GetSource());
                reader->Destroy();
             }
           	SDI_TRACE_LOG("Leave");
            return true;
        }


    	bool FFManager::Write(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& buf, 
                              std::string path, 
                              SDIImageFormat format, 
                              const ESImageInfo& imageInfo,
                              KeyMgr& keyMgr,
                              bool releaseBufferWhenDone)
        {
            	SDI_TRACE_LOG("Enter format = %d ", format);

                bool isSuccess = true;

            {
                if (initialized_ == false)
                {
                   	SDI_TRACE_LOG("FFManager not initialized");
                    return false;
                }

            }
                IFKDestination* pcDestination = NULL;
                IFKWriter* writer = NULL;
                {
                    isSuccess = Create(path.c_str(), format, &pcDestination, &writer);
                    ES_ErrorBail(isSuccess, BAIL, this, L"Create fails");
                }

                {
                    isSuccess = Open(writer, format, imageInfo, false, keyMgr);
                    ES_ErrorBail(isSuccess, BAIL, this, L"Open fails");
                }

                {
                    isSuccess = DoWrite(writer, buf, releaseBufferWhenDone);
                    ES_ErrorBail(isSuccess, BAIL, this, L"DoWrite fails");
                }

                {
                    isSuccess = Close(writer);
                    ES_ErrorBail(isSuccess, BAIL, this, L"Close fails");
                }

                {
                     isSuccess = Finalize(&writer);
                     ES_ErrorBail(isSuccess, BAIL, this, L"Finalize fails"); 
                }

            BAIL:;

                if (isSuccess == false)
               {
                    if (writer)
                    {
                        writer->Destroy();
                        writer = NULL;
                   }
                }
       
                ES_Trace_Log2(__FUNCTION__, L"Leave RawWriter isSuccess = %d", isSuccess);

                return isSuccess;
        }


        static  FKWriterType WriteType(SDIImageFormat lFormat)
        {
            switch (lFormat)
            {
                case kSDIImageFormatTIFF:
                    return kFKWriterTypeTiff;
                    
                case kSDIImageFormatJPEG:
                return kFKWriterTypeJpeg;

                case kSDIImageFormatPDF:
                    return kFKWriterTypePDF;
                    
                case kSDIImageFormatPNG:
                    return kFKWriterTypePng;

                case kSDIImageFormatPNM:
                    return kFKWriterTypePnm;

                case kSDIImageFormatBMP:
                    return kFKWriterTypeBmp;
                
                case kSDIImageFormatOFD:
                    return kFKWriterTypeOFD;
            }

            return kFKWriterTypeBmp;
        }

       bool FFManager::Create(const char* filePath, 
                              SDIImageFormat format, 
                              IFKDestination** outpcDestination,
                              IFKWriter** outpcWriter)
       {
           bool isSuccess = true;
        	SDI_TRACE_LOG("Enter");
	        ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBufSource(filePath);

    	    if (ES_CMN_FUNCS::PATH::ES_IsExistFile(filePath))
	        {
		        ::DeleteFile(filePath);
	        }


            if (outpcDestination)
            {

                {
                    ESString strOptionAsJson;
                    {
                        ESDictionary dictOption;
                        ES_CMN_FUNCS::JSON::DictionaryToJSON(dictOption, strOptionAsJson);
                    }

                    *outpcWriter = m_pcManageFileFormatKit->Call_CreateAndInitFKWriterInstance(WriteType(format), strOptionAsJson.c_str());
                    ES_ErrorBailWithAction(*outpcWriter, BAIL, this, isSuccess = false, L"m_pcWriter is nil");

                    *outpcDestination = m_pcManageFileFormatKit->Call_CreateAndInitFKDestination(kFKDestinationTypePath, cBufSource);
                    ES_ErrorBailWithAction(outpcDestination, BAIL, this, isSuccess = false, L"destinationWithType fails");

                }
                {
                    ENUM_FK_ERROR_CODE eLocalError = kFKNoError;
                    ESString strOptionAsJson;
                    {
                        ESDictionary dictOption;
                        // Initialize options
                        ES_CMN_FUNCS::JSON::DictionaryToJSON(dictOption, strOptionAsJson);
                    }

                    if ((*outpcWriter)->InitializeDestinationAsJson(*outpcDestination, strOptionAsJson.c_str(), eLocalError) == FALSE)
                    {
                        ES_Trace_Log2(__FUNCTION__, L"Initialize fails %d", eLocalError);
                        isSuccess = false;
                    }
                }
            }
            BAIL:;

           	SDI_TRACE_LOG("Leave isSuccess = %d", isSuccess);

            return isSuccess;
       }



        bool FFManager::Open(IFKWriter* pcWriter,
                             SDIImageFormat format,  
                             const ESImageInfo& imageInfo, 
                             bool isMulti,
                             KeyMgr& keyMgr
                              )
        {
            SDI_TRACE_LOG("Enter");

            {
                ENUM_FK_ERROR_CODE eFKError = kFKNoError;
                ESDictionary dictWriterOption;
                if ( isMulti) 
                {
                    dictWriterOption[kFKWriterPageOpenOptionTiffAppendModeKey] = true;
                }else
                {
                    dictWriterOption[kFKWriterPageOpenOptionTiffAppendModeKey] = false;
                }

                if (keyMgr.GetValueInt(kSDITiffCompressionKey) == kSDITiffCompressionCCITFAX4)
                {
                     dictWriterOption[kFKWriterPageOpenOptionTiffCompressionKey] = (ESNumber)kFKWriterPageOpenOptionTiffCompressionCCITG4;
                }else
                {
                     dictWriterOption[kFKWriterPageOpenOptionTiffCompressionKey] = (ESNumber)kFKWriterPageOpenOptionTiffCompressionNone;
                }

                 if (keyMgr.GetValueInt(kSDIJpegQuality) != 0)
                {
                    dictWriterOption[kFKWriterPageOpenOptionJpegQualityKey] = (ESNumber)keyMgr.GetValueInt(kSDIJpegQuality);
                }

                if (keyMgr.GetValueInt(kSDIJpegProgressiveKey) != 0)
                {
                    dictWriterOption[kFKWriterPageOpenOptionJpegProgressiveModeKey] = true;
                }
                else
                {
                    dictWriterOption[kFKWriterPageOpenOptionJpegProgressiveModeKey] = false;                        
                }

                ESString strOption;
                ES_CMN_FUNCS::JSON::DictionaryToJSON(dictWriterOption, strOption);

                ESString strImageInfo;
                ES_IMAGE_INFO::GetImageInfoAsJson(strImageInfo, imageInfo);

                SDI_TRACE_LOG("jsonImageInfo = %s ", strImageInfo.c_str());

                //Imageîñðwè·éBA³A
                if (pcWriter->OpenPageWithImageInfoAsJson(strImageInfo.c_str(), strOption.c_str(), eFKError) == FALSE)
                {
                    SDI_TRACE_LOG(__FUNCTION__, L"Leave OpenPageWithImageInfoAsJson fails");
                    return false;
                }
            }

            SDI_TRACE_LOG("Leave");

            return true;
        }

         bool FFManager::DoWrite(IFKWriter* pcWriter, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& buf, bool releaseBufferWhenDone)
         {
            SDI_TRACE_LOG("Enter");

            IFKSource* pcSource = m_pcManageFileFormatKit->Call_CreateAndInitFKSource(kFKSourceTypeData, buf, FALSE);
            if (pcSource == nullptr)
            {
                SDI_TRACE_LOG("pcSource create fails");
                return false;
            }

            ENUM_FK_ERROR_CODE eFKError = kFKNoError;
            if (pcWriter->AppendSource(pcSource, eFKError) == FALSE) {
                  SDI_TRACE_LOG("AppendSource fails");
				return false;
			}

            if (releaseBufferWhenDone == false)
            {
                buf.Attach(pcSource->GetSource());
            }

            pcSource->Destroy();
            pcSource = nullptr;

        	SDI_TRACE_LOG("Leave success");
            return true;
         }

         bool FFManager::Close(IFKWriter* pcWriter)
         {
            SDI_TRACE_LOG("Enter");
            ENUM_FK_ERROR_CODE eFKError = kFKNoError;

            if (pcWriter->ClosePageAndReturnError(eFKError) == FALSE) {
		           return false;
            }

            SDI_TRACE_LOG("Leave");
           return true;
         }

         bool FFManager::Finalize(IFKWriter** pcWriter)
         {
             	SDI_TRACE_LOG("Enter");

                bool isSuccess = true;
                ENUM_FK_ERROR_CODE eFKError = kFKNoError;

                if ((*pcWriter)->FinalizeAndReturnError(eFKError) == FALSE) {
                    isSuccess = false;
                }
                ES_ErrorBail(isSuccess, BAIL, this, L"FinalizeAndReturnError fails -- eFKError:[%d]", eFKError);

                if (*pcWriter)
                {
                    (*pcWriter)->Destroy();
                    *pcWriter = NULL;
                 }    

            BAIL:;
                SDI_TRACE_LOG(L"Leave");

            	return isSuccess;
         }


        void error_handler  (HPDF_STATUS error_no,
                        HPDF_STATUS detail_no,
                        void *user_data)
        {
                SDI_TRACE_LOG("Haru PDF error");
        }

        bool FFManager::MultipageStart(const char* destPath,  SDIImageFormat format, std::shared_ptr<KeyMgr> keyMgr)
        {
        	SDI_TRACE_LOG("Enter");

            if (multipageStarted_)
            {
                return false;
            }

            keyMgrForMulti_ = keyMgr;

            bool isSuccess = true;

            formatForMulti_ = format;
            destPath_ = destPath;
            if (formatForMulti_ == kSDIImageFormatPDF)
            {
                pdf_ = HPDF_New (error_handler, NULL);
            }
            else if(format == kSDIImageFormatOFD )
            {
                ofdPluginModule_ = dlopen("/opt/epson/epsonscan2-ofd-component/libepsonscan2-ofd-component.so", RTLD_LAZY) ;
                if (ofdPluginModule_ == nullptr)
                {
                    SDI_TRACE_LOG("ofd module is null");
                    return false;
                }

                OFDPluginCreatePtr pCreateFunc = (OFDPluginCreatePtr)dlsym(ofdPluginModule_, "OFDPluginCreate");
                 if (pCreateFunc == nullptr)
                 {
                    SDI_TRACE_LOG("OFDPluginCreate is null");
                    return false;
                 }

                 if (pCreateFunc(&ofdPlugin_) != kOfdErrorNone)
                 {
                      SDI_TRACE_LOG("plugin create error");
                      return false;
                  }
                  SDI_TRACE_LOG("ofd OFDPluginCreate okay");

                 auto inFile = CESFile::CreateTempFileInstanceWithPrefix(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath() ,  "OFDTemp", CESFile::ES_OPEN_MODE_WRITE_PLUS);
                 inFile->CloseFile();

                  

                  OfdOption option = {0};;
                  strncpy((char*)option.targetFile, destPath, OFD_OPTION_MAX_PATH);
                  sprintf((char*)option.tempPDFFile,"%.250s.pdf", inFile->GetFileName().c_str());

                  auto err = ofdPlugin_->Initialize(&option);
                  if (err != kOfdErrorNone)
                  {
                      SDI_TRACE_LOG("Initialize error");
                      return false;
                  }

                  delete inFile;
                  inFile = nullptr;
            }
            else
            {
               IFKDestination* destForMulti;

                {
                    isSuccess = Create(destPath, format, &destForMulti, &writerForMulti_);
                    ES_ErrorBail(isSuccess, BAIL, this, L"Create fails");
                }
            }

        BAIL:;
            SDI_TRACE_LOG("Leave isSuccess = %d", isSuccess);

            multipageStarted_ = true;
            return isSuccess;
        }

        static void GetPNMInfoFromHeader(ESString pnmFile, int& width, int& height, int& offset , int &samplesPerPixel, int& bitsPerSample)
        {
           std::ifstream infile(pnmFile);
           std::string inputLine = "";
           getline(infile,inputLine);
            
           if(sscanf (inputLine.c_str(),"P6 %d %d", &width,&height) == 2)
           {
                offset = infile.tellg();
                samplesPerPixel = 3;
                bitsPerSample = 8;
                return;
           }
           if(sscanf (inputLine.c_str(),"P5 %d %d", &width,&height) == 2)
           {
                offset = infile.tellg();
                samplesPerPixel = 1;
                bitsPerSample = 8;
                return;
           }
           if(sscanf (inputLine.c_str(),"P4 %d %d", &width,&height) == 2)
           {
                offset = infile.tellg();
                samplesPerPixel = 1;
                bitsPerSample = 1;
                return;
           }    
           bool isMono = false;
           if(strcmp(inputLine.c_str(), "P4") == 0)
           {
                isMono = true;
                samplesPerPixel = 1;
                bitsPerSample = 1;
           }
           if(strcmp(inputLine.c_str(), "P5") == 0)
           {
                samplesPerPixel = 1;
                bitsPerSample = 8;
           }
           if(strcmp(inputLine.c_str(), "P6") == 0)
           {
                samplesPerPixel = 3;
                bitsPerSample = 8;
           }
           getline(infile,inputLine);               
           if(sscanf (inputLine.c_str(),"%d %d", &width,&height) == 2)
           {
                if (isMono == false)
                {
                    // P6, P5
                    getline(infile,inputLine);
                    offset = infile.tellg();
                }else{
                    offset = infile.tellg();
                }
                return;
           }
        }

        template <typename T>
        static bool SafeBoostAnyCast_Local2(const ESAny &anyIn, T &outputValue)
        {
            try
            {
                outputValue = boost::any_cast<T>(anyIn);
                return true;
            }
            catch (...)
            {
            }
            return false;
        }

        void FFManager::GetJpegHeader(ESString jpegFile, int& width, int& height)
        {
            SDI_TRACE_LOG("Enter");
            
            ES_CMN_FUNCS::BUFFER::CESHeapBuffer outBuf;
            ES_CMN_FUNCS::BUFFER::CESHeapBuffer dataBuf;

            CESFile file;
            file.Open(jpegFile);
            file.ReadDataToEndOfFile(dataBuf);
            file.CloseFile();


            IFKDestination* destination = m_pcManageFileFormatKit->Call_CreateAndInitFKDestination(kFKDestinationTypeEmpty, outBuf);
            if (destination == nullptr)
            {
                SDI_TRACE_LOG("destination create fails");
                return;
            }        
        	IFKReader* reader =  m_pcManageFileFormatKit->Call_CreateAndInitFKReaderInstance(kFKReaderTypeJpeg);;
            if (reader == nullptr)
            {
                SDI_TRACE_LOG("reader create fails");
                return;
            }
            {
		         ENUM_FK_ERROR_CODE eLocalError = kFKNoError;
                 BOOL isSuccess = reader->OpenWithDestination(destination, eLocalError);
                 if (isSuccess == false)
                 {
                    SDI_TRACE_LOG("OpenWithDestination fails");
                     return;
                 }
            }
            {
            		ENUM_FK_ERROR_CODE fkError = kFKNoError;
                    IFKSource* source = m_pcManageFileFormatKit->Call_CreateAndInitFKSource(kFKSourceTypeData,dataBuf , FALSE );
           			BOOL isSuccess = reader->AppendSource(source, fkError);
                    if (isSuccess == false)
                    {
                        SDI_TRACE_LOG("AppendSource fails");
                         return;
                    }
                    source->Destroy();
             }
             {
                  CESResultString imageInfoString;
                  reader->GetImageInfoAsJson(imageInfoString);

                  ESString dictImageInfo(imageInfoString.Get());
                  
                  ESDictionary dictOption;
                  ES_CMN_FUNCS::JSON::JSONtoDictionary(dictImageInfo, dictOption);

                  ESNumber outWidth = 0;
                  ESNumber outHeight = 0;
  
  	        	  SafeBoostAnyCast_Local2(dictOption[ES_IMAGE_INFO::kESImageInfoWidthKey], outWidth);
  	        	  SafeBoostAnyCast_Local2(dictOption[ES_IMAGE_INFO::kESImageInfoHeightKey], outHeight);

                  width  = outWidth;
                  height  =  outHeight;

             }
             {
		           ENUM_FK_ERROR_CODE fkError = kFKNoError;
                   BOOL bSuccess  = reader->CloseAndReturnError(fkError);
                    if (bSuccess == false)
                    {
                        SDI_TRACE_LOG("CloseAndReturnError fails");
                         return ;
                    }
             }
             {
                reader->Destroy();
             }
           	SDI_TRACE_LOG("Leave");          
        }

        bool FFManager::ConvertPNMToRAW(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inPNMDataBuf, bool isMono, int offset)
        {
            auto buffer = inPNMDataBuf.GetBufferPtr();
            auto length = inPNMDataBuf.GetLength();
                  
          if (buffer != nullptr)
          {
                int startPos = offset;

                 ES_CMN_FUNCS::BUFFER::CESHeapBuffer outBuffer;
                 outBuffer.AllocBuffer(inPNMDataBuf.GetLength() - startPos);
                 auto outBytes = outBuffer.GetBufferPtr();

                 if (startPos < length)
                 {
                    // Flip bits
                    for (uint32_t i = startPos; i < length; i++)
                    {
                        if (isMono)
                        {
                            outBytes[i - startPos] = 255 - buffer[i];
                        }else{
                            outBytes[i - startPos] = buffer[i];
                        }
                    }

                    inPNMDataBuf.FreeBuffer();
                    inPNMDataBuf.Attach(outBuffer);
                 }else{
                     return false;
                 }
          }else{
              return false;
          }

          return true;
        }

        bool  FFManager::MultipageAdd(const char* sourcePath)
        {
        	SDI_TRACE_LOG("Enter");

            if (multipageStarted_ == false)
            {
                return false;
            }

            bool isSuccess = true;

            int width = 0;
            int height = 0;
            int samplesPerPixel = 0;
            int bitsPerSample = 0;
            int resolution = 0;

            std::string fileName;
            ES_CMN_FUNCS::PATH::ES_GetFileName(fileName, sourcePath, FALSE);

            sscanf(fileName.c_str(), "%d_%d_%d_%d_%d_", &width, &height, &samplesPerPixel, &bitsPerSample, &resolution);
            SDI_TRACE_LOG("input image is %d %d %d %d", width, height, samplesPerPixel, bitsPerSample);

            int offset = 0;

            if (formatForMulti_ == kSDIImageFormatPDF)
            {
                if (bitsPerSample != 1)
                {
                    this->GetJpegHeader(sourcePath, width, height);
                }else{
                    GetPNMInfoFromHeader(sourcePath, width, height, offset, samplesPerPixel, bitsPerSample);
                }
             	SDI_TRACE_LOG("will add PDF");

                HPDF_Page page;
                HPDF_Destination dst;

                page = HPDF_AddPage (pdf_);

                HPDF_Page_SetWidth (page, (float)width / resolution * 72);
                HPDF_Page_SetHeight (page, (float)height / resolution * 72);
                HPDF_SetCompressionMode (pdf_, HPDF_COMP_ALL);
        
                HPDF_Image image;
                if (bitsPerSample != 1)
                {
                    image = HPDF_LoadJpegImageFromFile (pdf_, sourcePath);
                }else{
                    CESFile file;
                    file.Open(sourcePath);
                    ES_CMN_FUNCS::BUFFER::CESHeapBuffer inDataBuf;
                    file.ReadDataToEndOfFile(inDataBuf);
                    file.CloseFile();

                    isSuccess = ConvertPNMToRAW(inDataBuf, true, offset);
                    ES_ErrorBail(isSuccess, BAIL, this, L"ConvertPNMToRAW fails");


                    image = HPDF_Image_LoadRaw1BitImageFromMem(pdf_, (const HPDF_BYTE *)inDataBuf.GetConstBufferPtr(), width, height, (width + 7) / 8 , true, true);
                }

                HPDF_Page_DrawImage (page, image, 0, 0, HPDF_Page_GetWidth (page), HPDF_Page_GetHeight (page));

             	SDI_TRACE_LOG("did add PDF");

            }
            else if(formatForMulti_ == kSDIImageFormatOFD ) 
            {
                if (ofdPlugin_ && ofdPluginModule_)
                {

                    ES_CMN_FUNCS::BUFFER::CESHeapBuffer inDataBuf;

                    GetPNMInfoFromHeader(sourcePath, width, height, offset, samplesPerPixel, bitsPerSample);

                   {
                     CESFile file;
                     file.Open(sourcePath);
                     file.ReadDataToEndOfFile(inDataBuf);
                     file.CloseFile();
                  }
                  {
                     isSuccess = ConvertPNMToRAW(inDataBuf,  bitsPerSample == 1, offset);
                     ES_ErrorBail(isSuccess, BAIL, this, L"ConvertPNMToRAW fails");
                   }

                    OfdImage image = {0};
            
                    ESImageInfo imageInfo;

                    imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] = (ESNumber)width;
                    imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] = (ESNumber)height;
                    imageInfo[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey] = (ESNumber)samplesPerPixel;
                    imageInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey] = (ESNumber)bitsPerSample;
                    imageInfo[ES_IMAGE_INFO::kESImageInfoResolutionXKey] = (ESNumber)resolution;
                    imageInfo[ES_IMAGE_INFO::kESImageInfoResolutionYKey] = (ESNumber)resolution;
                    imageInfo[ES_IMAGE_INFO::kESImageInfoOutputResolutionKey] = (ESNumber)resolution;

                    image.width = width;
                    image.height = height;
                    image.samplesPerPixel = samplesPerPixel;
                    image.bitsPerSample = bitsPerSample;
                    image.bytesPerRow = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
                    image.imageData = inDataBuf.GetBufferPtr();
                    image.resolution = resolution;

                    auto err = ofdPlugin_->Add(&image);
                    if (err != kOfdErrorNone)
                    {
                        SDI_TRACE_LOG("ofdPlugin_ add error %d", err);
                        isSuccess = false;
                        goto BAIL;
                    }
                }

            }            
            else
            {
                GetPNMInfoFromHeader(sourcePath, width, height, offset, samplesPerPixel, bitsPerSample);

                SDI_TRACE_LOG("will add FFKit");

                ESImageInfo imageInfo;

                imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] = (ESNumber)width;
                imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] = (ESNumber)height;
                imageInfo[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey] = (ESNumber)samplesPerPixel;
                imageInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey] = (ESNumber)bitsPerSample;
                imageInfo[ES_IMAGE_INFO::kESImageInfoResolutionXKey] = (ESNumber)resolution;
                imageInfo[ES_IMAGE_INFO::kESImageInfoResolutionYKey] = (ESNumber)resolution;
                imageInfo[ES_IMAGE_INFO::kESImageInfoOutputResolutionKey] = (ESNumber)resolution;

                CESFile file;
                file.Open(sourcePath);
                ES_CMN_FUNCS::BUFFER::CESHeapBuffer inDataBuf;
                file.ReadDataToEndOfFile(inDataBuf);
                file.CloseFile();

                {
                    isSuccess = Open(writerForMulti_, formatForMulti_, imageInfo, true, *keyMgrForMulti_);
                    ES_ErrorBail(isSuccess, BAIL, this, L"Open fails");
                }

                // Convert PNM to RAW
                {
                    isSuccess = ConvertPNMToRAW(inDataBuf,  bitsPerSample == 1, offset);
                     ES_ErrorBail(isSuccess, BAIL, this, L"ConvertPNMToRAW fails");
                }
                {
                    isSuccess = DoWrite(writerForMulti_, inDataBuf);
                    ES_ErrorBail(isSuccess, BAIL, this, L"DoWrite fails");
                }

                {
                    isSuccess = Close(writerForMulti_);
                    ES_ErrorBail(isSuccess, BAIL, this, L"Close fails");
                }
             	SDI_TRACE_LOG("did add FFKit");
            }

        BAIL:;

			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(sourcePath))
			{
				DeleteFile(sourcePath);
			}

            SDI_TRACE_LOG("Leave isSuccess = %d", isSuccess);

            return isSuccess;
        }

        bool FFManager::MultipageFin()
        {
        	SDI_TRACE_LOG("Enter");
            bool isSuccess = true;
            if (multipageStarted_ == false)
            {
                return false;
            }

            if (formatForMulti_ == kSDIImageFormatPDF)
            {
                if (ES_CMN_FUNCS::PATH::ES_IsExistFile(destPath_))
    	        {
		            ::DeleteFile(destPath_.c_str());
	            }
                if (HPDF_SaveToFile (pdf_, destPath_.c_str()) != HPDF_OK)
                {
                    isSuccess = false;
                }
                HPDF_Free (pdf_);
            }
            else if(formatForMulti_ == kSDIImageFormatOFD ) 
            {
                auto err = ofdPlugin_->Fin();
                if (err != kOfdErrorNone)
                {
                    isSuccess = false;
                    goto BAIL;
                }

                OFDPluginFreePtr pFreeFunc = (OFDPluginFreePtr)dlsym(ofdPluginModule_, "OFDPluginFree");
                if (pFreeFunc == nullptr)
                {
                    SDI_TRACE_LOG("OFDPluginFree dlsym");
                    isSuccess = false;
                    goto BAIL;
                }

                if (pFreeFunc(ofdPlugin_) != kOfdErrorNone)
                {
                    SDI_TRACE_LOG("free error");
                   
                    isSuccess = false;
                    goto BAIL;
                }
                ofdPlugin_ = nullptr;

                dlclose(ofdPluginModule_);
                ofdPluginModule_ = nullptr;

                SDI_TRACE_LOG("free ok");
            }else{

                {
                     isSuccess = Finalize(&writerForMulti_);
                     ES_ErrorBail(isSuccess, BAIL, this, L"Finalize fails"); 
                }
            }

            keyMgrForMulti_ = nullptr;
        BAIL:;
             SDI_TRACE_LOG("Leave isSuccess = %d", isSuccess);

             multipageStarted_ = false;
            return isSuccess;
        }


}