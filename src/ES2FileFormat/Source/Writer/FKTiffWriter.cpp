//
//  FKTiffWriter.cpp
//  ESImageFormatKit
//
//  Created by T.Kondo on 2013/12/05.
//  Copyright (c) 2013 - 2015 EPSON. All rights reserved.
//
#include "stdafx.h"
#include "ESFile.h"
#include "ESImageInfo.h"
#include "FileFormatKitErrors.h"
#include "FKTiffWriter.h"
//#include "../TiffUtil/FKTiffEncodeUtil.h"
#include "../TiffUtil/FKTiffJpegEncodeUtil.h"
#include "ESAnyCastUtils.h"
#include "DbgLog.h"
#include "PathUtils.h"
static const ESNumber kDefaultJpegQuality = 84;

CFKTiffWriter::CFKTiffWriter()
{
	m_pCFKTiffEncodeUtil = NULL;
	m_unBytesPerLine = 0;
}

CFKTiffWriter::~CFKTiffWriter()
{
	if (m_pCFKTiffEncodeUtil){
		delete m_pCFKTiffEncodeUtil;
		m_pCFKTiffEncodeUtil = NULL;
	}
}
BOOL CFKTiffWriter::Initialize()
{
	return TRUE;
}

BOOL CFKTiffWriter::InitializeWriterWithDestination(
	CFKDestination* pCFKDestination,
	FKWriterInitializeDestDict* pOption,
	ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;

	ES_ErrorBailWithAction((pCFKDestination->GetFKDestinationType() == kFKDestinationTypePath), BAIL, this, eError = kFKUnsupportedDestination, ES_STRING("unsupported dest"));
	ES_ErrorBailWithAction(pOption,BAIL,this,eError = kFKParameterError,ES_STRING("initialize option is null"));
	{
		ESNumber dataType = kFKWriterInitializeDestOptionTiffDataTypeRaw;

		if(pOption->count(kFKWriterInitializeDestOptionTiffDataTypeKey) != 0){
			SAFE_ANY_VALUE_FROM_DICT(*pOption,kFKWriterInitializeDestOptionTiffDataTypeKey,ESNumber, kFKWriterInitializeDestOptionTiffDataTypeRaw);
		}
		if (dataType == kFKWriterInitializeDestOptionTiffDataTypeRaw)   {
			m_pCFKTiffEncodeUtil = safeNew FKTiffEncodeUtil();
		}
		else if (dataType == kFKWriterInitializeDestOptionTiffDataTypeEncoded) {
			m_pCFKTiffEncodeUtil = safeNew FKTiffJpegEncodeUtil();
	  }
		ES_ErrorBailWithAction((m_pCFKTiffEncodeUtil != NULL), BAIL, this, eError = kFKParameterError, ES_STRING("fails to insatnciate tiff"));
	}

BAIL:;

	return eError == kFKNoError;
}


BOOL CFKTiffWriter::OpenWriterPageWithImageInfo(
	ESImageInfo* pImageInfo,
	FKWriterPageOpenDict* pOption,
	ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	ES_ErrorBailWithAction(pOption, BAIL, this, eError = kFKParameterError, ES_STRING("pOption is null"));
	ES_ErrorBailWithAction(pImageInfo, BAIL, this, eError = kFKParameterError, ES_STRING("pImageInfo is null"));

	{
		ESNumber compressionLibTiff = COMPRESSION_NONE;
		bool     appendMode				= SAFE_ANY_VALUE_FROM_DICT(*pOption,kFKWriterPageOpenOptionTiffAppendModeKey,bool, false);
		ESString iccProfilePath			= SAFE_ANY_VALUE_FROM_DICT(*pOption,kFKWriterPageOpenOptionTiffICCProfilePathKey,ESString, ES_STRING(""));

		m_eState = kFKWriterStateProcessing;

		ES_ErrorBailWithAction(GetCFKDestination()->GetFKDestinationType() == kFKDestinationTypePath, BAIL, this, eError = kFKParameterError, ES_STRING("unsupported dest"));
		try
		{// compression types settings
			ESNumber compression = SAFE_ANY_VALUE_FROM_DICT(*pOption,kFKWriterPageOpenOptionTiffCompressionKey,ESNumber, 0);
			ESNumber nJpegQuality= 0;
			ESNumber bitsPerSample = ES_IMAGE_INFO::GetESImageBitsPerSample(*pImageInfo);

			if (compression) {
				switch (bitsPerSample) {
					case 1:

						if (compression == kFKWriterPageOpenOptionTiffCompressionCCITG4) {
							compressionLibTiff = COMPRESSION_CCITTFAX4;
						}

						if (compression == kFKWriterPageOpenOptionTiffCompressionLzw) {
							compressionLibTiff = COMPRESSION_LZW;
						}

						break;

					case 8:
					case 16:

						if (compression == kFKWriterPageOpenOptionTiffCompressionLzw) {
							compressionLibTiff = COMPRESSION_LZW;
						}

						if (compression == kFKWriterPageOpenOptionTiffCompressionJpeg) {
							compressionLibTiff = COMPRESSION_JPEG;
							nJpegQuality = SAFE_ANY_VALUE_FROM_DICT(*pOption,kFKWriterPageOpenOptionJpegQualityKey,ESNumber, 0);
						}

						break;

					default:
						eError = kFKParameterError;
						ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("invalid bits per sample"));
						break;
				}
			}
			m_unBytesPerLine = ES_IMAGE_INFO::GetESImageBytesPerRow(*pImageInfo);

			ESString strPath = (LPTSTR)(((GetCFKDestination()->GetSource())).GetBufferPtr());

			if (!this->m_pCFKTiffEncodeUtil->StartWithPath(strPath, *pImageInfo, compressionLibTiff,  appendMode, iccProfilePath, eError,nJpegQuality)){
				//		eError = kFKUnknownError;
				ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("startWithPath fails"));
			}
		}
		catch (const std::out_of_range& e){
			UNREFERENCED_PARAMETER(e);
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("tiff parameter is invalid"));
		}
		catch (const boost::bad_any_cast& e){
			UNREFERENCED_PARAMETER(e);
			ES_ErrorBailWithAction(FALSE, BAIL, this, eError = kFKPNGWriteError, ES_STRING("tiff parameter is invalid"));
		}
	}

BAIL:;
    return eError == kFKNoError;
}


BOOL CFKTiffWriter::CloseWriterPageWithOption(FKWriterPageCloseDict* pOption, ENUM_FK_ERROR_CODE& eError)
{
	UNREFERENCED_PARAMETER(pOption);

	eError = kFKNoError;

	if (!m_pCFKTiffEncodeUtil->FinalizeAndReturnError(eError)) {
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("finalize fails"));
    }

BAIL:;

	return eError == kFKNoError;
}

BOOL CFKTiffWriter::AppendSourceToDestination(CFKSource* pSource, ENUM_FK_ERROR_CODE& eError)
{
	return AppendConstSourceToDestination(pSource,eError);

}
BOOL CFKTiffWriter::AppendConstSourceToDestination(const CFKSource* pSource, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;

	if (pSource->GetFKSourceType() == kFKSourceTypeData) {


		BOOL isSuccess = m_pCFKTiffEncodeUtil->WriteScanlinesWithData(pSource->GetConstSource(), eError);
		ES_ErrorBailWithAction(isSuccess, BAIL, this, eError = kFKTiffWriteScanLineError,
			ES_STRING("writeScanlinesWithData fails"));
	}
	else if (pSource->GetFKSourceType() == kFKSourceTypeData) {
		ES_ErrorBailWithAction(m_unBytesPerLine, BAIL, this, eError = kFKTiffWriteScanLineError,
			ES_STRING("writeScanlinesWithData fails"));

		ES_CMN_FUNCS::BUFFER::CESHeapBuffer buf;
		ESString strPath = (LPTSTR)(pSource->GetConstSource().GetConstBufferPtr());
		std::unique_ptr<CESFile> pFile(CESFile::CreateFileInstanceWithPath(strPath.c_str()));
		int height = pFile->GetLength() / m_unBytesPerLine;
		for(int i = 0; i < height; i++){
			ES_ErrorBailWithAction(m_unBytesPerLine == pFile->ReadDataOfLength(m_unBytesPerLine, buf), BAIL, this,
				eError = kFKTiffWriteScanLineError, ES_STRING("writeScanlinesWithData ReadDataOfLength fails"));
			ES_ErrorBailWithAction(m_pCFKTiffEncodeUtil->WriteScanlinesWithData(buf, eError), BAIL, this,
				eError = kFKTiffWriteScanLineError, ES_STRING("writeScanlinesWithData fails"));

		}
	}

BAIL:;


    return eError == kFKNoError;
}


BOOL CFKTiffWriter::IsMultiPageSupport()
{
    return TRUE;
}


BOOL CFKTiffWriter::FinalizeWriterWithOption(FKWriterFinalizeDestDict* pOption,
	ENUM_FK_ERROR_CODE& eError)
{
	UNREFERENCED_PARAMETER(pOption);
	eError = kFKNoError;
	m_eState = kFKWriterStateNotOpened;
    return TRUE;
}


static const uint64_t kTiffMaxBytes = 2e+9;//TIFF max file size 2G

BOOL CFKTiffWriter::CheckFileFormatMaxbytes(ESNumber nNextImageSize) {

	if(GetCFKDestination()){
		if(GetCFKDestination()->GetFKDestinationType() == kFKDestinationTypePath){

			ESString strPath =(LPCTSTR) GetCFKDestination()->GetSource().GetBufferPtr();

			if(ES_CMN_FUNCS::PATH::ES_IsExistFile(strPath)){

				CESFile file;

				if(file.Open(strPath)){

					UInt32 un32Len = file.GetLength();

					file.CloseFile();

					ES_Info_Log(this,ES_STRING("CheckFileFormatMaxbytes [%u]"),un32Len+nNextImageSize);

					if((uint64_t)un32Len + (uint64_t)nNextImageSize > kTiffMaxBytes)
					{
						ES_Info_Log(this,ES_STRING("CheckFileFormatMaxbytes fail "));

						return FALSE;
					}

				}
			}
		}
	}
    return TRUE;

}
