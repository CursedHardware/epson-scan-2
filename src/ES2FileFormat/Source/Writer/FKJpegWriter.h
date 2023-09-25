//
//  FKJpegWriter.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 5/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once

#include "FKWriter.h"
#include "../Base/FKConcreteWriterProtocol.h"


class CFKJpegEncodeUtil;

class CFKJpegWriter : public CFKWriter
{
protected:
	CFKJpegEncodeUtil* m_pCFKJpegEncodeUtil;

public:
	CFKJpegWriter();
	~CFKJpegWriter();

protected:

	virtual BOOL Initialize();

	virtual BOOL InitializeWriterWithDestination( CFKDestination* pCFKDestination,
											FKWriterInitializeDestDict* pOption,
											ENUM_FK_ERROR_CODE& eError);

	virtual BOOL OpenWriterPageWithImageInfo(	ESImageInfo* pImageInfo,
										FKWriterPageOpenDict* pOption,
										ENUM_FK_ERROR_CODE& eError);

	virtual BOOL CloseWriterPageWithOption( FKWriterPageCloseDict* pOption,
									ENUM_FK_ERROR_CODE& eError );


	virtual BOOL FinalizeWriterWithOption( FKWriterFinalizeDestDict* pOption,
									ENUM_FK_ERROR_CODE& eError );

	virtual BOOL IsMultiPageSupport();

	virtual BOOL AppendSourceToDestination( CFKSource* pSource, ENUM_FK_ERROR_CODE& eError );
	virtual BOOL AppendConstSourceToDestination(const CFKSource* pSource, ENUM_FK_ERROR_CODE& eError);


};
