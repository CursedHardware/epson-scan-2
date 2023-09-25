#pragma once
#include <stdint.h>
#include <functional>

struct ESIMP2WrapperImageData
{
    uint32_t width;
	uint32_t height;
	uint32_t samplesPerPixel;
	uint32_t bitsPerSample;
	uint32_t resolutionX;
	uint32_t resolutionY;
	uint8_t* dataBuffer;
	uint32_t dataLength;
};

typedef enum : int32_t
{
    kESIMP2WrapperErrorNone = 0,
    kESIMP2WrapperErrorGeneric = 1
} ESIMP2WrapperError;

#define ESIMP_MAX_PATH 512
typedef struct 
{
	uint32_t modelID;
	char dtiParam[ESIMP_MAX_PATH];
	char esimp2Param[ESIMP_MAX_PATH];
	char dtrPath[ESIMP_MAX_PATH];
	char esimp2Path[ESIMP_MAX_PATH];
	char isCarrierSheet;
}ESIMP2WrapperCropParam;

typedef struct 
{
	double theta;				     // [deg] anlgle of the slanting image
	int32_t topPoint;				// [pixel] top point of the image
	int32_t leftPoint;				// [pixel] left point of the image
	int32_t locateW;				// [pixel] width of the image
	int32_t locateH;				// [pixel] height of the image
	int32_t actImgW;				// [pixel] width of the actual image
	int32_t actImgH;				// [pixel] height of the actual image
	int32_t xTangent;				// [pixel] x point of tangency
	int32_t yTangent;				// [pixel] y point of tangency
	int32_t analysisResolutionX;
	int32_t analysisResolutionY;
}ESIMP2WrapperLocationData;

typedef enum : uint32_t {
    kESIMP2Wrappe_DeficiencyDetectRepairModePunchHole,
    kESIMP2Wrappe_DeficiencyDetectRepairModeEdgeCorrect,
    kESIMP2Wrappe_DeficiencyDetectRepairModeBoth
}IESIMP2Wrapper_DeficiencyDetectRepairMode;


typedef struct {
    const char                *pszParamDataPath;
    IESIMP2Wrapper_DeficiencyDetectRepairMode mode;
    
    double grayAdjustR;
    double grayAdjustG;
    double grayAdjustB;
    
    int bgCenterR;
    int bgCenterG;
    int bgCenterB;
    
    int bgRangeR;
    int bgRangeG;
    int bgRangeB;

	char isBackSide;
        
}ESIMP2WrapperDeficiencyDetectRepairParam;

class IESIMP2Wrapper
{
public:
	virtual ESIMP2WrapperError Detect(const ESIMP2WrapperCropParam *cropParam, ESIMP2WrapperImageData* inImage, ESIMP2WrapperLocationData* outLocationData) = 0;
    virtual ESIMP2WrapperError Crop(const ESIMP2WrapperCropParam *cropParam, const ESIMP2WrapperLocationData *locationData,  ESIMP2WrapperImageData* inImage, ESIMP2WrapperImageData* outImage) = 0;
 
 	virtual ESIMP2WrapperError CorrectDeficienry(ESIMP2WrapperImageData* image, ESIMP2WrapperDeficiencyDetectRepairParam* deficiencyDetectRepairParam) = 0;
 };


#if __cplusplus
extern "C" {
#endif

ESIMP2WrapperError IESIMP2WrapperCreate(IESIMP2Wrapper** plugin);
ESIMP2WrapperError IESIMP2WrapperFree(IESIMP2Wrapper* plugin);


typedef ESIMP2WrapperError (*IESIMP2WrapperCreatePtr)(IESIMP2Wrapper** plugin);
typedef ESIMP2WrapperError (*IESIMP2WrapperFreePtr)(IESIMP2Wrapper* plugin);


#ifdef __cplusplus
}
#endif