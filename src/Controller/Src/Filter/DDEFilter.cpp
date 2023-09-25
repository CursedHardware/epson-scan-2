
#include "Utility.hpp"
#include "DDEFilter.hpp"
#include <stdlib.h>
#include <utils/PathUtils.h>
#include <utils/ESFile.h>
#include <ES2Command/ES2CommandAPI.h>
#include <math.h>

namespace epsonscan
{

static const int kMaxBuf = 256;

enum ReturnCode : int
{
    kSuccess = 0,
    kError   = 1
};

bool DDEFilter::IsDDEAvailable()
{
    std::string pluginRoot = Filter::GetPluginRootDir();
    ESString ddePlugin;
    ESString ddeso;
    ESString imfso;
    ES_CMN_FUNCS::PATH::ES_CombinePath(ddePlugin, pluginRoot, ES_STRING("ddeplugin"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(ddeso, pluginRoot, ES_STRING("libdde6.so"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(imfso, pluginRoot, ES_STRING("libimf6.so"));

    SDI_TRACE_LOG("ddePlugin = %s", ddePlugin.c_str());

    if (ES_CMN_FUNCS::PATH::ES_IsExistFile(ddePlugin.c_str()) &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(ddeso.c_str()) &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(imfso.c_str())) {
            return true;
    }

    return false;
}


void DDEFilter::DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) {
{

    if (IsDDEAvailable())
    {
        std::string pluginRoot = Filter::GetPluginRootDir();

        ESString ddePlugin;
        ES_CMN_FUNCS::PATH::ES_CombinePath(ddePlugin, pluginRoot, ES_STRING("ddeplugin"));


        auto inFile = CESFile::CreateTempFileInstanceWithPrefix( ES_CMN_FUNCS::PATH::ES_GetWorkTempPath(),  "DTRInput", CESFile::ES_OPEN_MODE_WRITE_PLUS);
        if (inFile == nullptr)
        {
            SDI_TRACE_LOG("DTRInput file creates error");
            throw std::bad_alloc();
            return;
        }

        if(inFile->WriteData(inDataBuf) == false)
        {
            SDI_TRACE_LOG("WriteData fails");
            delete inFile;
            inFile = nullptr;
            throw std::bad_alloc();
            return;
        }
        inFile->Flush();
        inFile->CloseFile();

        auto outFile = CESFile::CreateTempFileInstanceWithPrefix(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath(),  "DDEOutFile", CESFile::ES_OPEN_MODE_WRITE_PLUS );
        if (outFile == nullptr)
        {
            SDI_TRACE_LOG("DDEOutFile file creates error");
            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
             {
                remove(inFile->GetFileName().c_str());
             }
             delete inFile;
             inFile = nullptr;
            return ;
        }

       ESNumber ddeID = 0;

        // ADF 
       if (keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder ||
           keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitCarrierSheet)
       {
           modelInfo_->GetValue(kPrvHRD_DDEADF, ddeID);
       }else{
        //Flatbed
           modelInfo_->GetValue(kPrvHRD_DDEFlatbed, ddeID);
       }

        //OCR文字くっきりのモデルは固定パラメーター
        {
            ESNumber ocrPresion = 0;
            modelInfo_->GetValue(kPrvHRD_OCRPrecision, ocrPresion);
            if (ocrPresion){
                ddeID = 30;
            }
        }


       std::stringstream ddeExecCommand;
       ddeExecCommand << ddePlugin;
       ddeExecCommand << " ";
       if (actionType_ == kActionTypeBin)
       {
           ddeExecCommand << "0";//Action Type Bin
       }else{
           ddeExecCommand << "1";//Action Type ADE
       }
       ddeExecCommand << " ";
       ddeExecCommand << ddeID;//Param ID
       ddeExecCommand << " ";
       ddeExecCommand << pluginRoot;//Param Folder
       ddeExecCommand << " ";
       ddeExecCommand << pluginRoot;//Param Folder
       ddeExecCommand << " ";
       ddeExecCommand << ES_IMAGE_INFO::GetESImageWidth(imageInfo);//inWidth
       ddeExecCommand << " ";
       ddeExecCommand << ES_IMAGE_INFO::GetESImageHeight(imageInfo);//inHeight
       ddeExecCommand << " ";
       ddeExecCommand << ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);//inBytesPerRow
       ddeExecCommand << " ";
       ddeExecCommand << ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);//inResolution
       ddeExecCommand << " ";
       ddeExecCommand << inFile->GetFileName(); // inFile
       ddeExecCommand << " ";
       ddeExecCommand << outFile->GetFileName(); // outFile
       ddeExecCommand << " ";
       ddeExecCommand << keyMgr_->GetValueInt(kSDITextEnhanceNoiseReductionLevelKey);//unNoiseReductionLevel
       ddeExecCommand << " ";
       ddeExecCommand << keyMgr_->GetValueInt(kSDITextEnhanceSensitivityKey);//nSensitivity
       ddeExecCommand << " ";
       ddeExecCommand << keyMgr_->GetValueInt(kSDITextEnhancePaperCreaseReductionKey);//unPaperCreaseReduction
       ddeExecCommand << " ";
       ddeExecCommand << keyMgr_->GetValueInt(kSDITextEnhanceThresholdAdjustmentKey);//unPaperCreaseReduction
       ddeExecCommand << " ";
       ddeExecCommand << keyMgr_->GetValueInt(kSDIBrightnessKey);//nBrightness
       ddeExecCommand << " ";
       ddeExecCommand << keyMgr_->GetValueInt(kSDIContrastKey);//nContrast
       ddeExecCommand << " ";
       ddeExecCommand << 22;//nScreenGamma
       ddeExecCommand << " ";
       ddeExecCommand << ((keyMgr_->GetValueInt(kSDITextEnhanceKey) ==  kSDITextEnhanceLevelStandard) ?  0 : 1);//level

       SDI_TRACE_LOG("ddeExecCommand %s", ddeExecCommand.str().c_str());

       FILE* gid = popen(ddeExecCommand.str().c_str(), "r");
       if (gid)
       {
            SDI_TRACE_LOG("process init success");

            if(pclose(gid) == kError)
            {
                delete inFile;
                delete outFile;
                 SDI_TRACE_LOG("Leave kSDIErrorNoMemory");
                throw std::bad_alloc();
            }
        }

        if (actionType_ == kActionTypeBin)
        {
                imageInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey] = (ESNumber)1;
        }

        ES_CMN_FUNCS::BUFFER::CESHeapBuffer dataBuf;
        outFile->ReadDataToEndOfFile(dataBuf);
        inDataBuf.FreeBuffer();
		inDataBuf.Attach(dataBuf);
        if (inDataBuf.GetLength() == 0)
        {
             delete inFile;
             delete outFile;
             SDI_TRACE_LOG("dataBuf is empty");
             throw std::bad_alloc();      
        }

       if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
        {
           remove(inFile->GetFileName().c_str());
        }

        if(ES_CMN_FUNCS::PATH::ES_IsExistFile(outFile->GetFileName()))
        {
            remove(outFile->GetFileName().c_str());
        }
        
        delete inFile;
        inFile = nullptr;
        delete outFile;
        outFile = nullptr;
    }

    SDI_TRACE_LOG("Leave kSDIErrorNone");
    return;
  }
}

}
