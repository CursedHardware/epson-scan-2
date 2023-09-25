//
//  FKJpegEncodeUtil.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 14/12/19.
//  Copyright (c) 2014 EPSON. All rights reserved.
//
#pragma once
#define HAVE_BOOLEAN
typedef int boolean;
#include "jpeglib.h"
#include "FileFormatKitErrors.h"
#include "jpegUserDefines.h"
#include "FKDestination.h"


class CFKJpegEncodeUtil
{
private:
	struct jpeg_compress_struct	m_stCInfo;
	struct my_error_mgr			m_stJerr;

	FILE* m_pCFile;

	BOOL m_bStarted;

public:
	CFKJpegEncodeUtil();
	~CFKJpegEncodeUtil();

	//- (BOOL)startEncodingWithDest:(FKDestination*)destination
    //                imageInfo:(ESImageInfo*)imageInfo
    //                  quality:(int32_t)quality
    //          progressiveMode:(BOOL)progressiveMode
    //           iccProfilePath:(NSString*)iccProfilePath
    //                    error:(NSError**)outError;
	BOOL StartEncodingWithDest( CFKDestination* pDestination,
								ESImageInfo* pImageInfo,
								INT32 nQuality,
								BOOL bProgressiveMode,
								ESString strIccProfilePath,
								ENUM_FK_ERROR_CODE& eError );


	//- (BOOL)writeScanlinesWithData:(NSData*)data error:(NSError**)outError;
	BOOL WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ENUM_FK_ERROR_CODE& eError );

	//- (BOOL)finalizeEncodingAndReturnError:(NSError**)outError;
	BOOL FinalizeEncodingAndReturnError( ENUM_FK_ERROR_CODE& eError );

	//- (BOOL)openOutFileWithPath:(NSString*)outFilePath
	BOOL OpenOutFileWithPath( ESString* outFilePath );

	//- (void)closeOutFile
	void CloseOutFile();

	//- (FKErrorCode)updateJpegStructWithDest:(FKDestination*)destination
    //                          imageInfo:(ESImageInfo*)imageInfo
    //                            quality:(int32_t)quality
    //                    progressiveMode:(BOOL)progressiveMode
	ENUM_FK_ERROR_CODE UpdateJpegStructWithDest(	CFKDestination* pDestination,
											ESImageInfo* pImageInfo,
											INT32 nQuality,
											BOOL bProgressiveMode);

};