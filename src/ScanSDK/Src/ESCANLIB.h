/***********************************************************************
	File		:ESCANLIB.H
	Revision	:0.9.4
	Date		:2015/9/9

	Copyright (c) Seiko Epson Corporation 2014. All rights reserved.
************************************************************************/
#pragma once

#ifndef WIN32
#define ESCANLIB
#include <stdint.h>
#define WINAPI
#define APIENTRY
#define CALLBACK
typedef uint32_t DWORD;
typedef int BOOL;
#define WCHAR char
#define LPCWSTR const WCHAR*
typedef unsigned short WORD;
typedef void* PVOID;
typedef long LONG;
typedef void* HANDLE;
typedef const char* LPCSTR;
typedef LPCSTR LPCTSTR;
#ifndef MAX_PATH
#define MAX_PATH	260
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
// Device status code definitions
#define ESL_STAT_READY						0x00030001
#define ESL_STAT_ERR_COVER_OPEN				0x00030002
#define ESL_STAT_ERR_PAPER_JAM				0x00030003
#define ESL_STAT_ERR_PAPER_EMPTY			0x00030004
#define ESL_STAT_ERR_FATAL					0x00030005
#define ESL_STAT_ERR_LAMP_TIME_FATAL		0x00030006
#define ESL_STAT_ERR_CR_LOCK				0x00030007
#define ESL_STAT_ERR_DOUBLE_FEED			0x00030008
#define ESL_STAT_ERR_DOCUMENT_TRAY_CLOSE	0x00030009
#define ESL_STAT_ERR_LOW_BATTERY			0x00030010
#define ESL_STAT_ERR_CARRIER_SHEET_SENSOR	0x00030011
#define ESL_STAT_ERR_EDGE_TRACKING_SENSOR	0x00030012
#define ESL_STAT_ERR_PAPER_PROTECTION		0x00030013
#define ESL_STAT_ERR_SEPARATION_LEVER		0x00030014
#define ESL_STAT_ERR_POSE					0x00030015
#define ESL_STAT_ERR_GAP_SENSOR				0x00030016
#define ESL_STAT_ERR_UNKNOWN				0x00030017
typedef struct tagRECT
{
  LONG    left;
  LONG    top;
  LONG    right;
  LONG    bottom;
} RECT;
#endif

//////////////////////////////////////////////////////////////////////////////
// Scanner connection
//////////////////////////////////////////////////////////////////////////////
typedef enum _eConnectType{
	CT_DEFAULT	= 0,	// Default(Use scanner selected by EPSON Scan Settings)
	CT_LOCAL	= 1,	// USB
	CT_NET_IPV4	= 2,	// Network(IPv4)
	CT_NET_IPV6	= 3		// Network(IPv6)
} eConnectType;

typedef struct _ConnectInfo {
	eConnectType	connectType;
	WCHAR			deviceName[64];		// Scanner Device Name
	WCHAR			networkAddress[64];	// Network(IPv4) "111.222.333.444" or Network(IPv6) "::::"
} ConnectInfo;
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Scan Parameters
//////////////////////////////////////////////////////////////////////////////

// Parameter operation type definitions
typedef enum _eParamOpType{
	POT_SET = 0,			// Set parameter
	POT_GET_CURRENT,		// Get current parameter
	POT_GET_DEFAULT,		// Get default parameter 
} eParamOpType;

// Parameter Struct type definitions
#define ESL_PARAM_SCAN_BASIC		0x00000001
#define ESL_PARAM_SCAN_EX_V1		0x00000002
#define ESL_PARAM_SAVE_BASIC		0x00000011
#define ESL_PARAM_SAVE_EX_V1		0x00000012
#define ESL_PARAM_SAVE_JPEG			0x00000021
#define ESL_PARAM_SAVE_TIFF			0x00000022
#define ESL_PARAM_SAVE_PDF			0x00000023
#define ESL_PARAM_OBR_BASIC			0x00010001
#define ESL_PARAM_OCR_BASIC			0x00020002
#define ESL_PARAM_OCR_EX_V1			0x00020003
#define ESL_PARAM_INFO_BASIC		0x00000101

// Common Parameter Struct Header definitions
typedef struct _EslParamHdr {
	DWORD		type;		// Parameter Struct Type
	DWORD		size;		// Size of Parameter Struct
} EslParamHdr, *pEslParamHdr;

// Document Source definitions
typedef enum _eDocSource {
	SRC_AUTO			= -1,
	SRC_FLATBED			= 0,
	SRC_FEEDER_SIMPLEX	= 1,
	SRC_FEEDER_DUPLEX	= 2,
} eDocSource;

// Image Type definitions
typedef enum _eImageType {
	IT_AUTO		= -1,
	IT_COLOR	= 0,
	IT_GRAY		= 1,
	IT_MONO		= 2,
} eImageType;

// Document Side definitions
typedef enum _eDocSize {
	DS_AUTO			= -1,
	DS_USER			= 0,
	DS_A3			= 0x01,
	DS_A4			= 0x02,
	DS_A5			= 0x03,
	DS_B4			= 0x11,
	DS_B5			= 0x12,
	DS_US_LEDGER	= 0x21,
	DS_US_LEGAL		= 0x22,
	DS_US_LETTER	= 0x23,
} eDocSize;

// Size Unit definitions
typedef enum _eSizeUnit {
	SU_INCH			= 0,
	SU_CENTIMETER	= 1,
	SU_PIXEL		= 2,
} eSizeUnit;

// Document Rotation definitions
typedef enum _eDocRotate {
	DR_AUTO		= -1,
	DR_NONE		= 0,
	DR_R90		= 1,
	DR_R180		= 2,
	DR_R270		= 3,
} eDocRotate;

// Color Filter definitions
typedef enum _eColorFilter {
	CF_NONE				=	0x00000000,
	CF_DROPOUT_RED		=	0x00000001,
	CF_DROPOUT_GREEN	=	0x00000002,
	CF_DROPOUT_BLUE		=	0x00000003,
	CF_DROPOUT_PINK		=	0x00000004,
	CF_DROPOUT_PURPLE	=	0x00000005,
	CF_DROPOUT_YELLOW	=	0x00000006,
	CF_DROPOUT_ORANGE	=	0x00000007,
	CF_DROPOUT_EXPBLACK	=	0x00000008,
	CF_ENHANCE_RED		=	0x00000011,
	CF_ENHANCE_GREEN	=	0x00000012,
	CF_ENHANCE_BLUE		=	0x00000013,
	CF_ENHANCE_PINK		=	0x00000014,
	CF_ENHANCE_PURPLE	=	0x00000015,
	CF_ENHANCE_YELLOW	=	0x00000016,
	CF_ENHANCE_ORANGE	=	0x00000017,
	CF_ENHANCE_EXPBLACK	=	0x00000018,
} eColorFilter;

// Document Enhance definitions
typedef enum _eDocEnhance {
	DE_NONE				=	0x00000000,
	DE_TET_STD			=	0x00000001,
	DE_TET_HIGH			=	0x00000002,
	DE_AAS				=	0x00000011,
} eDocEnhance;

// ImageFilter definitions
#define ESL_IMGFILTER_USM			0x00000001
#define ESL_IMGFILTER_DESCREENING	0x00000002

// Double Feed Detection definitions
typedef enum _eDFDetect {
	DFD_NONE	= 0,
	DFD_LOW		= 1,
	DFD_HIGH	= 2,
} eDFDetect;

// Blank Page Skip definitions
typedef enum _eBPSkip {
	BPS_NONE		= 0,
	BPS_VERY_LOW	= 1,
	BPS_LOW			= 2,
	BPS_MEDIUM		= 3,
	BPS_HIGH		= 4,
} eBPSkip;

// Punch Hole Remove definitions
typedef enum _ePHRemove {
	PHR_NONE		= 0,
	PHR_ON			= 1,
} ePHRemove;

// Skew Correction definitions
typedef enum _eSkewCorrect {
	SC_NONE			= 0,
	SC_EDGE			= 1,
	SC_CONTENTS		= 2,
	SC_BOTH			= 3,
} eSkewCorrect;

typedef struct _EslParamScanBasic {
	DWORD				type;					// Parameter Struct Type = ESL_PARAM_SCAN_BASIC
	DWORD				size;					// Size of Parameter Struct
	// Basic scanning parameters
	eDocSource			docSource;				// Document Source, including auto select
	eImageType			imgType;				// Image Type, including auto
	WORD				resolution;				// Scan Resolution 50 to 600
	eDocSize			docSize;				// pre-define, auto, user
	eSizeUnit			sizeUnit;				// for user custom size
	RECT				sizeUser;				// for user custom size
	eDocRotate			docRotate;				// 90,180,270 and auto
	LONG				brightness;				// -1000 to 1000
	LONG				contrast;				// -1000 to 1000
	LONG				threshold;				// 0 to 255
	eColorFilter		colorFilter;			// RGB Dropout, RGB Enhance
	WORD				numScan;				// Number of Pages to scan for ADF (0:ALL)
} EslParamScanBasic;

typedef struct _EslParamScanExV1 {
	DWORD				type;					// Parameter Struct Type = ESL_PARAM_SCAN_EX_V1
	DWORD				size;					// Size of Parameter Struct
	// Basic scanning parameters
	eDocSource			docSource;				// Document Source, including auto select
	eImageType			imgType;				// Image Type, including auto
	WORD				resolution;				// Scan Resolution 50 to 600
	eDocSize			docSize;				// pre-define, auto, user
	eSizeUnit			sizeUnit;				// for user custom size
	RECT				sizeUser;				// for user custom size
	eDocRotate			docRotate;				// 90,180,270 and auto
	LONG				brightness;				// -1000 to 1000
	LONG				contrast;				// -1000 to 1000
	LONG				threshold;				// 0 to 255
	eColorFilter		colorFilter;			// RGB Dropout, RGB Enhance
	WORD				numScan;				// Number of Pages to scan for ADF (0:ALL)
	// Extended scanning parameters(V1)
	eDocEnhance			docEnhance;				// Text Enhancement(level), Auto Area Segmentation
	DWORD				imgFilter;				// Uusharp Mask, Descreening
	eDFDetect			optDoubleFeedDetect;	// Double Feed detect
	eBPSkip				optBlankPageSkip;		// Blank page skip
	ePHRemove			optPunchHoleRemove;		// Punch Hole removal
	eSkewCorrect		skewCorrect;			// Skew Correction Type
} EslParamScanExV1;

typedef enum _eFileFormat {
	FF_BMP			= 0,
	FF_JPEG			= 1,
	FF_TIFF			= 2,
	FF_MULTITIFF	= 3,
	FF_PDF			= 4,
	FF_PNG			= 5,
} eFileFormat;

typedef enum _eFileName {
	FN_PREFIX		= 0,
	FN_BARCODE		= 1,
	FN_OCR			= 2,
} eFileName;

typedef enum _eTimeFormat {
	TF_NONE				= 0,
	TF_YYYYMMDD			= 1,
	TF_MMDDYYYY			= 2,
	TF_DDMMYYYY			= 3,
	TF_YYYYMMDD_HHMMSS	= 4,
	TF_MMDDYYYY_HHMMSS	= 5,
	TF_DDMMYYYY_HHMMSS	= 6,
} eTimeFormat;

// Barcode symbol type definitions
#define ESL_SYMBOL_UPC_EAN_JAN	0x00000001
#define ESL_SYMBOL_ITF			0x00000002
#define ESL_SYMBOL_CODE39		0x00000004
#define ESL_SYMBOL_CODABAR		0x00000008
#define ESL_SYMBOL_CODE128		0x00000010
#define ESL_SYMBOL_QR			0x00000020
#define ESL_SYMBOL_PDF417		0x00000040
#define ESL_SYMBOL_DM			0x00000080

typedef enum _eBCOrientation {
	BCO_BOTH		= 0,
	BCO_HORIZONTAL	= 1,
	BCO_VERTICAL	= 2,
} eBCOrientation;

typedef enum _eOCRLanguage {
	OL_NONE		= 0,
	OL_OCR_A	= 1,		// OCR A Font (Not used for Searchable PDF)
	OL_OCR_B	= 2,		// OCR B Font (Not used for Searchable PDF)
	OL_EN		= 0x10,		// English
	OL_JA		= 0x11,		// Japanese
	OL_ZH_TW	= 0x12,		// Simplified Chinese
	OL_ZH_CN	= 0x13,		// Traditional Chinese
	OL_KO		= 0x14,		// Korean
	OL_FR		= 0x15,		// French
	OL_IT		= 0x16,		// Italian
	OL_DE		= 0x17,		// German
	OL_ES		= 0x18,		// Spanish
	OL_PT		= 0x19,		// Portuguese
	OL_NL		= 0x1A,		// Dutch
	OL_RU		= 0x1B,		// Russian
	OL_UK		= 0x1C,		// Ukrainian
} eOCRLanguage;

typedef enum _eAssortType {
	AT_NONE			= 0,
	AT_PAGECOUNT	= 1,
	AT_BLANKPAGE	= 2,
	AT_BARCODE		= 3,
	AT_OCR			= 4,
} eAssortType;

// Assort option definitions
#define ESL_REMOVE_SEPARATOR		0x00000001
#define ESL_REMOVE_SEPARATOR_BACK	0x00000002
#define ESL_DETECT_BLANKPAGE_BACK	0x00000004

typedef enum _eBPLevel {
	BPL_LOW			= 0,
	BPL_MEDIUM		= 1,
	BPL_HIGH		= 2,
} eBPLevel;

typedef enum _eFolderName {
	FON_NONE		= 0,
	FON_PREFIX		= 1,
	FON_TIMESTAMP	= 2,
	FON_BARCODE_OCR	= 3,
} eFolderName;

typedef struct _EslParamSaveBasic {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_SAVE_BASIC
	DWORD			size;					// Size of Parameter Struct
	// Basic file save parameters
	DWORD			numPageStart;			// Processing Start page number
	DWORD			numPageEnd;				// Processing End page number
	eFileFormat		fileFormat;				// File format
	WCHAR			filePath[MAX_PATH];		// File path
	eFileName		fileNameType;			// File name type = FN_PREFIX
	WCHAR			fileNamePrefix[40];		// File name prefix string
	eTimeFormat		fileTimeFormat;			// File name time format
	WORD			fileNumCounterDigits;	// File name number of counter digits
	WORD			fileNumStartCount;		// File name counter start number
} EslParamSaveBasic;

typedef struct _EslParamSaveExV1 {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_SAVE_EX_V1
	DWORD			size;					// Size of Parameter Struct
	// Basic file save parameters
	DWORD			numPageStart;			// Processing Start page number
	DWORD			numPageEnd;				// Processing End page number
	eFileFormat		fileFormat;				// File format
	WCHAR			filePath[MAX_PATH];		// File path
	eFileName		fileNameType;			// File name type = FN_PREFIX
	WCHAR			fileNamePrefix[40];		// File name prefix string
	eTimeFormat		fileTimeFormat;			// File name time format
	WORD			fileNumCounterDigits;	// File name number of counter digits
	WORD			fileNumStartCount;		// File name counter start number
	// Advanced file name parameters
	DWORD			fileBCType;				// Barcode type to decode
	eBCOrientation	fileBCOrientation;		// Barcode orientaion
	eOCRLanguage	fileOcrLanguage;		// OCR language	
	eSizeUnit		fileDetectUnit;			// Unit for detect area
	RECT			fileDetectRect;			// Area to detect barcodes
	// Assort function parameters
	eAssortType		assortType;				// Assort select type
	DWORD			assortOption;			// Assort options
	DWORD			assortDivPageCount;		// Assort option for Page Count type
	eBPLevel		assortBlankPageLevel;	// Assort option for Blank page type
	DWORD			assortBCType;			// Barcode type to decode
	eBCOrientation	assortBCOrientation;	// Barcode orientaion
	eOCRLanguage	assortOcrLanguage;		// OCR language	
	eSizeUnit		assortDetectUnit;		// Unit for detect area
	RECT			assortDetectRect;		// Area to detect barcodes
	// Folder name for assort parameters
	eFolderName		folderNameType;			// Folder name type
	WCHAR			folderNamePrefix[40];	// Folder name prefix string
	eTimeFormat		folderTimeFormat;		// Folder name time format
	WORD			folderNumCounterDigits;	// Folder name number of counter digits
	WORD			folderNumStartCount;	// Folder name counter start number
} EslParamSaveExV1;

typedef struct _EslParamSaveJPEG {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_SAVE_JPEG
	DWORD			size;					// Size of Parameter Struct
	// JPEG file format save parameters
	WORD			imgQuality;				// Image quality (1 - 100)
	BOOL			encProgressive;			// Use progressive encoding
} EslParamSaveJPEG;

typedef enum _eByteOrder {
	BO_WINDOWS		= 0,
	BO_MACINTOSH	= 1,
} eByteOrder;

typedef enum _eTiffComp {
	TC_NONE			= 0,	// No Compresion
	TC_LZW			= 1,	// Color/Gray only
	TC_JPEG			= 2,	// Color/Gray only
	TC_CCITT_G4		= 3,	// Mono only
} eTiffComp;

typedef struct _EslParamSaveTIFF {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_SAVE_TIFF
	DWORD			size;					// Size of Parameter Struct
	// TIFF/Multi-TIFF file format save parameters
	eByteOrder		byteOrder;				// Byte order type
	eTiffComp		compColorGray;			// Color/Gray compression type
	eTiffComp		compMono;				// Mono compression type
	WORD			imgQuality;				// Image quality (1 - 100), valid when Jpeg compression
} EslParamSaveTIFF;

typedef enum _ePdfType {
	PT_NORMAL		= 0,	// Normal PDF document
	PT_PDF_A		= 1,	// Make PDF/A document
	PT_HIGHCOMP		= 2,	// HIgh Compression ADF document (Efficient for Color/Gray only)
} ePdfType;

typedef struct _EslParamSavePDF {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_SAVE_PDF
	DWORD			size;					// Size of Parameter Struct
	// PDF file format save parameters
	ePdfType		pdfType;				// PDF file type
	BOOL			addOcrText;				// Use OCR function to make searchable PDF
	eOCRLanguage	ocrLanguage;			// OCR language	
} EslParamSavePDF;

typedef struct _EslParamOBRBasic {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_OBR_BASIC
	DWORD			size;					// Size of Parameter Struct
	// Basic OBR process parameters
	DWORD			numPageStart;			// Processing Start page number
	DWORD			numPageEnd;				// Processing End page number
	DWORD			numDecode;				// Maximum number of barcode to decode in 1 page
	DWORD			symbolType;				// Barcode type to decode
	eBCOrientation	orientation;			// Barcode orientaion
	eSizeUnit		detectUnit;				// Unit for detect area
	RECT			detectRect;				// Area to detect barcodes
} EslParamOBRBasic;

typedef struct _EslParamOCRBasic {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_OCR_BASIC
	DWORD			size;					// Size of Parameter Struct
	// Basic OCR process parameters
	DWORD			numPageStart;			// Processing Start page number
	DWORD			numPageEnd;				// Processing End page number
	eOCRLanguage	ocrLanguage;			// OCR language	
	eSizeUnit		detectUnit;				// Unit for detect area
	RECT			detectRect;				// Area to detect characters
} EslParamOCRBasic;

// OCR-A/B Font detect character type definitions
#define ESL_OCRFONT_ALL			0x00000000
#define ESL_OCRFONT_NUM			0x00000001
#define ESL_OCRFONT_ALPHA_CAP	0x00000002
#define ESL_OCRFONT_ALPHA_SML	0x00000004
#define ESL_OCRFONT_ALPHA		(ESL_OCRFONT_ALPHA_CAP | ESL_OCRFONT_ALPHA_SML)
#define ESL_OCRFONT_SYMBOL		0x00000008

typedef struct _EslParamOCRExV1 {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_OCR_EX_V1
	DWORD			size;					// Size of Parameter Struct
	// Basic OCR process parameters
	DWORD			numPageStart;			// Processing Start page number
	DWORD			numPageEnd;				// Processing End page number
	eOCRLanguage	ocrLanguage;			// OCR language	
	eSizeUnit		detectUnit;				// Unit for detect area
	RECT			detectRect;				// Area to detect characters
	DWORD			ocrCharType;			// Detect character types (for OCR-A/B Font)
} EslParamOCRExV1;

typedef struct _EslParamInfoBasic {
	DWORD			type;					// Parameter Struct Type = ESL_PARAM_INFO_BASIC
	DWORD			size;					// Size of Parameter Struct
	// Basic Scanner Information
	WCHAR			serialNumber[256];
	WCHAR			reserved1[256];
	DWORD			reserved2[3];
} EslParamInfoBasic;

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Scan Execute Parameters
//////////////////////////////////////////////////////////////////////////////

// Execute type definitions
typedef enum _eExecType{
	ET_SCAN_AND_SAVE = 0,	// Scan and Save to file directly
	ET_SCAN_AND_STORE,		// Scan and store image data in the library
	ET_SAVE_STORED_IMAGE,	// Save stored image data to file 
	ET_GET_STORED_IMAGE,	// Get stored image data via memory 
	ET_GET_OBR_DATA,		// Get OBR result from stored images
	ET_GET_OCR_DATA,		// Get OCR result from stored images
} eExecType;

//////////////////////////////////////////////////////////////////////////////
// Callback API
//////////////////////////////////////////////////////////////////////////////

// Callback data type definitions
#define CBDT_STATUS_SCAN		0x00000001
#define CBDT_IMAGE_FILE			0x00000011
#define CBDT_IMAGE_MEMORY		0x00000012
#define CBDT_DATA_OBR			0x00010001
#define CBDT_DATA_OCR			0x00010002

// Callback data struct header
typedef struct _EslCBDataHdr {
	DWORD		type;			// Callback Data Struct Type
	DWORD		size;			// Size of Callback Data Struct
} EslCBDataHdr;

// Callback function prototype
typedef BOOL (CALLBACK *peslCallback)(EslCBDataHdr* pData, PVOID pPrivate);

// Callback data struct for CBDT_STATUS_SCAN
#define STATUS_SCAN_UPDATE_PROGRESS		0x00000001
#define STATUS_SCAN_PAGE_END			0x00000002
typedef struct _EslCBDataScanStatus {
	DWORD		type;				// Callback Data Struct Type = CBDT_STATUS_SCAN
	DWORD		size;				// Size of Callback Data Struct
	DWORD		status;				// scan status
	DWORD		count;				// scan document count
	DWORD		progress;			// scan progress percentage in 1page
} EslCBDataScanStatus;

// Callback data struct for CBDT_IMAGE_FILE
#define STATUS_IMAGE_FILE_INFO			0x00000011
typedef struct _EslCBDataImageFile {
	DWORD		type;				// Callback Data Struct Type = CBDT_IMAGE_FILE
	DWORD		size;				// Size of Callback Data Struct
	DWORD		status;				// save status
	WCHAR		filename[MAX_PATH];	// save file name
} EslCBDImageFile;

// Callback data struct for CBDT_IMAGE_MEMORY
#define STATUS_IMAGE_MEMORY_INFO		0x00000021
typedef struct _EslCBDataImageMemory {
	DWORD		type;				// Callback Data Struct Type = CBDT_IMAGE_MEMORY
	DWORD		size;				// Size of Callback Data Struct
	DWORD		status;				// save status
	DWORD		count;				// scan document count
	LONG		img_width;
	LONG		img_height;
	DWORD		img_resolution;
	WORD		img_channel_per_pixel;
	WORD		img_bits_per_channel;
	DWORD		img_data_size;
	HANDLE		img_handle;			// Image Handle
} EslCBDataImageMemory;

// Callback data struct for CBDT_DATA_OBR
#define STATUS_DATA_OBR_RESULT			0x00000031
typedef struct _EslCBDataOBRData {
	DWORD		type;				// Callback Data Struct Type = CBDT_DATA_OBR
	DWORD		size;				// Size of Callback Data Struct
	DWORD		status;				// status
	DWORD		count;				// scan document count
	DWORD		index;				// barcode index number in 1page
	DWORD		symbolType;			// Barcode type
	LONG		dataLength;			// Number of characters in decoded message
	LPCWSTR		dataString;			// Decoded message
	LONG		orientation;		// Barcode orientation 1:vertical 0:horizontal
} EslCBDataOBRData;

// Callback data struct for CBDT_DATA_OCR
#define STATUS_DATA_OCR_RESULT			0x00000041
typedef struct _EslCBDataOCRData {
	DWORD		type;				// Callback Data Struct Type = CBDT_DATA_OCR
	DWORD		size;				// Size of Callback Data Struct
	DWORD		status;				// status
	DWORD		count;				// scan document count
	LONG		dataLength;			// Number of characters in decoded message
	LPCWSTR		dataString;			// Decoded message
} EslCBDataOCRData;

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Epson Scan Library APIs
//////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

// Global Initialization APIs
BOOL	WINAPI	eslInit(DWORD* pInstance, LPCSTR pszKey, DWORD* pErrCode);		// Initialize lib instance
BOOL	WINAPI	eslFree(const DWORD Instance, DWORD* pErrCode);					// Free lib instance

// Operation APIs for each Instance
BOOL	WINAPI	eslOpenScanner(const DWORD Instance, const ConnectInfo* pScanner, DWORD* pErrCode);			// Open and connect scanner
BOOL	WINAPI	eslCtrlParameter(const DWORD Instance, const eParamOpType OpType, EslParamHdr* pParam, DWORD* pErrCode);		// Operate parameters
BOOL	WINAPI	eslExecute(const DWORD Instance, const eExecType ExecType, peslCallback pCbFunc, PVOID pPrivate, DWORD* pErrCode);		// Execute process
BOOL	WINAPI	eslCloseScanner(const DWORD Instance, DWORD* pErrCode);								// Close scanner
#ifndef WIN32
BOOL	WINAPI	eslGetDeviceStatus(const DWORD Instance, DWORD* pStatusCode, DWORD* pErrCode);
#endif
#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Epson Scan Library Error Codes
//////////////////////////////////////////////////////////////////////////////
// Success Codes
#define	ESL_SUCCESS					0x00000000		// Succeeded
#define	ESL_CANCEL					0x00000001		// Operation is canceled


#define	ESL_ERR_PAPERJAM			0x40000020
#define	ESL_ERR_PAPERDOUBLEFEED		0x40000021
#define	ESL_ERR_OUTOFPAPER			0x40008002
#define	ESL_ERR_ADFCOVEROPEN		0x40008003

// Generic Errors
#define ESL_ERR_GENERAL				0x80000001		// General Error
#define ESL_ERR_NOT_INITIALIZED		0x80000002		// Library is not initialize correctly(Internal Error)
#define ESL_ERR_FILE_MISSING		0x80000003		// Required file is missing
#define ESL_ERR_INVALID_PARAM		0x80000004		// Invalid parameter are set
#define ESL_ERR_LOW_MEMORY			0x80000005		// Not enough memory to operate
#define ESL_ERR_LOW_DISKSPACE		0x80000006		// Not enough free disk space to operate
#define ESL_ERR_WRITE_FAIL			0x80000007		// Failed to write to disk
#define ESL_ERR_READ_FAIL			0x80000008		// Failed to read from disk
#define ESL_ERR_INVALID_OPERATION	0x80000009		// Invalid operation is specified

// eslInit specific Errors
#define ESL_ERR_INVALID_KEY			0x80010001		// Licencekey is invalid

// eslOpenScanner specific Errors
#define ESL_ERR_NOT_SUPPORTED		0x80020001		// Specified model is not supported
#define ESL_ERR_NO_DRIVER			0x80020002		// Scanner driver for specified model is not installed
#define ESL_ERR_OPEN_FAIL			0x80020003		// Failed to open scanner driver

// eslExecute specific Errors
#define ESL_ERR_SCAN_GENERAL		0x80030001		// Scanning operation failed
