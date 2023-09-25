#include "stdafx.h"
#include "ESImageInfo.h"
#include "boost/foreach.hpp"

namespace ES_IMAGE_INFO{

	ESNumber GetImageInfoValueForKey(const ESImageInfo& info,ESString strKey){

		try
		{
			if(info.count(strKey) != 0){
				return info.at(strKey);
			}else{
				return 0;
			}
		}catch(...){
//			assert(false);
			ES_Warning_Log2(__FUNCTION__,ES_STRING("get [%s] of ImageInfo fail"),strKey.c_str());
			return 0;
		}
	}

	void GetImageInfoAsDict(ESDictionary& dict,const ESImageInfo& imageInfo){

		ESImageInfo::const_iterator it = imageInfo.begin();
		for(;it != imageInfo.end();it++)
		{
			dict[it->first] =(ESNumber) it->second;
		}
	}

	void GetImageInfoAsJson(ESString& strImageInfoAsJson,const ESImageInfo& srcImageInfo){

		ESDictionary dict;
		GetImageInfoAsDict(dict,srcImageInfo);
		ES_CMN_FUNCS::JSON::DictionaryToJSON(dict,strImageInfoAsJson);

	}
	void GetImageInfoArrayAsJson(ESString& strImageInfoArrayAsJson,ESImageInfoArray& srcImageInfoArray){

		ESDicArray dicarImageInfo;

		MakeImageInfoArrayAsDicArray(dicarImageInfo,srcImageInfoArray);

		ESDictionary dictResult;

		dictResult[kESImageInfoArray] = dicarImageInfo;

		ES_CMN_FUNCS::JSON::DictionaryToJSON(dictResult,strImageInfoArrayAsJson);

	}

	void MakeImageInfoFromDict(ESImageInfo& imageInfoDest,ESDictionary dictImageInfo){

		imageInfoDest.clear();

		ESDictionary::iterator it = dictImageInfo.begin();
		ESDictionary dict;
		for(;it != dictImageInfo.end();it++)
		{
			if(it->second.type() ==typeid(ESNumber)){

				imageInfoDest[it->first] = boost::any_cast<ESNumber>(it->second);
			}

		}
	}
	void MakeImageInfoFromJson(ESImageInfo& imageInfoDest,ESString strImageInfoAsJson){

		imageInfoDest.clear();
		if(strImageInfoAsJson.empty()){
			return;
		}

		ESDictionary dictImageInfo;

		ES_CMN_FUNCS::JSON::JSONtoDictionary(strImageInfoAsJson,dictImageInfo);

		MakeImageInfoFromDict(imageInfoDest,dictImageInfo);

	}
	void MakeImageInfoArrayAsDicArray(ESDicArray& arDicImageInfos,const ESImageInfoArray& arImageInfos){

		arDicImageInfos.clear();

		ESImageInfoArray::const_iterator it = arImageInfos.begin();

		for(;it != arImageInfos.end();it++){
			ESDictionary dictTmp;

			GetImageInfoAsDict(dictTmp,*it);

			arDicImageInfos.push_back(dictTmp);

		}

	}
	void MakeImageInfoArrayFromDicArray(ESImageInfoArray& arDestImageInfos, const ESDicArray& arDicImageInfos){

		arDestImageInfos.clear();

		ESDicArray::const_iterator it = arDicImageInfos.begin();

		for(;it != arDicImageInfos.end();it++){
			ESImageInfo infoTmp;

			MakeImageInfoFromDict(infoTmp,*it);

			arDestImageInfos.push_back(infoTmp);

		}

	}
	void MakeImageInfoArrayFromJson(ESImageInfoArray& arDestImageInfos,const ESString& strImageInfosAsJson){

		ESDictionary dictdictImageInfos;

		ES_CMN_FUNCS::JSON::JSONtoDictionary(strImageInfosAsJson,dictdictImageInfos);

		const ESDicArray* pdicarImageInfo = SAFE_KEYS_DATA_CPTR(dictdictImageInfos,kESImageInfoArray,ESDicArray);

		if(pdicarImageInfo){
			MakeImageInfoArrayFromDicArray(arDestImageInfos,*pdicarImageInfo);
		}

	}


	ESNumber GetESImageResolutionX(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoResolutionXKey);
	}

	ESNumber GetESImageResolutionY(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoResolutionYKey);
	}

	ESNumber GetESImageOutputResolution(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoOutputResolutionKey);
	}

	ESNumber GetESImageBitsPerSample(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoBitsPerSampleKey);

	}

	ESNumber GetESImageSamplesPerPixel(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoSamplesPerPixelKey);

	}

	ESNumber GetESImageBitsPerPixel(const ESImageInfo& imageInfo)
	{
		return GetESImageBitsPerSample(imageInfo) * GetESImageSamplesPerPixel(imageInfo);
	}

	ESNumber GetESImageHeight(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoHeightKey);
	}

	ESNumber GetESImageWidth(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoWidthKey);
	}

	ESNumber IsESImageEmpty(const ESImageInfo& imageinfo){

		return (GetESImageWidth(imageinfo) == 0)|| (GetESImageHeight(imageinfo) ==0);
	}

	ESNumber GetESImageBytesPerRow(const ESImageInfo& imageInfo)
	{
		 return  (GetESImageWidth(imageInfo) * GetESImageBitsPerPixel(imageInfo) + 7) / 8;
	}

	ESNumber GetESImageBytesPerCol(const ESImageInfo& imageInfo)
	{
		return  (GetESImageHeight(imageInfo) * GetESImageBitsPerPixel(imageInfo) + 7) / 8;
	}

	ESNumber GetESImagePixelsPerRow(const ESImageInfo& imageInfo)
	{
		ESNumber pixelsPerRow = GetESImageSamplesPerPixel(imageInfo) * GetESImageWidth(imageInfo);


		if (pixelsPerRow == 0) {
	 //       ESlog(kESLogLevelInfo, nil, @"pixelsPerRow is 0");
		}

		return pixelsPerRow;
	}

	ESNumber GetESImageSize(const ESImageInfo& imageInfo)
	{
		return GetESImageBytesPerRow(imageInfo) * GetESImageHeight(imageInfo);
	}

	UInt32 GetESImageSizeUN(const ESImageInfo& imageInfo)
	{
		return (UInt32)GetESImageBytesPerRow(imageInfo) * GetESImageHeight(imageInfo);
	}

	ESNumber GetESImagePageCount(const ESImageInfo& imageInfo)
	{
		return GetImageInfoValueForKey(imageInfo,kESImageInfoPageCountKey);

	}

	ESNumber GetESImagePaperCount(const ESImageInfo& imageInfo){
		return GetImageInfoValueForKey(imageInfo,kESImageInfoPaperCountKey);

	}

	ESNumber GetESImageScanCount(const ESImageInfo& imageInfo){
		return GetImageInfoValueForKey(imageInfo,kESImageInfoScanCountKey);

	}

	ESImageDataType GetESImageDataType(const ESImageInfo& imageInfo)
	{
		return (ESImageDataType)GetImageInfoValueForKey(imageInfo,kESImageInfoDataTypeKey);
	}

	ESImageInfoSurfaceType GetESImageSurfaceType(const ESImageInfo& imageInfo)
	{
		return(ESImageInfoSurfaceType) GetImageInfoValueForKey(imageInfo,kESImageInfoSurfaceTypeKey);

	}
	ESImageInfoSurfaceType GetESImageOthersideSurfaceType(const ESImageInfo& imageInfo)
	{

		if(GetESImageSurfaceType(imageInfo) == kESImageInfoSurfaceTypeFrontSide) {
			return kESImageInfoSurfaceTypeBackSide;
		}else{
			return kESImageInfoSurfaceTypeFrontSide;
		}

	}

	ESImageInfoCarrierSheetType GetESImageCarrierSheetType(const ESImageInfo& imageInfo)
	{
		return(ESImageInfoCarrierSheetType) GetImageInfoValueForKey(imageInfo,kESImageInfoCarrierSheetTypeKey);

	}
	BOOL GetESImageAutoCropped(const ESImageInfo& imageInfo){
		return(BOOL) GetImageInfoValueForKey(imageInfo,kESImageInfoAutoCroppedKey);
	}

	BOOL GetESImageUpsideDown(const ESImageInfo& imageInfo){
		return(BOOL) GetImageInfoValueForKey(imageInfo,kESImageInfoUpsideDownKey);
	}

	ESImageColorType GetESImageColorType(const ESImageInfo& imageInfo)
	{

		ESNumber samplesPerPixel	= GetESImageSamplesPerPixel(imageInfo);
		ESNumber bitsPerSample	= GetESImageBitsPerSample(imageInfo);

		if (samplesPerPixel == 3) {
			if (bitsPerSample == 8) {

				if (GetESImageDataType(imageInfo) == kESImageDataTypeJpeg) {
					return kESImageColorTypeRGBJpeg;
				}

				return kESImageColorTypeRGB;
			}

			if (bitsPerSample == 16) {
				return kESImageColorTypeRGB16;
			}
		}


		if (samplesPerPixel == 4) {
			if (bitsPerSample == 8) {
				return kESImageColorTypeRGBA;
			}

			if (bitsPerSample == 16) {
				return kESImageColorTypeRGBA16;
			}
		}

		if (samplesPerPixel == 1) {

			if (bitsPerSample == 1) {
				return kESImageColorTypeMono;
			}

			if (bitsPerSample == 8) {

				if (GetESImageDataType(imageInfo) == kESImageDataTypeJpeg) {
					return kESImageColorTypeGrayJpeg;
				}

				return kESImageColorTypeGray;
			}

			if (bitsPerSample == 16) {
				return kESImageColorTypeGray16;
			}
		}

		return kESImageColorTypeOthers;
	}

	bool GetESImageBlankPageSkip(const ESImageInfo& imageInfo){
		return(bool) GetImageInfoValueForKey(imageInfo, kESImageInfoBlankPageKey);
	}

	bool GetESImageMonoPage(const ESImageInfo& imageInfo){
		return(bool) GetImageInfoValueForKey(imageInfo, kESImageInfoMonoPageKey);
	}

	}
