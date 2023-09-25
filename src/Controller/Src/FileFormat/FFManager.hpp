#pragma once
#include "EpsonScan2.h"
#include "ESImageInfo.h"
#include "hpdf.h"
#include "KeyMgr.hpp"
#include <utils/ESBuffer.h>
#include <ES2FileFormat/FKPAPI.h>

class CESManageFileFormatKit;
class IOfdPlugin;

namespace epsonscan
{
    class FFManager
    {
    public:

        static FFManager& GetInstance()
        {
            static FFManager ffM;
            return ffM;
        }
    
        FFManager();


        bool DecodeJpeg(ES_CMN_FUNCS::BUFFER::CESHeapBuffer&  dataBuf, 
                        ESNumber expectedHeight,
                        ESNumber expectedRawImageSize,
                        bool decodeOnlyValid
                        );

        // For signle page

    	bool Write(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& buf, 
                    std::string path, 
                   SDIImageFormat format, 
                   const ESImageInfo& imageInfo,
                   KeyMgr& keyMgr,
                   bool releaseBufferWhenDone = true
                   );

        // For multi page

        bool MultipageStart(const char* destPath,  SDIImageFormat format, std::shared_ptr<KeyMgr> keyMgr);
        bool MultipageAdd(const char* sourcePath);
        bool MultipageFin();

    private:

        bool Create(const char* filePath, 
                    SDIImageFormat format,
                    IFKDestination** outpcDestination,
                    IFKWriter** outpcWriter);
        bool Open(IFKWriter* pcWriter,
                  SDIImageFormat format,  
                  const ESImageInfo& imageInfo, 
                  bool isMulti ,
                  KeyMgr& keyMgr
                  );
        
        bool DoWrite(IFKWriter* pcWriter, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& buf, bool releaseBufferWhenDone = true);
        bool Close(IFKWriter* pcWriter);
        bool Finalize(IFKWriter** pcWriter);

        void GetJpegHeader(ESString jpegFile, int& width, int& height);
        
        bool ConvertPNMToRAW(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inPNMDataBuf, bool isMono, int offset);
        bool initialized_;
    	CESManageFileFormatKit* m_pcManageFileFormatKit;
        
        IFKWriter* writerForMulti_;
        std::string destPath_;
        SDIImageFormat formatForMulti_;

        HPDF_Doc  pdf_;
        std::shared_ptr<KeyMgr> keyMgrForMulti_;

        void* ofdPluginModule_;
        IOfdPlugin* ofdPlugin_;
        

        bool multipageStarted_ = false;
    };
}

