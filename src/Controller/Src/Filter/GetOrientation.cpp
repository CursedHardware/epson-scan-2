
#include "GetOrientation.hpp"
#include "../FileFormat/FFManager.hpp"
#include <utils/ESBuffer.h>
#include <utils/ESFile.h>
#include <utils/PathUtils.h>
#include "../../../Include/Config.h"
#include "Saving.hpp"

namespace epsonscan
{

static std::string GetExecPath()
{
#ifdef AKBMODE
    std::string strDst = "/usr/libexec/";
    strDst = strDst + DRIVER_NAME;
    strDst = strDst + "-ocr/ocr-engine-getrotate";
    return strDst.c_str() ;
#else
    return "/usr/libexec/epsonscan2-ocr/ocr-engine-getrotate" ;
#endif
}
static const int kMaxBuf = 256;

void GetOrientation::DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
{
    SDI_TRACE_LOG("Enter");
    
    if (keyMgr_->GetValueInt(kSDIOrientationKey) == kSDIOrientationAuto &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(GetExecPath().c_str()))
    {
        SDIOrientation result = kSDIOrientationNone;
         SDI_TRACE_LOG("will do get orientation");

        epsonscan::Saving::Convert16To8(imageInfo, inDataBuf);

        auto ffMgr = FFManager::GetInstance();
        auto file = CESFile::CreateTempFileInstanceWithPrefix(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath(), "GetOrientation");
        file->CloseFile();

        if(ffMgr.Write(inDataBuf, // input buffer  
                    file->GetFileName(), // destination
                    kSDIImageFormatBMP,
                    imageInfo,
                    *keyMgr_,
                    false
                    )  == false)
        {
                delete file;
                file = nullptr;
                SDI_TRACE_LOG("ffMgr.Write fails %d", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
                throw std::bad_alloc();
        }

        SDI_TRACE_LOG("Bmp Creates success = %s",  file->GetFileName().c_str());

        std::stringstream execCommand;
        execCommand << GetExecPath();
        execCommand << " ";
        execCommand << file->GetFileName();


        SDI_TRACE_LOG("execCommand = %s", execCommand.str().c_str());

        FILE* gid = popen(execCommand.str().c_str(), "r");
        if (gid)
        {
            SDI_TRACE_LOG("process init success");

            char buf[kMaxBuf];
            if(fgets(buf, kMaxBuf, gid))
            {
                int rotationAngle = 0;
                sscanf(buf, "Orientation in degrees: %d", &rotationAngle);
                SDI_TRACE_LOG("Orientation Result = %d", rotationAngle);

                if (rotationAngle == 0) {result = kSDIOrientationNone;}
                else if (rotationAngle == 90) {result = kSDIOrientation90;} 
                else if (rotationAngle == 180) {result = kSDIOrientation180;}
                else if (rotationAngle == 270) {result = kSDIOrientation270;}
                else {result = kSDIOrientationNone;}
            }
        }
        if(pclose(gid) != 0)
        {
            delete file;
            SDI_TRACE_LOG("Leave kSDIErrorNoMemory");
            throw std::bad_alloc();
        }

        judgeResult_ = result;

        delete file;
        file = nullptr;
    }

    SDI_TRACE_LOG("Leave");

}

bool GetOrientation::IsPluginAvailable()
{
    return ES_CMN_FUNCS::PATH::ES_IsExistFile(GetExecPath().c_str());
}



}
