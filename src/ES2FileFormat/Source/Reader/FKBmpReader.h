//
//  FKBmpReader.h
//  ESImageFormatKit
//
//  Created by T.Kondo on 2015/3/3.
//  Copyright (c) 2015 EPSON All rights reserved.
//
#pragma once

#include "FKReader.h"
#include "../Base/FKConcreteReaderProtocol.h"

//---------------------------------------------------------------------------------------
//  RAW形式でのBMPファイルの読み込みを提供
//-------------------------------------------------------------------------------------

class CFKBmpReader : public CFKReader,IFKConcreteReaderProtocol
{
protected:

public:
	CFKBmpReader();
	~CFKBmpReader();

	virtual BOOL GetImageInfo(ESImageInfo& outImageInfo);

	virtual BOOL OpenReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError) override;

	virtual BOOL AppendSourceToReader( IFKSource* pcSource, ENUM_FK_ERROR_CODE& eError ) override;

	virtual BOOL CloseReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) override;
private :
	ES_CMN_FUNCS::BUFFER::IESBuffer* GetInputBuffer(ES_CMN_FUNCS::BUFFER::IESBuffer& cBuffer);

	void StorePendingBuffer(ES_CMN_FUNCS::BUFFER::IESBuffer* pInputBuf, uint32_t bufferPosition);
	BOOL ReadHeaderFromData(ES_CMN_FUNCS::BUFFER::IESBuffer& cBuffer, ENUM_FK_ERROR_CODE& eError);
	BOOL Notify(ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ENUM_FK_ERROR_CODE& eError);

	BOOL NotifyToDestination(ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ENUM_FK_ERROR_CODE& eError);

	void AppendSourceFromPath(ESString& strPath, ENUM_FK_ERROR_CODE& eError);
	void AppendSourceFromData(ES_CMN_FUNCS::BUFFER::IESBuffer& pBuffer, ENUM_FK_ERROR_CODE& eError);

	ESString m_strImageInfoJson;
	uint32_t m_currentLine;
	uint32_t m_MaxLoadLine;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer m_pendingBuffer;
	BITMAPINFOHEADER m_stBmInfo;
	BOOL m_bStarted;
	__int64 m_dataPosition;

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer m_LoadedDataToNotify;
};

