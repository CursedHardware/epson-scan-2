//
//  FKWriterSubclassProtocol.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 11/12/14.
//  Copyright (c) 2014年 Sato Daisuke. All rights reserved.
//
#pragma once

#include "FKDestination.h"
#include "FKWriter.h"


class IFKConcreteWriterProtocol
{
protected:
//@required
	//- (BOOL)initializeWriterWithDestination:(FKDestination*)destination
    //                                 option:(FKWriterInitializeDestDict*)option
    //                                  error:(NSError**)outError;
	//
	virtual BOOL InitializeWriterWithDestination( CFKDestination* pCFKDestination,
											FKWriterInitializeDestDict* pOption,
											ENUM_FK_ERROR_CODE& eError) = 0;

	//- (BOOL)openWriterPageWithImageInfo:(ESImageInfo*)imageInfo
	//							 option:(FKWriterPageOpenDict*)option
	//							  error:(NSError**)error;
	virtual BOOL OpenWriterPageWithImageInfo(	ESImageInfo* pImageInfo,
										FKWriterPageOpenDict* pOption,
										ENUM_FK_ERROR_CODE& eError) = 0;


	//- (BOOL)closeWriterPageWithOption:(FKWriterPageCloseDict*)option
	//							error:(NSError**)outError;
	virtual BOOL CloseWriterPageWithOption( FKWriterPageCloseDict* pOption,
									ENUM_FK_ERROR_CODE& eError ) = 0;


	//- (BOOL)finalizeWriterWithOption:(FKWriterFinalizeDestDict*)option
	//							   error:(NSError **)outError;
	virtual BOOL FinalizeWriterWithOption( FKWriterFinalizeDestDict* pOption,
									ENUM_FK_ERROR_CODE& eError ) = 0;

	//- (BOOL)appendSourceToDestination:(FKSource*)source
	//							 error:(NSError**)outError
	virtual BOOL AppendSourceToDestination( CFKSource* pSource, ENUM_FK_ERROR_CODE& eError ) = 0;

//@optional

	//- (NSArray*)availablePageOpenOptionKeys;
	//- (NSArray*)availablePageCloseOptionKeys;
	//- (NSArray*)availableFinalizeOptionKeys;

	//- (BOOL)multiPageSupport;
	virtual BOOL IsMultiPageSupport() = 0;

};
