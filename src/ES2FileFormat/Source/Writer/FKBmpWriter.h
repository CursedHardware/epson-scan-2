//
//  FKJpegWriter.h
//  ESImageFormatKit
//
//  Created by Yumoto Atsushi on 8/12/13.
//  Copyright (c) 2013 EPSON All rights reserved.
//
#pragma once
#include "../Base/FKConcreteWriterProtocol.h"


class CFKBmpEncodeUtil;


class CFKBmpWriter : public CFKWriter
{
public:
	CFKBmpWriter();
	~CFKBmpWriter();

protected:
	virtual BOOL Initialize();

	virtual BOOL InitializeWriterWithDestination(CFKDestination* pCFKDestination,
		FKWriterInitializeDestDict* pOption,
		ENUM_FK_ERROR_CODE& eError);

	virtual BOOL OpenWriterPageWithImageInfo(ESImageInfo* pImageInfo,
		FKWriterPageOpenDict* pOption,
		ENUM_FK_ERROR_CODE& eError);

	virtual BOOL CloseWriterPageWithOption(FKWriterPageCloseDict* pOption,
		ENUM_FK_ERROR_CODE& eError);


	virtual BOOL FinalizeWriterWithOption(FKWriterFinalizeDestDict* pOption,
		ENUM_FK_ERROR_CODE& eError);

	virtual BOOL IsMultiPageSupport();

	virtual BOOL AppendSourceToDestination(CFKSource* pSource, ENUM_FK_ERROR_CODE& eError);
	virtual BOOL AppendConstSourceToDestination(const CFKSource* pSource, ENUM_FK_ERROR_CODE& eError);

private:

	CFKBmpEncodeUtil* m_pCFKBmpEncodeUtil;
};
