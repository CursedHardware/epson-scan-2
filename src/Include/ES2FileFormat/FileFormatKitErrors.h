//
//  FileFormatKitErrors.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 5/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once
#include <stdint.h>

typedef enum : uint32_t {
	kFKNoError                   = 0,

	kFKUnsupportedDestination    = 1000,
	kFKUnsupportedSource         = 1001,

	kFKInconsistentError         = 3001,
	kFKParameterError            = 3002,
	kFKUnknownError              = 3003,
	kFKMemError                  = 3004,

	kFKTiffSetFieldError         = 3101,
	kFKTiffGetFieldError         = 3102,
	kFKTiffWriteScanLineError    = 3103,
	kFKTiffReadScanLineError     = 3104,

	kFKPDFInvalidArgError        = 3201,
	kFKPDFWriteError             = 3202,
	kFKPDFMutexError             = 3203,
	kFKPDFMemError               = 3204,
	kFKPDFOCRError               = 3205,
	kFKPDFConvertError           = 3206,

	kFKJpegStartCompressError    = 3301,
	kFKJpegWriteScanlineError    = 3302,
	kFKJpegFinishCompressError   = 3303,

	kFKJpegStartDecompressError  = 3401,
	kFKJpegReadHaderError        = 3402,
	kFKJpegScanError             = 3403,
	kFKJpegFinishDecompressError = 3404,

	kFKFileCreateError           = 3501,

	kFKPNGInitializeError        = 3601,
	kFKPNGWriteError             = 3602,

	kFKBMPWriteError			 = 3701
}ENUM_FK_ERROR_CODE;
