//
//  FKPnmUtil.cpp
//  ESImageProcessor
//
//  Created by S.Kenichi on 6/20/17.
//  Copyright (c) 2017 EPSON. All rights reserved.
//
#include "stdafx.h"
#include "FKPnmUtil.h"
#include "FileFormatKit.h"
#include "FileFormatKitErrors.h"
#include "ESImageInfo.h"
#include "ESFile.h"
static const uint8_t k16Bits = 16;
#include <utils/PathUtils.h>

CFKPnmEncodeUtil::CFKPnmEncodeUtil()
{
	m_pTarget	= NULL;
	m_rowBytes	= 0;
	m_currentLine = 0;
}

CFKPnmEncodeUtil::~CFKPnmEncodeUtil()
{
	Destroy();
}


BOOL CFKPnmUtil::CheckImageInfo(const ESImageInfo& imageInfo)
{
	ES_IMAGE_INFO::ESImageColorType colorType = ES_IMAGE_INFO::GetESImageColorType(imageInfo);

	if (colorType == ES_IMAGE_INFO::kESImageColorTypeRGB ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeGray ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeMono) {
        return TRUE;
    }

    return FALSE;
}

uint32_t CFKPnmUtil::GetStride(const PNMINFO& pnmInfo)
{

	uint32_t lWidth = pnmInfo.biWidth;
	uint32_t lBitsPerPixel = pnmInfo.biBitsPerPixel;

	if(pnmInfo.biBitCount == 1){
		return ( (lWidth * lBitsPerPixel) + 7) / 8  ;
	}else {
		return pnmInfo.biBytesPerRow;
	}
}

long long CFKPnmUtil::GetFileSize(ESString name)
{
#ifdef WIN32
	HANDLE hFile = CreateFile(name.c_str(), GENERIC_READ,
		0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1; // error condition, could call GetLastError to find out more

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size))
	{
		CloseHandle(hFile);
		return -1; // error condition, could call GetLastError to find out more
	}

	CloseHandle(hFile);

	return size.QuadPart;
#else
	long long size = ES_CMN_FUNCS::PATH::ES_GetFileSize(name);
	return size;
#endif
}

void CFKPnmUtil::FlipBits(ES_CMN_FUNCS::BUFFER::IESBuffer&  cSource)
{
	auto bits  = cSource.GetBufferPtr();
	auto length = cSource.GetLength();

	for (uint32_t i = 0; i < length; i++)
	{
		bits[i] = 255 - bits[i];
	}
}

void CFKPnmEncodeUtil::InitPnmInfo(const ESImageInfo& imageInfo)
{
	memset(&m_stPnmInfo, 0, sizeof(PNMINFO));

	m_stPnmInfo.biWidth = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
	m_stPnmInfo.biHeight = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
	m_stPnmInfo.biChannel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
	m_stPnmInfo.biDepth = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);
	m_stPnmInfo.biBitsPerPixel = ES_IMAGE_INFO::GetESImageBitsPerPixel(imageInfo);
	m_stPnmInfo.biBytesPerRow = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);

	m_stPnmInfo.biBitCount = (unsigned short)(m_stPnmInfo.biChannel * m_stPnmInfo.biDepth);
}

void CFKPnmEncodeUtil::InitPnmFileHeader(const ESImageInfo& imageInfo)
{
	uint32_t lWidth = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
	int lHeight = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
	uint32_t lChannel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
	uint32_t lDepth = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo);

  if (8 == lDepth) {
    if (3 == lChannel) {
      m_pnmHeader = (boost::format(ES_STRING("P6 %d %d 255\n")) % lWidth % lHeight).str(); //Color
    } else if (1 == lChannel) {
      m_pnmHeader = (boost::format(ES_STRING("P5 %d %d 255\n")) % lWidth % lHeight).str(); //Gray
    }
  } else if (1 == lDepth && 1 == lChannel) {
    m_pnmHeader = (boost::format(ES_STRING("P4 %d %d\n")) % lWidth % lHeight).str(); //Monochrome
  }

  if (0 == m_pnmHeader.length()) {
    //error
  }
}

BOOL CFKPnmEncodeUtil::StartEncodingWithData(IFKDestination* pDest, const ESImageInfo& imageInfo,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError)
{
	try{
		eError = kFKNoError;

		ES_IMAGE_INFO::GetImageInfoAsJson(m_strImageJson,imageInfo);

		ES_ErrorBailWithAction(CFKPnmUtil::CheckImageInfo(imageInfo), BAIL, this, eError = kFKParameterError, ES_STRING("invalid color type error"));

		m_currentLine = 0;

		InitPnmInfo(imageInfo);

		//Write file header
		InitPnmFileHeader(imageInfo);

		uint32_t nHeaderSize = m_pnmHeader.length();
		uint32_t bodySize = CFKPnmUtil::GetStride(m_stPnmInfo) * std::abs(m_stPnmInfo.biHeight);
		uint32_t totalSize = nHeaderSize + bodySize;
		ES_CMN_FUNCS::BUFFER::CESHeapBuffer cData;
		if (pDest->GetFKDestinationType() == kFKDestinationTypeData){
			ES_ErrorBailWithAction(m_cData.AllocBuffer(totalSize), BAIL,
				this, eError = kFKMemError, ES_STRING("memory alloc error"));
		}
		else if (pDest->GetFKDestinationType() == kFKDestinationTypeEmpty){
			ES_ErrorBailWithAction(m_cData.AllocBuffer(nHeaderSize + 4), BAIL,
				this, eError = kFKMemError, ES_STRING("memory alloc error"));
		}

		memcpy(m_cData.GetBufferPtr(), m_pnmHeader.c_str(), sizeof(m_pnmHeader.c_str()));

		m_rowBytes = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);

		pDest->NotifyDidBeginReceivingForImageInfo(m_strImageJson.c_str());
		if (pDest->GetFKDestinationType() == kFKDestinationTypeEmpty){
			pDest->NotifyReceiveImageData(m_cData, m_strImageJson.c_str());
		}
	}
	catch (const std::out_of_range& e){
		UNREFERENCED_PARAMETER(e);
		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("pnm parameter is invalid"));
	}
	catch (const boost::bad_any_cast& e){
		UNREFERENCED_PARAMETER(e);

		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("pnm parameter is invalid"));
	}
BAIL:;

if (eError != kFKNoError) {
	Destroy(true);
}
return eError == kFKNoError;
}


BOOL CFKPnmEncodeUtil::StartEncodingWithPath(const ESString& path, const ESImageInfo& imageInfo,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError)
{
	try{
		eError = kFKNoError;
		ES_IMAGE_INFO::GetImageInfoAsJson(m_strImageJson,imageInfo);

		ES_ErrorBailWithAction(CFKPnmUtil::CheckImageInfo(imageInfo), BAIL, this, eError = kFKPNGWriteError, ES_STRING("invalid color type error"));
		InitPnmInfo(imageInfo);

		errno_t err = fopen_s(&m_pTarget, path.c_str(), ES_STRING("wb"));
		ES_ErrorBailWithAction(err == 0, BAIL, this, eError = kFKFileCreateError, ES_STRING("cannot open file"));
		m_currentLine = 0;
		m_strTargetPath = path;

		//Write file header
		InitPnmFileHeader(imageInfo);

		ES_ErrorBailWithAction(fwrite(m_pnmHeader.c_str(), strlen(m_pnmHeader.c_str()), 1, m_pTarget), BAIL,
			this, eError = kFKUnknownError, ES_STRING("write pnm header fail"));

			fflush(m_pTarget);

		m_rowBytes = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
	}
	catch (const std::out_of_range& e){
		UNREFERENCED_PARAMETER(e);

		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("pnm parameter is invalid"));
	}
	catch (const boost::bad_any_cast& e){
		UNREFERENCED_PARAMETER(e);

		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("pnm parameter is invalid"));
	}
BAIL:;

	if (eError != kFKNoError) {
		Destroy(true);
	}
	return eError == kFKNoError;
}

BOOL CFKPnmEncodeUtil::WriteDataAtPosition(IFKDestination* pDest, const uint8_t* pSrc, uint32_t position, uint32_t length)
{
	 if (pDest->GetFKDestinationType() == kFKDestinationTypePath){
		if(m_pTarget)
		{
			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;
			if (cBuf.AllocBuffer(length))
			{
				memcpy(cBuf.GetBufferPtr(), pSrc, length);

				if(m_stPnmInfo.biBitCount == 1)
				{
					CFKPnmUtil::FlipBits(cBuf);
				}
				return fwrite(cBuf.GetBufferPtr(), length, 1, m_pTarget) != 0;
			}else{
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CFKPnmEncodeUtil::WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSource, IFKDestination* pDest,  ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	try{
		ES_ErrorBailWithAction((m_rowBytes > 0), BAIL, this, eError = kFKInconsistentError, ES_STRING("incositent method error"));

		PNMINFO info = m_stPnmInfo;

		// ヘッダ部分のサイズ
		int nHeaderSize = m_pnmHeader.length();
		uint32_t lBytes = cSource.GetLength();

		const uint8_t *pSrcImage = cSource.GetConstBufferPtr();
		//////////////////////////////////////////////////////////////////////////////////////////////////
		uint32_t lLineBytes = m_rowBytes;
		uint32_t stride = CFKPnmUtil::GetStride(info);//stride of DIB
		if (m_currentLine == (uint32_t)std::abs(m_stPnmInfo.biHeight)){
			//all the data is already stored...
			//ignore the data.
			return TRUE;
		}
		uint32_t position = 0;
		int step;
		position = nHeaderSize + stride * m_currentLine;
		step = stride;

		int height = lBytes / lLineBytes;

		for (int y = 0; y < height; y++){
			ES_ErrorBailWithAction(WriteDataAtPosition(pDest, pSrcImage, position, lLineBytes), BAIL, this, eError = kFKParameterError, ES_STRING("pnm write error"));

			position += step;
			pSrcImage += lLineBytes;
			m_currentLine++;
		}
	}
	catch (const std::bad_alloc& e){
		UNREFERENCED_PARAMETER(e);
		eError = kFKUnknownError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("pnm write error"));
	}

BAIL:
	if (eError != kFKNoError) {
		Destroy(true);
    }

	return eError == kFKNoError;
}

BOOL CFKPnmEncodeUtil::FinalizeEncodingAndReturnError(IFKDestination* pDest, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	ES_ErrorBailWithAction(pDest != NULL,
		BAIL, this, eError = kFKParameterError, ES_STRING("pnm write error"));

	if (pDest->GetFKDestinationType() == kFKDestinationTypePath){

		if (m_pTarget){
			fflush(m_pTarget);
			fclose(m_pTarget);
			m_pTarget = NULL;
		}

		// ヘッダ部分のサイズ
		uint32_t nHeaderSize = m_pnmHeader.length();
		uint32_t bodySize = CFKPnmUtil::GetStride(m_stPnmInfo) * std::abs(m_stPnmInfo.biHeight);
		long long totalSize = nHeaderSize + bodySize;
		long long fsize = CFKPnmUtil::GetFileSize(m_strTargetPath);

		ES_ErrorBailWithAction(fsize == totalSize, BAIL, this, eError = kFKUnknownError, ES_STRING("pnm write error (size invalid)"));

		if (m_strTargetPath.length() > 0){

			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;
			if (cBuf.AllocBuffer((m_strTargetPath.length() + 1) * sizeof(ES_CHAR))){
				memcpy(cBuf.GetBufferPtr(), m_strTargetPath.c_str(), m_strTargetPath.length() * sizeof(ES_CHAR));

				pDest->GetSource().Attach(cBuf);
				pDest->NotifyDidEndReceivingForImageInfo(m_strImageJson.c_str());
			}
			m_strTargetPath.clear();
		}
	}
	else if (pDest->GetFKDestinationType() == kFKDestinationTypeData && m_currentLine == (uint32_t)std::abs(m_stPnmInfo.biHeight) && m_cData.GetLength() > 0){
		pDest->GetSource().Attach(m_cData);
		pDest->NotifyDidEndReceivingForImageInfo(m_strImageJson.c_str());
	}
	else if (pDest->GetFKDestinationType() == kFKDestinationTypeEmpty){
		pDest->NotifyDidEndReceivingForImageInfo(m_strImageJson.c_str());
	}
 BAIL:
	Destroy(eError != kFKNoError);

	return eError == kFKNoError;
}

void CFKPnmEncodeUtil::Destroy(bool bDeleteFile)
{
	if (m_pTarget){
		fclose(m_pTarget);
		m_pTarget = NULL;
	}

	if (bDeleteFile && m_strTargetPath.size() > 0){
		::DeleteFile(m_strTargetPath.c_str());
		m_strTargetPath.clear();
	}
}
