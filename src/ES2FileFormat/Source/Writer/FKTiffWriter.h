//
//  FKTiffWriter.h
//  ESImageFormatKit
//
//  Created by Yumoto Atsushi on 2013/12/05.
//  Copyright (c) 2013年 EPSON. All rights reserved.
//

#include "FKWriter.h"

class FKTiffEncodeUtil;

class CFKTiffWriter : public CFKWriter
{
private:
	FKTiffEncodeUtil* m_pCFKTiffEncodeUtil;
	uint32_t	m_unBytesPerLine;

public:
	CFKTiffWriter();
	~CFKTiffWriter();

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
	virtual BOOL CheckFileFormatMaxbytes(ESNumber nNextImageSize) ;

};
