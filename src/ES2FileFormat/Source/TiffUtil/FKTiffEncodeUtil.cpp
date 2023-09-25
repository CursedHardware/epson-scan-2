//
//  FKTiffEncodeUtil.m
//  ESImageFormatKit
//
//  Created by T.Kondo on 2015/3/03.
//  Copyright2013 EPSON. All rights reserved.
//

#include "stdafx.h"
#include "FKTiffEncodeUtil.h"
#include "FileFormatKit.h"
#include "FileFormatKitErrors.h"
#include "ESImageInfo.h"
#include "ESFile.h"

FKTiffEncodeUtil::FKTiffEncodeUtil()
{
	m_bStarted = FALSE;
	m_pOutFile = NULL;
	m_dataType = TIFF_NOTYPE;
	m_nCurrentRow = 0;
	m_nCompressionType = 0;
	m_nBytesPerLine =0;
	m_nSamplesPerPixel=0;
}

FKTiffEncodeUtil::~FKTiffEncodeUtil()
{
	CloseOutFile();
}

BOOL FKTiffEncodeUtil::OpenOutFileWithPath(ESString* outFilePath)
{
    return OpenOutFileWithPath(outFilePath, FALSE);
}


BOOL FKTiffEncodeUtil::OpenOutFileWithPath(ESString* outFilePath, BOOL appendMode)
{
  if(outFilePath){
#ifdef WIN32
    if (appendMode) {
			m_pOutFile = TIFFOpenW(outFilePath->c_str(), "a");
    }else{
			m_pOutFile = TIFFOpenW(outFilePath->c_str(), "w");
    }
#else
		if (appendMode) {
			m_pOutFile = TIFFOpen(outFilePath->c_str(), "a");
		}else{
			m_pOutFile = TIFFOpen(outFilePath->c_str(), "w");
		}
#endif

		if (m_pOutFile != NULL){
			return TRUE;
		}
  }

	//ESlog(kESLogLevelError, nil, ES_STRING("fails to open file"));
	return FALSE;

}

void FKTiffEncodeUtil::CloseOutFile(bool bDeleteFile)
{
	if (m_pOutFile) {
		TIFFClose(m_pOutFile);
		m_pOutFile = NULL;
    }
	if (bDeleteFile){
		::DeleteFile(m_strTargetPath.c_str());
		m_strTargetPath.clear();
	}

	m_nCurrentRow = 0;
}

uint32_t FKTiffEncodeUtil::GetRowsPerStrip(uint32_t compression)
{
	if (compression == COMPRESSION_JPEG){
		return 8;//rows per strip must be 8 of multiple.
	}else if(compression == COMPRESSION_CCITTFAX4){

		uint32_t height = 0;
		if (TIFFGetField(m_pOutFile, TIFFTAG_IMAGELENGTH, &height) != 1) {
			height = 1;
		}
		return height;

	}

    return 1;
}

uint32_t FKTiffEncodeUtil::GetPlannerConfig()
{
    return PLANARCONFIG_CONTIG;
}

uint32_t FKTiffEncodeUtil::GetOrientation()
{
    return ORIENTATION_TOPLEFT;
}

uint32_t FKTiffEncodeUtil::GetRgbColorSpaceForCompression(uint32_t compression)
{
	UNREFERENCED_PARAMETER(compression);

    return PHOTOMETRIC_RGB;
}

uint32_t FKTiffEncodeUtil::GetMonoColorSpaceForCompression(uint32_t compression)
{
	UNREFERENCED_PARAMETER(compression);

//Windows はMinIsWthie
	if (m_nCompressionType != COMPRESSION_NONE && m_nSamplesPerPixel == 1) {
		return PHOTOMETRIC_MINISWHITE;
	}else{
		return PHOTOMETRIC_MINISBLACK;

	}

}


BOOL FKTiffEncodeUtil::StartWriting()
{
    return TRUE;
}

uint32_t FKTiffEncodeUtil::GetTiffColorSpace(uint32_t samplesPerPixel, uint32_t compression)
{
    if (samplesPerPixel == 3) {
        return GetRgbColorSpaceForCompression(compression);
    }else if (samplesPerPixel == 1){
        return GetMonoColorSpaceForCompression(compression);
    }else{
        assert(FALSE);
    }
    return  GetRgbColorSpaceForCompression(compression);
}

BOOL FKTiffEncodeUtil::GetTiffSetICCProfileWithPath(ESString* iccProfilePath)
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;

	if(iccProfilePath){
		CESFile cIccFile;
		if(cIccFile.Open(*iccProfilePath)){

			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBufIcc;
			cIccFile.ReadDataToEndOfFile(cBufIcc);

			if(cBufIcc.GetLength()){
				if (TIFFSetField(m_pOutFile, TIFFTAG_ICCPROFILE,  cBufIcc.GetLength(),cBufIcc.GetBufferPtr()) != 1) {
					ES_ErrorBailWithAction(FALSE, BAIL, this, errorCode = kFKTiffSetFieldError, ES_STRING("tiffGetICCProfileWithPath fails"));
				}
			}
		}
	}


BAIL:;

    return errorCode == kFKNoError;
}

BOOL FKTiffEncodeUtil::WriteScanlinesWithInversePixelData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;

	uint8_t* writeBitmap = NULL;
	if (!m_bStarted) {
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKInconsistentError;
		, ES_STRING("isStarted should be true"));
	}

	{//copy data to write buffer
		writeBitmap = (unsigned char*)malloc(cData.GetLength());
		if (writeBitmap == NULL) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKMemError;
			, ES_STRING("malloc fails"));

		}
		memcpy(writeBitmap, cData.GetConstBufferPtr(), cData.GetLength());
	}
	{// We need to convert colorSpace 'MINISBLACK' to 'MINISWHITE' to avoid viewers bug
		uint32_t len = cData.GetLength();
		for (uint32_t i = 0; i <len; i++) {
			writeBitmap[i] = ~writeBitmap[i];
		}
	}
	{// write scan line
		uint32_t bytesPerLine = m_nBytesPerLine;
		uint32_t dataHeight = ((uint32_t)cData.GetLength()) / bytesPerLine;

		BOOL isSuccess = WriteScanlinesWithHeight(dataHeight, bytesPerLine, writeBitmap);
		ES_ErrorBailWithAction(isSuccess, BAIL, this, eError = kFKTiffWriteScanLineError, ES_STRING("writeScanlinesWithHeight fails"));
	}

BAIL:;
	if (writeBitmap){
		free(writeBitmap);
	}

	return eError == kFKNoError;
}

BOOL FKTiffEncodeUtil::WriteScanlinesWithHeight(uint32_t height,  uint32_t bytesPerLine,  uint8_t* bytes)
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;

	if (height >= 1) {
		for (uint32_t i = 0; i < height; i++) {
			ES_ErrorBailWithAction( m_pOutFile!=NULL , BAIL, this, errorCode = kFKTiffWriteScanLineError, ES_STRING("m_pOutFile is null"));
			int nRet = TIFFWriteScanline(m_pOutFile, &bytes[i * bytesPerLine], m_nCurrentRow, 0);
			if (nRet != 1) {
				ES_ErrorBailWithAction(FALSE, BAIL, this, errorCode = kFKTiffWriteScanLineError, ES_STRING("TIFFWriteScanline fails ret=%d"),nRet);
			}
			m_nCurrentRow += 1;
		}
	}

BAIL:;

	return errorCode == kFKNoError;
}

BOOL FKTiffEncodeUtil::StartWithPath(ESString& path, ESImageInfo& imageInfo, uint32 compression,
									 BOOL appendMode, ESString& iccProfilePath, ENUM_FK_ERROR_CODE& eError,uint32 un32JpegQuality)
{
	eError = kFKNoError;

	if (m_bStarted || !OpenOutFileWithPath(&path, appendMode)){
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKInconsistentError, ES_STRING("isStarted_ should be false"));
    }
	m_strTargetPath = path;
	try{



		uint32_t imageWidth = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
		uint32_t imageHeight = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
		uint32_t samplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
		uint32_t bitsPerSample = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
		uint32_t xResolution =  ES_IMAGE_INFO::GetESImageOutputResolution(imageInfo);
		uint32_t yResolution =  ES_IMAGE_INFO::GetESImageOutputResolution(imageInfo);
		m_nSamplesPerPixel = samplesPerPixel;
		m_nBytesPerLine = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
		m_nCompressionType = compression;

		uint32   colorSpace = GetTiffColorSpace(samplesPerPixel, compression);



		if (TIFFSetField(m_pOutFile, TIFFTAG_IMAGEWIDTH, imageWidth) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_IMAGEWIDTH fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_IMAGELENGTH, imageHeight) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_IMAGELENGTH fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_SAMPLESPERPIXEL fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_BITSPERSAMPLE, bitsPerSample) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_BITSPERSAMPLE fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_RESOLUTIONUNIT fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_XRESOLUTION, (float)xResolution) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_XRESOLUTION fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_YRESOLUTION, (float)yResolution) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_YRESOLUTION fails"));

		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_ORIENTATION, GetOrientation()) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_ORIENTATION fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_PLANARCONFIG, GetPlannerConfig()) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_PLANARCONFIG fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_PHOTOMETRIC, colorSpace) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_PHOTOMETRIC fails"));
		}
		int rowsPerStrip = GetRowsPerStrip(compression);

		if (TIFFSetField(m_pOutFile, TIFFTAG_ROWSPERSTRIP, rowsPerStrip) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_ROWSPERSTRIP fails"));
		}

		if (TIFFSetField(m_pOutFile, TIFFTAG_COMPRESSION, compression) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_COMPRESSION fails"));
		}

		if(compression == COMPRESSION_JPEG){
		if (TIFFSetField(m_pOutFile, TIFFTAG_JPEGQUALITY, un32JpegQuality) != 1) {
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("TIFFSetField TIFFTAG_COMPRESSION fails"));
		}
		}

		if (iccProfilePath.size() > 0 && colorSpace != GetMonoColorSpaceForCompression(compression)) {
			BOOL isSuccess = GetTiffSetICCProfileWithPath(&iccProfilePath);
			ES_ErrorBailWithAction(isSuccess, BAIL, this, eError = kFKTiffSetFieldError, ES_STRING("tiffSetICCProfileWithPath fails"));
		}
	}
	catch (const std::out_of_range& e){
		UNREFERENCED_PARAMETER(e);
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKParameterError, ES_STRING("tiff parameter is invalid"));
	}
	catch (const boost::bad_any_cast& e){
		UNREFERENCED_PARAMETER(e);
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKParameterError, ES_STRING("tiff parameter is invalid"));
	}

    if (StartWriting() == FALSE) {
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKParameterError, ES_STRING("startWriting fails"));
    }

	m_bStarted = TRUE;

BAIL:;
	if (eError != kFKNoError){
		CloseOutFile(false);
	}
    return eError == kFKNoError;
}


BOOL FKTiffEncodeUtil::WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;

	if (m_nCompressionType != COMPRESSION_NONE && m_nSamplesPerPixel == 1) {
		// Windows Fax Viewerのバグ回避のため、圧縮時はMINISWHITEで書き込む
		BOOL isSuccess = WriteScanlinesWithInversePixelData(cData, eError);
		ES_ErrorBailWithAction(isSuccess, BAIL, this, eError = kFKTiffWriteScanLineError,
			ES_STRING("writeScanlinesWithInversePixelData fails"));
	}
	else{

		uint32_t dataHeight = ((uint32_t)cData.GetLength()) / m_nBytesPerLine;

		BOOL isSuccess = WriteScanlinesWithHeight(dataHeight, m_nBytesPerLine, cData.GetConstBufferPtr());
		ES_ErrorBailWithAction(isSuccess, BAIL, this, eError = kFKTiffWriteScanLineError, ES_STRING("writeScanlinesWithHeight[%d] fails"),dataHeight);
	}
	return TRUE;

BAIL:;
	if (eError != kFKNoError){
		CloseOutFile(false);
	}

	return eError == kFKNoError;
}

BOOL FKTiffEncodeUtil::FinalizeWriting()
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;


	return errorCode == kFKNoError;
}

BOOL FKTiffEncodeUtil::FinalizeAndReturnError(ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;

    if (!m_bStarted) {
		eError = kFKInconsistentError;
		return FALSE;
    }

    if (!FinalizeWriting()) {
		eError = kFKInconsistentError;
		return FALSE;
    }



    CloseOutFile();

	m_bStarted = FALSE;

    return TRUE;
}
