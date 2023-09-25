////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2Defs.h
//!
//! @brif     ESCI2コマンドパラメーター値の定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Include/es2CommnoType.h"

// Data Type
typedef enum : UInt32 {
	DATATYPE_LIST	= 'LIST',
	DATATYPE_RANGE	= 'RANG',
} ESCI2DataType;

// Header Information Code
typedef enum : UInt32 {
	HEADER_ERROR					= '#err',
	HEADER_NOTREADY					= '#nrd',
	HEADER_PAGESTART				= '#pst',
	HEADER_PAGEEND					= '#pen',
	HEADER_LEFTOVER_PAGE			= '#lft',
	HEADER_ATTENTION				= '#atn',
	HEADER_SURFACETYPE				= '#typ',
	HEADER_PARAMETER				= '#par',
	HEADER_DOCUMENTTYPE				= '#doc',
	HEADER_DOUBLEFEED_INFORMATION	= '#dfi',
	HEADER_SUSPEND_NOTIFY			= '#sus',
	HEADER_TERMINATOR				= '#---',
} ESCI2DataHeaderKey;

// '#err' '#ERR' '#WRN'
typedef enum : UInt32 {
	ERROR_PART_ADF	= 'ADF ',
	ERROR_PART_TPU	= 'TPU ',
	ERROR_PART_FB	= 'FB  ',
} ESCI2ErrorPart;

// '#err' '#ERR'
typedef enum : UInt32 {
	ERROR_FACTOR_COVEROPEN		= 'OPN ',
	ERROR_FACTOR_PAPERJAM		= 'PJ  ',
	ERROR_FACTOR_PAPEREMPTY		= 'PE  ',
	ERROR_FACTOR_FATAL			= 'ERR ',
	ERROR_FACTOR_LAMPTIME		= 'LTF ',
	ERROR_FACTOR_CRLOCK			= 'LOCK',
	ERROR_FACTOR_DOUBLEFEED		= 'DFED',
	ERROR_FACTOR_TRAYCLOSE		= 'DTCL',
    ERROR_FACTOR_AUTHFAIL		= 'AUTH',
    ERROR_FACTOR_NOPERMISSION	= 'PERM',
    ERROR_FACTOR_LOWBATTERY		= 'BTLO',
	ERROR_FACTOR_CRSHEETSENSOR	= 'CSS ',
	ERROR_FACTOR_ETSENSOR		= 'ETS ',
	ERROR_FACTOR_PAPERPROTECT	= 'PPRT',
	ERROR_FACTOR_SEPARATIONLEVER= 'SEP ',
	ERROR_FACTOR_POSE			= 'POSE',
	ERROR_FACTOR_GAPSENSOR		= 'GAP ',
    ERROR_FACTOR_ADFLIFT        = 'LIFT',
    ERROR_FACTOR_PAPER_REMOVE_ERROR = 'PRMW',
    ERROR_FACTOR_STACKER_ERROR      = 'STK ',
    ERROR_FACTOR_SKEW_ERROR         = 'PSKW',
	ERROR_FACTOR_PESENSOR		= 'PES ',
} ESCI2ErrorFactor;

// '#nrd'
typedef enum : UInt32 {
	NOTREADY_USERAUTH   = 'AFEN',
	NOTREADY_RESERVED	= 'RSVD',
	NOTREADY_WARMINGUP	= 'WUP ',
	NOTREADY_BUSY		= 'BUSY',
	NOTREADY_NONE		= 'NONE',
} ESCI2NotReadyReason;

// '#atn'
typedef enum : UInt32 {
	ATTENTION_CANCEL	= 'CAN ',
	ATTENTION_NOREQUEST	= 'NONE',
} ESCI2AttentionType;

// '#typ'
typedef enum : UInt32 {
	SURFACETYPE_FRONT	= 'IMGA',
	SURFACETYPE_BACK	= 'IMGB',
} ESCI2SurfaceType;

// '#par'
typedef enum : UInt32 {
	PARAMETER_OK	= 'OK  ',
	PARAMETER_FAIL	= 'FAIL',
	PARAMETER_LOST	= 'LOST',
} ESCI2ParameterResult;

// '#doc'
typedef enum : UInt32 {
	DOCUMENTTYPE_CRSHEET	= 'CRST',
	DOCUMENTTYPE_BLANKPAGE	= 'BLNK',
	DOCUMENTTYPE_MONOPAGE	= 'MONO',
} ESCI2DocumentType;

// #dfi
typedef enum : UInt32 {
	DOUBLEFEED_INFORMATION_DF = 'DFED',
} ESCI2DoubleFeedInformationType;

// 'INFO'
typedef enum : UInt32 {
	INFO_ADF			= '#ADF',
	INFO_TPU			= '#TPU',
	INFO_FB				= '#FB ',
	INFO_MAX_IMAGEPIX	= '#IMX',
	INFO_PUSHBUTTON		= '#PB ',
	INFO_PRODUCTNAME	= '#PRD',
	INFO_SERIALNUMBER	= '#S/N',
	INFO_VERSION		= '#VER',
	INFO_DEV_BUFFERSIZE	= '#DSZ',
	INFO_EXT_DATAPORT	= '#EXT',
	INFO_DUMMYLESS		= '#DLS',
	INFO_AUTHENTICATION = '#ATH',
	INFO_INITIALIZATION = '#INI',
	INFO_AUTOFOCUS      = '#FCA',
	INFO_MANUALFOCUS    = '#FCM',
	INFO_AUTOFEEDINGMODE = '#AFM',
	INFO_MIN_DOUBLEFEEDLEN = '#DFM',
	INFO_CROPPABLERESOL = '#CRR',
	INFO_PUSHSCANREADY  = '#SRD',
	INFO_JOB            = '#JOB',
	INFO_EXTINFO        = '#EXI',

	// Maintenance
	INFO_POWERSAVE		= '#psv',
	INFO_ADMIN_LOCK		= '#als',
	INFO_DEF_PASSWDTYPE	= '#npd'
} ESCI2InfoKey;


// '#ADF'
typedef enum : UInt32 {
	INFO_ADF_TYPE				= 'TYPE',
	INFO_ADF_DUPLEX				= 'DPLX',
	INFO_ADF_ORDER				= 'FORD',
	INFO_ADF_PREFEED			= 'PREF',
	INFO_ADF_WIDTHDETECTION		= 'DETX',
	INFO_ADF_HEIGHTDETECTION	= 'DETY',
	INFO_ADF_GUIDEPOSITION		= 'ALGN',
	INFO_ADF_GUIDELESS			= 'GDLS',
	INFO_ADF_AUTOSCAN			= 'ASCN',
	INFO_ADF_SCANAREA			= 'AREA',
	INFO_ADF_MIN_DOCUMENTSIZE	= 'AMIN',
	INFO_ADF_MAX_DOCUMENTSIZE	= 'AMAX',
	INFO_ADF_BASIC_RESOLUTION	= 'RESO',
	INFO_ADF_AUTORECOVERY		= 'RCVR',
	INFO_ADF_OVERSCAN_SIZE		= 'OVSN',
	INFO_ADF_CRSHEETDETECTION	= 'CRST',
    INFO_ADF_LOAD               = 'LOAD',

	// Maintenance
	INFO_ADF_LIFELIMIT          = 'LIFE',
	INFO_ADF_LAMP1LIFELIMIT     = 'LMP1',
	INFO_ADF_LAMP2LIFELIMIT     = 'LMP2',
	INFO_ADF_PICKUPROLLERLIFELIMIT  = 'PURL',
	INFO_ADF_PICKUPROLLERLIFENEAREND = 'purl',
	INFO_ADF_SEPARATIONPADLIFELIMIT = 'SPAD',
	INFO_ADF_SEPARATIONPADLIFENEAREND = 'spad',
	INFO_ADF_RETARDROLLERLIFELIMIT  = 'RTRL',
	INFO_ADF_RETARDROLLERLIFENEAREND = 'rtrl',
	INFO_ADF_ROLLERKITLIFELIMIT     = 'RKIT',
	INFO_ADF_ROLLERKITLIFENEAREND   = 'rkit',
	INFO_ADF_BACKGROUNDGLEVEL_FRONT = 'BGLF',
	INFO_ADF_BACKGROUNDGLEVEL_BACK  = 'BGLB',
} ESCI2ADFInfo;

// 'TYPE'
typedef enum : UInt32 {
	INFO_ADF_TYPE_PAGE		= 'PAGE',
	INFO_ADF_TYPE_SHEETFEED	= 'FEED',
} ESCI2ADFType;

// 'DPLX'
typedef enum : UInt32 {
	INFO_ADF_DUPLEX_1PASS	= '1SCN',
	INFO_ADF_DUPLEX_2PASS	= '2SCN',
} ESCI2ADFDuplex;

// 'FORD'
typedef enum : UInt32 {
	INFO_ADF_ORDER_1TON		= 'PF1N',
	INFO_ADF_ORDER_NTO1		= 'PFN1',
} ESCI2ADFOrder;

// 'ALGN'
typedef enum : UInt32 {
	INFO_GUIDEPOSITION_LEFT		= 'LEFT',
	INFO_GUIDEPOSITION_CENTER	= 'CNTR',
	INFO_GUIDEPOSITION_RIGHT	= 'RIGT',
} ESCI2PaperAlignment;

// '#TPU'
typedef enum : UInt32 {
	INFO_TPU_SCANAREA			= 'AREA',
	INFO_TPU_BASICRESOLUTION	= 'RESO',
	INFO_TPU_OVERSCAN_SIZE		= 'OVSN',
} ESCI2TPUInfo;
	
// '#FB '
typedef enum : UInt32 {
	INFO_FB_WIDTHDETECTION		= 'DETX',
	INFO_FB_HEIGHTDETECTION		= 'DETY',
	INFO_FB_GUIDEPOSITION		= 'ALGN',
	INFO_FB_SCANAREA			= 'AREA',
	INFO_FB_BASICRESOLUTION		= 'RESO',
	INFO_FB_OVERSCAN_SIZE		= 'OVSN',
    
    // Maintenance
    INFO_FB_LIFELIMIT          = 'LIFE',
    INFO_FB_LMP1LIFELIMIT      = 'LMP1',
    INFO_FB_LMP2LIFELIMIT      = 'LMP2',
} ESCI2FBInfo;

// '#VER'
typedef enum : UInt32 {
	INFO_VERSION_IPL	= 'IPL ',
	INFO_VERSION_FB		= 'FB  ',
	INFO_VERSION_ADF	= 'ADF ',
	INFO_VERSION_TPU	= 'TPU ',
} ESCI2VersionLabel;

// '#EXT'
typedef enum : UInt32 {
	INFO_EXT_DATAPORT_0 = 'EXT0',
	INFO_EXT_DATAPORT_1 = 'EXT1',
	INFO_EXT_DATAPORT_2 = 'EXT2',
} ESCI2BinaryDataPort;

// '#JOB'
typedef enum : UInt32 {
	INFO_JOB_STANDARD   = 'STD ',
	INFO_JOB_CONTINUE   = 'CONT',
	INFO_JOB_AFM        = 'AFM ',
	INFO_JOB_AFMC       = 'AFMC',
	INFO_JOB_END        = 'END ',
} ESCI2JobMode;

// 'CAPA' 'CAPB' 'PARA' 'PARB' 'RESA' 'RESB'
typedef enum : UInt32 {
	// Common codes
	CAP_ADF				= '#ADF',
	CAP_TPU				= '#TPU',
	CAP_FB				= '#FB ',
	CAP_COLORFORMAT		= '#COL',
	CAP_IMAGEFORMAT		= '#FMT',
	CAP_JPEGQUALITY		= '#JPG',
	CAP_THRESHOLD		= '#THR',
	CAP_DITHER_HALFTONE	= '#DTH',
	CAP_GAMMAMODE		= '#GMM',
	CAP_GAMMATABLE		= '#GMT',
	CAP_COLORMATRIX		= '#CMX',
	CAP_SHARPNESSFILTER	= '#SFL',
	CAP_MIRRORING		= '#MRR',
	CAP_RESOLUTION_MAIN	= '#RSM',
	CAP_RESOLUTION_SUB	= '#RSS',
	CAP_CROPPINGSIZE    = '#CRP',
	CAP_FOCUS			= '#FCS',
	CAP_EDGEFILL_COLOR	= '#FLC',
	CAP_EDGEFILL_AREA	= '#FLA',
	CAP_QUIETMODE		= '#QIT',
	CAP_LAMINATEDPAPER  = '#LAM',
	CAP_CLIENT_APPLICATION = '#APL',

    CAP_BG_REMOVAL        = '#BGR',
    CAP_BRTIGHTNESS        = '#BRT',
    CAP_CONSTRAST        = '#CTR',
    CAP_GAMMA_SCALE        = '#GMS',

	CAP_DETECTBP		= '#DBP',
	CAP_DETECTBPLEVEL	= '#DBL',
	CAP_DETECTCT		= '#DCT',
	CAP_SKIPIMAGE		= '#SIE',
	CAP_COLORCOUNTERTYPE= '#CCT',

	// Maintenance
	CAP_POWERSAVE       = '#SAV',
	CAP_POWEROFF        = '#POF',
	CAP_POWEROFF2ND     = '#PO2',
	CAP_DATEANDTIME     = '#D&T',
	CAP_SENSORGLASS     = '#GLS',
	CAP_DIRECTPOWERON   = '#DPO',
	CAP_NONCONNECT_POWEROFF = '#PNC',
	// PAR specific codes
	PAR_PAGECOUNT		= '#PAG',
	PAR_ACQUISITIONAREA	= '#ACQ',
	PAR_MAX_BUFFERSIZE	= '#BSZ',
	PAR_LENGTHDOUBLEFEED_LEN  = '#LDF',
	PAR_DOUBLEFEED_AREA = '#DFA',
	
} ESCI2CapabilityKey;


// '#ADF'
typedef enum : UInt32 {
	CAP_ADF_DUPLEX					= 'DPLX',
	CAP_ADF_PAPEREND_DETECTION		= 'PEDT',
	CAP_ADF_DOUBLEFEED_DETECTION0	= 'DFL0',
	CAP_ADF_DOUBLEFEED_DETECTION1	= 'DFL1',
	CAP_ADF_DOUBLEFEED_DETECTION2	= 'DFL2',
	CAP_ADF_DOUBLEFEED_DETECTION3	= 'DFL3',
	CAP_ADF_LENGTHDOUBLEFEED_DETECTION = 'LDF ',
	CAP_ADF_SWDOUBLEFEED_DETECTION  = 'SDF ',
	CAP_ADF_NONSTOP_DOUBLEFEED      = 'NSDF',	// ADF Non-Stop DoubleFeed Error  function is installed.
	CAP_ADF_SWPAPERPROTECTION       = 'SPP ',
	CAP_ADF_SCANSPEED_SELECTION		= 'FAST',
	CAP_ADF_BGCOLOR_WHITE			= 'BGWH',
	CAP_ADF_BGCOLOR_BLACK			= 'BGBK',
	CAP_ADF_BGCOLOR_GRAY			= 'BGGY',
	CAP_ADF_PAPERLOAD				= 'LOAD',
	CAP_ADF_PAPEREJECTION			= 'EJCT',
	CAP_ADF_CROPPING				= 'CRP ',
	CAP_ADF_CROPPING_INSCRIBED		= 'CRPI',
	CAP_ADF_SKEWCORRECTION			= 'SKEW',
	CAP_ADF_OVERSCAN_SIZE			= 'OVSN',
	CAP_ADF_OVERSCAN_SIZE_CROPPING  = 'OVTN',
	CAP_ADF_PASSPORTCRSHEET			= 'PCS ',	// ADF Passport Carrier Sheet Scanning function is installed.
	CAP_ADF_RESOLUTION				= 'RSMS',
	CAP_ADF_CLEANING				= 'CLEN',
	CAP_ADF_CALIBRATION				= 'CALB',

	// Maintenance
	CAP_ADF_SIMPLEXSCANCOUNTER      = 'SCNT',
	CAP_ADF_SIMPLEXCARDSCANCOUNTER  = 'SCRD',
	CAP_ADF_DUPLEXSCANCOUNTER       = 'DCNT',
	CAP_ADF_DUPLEXCARDSCANCOUNTER   = 'DCRD',
	CAP_ADF_LAMP1COUNTER            = 'LMP1',
	CAP_ADF_LAMP2COUNTER            = 'LMP2',
	CAP_ADF_PAPERJAMCOUNTER         = 'JAM ',
	CAP_ADF_DFEEDCOUNTER_ULTRASONIC = 'DFUS',
	CAP_ADF_DFEEDCOUNTER_LENGTH     = 'DFLN',
	CAP_ADF_DFEEDCOUNTER_SOFTWARE   = 'DFSW',
	CAP_ADF_PAPERPROTECTCOUNTER     = 'PPRT',
	CAP_ADF_PICKUPROLLERCOUNTER     = 'PURL',
	CAP_ADF_SEPARATIONPADCOUNTER    = 'SPAD',
	CAP_ADF_RETARDROLLERCOUNTER     = 'RTRL',
	CAP_ADF_ROLLERKITCOUNTER        = 'RKIT',
	CAP_ADF_CLEANINGWARNINGCOUNTER  = 'CLN ',	// ADF Cleaning warning  counter is installed.
	CAP_ADF_SPECIALDOCUMENTCOUNTER  = 'SPDC',
	CAP_ADF_PASSPORT_CS_COUNTER     = 'PCSC',		// ADF pass through Passport Carrier sheet counter is installed.
	CAP_ADF_ORIGINPOINTCORRECTION   = 'ORGM',
	CAP_ADF_TOPCORRECTION_FRONT     = 'TOPF',
	CAP_ADF_TOPCORRECTION_BACK      = 'TOPB',
	CAP_ADF_DOCUMENTDRIVECORRECTION_FRONT   = 'MAGF',
	CAP_ADF_DOCUMENTDRIVECORRECTION_BACK    = 'MAGB',
	CAP_ADF_LIGHTINTENSITY_BACK     = 'DENB',
	CAP_ADF_CLEANINGWARNING_TH      = 'THCL',	// Number of the cleaning warning(threshold) is installed. The unit of this parameter is 1000page. The setting range is from 1 to 255.(1000page to 255,000page)
	CAP_ADF_ROLLERKITCOUNTER_TH     = 'THRK',	// Number of the RKIT Life Limit warning is installed.
	CAP_ADF_RETARDROLLERCOUNTER_TH  = 'THRT',	// Number of the RTRL Life Limit warning is installed.
	CAP_ADF_SEPARATIONPADCOUNTER_TH = 'THSP',	// Number of the SPAD Life Limit warning is installed.
	CAP_ADF_PICKUPROLLERCOUNTER_TH  = 'THPU',	// Number of the PURL Life Limit warning is installed.
	CAP_ADF_PAPERPROTECTION         = 'PRTF',	// ADF Paper Protection error function is installed.
	CAP_ADF_CLEANINGWARNING         = 'CLNF',	// Cleaning warning function is detected.
	CAP_ADF_BEHAVIORWHEN_DF         = 'AADF',	// Behavior when Double Feed is detected.
} ESCI2ADFCapability;

// '#TPU'
typedef enum : UInt32 {
	CAP_TPU_AREA1					= 'ARE1',
	CAP_TPU_AREA2					= 'ARE2',
	CAP_TPU_NEGATIVE				= 'NEGL',
	CAP_TPU_INFRARED				= 'IR  ',
	CAP_TPU_MAGNIFICATION_CORRECT	= 'MAGC',
	CAP_TPU_SCANSPEED_SELECTION		= 'FAST',
	CAP_TPU_CROPPING				= 'CRP ',
	CAP_TPU_SKEWCORRECTION			= 'SKEW',
	CAP_TPU_OVERSCAN_SIZE			= 'OVSN',
    CAP_TPU_RESOLUTION              = 'RSMS',
} ESCI2TPUCapability;

// '#FB '
typedef enum : UInt32 {
	CAP_FB_LAMP1				= 'LMP1',
	CAP_FB_LAMP2				= 'LMP2',
	CAP_FB_SCANSPEED_SELECTION	= 'FAST',
	CAP_FB_CROPPING				= 'CRP ',
	CAP_FB_SKEWCORRECTION		= 'SKEW',
	CAP_FB_OVERSCAN_SIZE		= 'OVSN',
    CAP_FB_RESOLUTION           = 'RSMS',
    
    // Maintenance
    CAP_FB_SCANCOUNTER              = 'CNT ',
    CAP_FB_LAMP1COUNTER             = 'LMP1',
    CAP_FB_LAMP2COUNTER             = 'LMP2',
    CAP_FB_ORIGINPOINTCORRECTION_MAIN   = 'ORGM',
    CAP_FB_ORIGINPOINTCORRECTION_SUB    = 'ORGS',
    CAP_FB_CARRIAGEDRIVECORRECTION      = 'MAG ',
} ESCI2FBCapability;

// '#COL'
typedef enum : UInt32 {
	CAP_COLORFORMAT_RGB3		= 'C003',
	CAP_COLORFORMAT_RGB24		= 'C024',
	CAP_COLORFORMAT_RGB48		= 'C048',
	CAP_COLORFORMAT_MONO1		= 'M001',
	CAP_COLORFORMAT_MONO8		= 'M008',
	CAP_COLORFORMAT_MONO16		= 'M016',
	CAP_COLORFORMAT_DROP_R1		= 'R001',
	CAP_COLORFORMAT_DROP_R8		= 'R008',
	CAP_COLORFORMAT_DROP_R16	= 'R016',
	CAP_COLORFORMAT_DROP_G1		= 'G001',
	CAP_COLORFORMAT_DROP_G8		= 'G008',
	CAP_COLORFORMAT_DROP_G16	= 'G016',
	CAP_COLORFORMAT_DROP_B1		= 'B001',
	CAP_COLORFORMAT_DROP_B8		= 'B008',
	CAP_COLORFORMAT_DROP_B16	= 'B016',
} ESCI2ColorFormat;

// '#FMT'
typedef enum : UInt32 {
	CAP_IMAGEFORMAT_RAW		= 'RAW ',
	CAP_IMAGEFORMAT_JPEG	= 'JPG ',
} ESCI2ImageFormat;

// '#DTH'
typedef enum : UInt32 {
	CAP_DITHER_HALFTONE_NONE	= 'NONE',
	CAP_HALFTONE_A				= 'MIDA',
	CAP_HALFTONE_B				= 'MIDB',
	CAP_HALFTONE_C				= 'MIDC',
	CAP_DITHER_A				= 'DTHA',
	CAP_DITHER_B				= 'DTHB',
	CAP_DITHER_C				= 'DTHC',
	CAP_DITHER_D				= 'DTHD',
} ESCI2DitherHalftoneType;

// '#GMM'
typedef enum : UInt32 {
	CAP_GAMMAMODE_10 = 'UG10',
	CAP_GAMMAMODE_18 = 'UG18',
	CAP_GAMMAMODE_22 = 'UG22',
} ESCI2GammaMode;

// '#GMT'
typedef enum : UInt32 {
	CAP_GAMMATABLE_RED		= 'RED ',
	CAP_GAMMATABLE_GREEN	= 'GRN ',
	CAP_GAMMATABLE_BLUE		= 'BLU ',
	CAP_GAMMATABLE_MONO		= 'MONO',
} ESCI2GammaTableChannel;

// '#CMX'
typedef enum : UInt32 {
	CAP_COLORMATRIX_UNIT	= 'UNIT',
	CAP_COLORMATRIX_08		= 'UM08',
	CAP_COLORMATRIX_16		= 'UM16',
} ESCI2ColorMatrixMode;

// '#SFL'
typedef enum : UInt32 {
    CAP_SHARPNESSFILTER_SMOOTHING4    = 'SMT4',
    CAP_SHARPNESSFILTER_SMOOTHING3    = 'SMT3',
    CAP_SHARPNESSFILTER_SMOOTHING2    = 'SMT2',
    CAP_SHARPNESSFILTER_SMOOTHING1    = 'SMT1',
    CAP_SHARPNESSFILTER_NORMAL        = 'NORM',
    CAP_SHARPNESSFILTER_SHARPNESS1    = 'SHP1',
    CAP_SHARPNESSFILTER_SHARPNESS2    = 'SHP2',
    CAP_SHARPNESSFILTER_SHARPNESS3    = 'SHP3',
    CAP_SHARPNESSFILTER_SHARPNESS4    = 'SHP4',
} ESCI2SharpnessFilter;


// '#DBP'
typedef enum : UInt32 {
	CAP_DETECTBP_ON		= 'ON  ',
	CAP_DETECTBP_OFF	= 'OFF ',
} ESCI2DetectBlankPageMode;

// '#DCT'
typedef enum : UInt32 {
	CAP_DETECTCT_ON		= 'ON  ',
	CAP_DETECTCT_OFF	= 'OFF ',
} ESCI2DetectColorTypeMode;

// '#SIE'
typedef enum : UInt32 {
	CAP_SKIPIMAGE_ON	= 'ON  ',
	CAP_SKIPIMAGE_OFF	= 'OFF ',
} ESCI2SkipImageEnhancementMode;

// '#MRR'
typedef enum : UInt32 {
	CAP_MIRRORING_ON	= 'ON  ',
	CAP_MIRRORING_OFF	= 'OFF ',
} ESCI2MirroringMode;

// '#FCS'
typedef enum : UInt32 {
	CAP_FOCUS_AUTO = 'AUTO',
	CAP_FOCUS_MANU = 'MANU'
} ESCI2FocusMode;

// '#FLC'
typedef enum : UInt32 {
	CAP_EDGEFILL_COLOR_WH = 'WH  ',
	CAP_EDGEFILL_COLOR_BK = 'BK  ',
} ESCI2EdgeFillColor;

// '#QIT'
typedef enum : UInt32 {
	CAP_QUIETMODE_PREF	= 'PREF',
	CAP_QUIETMODE_ON	= 'ON  ',
	CAP_QUIETMODE_OFF	= 'OFF ',
} ESCI2QuietMode;

// '#LAM'
typedef enum : UInt32 {
	CAP_LAMINATEDPAPER_ON   = 'ON  ',
	CAP_LAMINATEDPAPER_OFF	= 'OFF ',
} ESCI2LaminatedPaperMode;

// '#GLS'
typedef enum : UInt32 {
	CAP_SENSORGLASS_OFF		= 'OFF ',
	CAP_SENSORGLASS_LOW		= 'LOW ',
	CAP_SENSORGLASS_NORMAL	= 'NORM',
} ESCI2SensorGlassDirtSensitivity;


// '#BGR'
typedef enum : UInt32 {
    CAP_BG_REMOVAL_OFF    = 'OFF ',
    CAP_BG_REMOVAL_LOW    = 'LOW ',
    CAP_BG_REMOVAL_HIGH    = 'HIGH',
} ESCI2BackgroundRemoval;


// '#DPO'
typedef enum : UInt32 {
	CAP_DIRECTPOWERON_ON	= 'ON  ',
	CAP_DIRECTPOWERON_OFF	= 'OFF ',
} ESCI2DirectPowerOn;

// '#PNC'
typedef enum : UInt32 {
	CAP_NONCONNECT_POWEROFF_ON	= 'ON  ',
	CAP_NONCONNECT_POWEROFF_OFF	= 'OFF ',
} ESCI2NonConnectPowerOff;

// '#ADFPRTF'
typedef enum : UInt32 {
	CAP_ADF_PAPERPROTECTION_OFF		= 'OFF ',
	CAP_ADF_PAPERPROTECTION_LOW		= 'LOW ',
	CAP_ADF_PAPERPROTECTION_NORM	= 'MID ',
	CAP_ADF_PAPERPROTECTION_HIGH	= 'HIGH',
} ESCI2ADFPaperProtection;

// '#ADFDF  '
typedef enum : UInt32 {
	CAP_ADF_BEHAVIORWHEN_DF_SIMD	= 'SIMD',
	CAP_ADF_BEHAVIORWHEN_DF_SEJ1	= 'SEJ1',
} ESCI2BehaviorWhenDoubleFeed;

// '#ADFCLNF'
typedef enum : UInt32 {
	CAP_ADF_CLEANINGWARNING_OFF		= 'OFF ',
	CAP_ADF_CLEANINGWARNING_ON		= 'ON  ',
} ESCI2CleaningWarningNotify;
// 'STAT'
typedef enum : UInt32 {
	STAT_ERROR			= '#ERR',
	STAT_PAPERSIZE		= '#PSZ',
	STAT_FOCUS			= '#FCS',
	STAT_PUSHBUTTON		= '#PB ',
	STAT_DOCSEPARATION	= '#SEP',
	STAT_SCANNERPOSITION= '#POS',
	STAT_CARDSCANNING	= '#CSL',
	STAT_BATTERY		= '#BAT',
	STAT_SENSORGLASS	= '#GLS',
	STAT_WARNING		= '#WRN',
	STAT_SCANPARAMETER	= '#USP',
	STAT_ERRORCODE		= '#ERC',

	STAT_ADMIN_LOCK		= '#als',
} ESCI2StatusKey;

// '#PSZ'
typedef enum : UInt32 {
	STAT_PAPERSIZE_A3V	= 'A3V ',
	STAT_PAPERSIZE_WLT	= 'WLT ',
	STAT_PAPERSIZE_B4V	= 'B4V ',
	STAT_PAPERSIZE_LGV	= 'LGV ',
	STAT_PAPERSIZE_A4V	= 'A4V ',
	STAT_PAPERSIZE_A4H	= 'A4H ',
	STAT_PAPERSIZE_LTV	= 'LTV ',
	STAT_PAPERSIZE_LTH	= 'LTH ',
	STAT_PAPERSIZE_B5V	= 'B5V ',
	STAT_PAPERSIZE_B5H	= 'B5H ',
    STAT_PAPERSIZE_A5V	= 'A5V ',
	STAT_PAPERSIZE_A5H	= 'A5H ',
	STAT_PAPERSIZE_B6V	= 'B6V ',
	STAT_PAPERSIZE_B6H	= 'B6H ',
	STAT_PAPERSIZE_A6V	= 'A6V ',
	STAT_PAPERSIZE_A6H	= 'A6H ',
	STAT_PAPERSIZE_EXV	= 'EXV ',
	STAT_PAPERSIZE_EXH	= 'EXH ',
	STAT_PAPERSIZE_HLTV	= 'HLTV',
	STAT_PAPERSIZE_HLTH	= 'HLTH',
	STAT_PAPERSIZE_PCV	= 'PCV ',
	STAT_PAPERSIZE_PCH	= 'PCH ',
	STAT_PAPERSIZE_KGV	= 'KGV ',
	STAT_PAPERSIZE_KGH	= 'KGH ',
	STAT_PAPERSIZE_CKV	= 'CKV ',
	STAT_PAPERSIZE_CKH	= 'CKH ',
	STAT_PAPERSIZE_8KV	= '8KV ',
	STAT_PAPERSIZE_16KV	= '16KV',
	STAT_PAPERSIZE_16KH	= '16KH',
	STAT_PAPERSIZE_OTHR	= 'OTHR',
	STAT_PAPERSIZE_INVD	= 'INVD',
} ESCI2PaperSize;

// '#FCS'
typedef enum : UInt32 {
	STAT_FOCUS_VALID	= 'VALD',
	STAT_FOCUS_INVALID	= 'INVD',
} ESCI2FocusStatus;

// '#SEP'
typedef enum : UInt32 {
	STAT_DOCSEPARATION_ON	= 'ON  ',
	STAT_DOCSEPARATION_OFF	= 'OFF ',
	STAT_DOCSEPARATION_SOFT	= 'SOFT',
} ESCI2DocumentSeparationStatus;

// '#POS'
typedef enum : UInt32 {
	STAT_SCANNERPOSITION_FLAT		= 'FLAT',
	STAT_SCANNERPOSITION_TILT		= 'TILT',
	STAT_SCANNERPOSITION_INVALID	= 'INVD',
} ESCI2ScannerPositionStatus;

// '#CSL'
typedef enum : UInt32 {
	STAT_CARDSCANNING_ON	= 'ON  ',
	STAT_CARDSCANNING_OFF	= 'OFF ',
} ESCI2CardScanningStatus;

// '#BTL'
typedef enum : UInt32 {
	STAT_BATTERY_LOW	= 'LOW ',
} ESCI2BatteryStatus;

// '#GLS'
typedef enum : UInt32 {
	STAT_SENSORGLASS_DIRTY		= 'DIRT',
	STAT_SENSORGLASS_INVALID	= 'INVD',
} ESCI2SensorGlassStatus;

// '#WRN'
typedef enum : UInt32 {
	STAT_WARNING_NEEDCLEANING = 'CLN ',
} ESCI2WarningStatus;

// '#USP'
typedef enum : UInt32 {
	STAT_SCANPARAMETER_ALL	= 'ALL ',
	STAT_SCANPARAMETER_AFMC	= 'AFMC',
	STAT_SCANPARAMETER_PCS	= 'PCS ',
} ESCI2UnavailableScanParameterStatus;

// 'AFM '
typedef enum : UInt32 {
	AFM_ON  = '#ON ',
	AFM_OFF = '#OFF',
} ESCI2AutoFeedinMode;

// 'als '
typedef enum : UInt32 {
	ADMINLOCK_ON   = 'ON  ',
	ADMINLOCK_OFF  = 'OFF ',
} ESCI2AdministratorLockMode;

// 'SRDY'
typedef enum : UInt32 {
	SRDY_ON  = '#ON ',
	SRDY_OFF = '#OFF',
} ESCI2PushScanReadyMode;

// 'JOB '
typedef enum : UInt32 {
	JOB_STANDARD    = '#STD',
	JOB_CONTINUE    = '#CON',
	JOB_AFM         = '#AFM',
	JOB_AFMC        = '#AFC',
	JOB_END         = '#END',
} ESCI2JobRequestMode;


// 'MECH'

typedef enum : UInt32 {
	MECH_ADF            = '#ADF',
	MECH_AUTOFOCUS      = '#FCA',
	MECH_MANUALFOCUS    = '#FCM',
	MECH_INITIALIZATION = '#INI',
} ESCI2MechRequestMode;


// 'APL'
typedef enum : UInt32 {
    APL_ES2    = 'ES2 ',
    APL_ESS    = 'ESP ',
} ESCI2APLType;

typedef enum : UInt32 {
	REQUEST_FINALIZE		= 'FIN ',	// ESC/I-2モードの終了
	REQUEST_CANCEL			= 'CAN ',	// キャンセル要求
	REQUEST_INFO			= 'INFO',	// スキャナ情報要求
	REQUEST_CAP_A			= 'CAPA',	// 操作可能な項目の要求
	REQUEST_CAP_B			= 'CAPB',	// 操作可能な項目の要求
	REQUEST_PARAM_A			= 'PARA',	// 読み取りパラメーター設定要求
	REQUEST_PARAM_B			= 'PARB',	// 読み取りパラメーター設定要求
	REQUEST_RESULT_A		= 'RESA',	// 読み取りパラメーター確認要求
	REQUEST_RESULT_B		= 'RESB',	// 読み取りパラメーター確認要求
	REQUEST_STATUS			= 'STAT',	// スキャナーステータス要求
	REQUEST_AFM				= 'AFM ',
	REQUEST_MECHCONTROL		= 'MECH',	// メカ動作要求
	REQUEST_TRANSFERDATA	= 'TRDT',	// 読み取り開始（DataStateへのステーと変更)
	REQUEST_IMAGE			= 'IMG ',	// 読み取り画像データー要求
	REQUEST_PUSHSCANREADY	= 'SRDY',
	REQUEST_JOB				= 'JOB ',
	REQUEST_EXTINFO         = 'EXI ',
	REQUEST_ADMIN_LOCK      = 'als ',	// Administartor Lock for Setting function temporary unlock / lock.

	// Reply code
	REPLY_UNKNOWN			= 'UNKN',	// 定義されていないリクエストコード
	REPLY_INVALID			= 'INVD',	// サポートされていないコマンドステート
} ESCI2RequestCode;

typedef enum : UInt32 {
    FS_REQUEST_CONTROL  = 'X',
    FS_REQUEST_INQUIRY  = 'Y',
    FS_REQUEST_MAINTENANCE  = 'Z',
} FSCode;

// '#CCT'
typedef enum : UInt32 {
	CAP_COLORCOUNTERTYPE_PREF		= 'PREF',
	CAP_COLORCOUNTERTYPE_COLOR		= 'COL ',
	CAP_COLORCOUNTERTYPE_MONO		= 'MONO',
} ESCI2ColorCounterType;


#define ESCI2_DIC_KEY_WIDTH						"width"
#define ESCI2_DIC_KEY_HEIGHT					"height"
#define ESCI2_DIC_KEY_DUMMY_BYTES_PER_LINE		"dummyBytesPerLine"

#define ESCI2_DIC_KEY_LEFT						"left"
#define ESCI2_DIC_KEY_RIGHT						"right"
#define ESCI2_DIC_KEY_TOP						"top"
#define ESCI2_DIC_KEY_BOTTOM					"bottom"

#define ESCI2_DIC_KEY_OFFSET					"offset"
#define ESCI2_DIC_KEY_LENGTH					"length"