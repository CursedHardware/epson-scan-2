//
//  FKConcreteReaderProtocol.h
//  FileFormatKit
//
//  Created by Nakazawa Yuichi on 11/12/14.
//  Copyright (c) 2014年 epson. All rights reserved.
//
#pragma once

#include "FKSource.h"

class CFKDestination;

class IFKConcreteReaderProtocol
{
protected:
//@required
	//- (BOOL)openReaderWithOption:(NSDictionary*)option error:(NSError**)outError;
	virtual BOOL OpenReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;

	//- (BOOL)appendSourceToReader:(FKSource*)source error:(NSError**)outError;
	virtual BOOL AppendSourceToReader( IFKSource* pcSource, ENUM_FK_ERROR_CODE& eError ) = 0;

	//- (BOOL)closeReaderWithOption:(NSDictionary*)option error:(NSError**)outError;
	virtual BOOL CloseReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;

//@optional

	//- (NSArray*)availableOpenOptionKeys;

	//- (NSArray*)availableCloseOptionKeys;


};
