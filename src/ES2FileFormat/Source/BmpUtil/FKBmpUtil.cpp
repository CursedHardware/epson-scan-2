//
//  FKBmpUtil.cpp
//  ESImageProcessor
//
//  Created by T.Kondo on 8/28/14.
//  Copyright (c) 2014 EPSON. All rights reserved.
//
#ifndef WIN32
#include "Platform.h"
#endif

#include "stdafx.h"
#include "FKBmpUtil.h"
#include "FileFormatKit.h"
#include "FileFormatKitErrors.h"
#include "ESImageInfo.h"
#include "ESFile.h"
#include <utils/PathUtils.h>

static const uint8_t k16Bits = 16;

CFKBmpEncodeUtil::CFKBmpEncodeUtil()
{
	m_pTarget	= NULL;
	m_rowBytes	= 0;
	m_currentLine = 0;
	m_bTopDown = false;
	m_imageOffset =0;
}

CFKBmpEncodeUtil::~CFKBmpEncodeUtil()
{
	Destroy();
}


BOOL CFKBmpUtil::CheckImageInfo(const ESImageInfo& imageInfo)
{
	ES_IMAGE_INFO::ESImageColorType colorType = ES_IMAGE_INFO::GetESImageColorType(imageInfo);
    
	if (colorType == ES_IMAGE_INFO::kESImageColorTypeRGB ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeGray ||
		colorType == ES_IMAGE_INFO::kESImageColorTypeMono) {
        return TRUE;
    }
    
    return FALSE;
}

uint32_t CFKBmpUtil::GetStride(const BITMAPINFOHEADER& bmInfo)
{
	uint32_t lLineBytes = 1;
	if (bmInfo.biBitCount == 24){
		lLineBytes = (bmInfo.biWidth * 3 + 3) & ~3;
	}
	else if (bmInfo.biBitCount == 1){
		lLineBytes = (((bmInfo.biWidth + 7) / 8) + 3) & ~3;
	}
	else if (bmInfo.biBitCount == 8){
		lLineBytes = (bmInfo.biWidth + 3) & ~3;
	}

	return lLineBytes;
}

int CFKBmpUtil::GetPaletteEntryCount(const BITMAPINFOHEADER& bmInfo)
{
	int nPaletteCount = 0;
	if (bmInfo.biBitCount <= 8){
		nPaletteCount = 1 << bmInfo.biBitCount;
	}
	return nPaletteCount;
}

void CFKBmpUtil::SwapCopyAsBGRBytes(const uint8_t* pSrc, uint8_t* pDest, const uint32_t widthInPixel)
{
	for (uint32_t nLine = 0; nLine < widthInPixel; nLine++){
		//swap Red & Blue
		pDest[0] = pSrc[2];
		pDest[1] = pSrc[1];
		pDest[2] = pSrc[0];

		pDest += 3;
		pSrc += 3;
	}
}


int64_t CFKBmpUtil::GetFileSize(ESString& name)
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

void CFKBmpEncodeUtil::InitBitmapInfo(const ESImageInfo& imageInfo, bool isTopDown,ESNumber nResolutionForFileMetadata)
{
	uint32_t lWidth = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
	int lHeight = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
	uint32_t lBitDepth = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo) * ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
	memset(&m_stBmInfo, 0, sizeof(BITMAPINFOHEADER));
	m_bTopDown = isTopDown;

	m_stBmInfo.biSize = sizeof(BITMAPINFOHEADER);
	m_stBmInfo.biWidth = lWidth;
	if (isTopDown){
		m_stBmInfo.biHeight = -lHeight;
	}
	else{
		m_stBmInfo.biHeight = lHeight;
	}

	m_stBmInfo.biPlanes = 1;
	m_stBmInfo.biBitCount = (unsigned short)lBitDepth;
	m_stBmInfo.biCompression = BI_RGB;
	m_stBmInfo.biSizeImage = 0;
	m_stBmInfo.biXPelsPerMeter = (int)(((double)nResolutionForFileMetadata / (0.0254)) + 0.5);
	m_stBmInfo.biYPelsPerMeter = (int)(((double)nResolutionForFileMetadata / (0.0254)) + 0.5);
	m_stBmInfo.biClrUsed = 0;
	m_stBmInfo.biClrImportant = 0;
}

void CFKBmpEncodeUtil::InitBitmapFileHeader(const ESImageInfo& imageInfo, BITMAPFILEHEADER &BF)
{
	memset(&BF, 0, sizeof(BF));

	uint32_t lWidth = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
	uint32_t lHeight = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
	uint32_t lBitDepth = ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo) * ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);

	#ifdef WIN32
	BF.bfType = 'MB';
	#else
	BF.bfType = 0x4D42;
	#endif
	BF.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * CFKBmpUtil::GetPaletteEntryCount(m_stBmInfo);
	BF.bfSize = BF.bfOffBits + (((lWidth * lBitDepth + 7) / 8 + 3) & ~3) * lHeight;
	BF.bfReserved1 = 0;
	BF.bfReserved2 = 0;
}

BOOL CFKBmpEncodeUtil::StartEncodingWithData(IFKDestination* pDest, const ESImageInfo& imageInfo, bool bIsTopDown,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError)
{
	try{
		eError = kFKNoError;

		ES_IMAGE_INFO::GetImageInfoAsJson(m_strImageJson,imageInfo);

		ES_ErrorBailWithAction(CFKBmpUtil::CheckImageInfo(imageInfo), BAIL, this, eError = kFKParameterError, L"invalid color type error");
		m_bTopDown = bIsTopDown;
		m_currentLine = 0;

		InitBitmapInfo(imageInfo, bIsTopDown,nResolutionForMetadata);

		//Write file header
		BITMAPFILEHEADER FH;
		InitBitmapFileHeader(imageInfo, FH);

		m_imageOffset = FH.bfOffBits;
		long lPaletteCount = CFKBmpUtil::GetPaletteEntryCount(m_stBmInfo);

		uint32_t nHeaderSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * lPaletteCount;
		uint32_t bodySize = CFKBmpUtil::GetStride(m_stBmInfo) * std::abs(m_stBmInfo.biHeight);
		uint32_t totalSize = nHeaderSize + bodySize;
		ES_CMN_FUNCS::BUFFER::CESHeapBuffer cData;
		if (pDest->GetFKDestinationType() == kFKDestinationTypeData){
			ES_ErrorBailWithAction(m_cData.AllocBuffer(totalSize), BAIL,
				this, eError = kFKMemError, L"memory alloc error");
		}
		else if (pDest->GetFKDestinationType() == kFKDestinationTypeEmpty){
			ES_ErrorBailWithAction(m_cData.AllocBuffer(nHeaderSize + 4), BAIL,
				this, eError = kFKMemError, L"memory alloc error");
		}
	
		memcpy(m_cData.GetBufferPtr(), &FH, sizeof(FH));
		memcpy(m_cData.GetBufferPtr() + sizeof(FH), &m_stBmInfo, sizeof(m_stBmInfo));

		//Write Palette
		if (lPaletteCount != 0){
			RGBQUAD* pPalette = (RGBQUAD*)(m_cData.GetBufferPtr() + sizeof(FH) + sizeof(m_stBmInfo));

			for (int i = 0; i < lPaletteCount; i++){


				BYTE byColor = (BYTE)(i * 255 / (lPaletteCount - 1));
				pPalette->rgbBlue = byColor;
				pPalette->rgbGreen = byColor;
				pPalette->rgbRed = byColor;
				pPalette->rgbReserved = 0;

				pPalette ++;

			}
		}

		m_rowBytes = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);

		pDest->NotifyDidBeginReceivingForImageInfo(m_strImageJson.c_str());
		if (pDest->GetFKDestinationType() == kFKDestinationTypeEmpty){
			pDest->NotifyReceiveImageData(m_cData, m_strImageJson.c_str());
		}
	}
	catch (const std::out_of_range& e){
		UNREFERENCED_PARAMETER(e);
		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("bmp parameter is invalid"));
	}
	catch (const boost::bad_any_cast& e){
		UNREFERENCED_PARAMETER(e);

		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("bmp parameter is invalid"));
	}
BAIL:;

if (eError != kFKNoError) {
	Destroy(true);
}
return eError == kFKNoError;
}



BOOL CFKBmpEncodeUtil::StartEncodingWithPath(const ESString& path, const ESImageInfo& imageInfo, bool bIsTopDown,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError)
{
	try{
		eError = kFKNoError;
		ES_IMAGE_INFO::GetImageInfoAsJson(m_strImageJson,imageInfo);

		ES_ErrorBailWithAction(CFKBmpUtil::CheckImageInfo(imageInfo), BAIL, this, eError = kFKPNGWriteError, ES_STRING("invalid color type error"));
		m_bTopDown = bIsTopDown;
		InitBitmapInfo(imageInfo, bIsTopDown,nResolutionForMetadata);
		long lPaletteCount = CFKBmpUtil::GetPaletteEntryCount(m_stBmInfo);

#ifdef WIN32
		errno_t err = _wfopen_s(&m_pTarget, path.c_str(), ES_STRING("wb"));
#else
		errno_t err = fopen_s(&m_pTarget, path.c_str(), ES_STRING("wb"));
#endif	
		ES_ErrorBailWithAction(err == 0, BAIL, this, eError = kFKFileCreateError, ES_STRING("cannot open file"));
		m_currentLine = 0;
		m_strTargetPath = path;

		//Write file header
		BITMAPFILEHEADER FH;
		InitBitmapFileHeader(imageInfo, FH);
		fwrite(&FH, sizeof(FH), 1, m_pTarget);
		m_imageOffset = FH.bfOffBits;

		ES_ErrorBailWithAction(fwrite(&m_stBmInfo, sizeof(m_stBmInfo), 1, m_pTarget), BAIL,
			this, eError = kFKUnknownError, ES_STRING("write bm header fail"));

		//Write Palette
		if (lPaletteCount != 0){
			RGBQUAD palette;
			for (int i = 0; i < lPaletteCount; i++){
				BYTE byColor = (BYTE)(i * 255 / (lPaletteCount - 1));
				palette.rgbBlue = byColor;
				palette.rgbGreen = byColor;
				palette.rgbRed = byColor;
				palette.rgbReserved = 0;

				ES_ErrorBailWithAction(fwrite(&palette, sizeof(RGBQUAD), 1, m_pTarget), BAIL,
					this, eError = kFKUnknownError, ES_STRING("write bm header fail"));
			}
		}

		m_rowBytes = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);
	}
	catch (const std::out_of_range& e){
		UNREFERENCED_PARAMETER(e);

		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("bmp parameter is invalid"));
	}
	catch (const boost::bad_any_cast& e){
		UNREFERENCED_PARAMETER(e);

		eError = kFKParameterError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("bmp parameter is invalid"));
	}
BAIL:;

	if (eError != kFKNoError) {
		Destroy(true);
	}
	return eError == kFKNoError;
}

BOOL CFKBmpEncodeUtil::WriteDataAtPosition(IFKDestination* pDest, const uint8_t* pSrc, uint32_t position, uint32_t length)
{
	if (pDest->GetFKDestinationType() == kFKDestinationTypeEmpty){
		ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;
		if (cBuf.AllocBuffer(length)){
			memcpy(cBuf.GetBufferPtr(), pSrc, length);
			pDest->NotifyReceiveImageData(cBuf, m_strImageJson.c_str());
		}
		else{
			return FALSE;
		}

	}else
	if (pDest->GetFKDestinationType() == kFKDestinationTypeData){
		if (position + length <= m_cData.GetLength()){
			memcpy(m_cData.GetBufferPtr() + position, pSrc, length);
			return TRUE;
		}

		return FALSE;
	}
	else if (pDest->GetFKDestinationType() == kFKDestinationTypePath){
		if(m_pTarget){
			#ifdef WIN32
			int err = _fseeki64(m_pTarget, position, SEEK_SET);
			#else
			int err = fseek(m_pTarget, position, SEEK_SET);
			#endif
			if (err != 0){
				return FALSE;
			}
			return fwrite(pSrc, length, 1, m_pTarget) != 0;
		}
	}

	return FALSE;
}

BOOL CFKBmpEncodeUtil::WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSource, IFKDestination* pDest,  ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	try{
		ES_ErrorBailWithAction((m_rowBytes > 0), BAIL, this, eError = kFKInconsistentError, ES_STRING("incositent method error"));
		ES_ErrorBailWithAction((m_stBmInfo.biBitCount > 0), BAIL, this, eError = kFKInconsistentError, ES_STRING("incositent method error"));

		// �Y���摜�� BITMAPINFOHEADER
		BITMAPINFOHEADER info = m_stBmInfo;

		// �p���b�g�v�f��
		int nPaletteCount = CFKBmpUtil::GetPaletteEntryCount(m_stBmInfo);

		// �w�b�_�����̃T�C�Y
		int nHeaderSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nPaletteCount;
		uint32_t lBytes = cSource.GetLength();

		const uint8_t *pSrcImage = cSource.GetConstBufferPtr();
		//////////////////////////////////////////////////////////////////////////////////////////////////
		uint32_t lLineBytes = m_rowBytes;
		uint32_t stride = CFKBmpUtil::GetStride(m_stBmInfo);//stride of DIB
		if (m_currentLine == (uint32_t)std::abs(m_stBmInfo.biHeight)){
			//all the data is already stored...
			//ignore the data.
			return TRUE;
		}
		uint32_t position = 0;
		int step;
		if (m_bTopDown){
			position = nHeaderSize + stride * m_currentLine;
			step = stride;
		}
		else{
			position = nHeaderSize + stride * (std::abs(info.biHeight) - m_currentLine - 1);
			step = -(int)stride;
		}

		if (info.biBitCount == 24){
			//byte swap is required for BGR format.
			std::vector<uint8_t> vBuf(stride);
			int height = lBytes / lLineBytes;

			if(m_bTopDown ){
				for (int i = 0; i < height; i++){
					CFKBmpUtil::SwapCopyAsBGRBytes(pSrcImage, &vBuf[0], info.biWidth);
				
					ES_ErrorBailWithAction(WriteDataAtPosition(pDest, &vBuf[0], position, stride), 
						BAIL, this, eError = kFKParameterError, L"bmp write error");

					pSrcImage += lLineBytes;
					m_currentLine++;
					position += step;
				}
			}
			else
			{
				ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuf;

				ES_ErrorBailWithAction(cBuf.AllocBuffer(stride * height),BAIL,this,eError = kFKParameterError,ES_STRING("MemError"));

				//TopDown�ł�Band�P�ʂŏ���
				for (int i = 0; i < height; i++){
					CFKBmpUtil::SwapCopyAsBGRBytes(pSrcImage, cBuf.GetBufferPtr() + (height - i-1)*stride, info.biWidth);
				
					pSrcImage += lLineBytes;
					m_currentLine++;

				}				
				position += step * (height -1);

				ES_ErrorBailWithAction(WriteDataAtPosition(pDest, cBuf.GetBufferPtr(),position, cBuf.GetLength()), 
						BAIL, this, eError = kFKParameterError, ES_STRING("bmp write error"));				
			
			}

		}
		else{
			int height = lBytes / lLineBytes;
			int paddingBytes = 0;
			if (lLineBytes % 4 != 0){
				paddingBytes = 4 - (lLineBytes % 4);
			}
			uint8_t padding[4];
			memset(padding, 0, sizeof(padding));

			for (int y = 0; y < height; y++){
				ES_ErrorBailWithAction(WriteDataAtPosition(pDest, pSrcImage, position, lLineBytes), BAIL, this, eError = kFKParameterError, ES_STRING("bmp write error"));

				if (paddingBytes){
					//To adjust file size, padding is required...
					if (m_currentLine == 0 || m_currentLine == (uint32_t)(std::abs(info.biHeight) - 1)){
						//Add padding in tail of the file
						WriteDataAtPosition(pDest, padding, position + lLineBytes, paddingBytes);
					}
				}

				position += step;
				pSrcImage += lLineBytes;
				m_currentLine++;
			}
		}
	}
	catch (const std::bad_alloc& e){
		UNREFERENCED_PARAMETER(e);
		eError = kFKUnknownError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("bmp write error"));
	}

BAIL:
	if (eError != kFKNoError) {
		Destroy(true);
    }
    
	return eError == kFKNoError;
}

BOOL CFKBmpEncodeUtil::FinalizeEncodingAndReturnError(IFKDestination* pDest, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	ES_ErrorBailWithAction(pDest != NULL,
		BAIL, this, eError = kFKParameterError, ES_STRING("bmp write error"));

	if (pDest->GetFKDestinationType() == kFKDestinationTypePath){

		if (m_pTarget){
			fflush(m_pTarget);
			fclose(m_pTarget);
			m_pTarget = NULL;
		}
		int nPaletteCount = CFKBmpUtil::GetPaletteEntryCount(m_stBmInfo);

		// �w�b�_�����̃T�C�Y
		uint32_t nHeaderSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nPaletteCount;
		uint32_t bodySize = CFKBmpUtil::GetStride(m_stBmInfo) * std::abs(m_stBmInfo.biHeight);
		int64_t totalSize = nHeaderSize + bodySize;
		int64_t fsize = CFKBmpUtil::GetFileSize(m_strTargetPath);

		ES_ErrorBailWithAction(fsize == totalSize, BAIL, this, eError = kFKUnknownError, ES_STRING("bmp write error (size invalid)"));

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
	else if (pDest->GetFKDestinationType() == kFKDestinationTypeData && m_currentLine == (uint32_t)std::abs(m_stBmInfo.biHeight) && m_cData.GetLength() > 0){
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

void CFKBmpEncodeUtil::Destroy(bool bDeleteFile)
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
