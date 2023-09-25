//
//  FKJpegReader.cpp
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 8/1/15.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#include "stdafx.h"

#include "FKJpegReader.h"
#include "../JpegUtil/FKJpegDecodeUtil.h"
#include "ESImageInfo.h"

CFKJpegReader::CFKJpegReader()
{
	m_pCFKJpegDecodeUtil = new (std::nothrow) CFKJpegDecodeUtil();
}

CFKJpegReader::~CFKJpegReader()
{
	if(m_pCFKJpegDecodeUtil) {
		delete m_pCFKJpegDecodeUtil;
		m_pCFKJpegDecodeUtil = NULL;
	}
}

BOOL CFKJpegReader::GetImageInfo(ESImageInfo &outImageInfo )
{
	if(!m_pCFKJpegDecodeUtil) {
		return FALSE;
	}

	if(m_pCFKJpegDecodeUtil->IsStart() == FALSE) {
		return FALSE;
	}

	jpeg_decompress_struct* pstjpeg = m_pCFKJpegDecodeUtil->GetJpegDecompressInfo();
	if(!pstjpeg) {
		return FALSE;
	}

	outImageInfo.clear();

	outImageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey]				= pstjpeg->image_width;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey]		  	   = pstjpeg->image_height;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey]  = (ESNumber)pstjpeg->num_components;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey]	  = (ESNumber)8;

    return  TRUE;
}

BOOL CFKJpegReader::OpenReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError )
{
	if(!m_pCFKJpegDecodeUtil) {
		return FALSE;
	}

	m_piFkDestination->NotifyDidBeginReceivingForImageInfo(pszOptionAsJson);
	return m_pCFKJpegDecodeUtil->StartDecodingAndReturnError(eError);
}

BOOL CFKJpegReader::CloseReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError )
{
	if(!m_pCFKJpegDecodeUtil) {
		return FALSE;
	}

	m_piFkDestination->NotifyDidEndReceivingForImageInfo(pszOptionAsJson);
	return m_pCFKJpegDecodeUtil->FinalizeDecodingAndReturnError(eError);
}

BOOL CFKJpegReader::AppendSourceToReader( IFKSource* pcSource, ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;


	{

		ES_ErrorBailWithAction(pcSource != NULL , BAIL, this, errorCode = kFKUnsupportedSource , ES_STRING("pcSource is NULL"));
		ES_ErrorBailWithAction(m_pCFKJpegDecodeUtil != NULL , BAIL, this, errorCode = kFKUnsupportedSource , ES_STRING("m_pCFKJpegDecodeUtil is NULL"));
		ES_ErrorBailWithAction((pcSource->GetFKSourceType() == kFKSourceTypeData ) , BAIL, this, errorCode = kFKUnsupportedSource , ES_STRING("unsupported source"));
	}
	{

		ES_CMN_FUNCS::BUFFER::CESHeapBuffer cSourceBuffer;
		cSourceBuffer.Attach(pcSource->GetSource());
		//Decode用バッファへ画像を追加
		m_pCFKJpegDecodeUtil->AppendData(cSourceBuffer );

	}
	{
		//Decodeされた画像を取り出す

		while(1){
			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cDecodedData;

			BOOL bComplete = m_pCFKJpegDecodeUtil->DecodedDataAndReturnError( cDecodedData,errorCode,m_nValidImageHeight,m_bReadOnlyValidData);
			ES_ErrorBail((cDecodedData.GetLength() != 0 || errorCode == kFKNoError) , BAIL, this , ES_STRING("pData is NULL len[%d] err[%d]"),cDecodedData.GetLength(),errorCode);

			UInt32 un32DecodedDataLen = cDecodedData.GetLength();

			if(un32DecodedDataLen == 0){
				goto BAIL;
			}


			if(m_piFkDestination->GetFKDestinationType() == kFKDestinationTypeData) {
				//Paddingされていないデータが保存されるので要注意
				(m_piFkDestination->GetSource()).AppendBuffer(cDecodedData.GetBufferPtr(),cDecodedData.GetLength());

			}else if(m_piFkDestination->GetFKDestinationType() == kFKDestinationTypeEmpty) {

				ESImageInfo ImageInfo;
				ESString strImageInfoAsJson;

				GetImageInfo(ImageInfo);
				ES_IMAGE_INFO::GetImageInfoAsJson(strImageInfoAsJson,ImageInfo);
				m_esImageInfo = ImageInfo;

				//この中でPaddingをおこなう。
				m_piFkDestination->NotifyReceiveImageData(cDecodedData, (ES_CHAR_CPTR)strImageInfoAsJson.c_str());

			}else{
				ES_ErrorBailWithAction(FALSE, BAIL, this, errorCode = kFKUnsupportedDestination, ES_STRING("unsupported destination"));
			}

			if(bComplete){
				break;
			}

		}

	}

BAIL:;

    eError = errorCode;

    return errorCode == kFKNoError;
}
