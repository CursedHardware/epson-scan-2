//
//  FKPngEncodeUtil.cpp
//  ESImageProcessor
//
//  Created by Yumoto Atsushi on 8/28/14.
//  Copyright (c) 2014 EPSON. All rights reserved.
//
#include "stdafx.h"
#include "FKPngEncodeUtil.h"
#include "FileFormatKit.h"
#include "zlib.h"
#include "FileFormatKitErrors.h"
#include "ESImageInfo.h"
#include "ESFile.h"
static const uint8_t k16Bits = 16;

CFKPngEncodeUtil::CFKPngEncodeUtil()
{
	m_pTarget	= NULL;
	m_pPng		= NULL;
	m_pPngInfo	= NULL;
	m_rowBytes	= 0;
}

CFKPngEncodeUtil::~CFKPngEncodeUtil()
{
	DestroyPngResources();
}


static
void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
	UNREFERENCED_PARAMETER(png_ptr);
	throw std::domain_error(error_msg);
}

static
uint8_t MakePNGColorType(const ESImageInfo& imageInfo)
{

	ES_IMAGE_INFO::ESImageColorType colorType = ES_IMAGE_INFO::GetESImageColorType(imageInfo);

	if (colorType == ES_IMAGE_INFO::kESImageColorTypeGray || colorType == ES_IMAGE_INFO::kESImageColorTypeGray16 || colorType == ES_IMAGE_INFO::kESImageColorTypeMono) {
        return PNG_COLOR_TYPE_GRAY;
    }

	if (colorType == ES_IMAGE_INFO::kESImageColorTypeRGB || colorType == ES_IMAGE_INFO::kESImageColorTypeRGB16) {
        return PNG_COLOR_TYPE_RGB;
    }

    assert(FALSE);

    return 0;
}

static BOOL CheckImageInfo(const ESImageInfo& imageInfo)
{
	ES_IMAGE_INFO::ESImageColorType colorType = ES_IMAGE_INFO::GetESImageColorType(imageInfo);

	if (colorType == ES_IMAGE_INFO::kESImageColorTypeRGB ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeRGB16 ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeGray ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeGray16 ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeMono) {
        return TRUE;
    }

    return FALSE;
}


static
uint32_t DotPerMeterFromDotPerInch(uint32_t resolution)
{
	return (uint32_t)((resolution / 0.0254) + 0.5);
}


BOOL CFKPngEncodeUtil::StartEncodingWithPath(const ESString& path, const ESImageInfo& imageInfo, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	try{
		ES_ErrorBailWithAction(CheckImageInfo(imageInfo), BAIL,
			this, eError = kFKParameterError, ES_STRING("parameter error"));

		ES_ErrorBailWithAction(m_pPng == NULL, BAIL,
			this, eError = kFKInconsistentError, ES_STRING("double initialization error"));
#ifdef WIN32
		errno_t err = _wfopen_s(&m_pTarget, path.c_str(), ES_STRING("wb"));
#else
		errno_t err = fopen_s(&m_pTarget, path.c_str(), ES_STRING("wb"));
#endif
		ES_ErrorBailWithAction(err == 0, BAIL, this, eError = kFKFileCreateError, ES_STRING("cannot open file"));

		m_pPng = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, NULL);
		ES_ErrorBailWithAction(m_pPng, BAIL, this, eError = kFKPNGInitializeError, ES_STRING("target is nil"));

		png_set_compression_level(m_pPng, Z_BEST_SPEED);

		m_pPngInfo = png_create_info_struct(m_pPng);
		ES_ErrorBailWithAction(m_pPngInfo, BAIL, this, eError = kFKPNGInitializeError, ES_STRING("pngInfo is nil"));

		png_init_io(m_pPng, m_pTarget);

		png_set_pHYs(
			m_pPng,
			m_pPngInfo,
			DotPerMeterFromDotPerInch(ES_IMAGE_INFO::GetESImageOutputResolution(imageInfo)),
			DotPerMeterFromDotPerInch(ES_IMAGE_INFO::GetESImageOutputResolution(imageInfo)),
			PNG_RESOLUTION_METER);

		png_set_IHDR(m_pPng,
			m_pPngInfo,
			ES_IMAGE_INFO::GetESImageWidth(imageInfo),
			ES_IMAGE_INFO::GetESImageHeight(imageInfo),
			ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo),
			MakePNGColorType(imageInfo),
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);


		png_write_info(m_pPng, m_pPngInfo);

		if (ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo) == k16Bits) {
			png_set_swap(m_pPng);
		}
		m_rowBytes = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);

	}
	catch (std::domain_error e){
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("png internal error"));
	}
	catch(std::out_of_range e){
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("png parameter is invalid"));
	}
	catch (boost::bad_any_cast e){
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("png parameter is invalid"));
	}
BAIL:;

	if (eError != kFKNoError) {
		DestroyPngResources(true);
    }
	return eError == kFKNoError;
}

BOOL CFKPngEncodeUtil::WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	try{
		ES_ErrorBailWithAction(m_pPng, BAIL,
			this, eError = kFKInconsistentError, ES_STRING("libpng not initialized error"));

		uint8_t* inputBuffer = NULL;

		uint32_t rowBytes = m_rowBytes;
		ES_ErrorBail(rowBytes != 0, BAIL, this, ES_STRING("rowBytes is 0"));

		uint32_t dataRows = (uint32_t)cData.GetLength() / rowBytes;

		inputBuffer = (uint8_t*)cData.GetConstBufferPtr();

		for (uint32_t i = 0; i < dataRows; i++) {
			png_write_row(m_pPng, &inputBuffer[i * rowBytes]);
		}

	}
	catch (std::domain_error e){
		ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("png internal error"));
	}

BAIL:


	if (eError != kFKNoError) {
		DestroyPngResources(true);
    }


	return eError == kFKNoError;
}


BOOL CFKPngEncodeUtil::FinalizeEncodingAndReturnError(ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;

	try{
		png_write_end(m_pPng, m_pPngInfo);
	}
	catch (std::domain_error e){
		//Png write end never fail in LibPng.
		//ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("png internal error"));
	}

	DestroyPngResources();

	return eError == kFKNoError;
}

void CFKPngEncodeUtil::DestroyPngResources(bool bDeleteFile)
{
	try{
		if (m_pPng != NULL && m_pPngInfo != NULL){
			png_destroy_write_struct(&m_pPng, &m_pPngInfo);
		}

		if (m_pTarget){
			fclose(m_pTarget);
			m_pTarget = NULL;
		}

		if (bDeleteFile && m_strTargetPath.size() > 0){
			::DeleteFile(m_strTargetPath.c_str());
			m_strTargetPath.clear();
		}

	}
	catch (std::domain_error e){
		//Png Destroy never fail in LibPng.
	//	ES_ErrorBail(FALSE, BAIL, this, ES_STRING("png internal error"));
	}


	m_pPng = NULL;
	m_pPngInfo = NULL;
}
