    
#include "ESIMP2.hpp"
#include "ESIMP2Wrapper.h"
#include <utils/PathUtils.h>
#include "PaperDeskew.hpp"
#include "DeficiencyCorrection.hpp"
#include <math.h>

namespace epsonscan
{


    class MyESIMP2Wrapper
    {
        public:
            MyESIMP2Wrapper(std::string imp2Path) : fnCreatePtr_(nullptr), fnFreePtr_(nullptr), wrapper_(nullptr)
            {
                SDI_TRACE_LOG("Run constructor");      

                esimp2_ = dlopen(imp2Path.c_str(), RTLD_LAZY);
                if(esimp2_ == nullptr)
                {
                    std::string error = dlerror();
                    throw RuntimeError("dtr2Lib is null");            
                }

                fnCreatePtr_ = (IESIMP2WrapperCreatePtr)dlsym(esimp2_, "IESIMP2WrapperCreate");
                if (fnCreatePtr_ == nullptr)
                {
                    throw RuntimeError("fnCreatePtr is null");            
                }

                fnFreePtr_ = (IESIMP2WrapperFreePtr)dlsym(esimp2_, "IESIMP2WrapperFree");
                if (fnFreePtr_ == nullptr)
                {
                    throw RuntimeError("fnFreePtr is null");            
                }

                if (fnCreatePtr_(&wrapper_) != kESIMP2WrapperErrorNone)
                {
                    throw RuntimeError("IESIMP2Wrapper create fails");            
                }
                if (wrapper_ == nullptr)
                {
                    throw RuntimeError("wrapper is nullptr");            
                }
            }


            IESIMP2Wrapper* GetWrapper()
            {
                return wrapper_;
            }

            virtual ~MyESIMP2Wrapper()
            {
                SDI_TRACE_LOG("Run destrractor");      
                if (wrapper_)
                {
                    fnFreePtr_(wrapper_);
                    wrapper_ = nullptr;
                }

                 if (esimp2_)
                 {
                   dlclose(esimp2_);
                   esimp2_ = nullptr;
                 }   
            }

        private:
           void *esimp2_;
           IESIMP2WrapperCreatePtr fnCreatePtr_;
           IESIMP2WrapperFreePtr fnFreePtr_;
           IESIMP2Wrapper *wrapper_;
    };

    ESIMP2::ESIMP2(std::shared_ptr<KeyMgr> keyMgr,
                     std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo), locationData_(nullptr)
    {   
    }

    ESIMP2::~ESIMP2()
    {
        if (locationData_)
        {
             free(locationData_);
             locationData_ = nullptr;
        }
    }

    void ESIMP2::Detect(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
    {
        AutoSize* autoSize =(AutoSize*)(keyMgr_->GetKeyInstance(kSDIAutoSizeKey).get());   
        bool isCarrerSheetCrop = IsCarrerSheetCrop(imageInfo);

        if (autoSize->IsAdvancedSotwareAutoCropEnabled() || isCarrerSheetCrop)
        {
            if (IsESIMP2Available())
            {
                MyESIMP2Wrapper wrapper(GetESIMP2Path());

                ESIMP2WrapperCropParam param = {0};
                ESIMP2WrapperImageData inImage = {0};

                locationData_ = (ESIMP2WrapperLocationData*)malloc(sizeof(ESIMP2WrapperLocationData));

                inImage.width = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
                inImage.height = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
                inImage.samplesPerPixel= ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
                inImage.bitsPerSample = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
                inImage.dataBuffer = inDataBuf.GetBufferPtr();
                inImage.resolutionX = ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
                inImage.resolutionY = ES_IMAGE_INFO::GetESImageResolutionY(imageInfo);

                param.modelID = modelInfo_->GetModelID();
                param.isCarrierSheet = isCarrerSheetCrop;
                                
                wrapper.GetWrapper()->Detect(&param, &inImage, (ESIMP2WrapperLocationData*)locationData_);      
                SDI_TRACE_LOG("Detect completed");      


                if (keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder)
                {
                       ESIMP2WrapperLocationData* locData = (ESIMP2WrapperLocationData*)locationData_;


                     //　定型サイズの場合には定型化する
                    if (keyMgr_->GetValueInt(kSDIAutoSizeKey) == kSDIAutoSizeNone)
                    {

                         // インチでのターゲットサイズを求める
                        double widthAsInch = (double)keyMgr_->GetValueInt(kSDIScanAreaWidthKey) / (double)keyMgr_->GetValueInt(kSDIResolutionKey);
                        double heightAsInch = (double)keyMgr_->GetValueInt(kSDIScanAreaHeightKey) / (double)keyMgr_->GetValueInt(kSDIResolutionKey);

                        // サイズの定型化を行う
                        double fixedSizeX = ES_IMAGE_INFO::GetESImageResolutionX(imageInfo) * widthAsInch;
                        double fixedSizeY = ES_IMAGE_INFO::GetESImageResolutionY(imageInfo) * heightAsInch;

                        // x,y の不足分
                       double xDiff = fixedSizeX - locData->actImgW;
                       double yDiff = fixedSizeY - locData->actImgH;

                       locData->actImgH = locData->actImgH + yDiff;
                       locData->actImgW = locData->actImgW + xDiff;
                       locData->leftPoint = locData->leftPoint - xDiff / 2;

                        // locateH, locateWの再計算
                        double radian = fabs(locData->theta * 0.0174533);
                        
                        
                        if (locData->xTangent > 0)
                        {
                            locData->locateW =  locData->actImgW * cos(radian) + locData->actImgH * sin(radian);
                        }else{
                            locData->locateW =  locData->actImgW * cos(radian) + locData->actImgH * sin(radian) + locData->xTangent;
                        }
                        
                        if (locData->yTangent > 0)
                        {
                            locData->locateH =  locData->actImgH * cos(radian) + locData->actImgW * sin(radian);
                        }else{
                            locData->locateH =  locData->actImgH * cos(radian) + locData->actImgW * sin(radian) + locData->yTangent;
                        }

                    }
                
                     PaperDeskew* paperDeskew = (PaperDeskew*)keyMgr_->GetKeyInstance(kSDIPaperDeskewKey).get();
                     if (paperDeskew)
                     {
                        if (paperDeskew->IsAutoDeskewForWhiteBGEnabled() == false)
                        {
                            locData->theta = 0;
                            locData->xTangent = 0;
                            locData->yTangent = 0;
                            locData->actImgH = locData->locateH;
                            locData->actImgW = locData->locateW;
                        }
                     }
                }            


            }
        }

    }

    bool ESIMP2::IsCarrerSheetCrop(ESImageInfo& imageInfo)
    {
        bool isHardWareCropEnabled = (keyMgr_->GetCommandValueAsBool(kESAutoCroppingInscribed) || keyMgr_->GetCommandValueAsBool(kESAutoCropping) );
        return isHardWareCropEnabled && 
                ES_IMAGE_INFO::GetESImageAutoCropped(imageInfo) == FALSE &&
                keyMgr_->GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder &&
                keyMgr_->GetValueInt(kSDIBackgroundColorKey) == kSDIBackgroundColorWhite;
    }

    void ESIMP2::Crop(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
    {
        AutoSize* autoSize =(AutoSize*)(keyMgr_->GetKeyInstance(kSDIAutoSizeKey).get());   
        bool isCarrerSheetCrop = IsCarrerSheetCrop(imageInfo);

        if (autoSize->IsAdvancedSotwareAutoCropEnabled() || isCarrerSheetCrop)
        {
            if (IsESIMP2Available() && locationData_)
            {
                MyESIMP2Wrapper wrapper(GetESIMP2Path());

                ESIMP2WrapperCropParam param = {0};
                ESIMP2WrapperImageData inImage = {0};
                ESIMP2WrapperImageData outImage = {0};

                ESString dtiPath;
                modelInfo_->GetDtiPath(dtiPath);
                SDI_TRACE_LOG("modelID = %d, dtiPath = %s", modelInfo_->GetModelID(), dtiPath.c_str());

                ESString esimp2Path = GetESIMP2Path();

                ESString pluginRoot = Filter::GetPluginRootDir();
                ESString dtrPath;
                ES_CMN_FUNCS::PATH::ES_CombinePath(dtrPath, pluginRoot, ES_STRING("libesdtr.so"));

                param.modelID = modelInfo_->GetModelID();
                strncpy(param.dtiParam, dtiPath.c_str(), ESIMP_MAX_PATH - 1);
                strncpy(param.esimp2Path, esimp2Path.c_str(), ESIMP_MAX_PATH - 1);
                strncpy(param.dtrPath, dtrPath.c_str(), ESIMP_MAX_PATH - 1);

                ESIMP2WrapperLocationData* locData = (ESIMP2WrapperLocationData*)locationData_;
                SDI_TRACE_LOG("locData->actImgW = %d", locData->actImgW);
                SDI_TRACE_LOG("locData->actImgH = %d", locData->actImgH);
                SDI_TRACE_LOG("locData->leftPoint = %d", locData->leftPoint);
                SDI_TRACE_LOG("locData->topPoint = %d", locData->topPoint);
                SDI_TRACE_LOG("locData->locateH = %d", locData->locateH);
                SDI_TRACE_LOG("locData->locateW = %d", locData->locateW);
                SDI_TRACE_LOG("locData->theta = %lf", locData->theta);

                inImage.width = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
                inImage.height = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
                inImage.samplesPerPixel= ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
                inImage.bitsPerSample = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
                inImage.dataBuffer = inDataBuf.GetBufferPtr();
                inImage.resolutionX = ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
                inImage.resolutionY = ES_IMAGE_INFO::GetESImageResolutionY(imageInfo);


                double scaleX = (double)inImage.resolutionX / (double)locData->analysisResolutionX;
                double scaleY = (double)inImage.resolutionY / (double)locData->analysisResolutionY;

                locData->topPoint = locData->topPoint * scaleY;
                locData->leftPoint = locData->leftPoint * scaleX;
                locData->locateW = locData->locateW * scaleX;
                locData->locateH = locData->locateH * scaleY;
                locData->actImgW = locData->actImgW * scaleX;
                locData->actImgH = locData->actImgH * scaleY;
                locData->xTangent = locData->xTangent * scaleX;
                locData->yTangent = locData->yTangent * scaleY;

                outImage.width = locData->actImgW;
                outImage.height = locData->actImgH;
                outImage.samplesPerPixel= ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
                outImage.bitsPerSample = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
                outImage.resolutionX = ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
                outImage.resolutionY = ES_IMAGE_INFO::GetESImageResolutionY(imageInfo);

                ES_CMN_FUNCS::BUFFER::CESHeapBuffer outBuffer;
                if(outBuffer.AllocBuffer(outImage.width  * outImage.height *  outImage.samplesPerPixel) == false) {
                    throw std::bad_alloc();
                }
                outImage.dataBuffer = outBuffer.GetBufferPtr();


                wrapper.GetWrapper()->Crop(&param, locData, &inImage, &outImage);

                free(locationData_);
                locationData_ = nullptr;

                imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] = (ESNumber)outImage.width;
                imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] = (ESNumber)outImage.height;

                inDataBuf.Attach(outBuffer);

                SDI_TRACE_LOG("Crop completed");      
            }
        }
    }

    void ESIMP2::DoDeficiencyCorrection(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
    {
         DeficiencyCorrection* deficiencyCorrection = (DeficiencyCorrection*)(keyMgr_->GetKeyInstance(kSDIDeficiencyCorrectionKey).get());
        if(deficiencyCorrection->IsProcessDeficiencyCorrection() &&  IsESIMP2Available()) 
        {
            MyESIMP2Wrapper wrapper(GetESIMP2Path());
            ESIMP2WrapperDeficiencyDetectRepairParam param = {0};

            ESDictionary bgLevel;
            if (ES_IMAGE_INFO::GetESImageSurfaceType(imageInfo) == ES_IMAGE_INFO::kESImageInfoSurfaceTypeFrontSide)
            {
                bgLevel = modelInfo_->GetBackGroundColorFront();
            }else{
                bgLevel =  modelInfo_->GetBackGroundColorBack();
            }


            param.bgCenterR = SAFE_ANY_VALUE_FROM_DICT(bgLevel, kESBGLevelKeyRed ,ESNumber,0);
            param.bgCenterG = SAFE_ANY_VALUE_FROM_DICT(bgLevel, kESBGLevelKeyGreen ,ESNumber,0);
            param.bgCenterB = SAFE_ANY_VALUE_FROM_DICT(bgLevel, kESBGLevelKeyBlue ,ESNumber,0);

            param.bgRangeR = SAFE_ANY_VALUE_FROM_DICT(bgLevel, kESBGLevelKeyRedWidth ,ESNumber,0);
            param.bgRangeG = SAFE_ANY_VALUE_FROM_DICT(bgLevel, kESBGLevelKeyGreenWidth ,ESNumber,0);
            param.bgRangeB = SAFE_ANY_VALUE_FROM_DICT(bgLevel, kESBGLevelKeyBlueWidth ,ESNumber,0);

            param.grayAdjustR = 1.01;
            param.grayAdjustG = 0.99;
            param.grayAdjustB = 0.99;

            param.isBackSide = (ES_IMAGE_INFO::GetESImageSurfaceType(imageInfo) == ES_IMAGE_INFO::kESImageInfoSurfaceTypeBackSide); 
           
            if (keyMgr_->GetValueInt(kSDIRemovePunchholeKey) != 0 &&
                keyMgr_->GetValueInt(kSDIDeficiencyCorrectionKey) != 0)
            {
                param.mode = kESIMP2Wrappe_DeficiencyDetectRepairModeBoth;
            }else if (keyMgr_->GetValueInt(kSDIRemovePunchholeKey) == 0 &&
                      keyMgr_->GetValueInt(kSDIDeficiencyCorrectionKey) != 0)
            {
                param.mode = kESIMP2Wrappe_DeficiencyDetectRepairModeEdgeCorrect;
            }else if (keyMgr_->GetValueInt(kSDIRemovePunchholeKey) != 0 &&
                      keyMgr_->GetValueInt(kSDIDeficiencyCorrectionKey) == 0)
            {
                param.mode = kESIMP2Wrappe_DeficiencyDetectRepairModePunchHole;
            }
            ESIMP2WrapperImageData inImage = {0};
            inImage.width = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
            inImage.height = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
            inImage.samplesPerPixel= ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
            inImage.bitsPerSample = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
            inImage.dataBuffer = inDataBuf.GetBufferPtr();
            inImage.resolutionX = ES_IMAGE_INFO::GetESImageResolutionX(imageInfo);
            inImage.resolutionY = ES_IMAGE_INFO::GetESImageResolutionY(imageInfo);

            wrapper.GetWrapper()->CorrectDeficienry(&inImage, &param);            
        }
    }


    std::string ESIMP2::GetESIMP2Path()
    {
        std::string pluginRoot = Filter::GetPluginRootDir();
        ESString esimp2Path;
        ES_CMN_FUNCS::PATH::ES_CombinePath(esimp2Path, pluginRoot, ES_STRING("libesimp2.so"));
        return  esimp2Path;
    }

    bool ESIMP2::IsESIMP2Available()
    {
        std::string pluginRoot = Filter::GetPluginRootDir();
        ESString esimp2Path = GetESIMP2Path();

        SDI_TRACE_LOG("libesimp2 = %s", esimp2Path.c_str());

        if (ES_CMN_FUNCS::PATH::ES_IsExistFile(esimp2Path.c_str())) {
                return true;
        }

        return false;
    }


}