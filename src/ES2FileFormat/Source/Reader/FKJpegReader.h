//
//  FKJpegReader.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 8/1/15.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once

#include "FKReader.h"
#include "../Base/FKConcreteReaderProtocol.h"

class CFKJpegDecodeUtil;


//---------------------------------------------------------------------------------------
//  RAW形式でのJPEGファイルの読み込みを提供
//-------------------------------------------------------------------------------------

class CFKJpegReader : public CFKReader,IFKConcreteReaderProtocol
{
protected:
	CFKJpegDecodeUtil* m_pCFKJpegDecodeUtil;
public:
	CFKJpegReader();
	~CFKJpegReader();

	virtual BOOL GetImageInfo(ESImageInfo& outImageInfo);

	virtual BOOL OpenReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError) override;

	virtual BOOL AppendSourceToReader( IFKSource* pcSource, ENUM_FK_ERROR_CODE& eError ) override;

	virtual BOOL CloseReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) override;

};
