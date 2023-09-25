//
//  FKPngEncodeUtil.h
//  ESImageProcessor
//
//  Created by T.Kondo
//  Copyright (c) 2015 Seiko Epson Corporation. All rights reserved.
//
#pragma once
#include "FKDestination.h"

#ifndef WIN32
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(2)     /* set alignment to 2 byte boundary */

#define BI_RGB 0L

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD *LPRGBQUAD;

typedef struct tagRGBTRIPLE { 
	BYTE rgbtBlue; 
	BYTE rgbtGreen; 
	BYTE rgbtRed; 
} RGBTRIPLE; 

typedef struct tagBITMAPCOREHEADER {
	DWORD   bcSize; 
	WORD    bcWidth; 
	WORD    bcHeight; 
	WORD    bcPlanes; 
	WORD    bcBitCount; 
} BITMAPCOREHEADER, * LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;



typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        int32_t       biWidth;
        int32_t       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        int32_t       biXPelsPerMeter;
        int32_t       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO,  *LPBITMAPINFO, *PBITMAPINFO;


typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

#pragma pack(pop)   /* restore original alignment from stack */

#endif

class CFKBmpEncodeUtil
{
public:
	CFKBmpEncodeUtil();
	virtual ~CFKBmpEncodeUtil();

	BOOL StartEncodingWithPath(const ESString& strPath, const ESImageInfo& pImageInfo, bool bIsTopDown,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError);
	BOOL StartEncodingWithData(IFKDestination* pDest, const ESImageInfo& pImageInfo, bool bIsTopDown,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError);
	BOOL WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cSource, IFKDestination* pDest,  ENUM_FK_ERROR_CODE& eError);
	
	BOOL FinalizeEncodingAndReturnError(IFKDestination* pDest, ENUM_FK_ERROR_CODE& eError);
	
private:
	int WriteDataAtPosition(IFKDestination* pDest, const uint8_t* pSrc, uint32_t position, uint32_t length);
	void InitBitmapInfo(const ESImageInfo& imageInfo, bool isTopDown,ESNumber nResolutionForFileMetadata);
	void InitBitmapFileHeader(const ESImageInfo& imageInfo, BITMAPFILEHEADER &BF);

	bool m_bTopDown;
	BITMAPINFOHEADER		m_stBmInfo;
	void Destroy(bool bDeleteFile = false);
	uint32_t		m_imageOffset;
	uint32_t		m_rowBytes;
	uint32_t		m_currentLine;
	FILE*			m_pTarget;
	//png_structp		m_pPng;
	//png_infop		m_pPngInfo;
	ESString		m_strTargetPath;
	//ESImageInfo*	m_imageInfo;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer m_cData;
	ESString	m_strImageJson;
};

class CFKBmpUtil
{
public:
	static int GetPaletteEntryCount(const BITMAPINFOHEADER& bmInfo);
	static uint32_t GetStride(const BITMAPINFOHEADER& bmInfo);
	static void SwapCopyAsBGRBytes(const uint8_t* pSrc, uint8_t* pDest, const uint32_t widthInPixel);
	static BOOL CheckImageInfo(const ESImageInfo& imageInfo);
	static int64_t GetFileSize(ESString& name);
};
