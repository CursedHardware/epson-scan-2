
#include "Utility.hpp"
#include "DTR.hpp"
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
    kError   = 1,
    kResultIsBlank = 10,
    kResultIsNotBlank = 11,
    kResultIsColor = 20,
    kResultIsBlackWhite = 21,
    kResultIsGray = 22
};

enum OpeType : int 
{
    kGetDocumentLocation = 0,
    kJudgeBlankPage = 1,
    kJudgeColorType = 2,
    kRemovePunchHole = 3
};

#define	GAMMAREVISION(x)		(255.*pow(((x)/255.) , (1.0/1.8)))
#define MAX_IN(A,B,C) (std::max(A, std::max(B,C)))

void ConvertBackground10To18(const ESDictionary& backgroundColor,ESDictionary& convertedColor)
{
	int32_t rMax = std::min(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyRed,ESNumber,0) + SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyRedWidth,ESNumber,0), 255);
    int32_t rMin = std::max(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyRed,ESNumber,0) - SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyRedWidth,ESNumber,0), 0);
    int32_t convertedRMax = (float)GAMMAREVISION((float)rMax);
    int32_t convertedRMin = (float)GAMMAREVISION((float)rMin);
    int32_t convertedRRange = (convertedRMax - convertedRMin) ;
    uint32_t convertedR    = std::min(convertedRMin + convertedRRange/ 2, 255);
    
	int32_t gMax = std::min(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyGreen,ESNumber,0) + SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyGreenWidth,ESNumber,0), 255);
    int32_t gMin = std::max(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyGreen,ESNumber,0) - SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyGreenWidth,ESNumber,0), 0);
    int32_t convertedGMax = (float)GAMMAREVISION((float)gMax);
    int32_t convertedGMin = (float)GAMMAREVISION((float)gMin);
    int32_t convertedGRange= (convertedGMax - convertedGMin) ;
    uint32_t convertedG    = std::min(convertedGMin + convertedGRange/ 2, 255);
    
	int32_t bMax = std::min(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyBlue,ESNumber,0) + SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyBlueWidth,ESNumber,0), 255);
    int32_t bMin = std::max(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyBlue,ESNumber,0) - SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyBlueWidth,ESNumber,0), 0);
    int32_t convertedBMax = (float)GAMMAREVISION((float)bMax);
    int32_t convertedBMin = (float)GAMMAREVISION((float)bMin);
    int32_t convertedBRange= (convertedBMax - convertedBMin) ;
    uint32_t convertedB    = std::min(convertedBMin + convertedBRange/ 2, 255);

	int32_t mMax = std::min(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyMono,ESNumber,0) + SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyMonoWidth,ESNumber,0), 255);
    int32_t mMin = std::max(SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyMono,ESNumber,0) - SAFE_ANY_VALUE_FROM_DICT(backgroundColor,kESBGLevelKeyMonoWidth,ESNumber,0), 0);
    int32_t convertedMMax = (float)GAMMAREVISION((float)mMax);
    int32_t convertedMMin = (float)GAMMAREVISION((float)mMin);
    int32_t convertedMRange= (convertedMMax - convertedMMin) ;
    uint32_t convertedM    = std::min(convertedMMin + convertedMRange/ 2, 255);
    
 
	convertedColor.clear();
	convertedColor[kESBGLevelKeyRed]       = (ESNumber)convertedR;
	convertedColor[kESBGLevelKeyGreen]     = (ESNumber)convertedG;
	convertedColor[kESBGLevelKeyBlue]      = (ESNumber)convertedB;
	convertedColor[kESBGLevelKeyMono]      = (ESNumber)convertedM;
	convertedColor[kESBGLevelKeyRedWidth]  = (ESNumber)convertedRRange;
	convertedColor[kESBGLevelKeyGreenWidth] = (ESNumber)convertedGRange;
	convertedColor[kESBGLevelKeyBlueWidth] = (ESNumber)convertedBRange;
	convertedColor[kESBGLevelKeyMonoWidth] = (ESNumber)convertedMRange;   
}

bool DTR::IsDTRAvailable()
{
    std::string pluginRoot = Filter::GetPluginRootDir();
    ESString dtrplugin;
    ESString dtrso;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrplugin, pluginRoot, ES_STRING("dtrplugin"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrso, pluginRoot, ES_STRING("libesdtr.so"));
    return ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrplugin.c_str()) &&
           ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrso.c_str());
}

void DTR::DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) 
{
    if (actionType_ == kActionTypeGetDocumentLocation )
    {

        bool isHardWareCropEnabled = (keyMgr_->GetCommandValueAsBool(kESAutoCroppingInscribed) || keyMgr_->GetCommandValueAsBool(kESAutoCropping) );

        SDI_TRACE_LOG("isHardWareCropEnabled = %d", isHardWareCropEnabled);

        AutoSize* autoSize =(AutoSize*)(keyMgr_->GetKeyInstance(kSDIAutoSizeKey).get());   

        if (keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder &&
           (keyMgr_->GetValueInt(kSDIAutoSizeKey) == kSDIAutoSizeLongPaper ||
            autoSize->GetProcessSoftwareAutoCrop() ||
            (isHardWareCropEnabled && ES_IMAGE_INFO::GetESImageAutoCropped(imageInfo) == FALSE)))
        {
                if(DoAutoCrop(imageInfo,
                              inDataBuf, 
                              modelInfo_,
                              ES_CMN_FUNCS::PATH::ES_GetWorkTempPath()) != kSDIErrorNone)
                    {
                        SDI_TRACE_LOG("Auto crop fails");
                        throw RuntimeError("Do auto crop fails");     
                    } 

        }
    }

    if (actionType_ == kActionTypeJudgeBlankPage )
    {
		if (keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder &&
            (keyMgr_->GetValueInt(kSDIBlankPageSkipKey) != 0 &&
            keyMgr_->GetValueInt(kSDIBlankPageSkipLevelKey) != 0))
		{
			if(JudgeBlankPage(imageInfo,
		  					   inDataBuf, 
							   modelInfo_,
							   keyMgr_->GetValueInt(kSDIBlankPageSkipLevelKey),
							   ES_CMN_FUNCS::PATH::ES_GetWorkTempPath()) == true)
			{
					SDI_TRACE_LOG("page = %d is blankpage", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
					isBlankPage_ = true;
			}
		}
    }
    if (actionType_ == kActionTypeJudgeColorType)
    {
        if (keyMgr_->GetValueInt(kSDIColorTypeKey) == kSDIColorTypeAuto)
		{
			if(JudgeColorType(imageInfo,
		  					   inDataBuf, 
							   modelInfo_,
                               keyMgr_,
							   ES_CMN_FUNCS::PATH::ES_GetWorkTempPath(), 
                               autoColorPixel_
                               ) == true)
			{
				SDI_TRACE_LOG("page = %d is blankpage", ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
				isColor_ = true;
			}else{
                isColor_ = false;
            }
		}
    }     
    if (actionType_ == kActionTypeRemovePunchHole)
    {

        if ( keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder &&
             keyMgr_->GetValueInt(kSDIRemovePunchholeKey) != kSDIRemovePunchHoleNone)
        {
                if(DoRemovePunchHole(imageInfo,
                                     inDataBuf, 
                                     modelInfo_,
                                     keyMgr_->GetValueInt(kSDIRemovePunchholeKey),
                                     ES_CMN_FUNCS::PATH::ES_GetWorkTempPath()) != kSDIErrorNone)
                {
                    SDI_TRACE_LOG("Auto crop fails");
                    throw RuntimeError("Do auto crop fails");     
                } 
         }
    }
}

SDIError DTR::DoAutoCrop(ESImageInfo& imageInfo, 
				                ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                                std::shared_ptr<ModelInfo> modelInfo,
                                std::string workFolder
                                )
{
    SDI_TRACE_LOG("Enter");
   
    std::string pluginRoot = Filter::GetPluginRootDir();
    ESString dtrplugin;
    ESString dtrso;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrplugin, pluginRoot, ES_STRING("dtrplugin"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrso, pluginRoot, ES_STRING("libesdtr.so"));

    SDI_TRACE_LOG("dtrplugin = %s", dtrplugin.c_str());

    if (ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrplugin.c_str()) &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrso.c_str()))
    {
        SDI_TRACE_LOG("dtrplugin Exists");

        auto inFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "DTRInput", CESFile::ES_OPEN_MODE_WRITE_PLUS);
        if (inFile == nullptr)
        {
            SDI_TRACE_LOG("DTRInput file creates error");
            return kSDIErrorNoMemory;
        }

        if(inFile->WriteData(inDataBuf) == false)
        {
            SDI_TRACE_LOG("WriteData fails");

            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
            {
                remove(inFile->GetFileName().c_str());
            }

            delete inFile;
            inFile = nullptr;
            return kSDIErrorWriteFile;
        }
        inFile->Flush();
        inFile->CloseFile();

        auto outFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "DTROutput", CESFile::ES_OPEN_MODE_WRITE_PLUS );
        if (outFile == nullptr)
        {
            SDI_TRACE_LOG("DTROutput file creates error");
            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
             {
                remove(inFile->GetFileName().c_str());
             }
             delete inFile;
             inFile = nullptr;

            return kSDIErrorNoMemory;
        }
        ESNumber isCarrieSheet = false;

        if(ES_IMAGE_INFO::GetESImageCarrierSheetType(imageInfo) == ES_IMAGE_INFO::kESImageInfoCarrierSheetTypeON)
        {
            isCarrieSheet = true;
        }

        ESString dtiPath;
        modelInfo->GetDtiPath(dtiPath);
        SDI_TRACE_LOG("modelID = %d, dtiPath = %s", modelInfo->GetModelID(), dtiPath.c_str());

        ESDictionary bgLevel;
        if (ES_IMAGE_INFO::GetESImageSurfaceType(imageInfo) == ES_IMAGE_INFO::kESImageInfoSurfaceTypeFrontSide)
        {
            bgLevel = modelInfo->GetBackGroundColorFront();
        }else{
            bgLevel =  modelInfo->GetBackGroundColorBack();
        }

        ESDictionary convertedBg;
        ConvertBackground10To18(bgLevel, convertedBg);

        ESNumber bgRed = 0;
        ESNumber bgGreen = 0;
        ESNumber bgBlue = 0;
        ESNumber bgRange = 0;

        if(ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo) == 1) {
            bgRed = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyMono	,ESNumber,0));
            bgGreen = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyMono		,ESNumber,0));
            bgBlue = (SAFE_ANY_VALUE_FROM_DICT(convertedBg,kESBGLevelKeyMono		,ESNumber,0));
            bgRange = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyMonoWidth,ESNumber,0));
        }else{
            bgRed = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyRed		,ESNumber,0));
            bgGreen = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyGreen	,ESNumber,0));
            bgBlue = (SAFE_ANY_VALUE_FROM_DICT(convertedBg,kESBGLevelKeyBlue		,ESNumber,0));
            bgRange = MAX_IN(	 SAFE_ANY_VALUE_FROM_DICT(convertedBg,kESBGLevelKeyRedWidth	,ESNumber,0)
                                ,SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyGreenWidth		,ESNumber,0)
                                ,SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyBlueWidth		,ESNumber,0));
           // bgRange += 20; // margin
        }
 
        SDI_TRACE_LOG("bgRed = %d bgGreen = %d bgBlue = %d bgRange = %d", bgRed, bgGreen, bgBlue, bgRange);

        std::stringstream dtrExecCommand;
        dtrExecCommand << dtrplugin;
        dtrExecCommand << " ";
        dtrExecCommand << modelInfo->GetModelID();
        dtrExecCommand << " ";        
        dtrExecCommand << dtiPath;
        dtrExecCommand << " ";
        dtrExecCommand << (ESNumber)kGetDocumentLocation;
        dtrExecCommand << " ";
        dtrExecCommand << bgRed;
        dtrExecCommand << " ";
        dtrExecCommand << bgGreen;
        dtrExecCommand << " ";
        dtrExecCommand << bgBlue;
        dtrExecCommand << " ";
        dtrExecCommand << bgRange;
        dtrExecCommand << " ";
        dtrExecCommand << isCarrieSheet;
        dtrExecCommand << " ";
        dtrExecCommand << inFile->GetFileName();
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageWidth(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageHeight(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << outFile->GetFileName();
        dtrExecCommand << " ";
        dtrExecCommand << "70";
        dtrExecCommand << " ";
        dtrExecCommand << dtrso;

        SDI_TRACE_LOG("dtrExecCommand = %s", dtrExecCommand.str().c_str());

        FILE* gid = popen(dtrExecCommand.str().c_str(), "r");
        if (gid)
        {
            SDI_TRACE_LOG("process init success");

            char buf[kMaxBuf];
            if(fgets(buf, kMaxBuf, gid))
            {
                int width = 0;
                int height = 0;
                sscanf(buf, "%d_%d", &width, &height);
                imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] = (ESNumber)width;
                imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] = (ESNumber)height;

                SDI_TRACE_LOG("DTR Skew Result = %d %d", width, height);
            }

            if(pclose(gid) == kError)
            {
                delete inFile;
                delete outFile;
                 SDI_TRACE_LOG("Leave kSDIErrorNoMemory");
                return kSDIErrorNoMemory;
            }
        }

        ES_CMN_FUNCS::BUFFER::CESHeapBuffer dataBuf;
        outFile->ReadDataToEndOfFile(dataBuf);
        inDataBuf.FreeBuffer();
		inDataBuf.Attach(dataBuf);


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
    return kSDIErrorNone;
}

ESNumber CustomSkipLevelAsPercent(ESNumber inLevel)
{
	if (0 <= inLevel) {
		if (inLevel < 10) {
			return inLevel;
		}
		if (inLevel < 19) {
			return (10 * (inLevel - 9));
		}
		if (inLevel < 31) {
			return (100 * (inLevel - 18));
		}
	}	
	return 0;
}

 SDIError DTR::DoRemovePunchHole(ESImageInfo& imageInfo, 
				                      ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                                      std::shared_ptr<ModelInfo> modelInfo,
                                      ESNumber removeArea,
                                      std::string workFolder)
{
    SDI_TRACE_LOG("Enter");
   
    std::string pluginRoot = Filter::GetPluginRootDir();
    ESString dtrplugin;
    ESString dtrso;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrplugin, pluginRoot, ES_STRING("dtrplugin"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrso, pluginRoot, ES_STRING("libesdtr.so"));

    SDI_TRACE_LOG("dtrplugin = %s", dtrplugin.c_str());

    if (ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrplugin.c_str()) &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrso.c_str()))
    {
        SDI_TRACE_LOG("dtrplugin Exists");

        auto inFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "DTRInput", CESFile::ES_OPEN_MODE_WRITE_PLUS);
        if (inFile == nullptr)
        {
            SDI_TRACE_LOG("DTRInput file creates error");
            return kSDIErrorNoMemory;
        }

        if(inFile->WriteData(inDataBuf) == false)
        {
            SDI_TRACE_LOG("WriteData fails");

            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
            {
                remove(inFile->GetFileName().c_str());
            }

            delete inFile;
            inFile = nullptr;
            return kSDIErrorWriteFile;
        }
        inFile->Flush();
        inFile->CloseFile();

        auto outFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "DTROutput", CESFile::ES_OPEN_MODE_WRITE_PLUS );
        if (outFile == nullptr)
        {
            SDI_TRACE_LOG("DTROutput file creates error");
            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(inFile->GetFileName()))
             {
                remove(inFile->GetFileName().c_str());
                delete inFile;
                inFile = nullptr;
             }

            return kSDIErrorNoMemory;
        }
        ESNumber isCarrieSheet = false;

        if(ES_IMAGE_INFO::GetESImageCarrierSheetType(imageInfo) == ES_IMAGE_INFO::kESImageInfoCarrierSheetTypeON)
        {
            isCarrieSheet = true;
        }

        ESString dtiPath;
        modelInfo->GetDtiPath(dtiPath);
        SDI_TRACE_LOG("modelID = %d, dtiPath = %s", modelInfo->GetModelID(), dtiPath.c_str());

        ESDictionary bgLevel;
        if (ES_IMAGE_INFO::GetESImageSurfaceType(imageInfo) == ES_IMAGE_INFO::kESImageInfoSurfaceTypeFrontSide)
        {
            bgLevel = modelInfo->GetBackGroundColorFront();
        }else{
            bgLevel =  modelInfo->GetBackGroundColorBack();
        }

        ESDictionary convertedBg;
        ConvertBackground10To18(bgLevel, convertedBg);

        ESNumber bgRed = 0;
        ESNumber bgGreen = 0;
        ESNumber bgBlue = 0;
        ESNumber bgRange = 0;

        if(ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo) == 1) {
            bgRed = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyMono	,ESNumber,0));
            bgGreen = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyMono		,ESNumber,0));
            bgBlue = (SAFE_ANY_VALUE_FROM_DICT(convertedBg,kESBGLevelKeyMono		,ESNumber,0));
            bgRange = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyMonoWidth,ESNumber,0));
        }else{
            bgRed = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyRed		,ESNumber,0));
            bgGreen = (SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyGreen	,ESNumber,0));
            bgBlue = (SAFE_ANY_VALUE_FROM_DICT(convertedBg,kESBGLevelKeyBlue		,ESNumber,0));
            bgRange = MAX_IN(	 SAFE_ANY_VALUE_FROM_DICT(convertedBg,kESBGLevelKeyRedWidth	,ESNumber,0)
                                ,SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyGreenWidth		,ESNumber,0)
                                ,SAFE_ANY_VALUE_FROM_DICT(convertedBg, kESBGLevelKeyBlueWidth		,ESNumber,0));
           // bgRange += 20; // margin
        }
 
        SDI_TRACE_LOG("bgRed = %d bgGreen = %d bgBlue = %d bgRange = %d", bgRed, bgGreen, bgBlue, bgRange);

        std::stringstream dtrExecCommand;
        dtrExecCommand << dtrplugin;
        dtrExecCommand << " ";
        dtrExecCommand << modelInfo->GetModelID();
        dtrExecCommand << " ";        
        dtrExecCommand << dtiPath;
        dtrExecCommand << " ";
        dtrExecCommand << (ESNumber)kRemovePunchHole;
        dtrExecCommand << " ";
        dtrExecCommand << bgRed;
        dtrExecCommand << " ";
        dtrExecCommand << bgGreen;
        dtrExecCommand << " ";
        dtrExecCommand << bgBlue;
        dtrExecCommand << " ";
        dtrExecCommand << bgRange;
        dtrExecCommand << " ";
        dtrExecCommand << isCarrieSheet;
        dtrExecCommand << " ";
        dtrExecCommand << inFile->GetFileName();
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageWidth(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageHeight(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << outFile->GetFileName();
        dtrExecCommand << " ";
        dtrExecCommand << "70";
        dtrExecCommand << " ";
        dtrExecCommand << dtrso;
        dtrExecCommand << " ";
        dtrExecCommand << (ESNumber)removeArea - 1;

        SDI_TRACE_LOG("dtrExecCommand = %s", dtrExecCommand.str().c_str());

        FILE* gid = popen(dtrExecCommand.str().c_str(), "r");
        if (gid)
        {
            SDI_TRACE_LOG("process init success");

            char buf[kMaxBuf];
            if(fgets(buf, kMaxBuf, gid))
            {
                int width = 0;
                int height = 0;
                sscanf(buf, "%d_%d", &width, &height);
                imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] = (ESNumber)width;
                imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] = (ESNumber)height;

                SDI_TRACE_LOG("DTR Skew Result = %d %d", width, height);
            }

            if(pclose(gid) == kError)
            {
                delete inFile;
                delete outFile;
                 SDI_TRACE_LOG("Leave kSDIErrorNoMemory");
                return kSDIErrorNoMemory;
            }
        }

        ES_CMN_FUNCS::BUFFER::CESHeapBuffer dataBuf;
        outFile->ReadDataToEndOfFile(dataBuf);
        inDataBuf.FreeBuffer();
		inDataBuf.Attach(dataBuf);


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
    return kSDIErrorNone;
}

 bool DTR::JudgeColorType(ESImageInfo& imageInfo, 
				               ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                               std::shared_ptr<ModelInfo> modelInfo,
                               std::shared_ptr<KeyMgr> keyMgr,
                               std::string workFolder,
                               SDIAutoColorPixelType &autoColorPiexelType
                               )
{
    std::string pluginRoot = Filter::GetPluginRootDir();
    ESString dtrplugin;
    ESString dtrso;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrplugin, pluginRoot, ES_STRING("dtrplugin"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrso, pluginRoot, ES_STRING("libesdtr.so"));

    SDI_TRACE_LOG("dtrplugin = %s", dtrplugin.c_str());

    if (ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrplugin.c_str()) &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrso.c_str()))
    {
        auto inFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "DTRInput", CESFile::ES_OPEN_MODE_WRITE_PLUS);
        if (inFile == nullptr)
        {
            SDI_TRACE_LOG("DTRInput file creates error");
            return kSDIErrorNoMemory;
        }

        if(inFile->WriteData(inDataBuf) == false)
        {
            SDI_TRACE_LOG("WriteData fails");
            delete inFile;
            inFile = nullptr;
            return kSDIErrorWriteFile;
        }
        inFile->Flush();
        inFile->CloseFile();

        ESString dtiPath;
        modelInfo->GetDtiPath(dtiPath);
        SDI_TRACE_LOG("modelID = %d, dtiPath = %s", modelInfo->GetModelID(), dtiPath.c_str());

       std::stringstream dtrExecCommand;
        dtrExecCommand << dtrplugin;
        dtrExecCommand << " ";
        dtrExecCommand << modelInfo->GetModelID();
        dtrExecCommand << " ";        
        dtrExecCommand << dtiPath;
        dtrExecCommand << " ";
        dtrExecCommand << (ESNumber)kJudgeColorType;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << inFile->GetFileName();
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageWidth(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageHeight(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << "none";
        dtrExecCommand << " ";
        dtrExecCommand << "0";
        dtrExecCommand << " ";
        dtrExecCommand << dtrso;
        dtrExecCommand << " ";
        dtrExecCommand << "0";
        dtrExecCommand << " ";
        dtrExecCommand << (keyMgr->GetValueInt(kSDIAutoColorPixelTypeKey) == kSDIAutoColorPixelTypeAuto) ? 1 : 0;
        dtrExecCommand << " ";
        dtrExecCommand << keyMgr->GetValueInt(kSDIAutoColorPixelLevelKey);

        SDI_TRACE_LOG("dtrExecCommand = %s", dtrExecCommand.str().c_str());

        bool judgeResult = false;
        FILE* gid = popen(dtrExecCommand.str().c_str(), "r");
        if (gid)
        {
            int exitCode = pclose(gid);
            if (WEXITSTATUS(exitCode) == kResultIsColor)
            {
                SDI_TRACE_LOG("Leave colorPage");
                judgeResult = true;
            }else if (WEXITSTATUS(exitCode) == kResultIsGray)
            {
                SDI_TRACE_LOG("Leave GrayPage");
                judgeResult = false;
                autoColorPiexelType = kSDIAutoColorPixelTypeGray;
            }else{
                SDI_TRACE_LOG("Leave not not color page exitCode = %d", exitCode);
                judgeResult = false;
                if (keyMgr->GetValueInt(kSDIAutoColorPixelTypeKey) == kSDIAutoColorPixelTypeGray)
                {
                    autoColorPiexelType = kSDIAutoColorPixelTypeGray;
                }else
                {
                    autoColorPiexelType = kSDIAutoColorPixelTypeMono;
                }
            }
        }

        if (inFile)
        {
            ESString filePath = inFile->GetFileName();

            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(filePath))
            {
                remove(filePath.c_str());
            }

            delete inFile;
            inFile = nullptr;
        }

        return judgeResult;
    }
   return true;
}

bool DTR::JudgeBlankPage(ESImageInfo& imageInfo, 
				               ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                               std::shared_ptr<ModelInfo> modelInfo,
                               ESNumber skipLevel,
                               std::string workFolder)
{
    std::string pluginRoot = Filter::GetPluginRootDir();
    ESString dtrplugin;
    ESString dtrso;
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrplugin, pluginRoot, ES_STRING("dtrplugin"));
    ES_CMN_FUNCS::PATH::ES_CombinePath(dtrso, pluginRoot, ES_STRING("libesdtr.so"));

    SDI_TRACE_LOG("dtrplugin = %s", dtrplugin.c_str());

    if (ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrplugin.c_str()) &&
        ES_CMN_FUNCS::PATH::ES_IsExistFile(dtrso.c_str()))
    {
        auto inFile = CESFile::CreateTempFileInstanceWithPrefix(workFolder,  "DTRInput", CESFile::ES_OPEN_MODE_WRITE_PLUS);
        if (inFile == nullptr)
        {
            SDI_TRACE_LOG("DTRInput file creates error");
            return kSDIErrorNoMemory;
        }

        if(inFile->WriteData(inDataBuf) == false)
        {
            SDI_TRACE_LOG("WriteData fails");
            delete inFile;
            inFile = nullptr;
            return kSDIErrorWriteFile;
        }
        inFile->Flush();
        inFile->CloseFile();

        ESString dtiPath;
        modelInfo->GetDtiPath(dtiPath);
        SDI_TRACE_LOG("modelID = %d, dtiPath = %s", modelInfo->GetModelID(), dtiPath.c_str());

       std::stringstream dtrExecCommand;
        dtrExecCommand << dtrplugin;
        dtrExecCommand << " ";
        dtrExecCommand << modelInfo->GetModelID();
        dtrExecCommand << " ";        
        dtrExecCommand << dtiPath;
        dtrExecCommand << " ";
        dtrExecCommand << (ESNumber)kJudgeBlankPage;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << 0;
        dtrExecCommand << " ";
        dtrExecCommand << inFile->GetFileName();
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageWidth(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageHeight(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
        dtrExecCommand << " ";
        dtrExecCommand << "none";
        dtrExecCommand << " ";
        dtrExecCommand << CustomSkipLevelAsPercent(skipLevel);
        dtrExecCommand << " ";
        dtrExecCommand << dtrso;
        SDI_TRACE_LOG("dtrExecCommand = %s", dtrExecCommand.str().c_str());

        bool judgeResult = false;
        FILE* gid = popen(dtrExecCommand.str().c_str(), "r");
        if (gid)
        {
            int exitCode = pclose(gid);
            if (WEXITSTATUS(exitCode) == kResultIsBlank)
            {
                SDI_TRACE_LOG("Leave BLANKPAGE");
                judgeResult = true;
            }else{
                SDI_TRACE_LOG("Leave not BLANKPAGE exitCode = %d", exitCode);
                judgeResult = false;
            }
        }

        if (inFile)
        {
            ESString filePath = inFile->GetFileName();

            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(filePath))
            {
                remove(filePath.c_str());
            }

            delete inFile;
            inFile = nullptr;
        }

        return judgeResult;
    }
   return false;
}

}
