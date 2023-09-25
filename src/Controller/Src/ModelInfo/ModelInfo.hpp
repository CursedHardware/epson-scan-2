#pragma once
#include "EpsonScan2.h"
#include <ESCommonTypedef.h>
#include <stdint.h>
#include <algorithm>
#include <string>

namespace epsonscan {

//
//  機種情報ファイルのフォルダーパス
//
//static const ES_CHAR* kBaseResourceDirPath					 = "/home/build/workspace/trunk/Core/bin/Resources/";
static const ES_CHAR* kBaseResourceDirPath					 = "Resources/";

//---------------------------------------------------------
// Hardware Info
//---------------------------------------------------------
static const ES_CHAR* kPrvHRD_HRDVersion                     = "ESHRDVersion";

static const ES_CHAR* kPrvHRD_ModelName                      = "ESModelName";

static const ES_CHAR* kPrvHRD_ModelID                        = "ESModelID";

static const ES_CHAR* kPrvHRD_Kind                           = "ESKind";
typedef enum: uint32_t
{
	kPrvHRD_Kind_SF_Document = 11,
	kPrvHRD_Kind_SF_Photo = 12,
	kPrvHRD_Kind_MF_Business = 21,
	kPrvHRD_Kind_MF_Consumer = 22,
	kPrvHRD_Kind_MF_LFP = 23,
}PrvHRD_Kind;

static const ES_CHAR* kPrvHRD_ConnectionType                 = "ESConnectionType";
typedef enum: uint32_t
{
	kPrvHRD_ConnectionType_USB = 0,
	kPrvHRD_ConnectionType_Network,
	kPrvHRD_ConnectionType_Both,
}PrvHRD_ConnectionType;

static const ES_CHAR* kPrvHRD_CommandType                    = "ESCommandType";
typedef enum: uint32_t
{
	kPrvHRD_CommandType_ESCI_B7_B8 = 0,
	kPrvHRD_CommandType_FS,
	kPrvHRD_CommandType_Host_Base_ASIC,
	kPrvHRD_CommandType_Host_Base_FS_D7_8,
	kPrvHRD_CommandType_ESCI2,
}PrvHRD_CommandType;

static const ES_CHAR* kPrvHRD_BitDepth                       = "ESBitDepth";

static const ES_CHAR* kPrvHRD_ProductName                    = "ESProductName";

static const ES_CHAR* kPrvHRD_ProfileName                    = "ESProfileName";

static const ES_CHAR* kPrvHRD_Resolution                     = "ESResolution";

static const ES_CHAR* kPrvHRD_DustRemoval                    = "ESDustRemoval";
typedef enum: uint32_t
{
	kPrvHRD_DustRemoval_NotSupport = 0,
	kPrvHRD_DustRemoval_Support,
}PrvHRD_DustRemoval;

static const ES_CHAR* kPrvHRD_DICE                           = "ESDICE";
typedef enum: uint32_t
{
	kPrvHRD_DICE_NotSupport = 0,
	kPrvHRD_DICE_Support,
}PrvHRD_DICE;

static const ES_CHAR* kPrvHRD_DICELite                       = "ESDICELite";
typedef enum: uint32_t
{
    kPrvHRD_DICELite_NotSupport = 0,
    kPrvHRD_DICELite_Support,
}PrvHRD_DICELite;

static const ES_CHAR* kPrvHRD_HalftoneProcess                = "ESHalftoneProcess";
typedef enum: uint32_t
{
	kPrvHRD_HalftoneProcess_NotSupport = 0,
	kPrvHRD_HalftoneProcess_Support,
}PrvHRD_HalftoneProcess;

static const ES_CHAR* kPrvHRD_HalftoneMode                   = "ESHalftoneMode";
typedef enum: uint32_t
{
	kPrvHRD_HalftoneMode_NotSupport = 0,
	kPrvHRD_HalftoneMode_Support,
}PrvHRD_HalftoneMode;

static const ES_CHAR* kPrvHRD_Dither                         = "ESDither";
typedef enum: uint32_t
{
	kPrvHRD_Dither_NotSupport = 0,
	kPrvHRD_Dither_Support,
}PrvHRD_Dither;

static const ES_CHAR* kPrvHRD_AAS                            = "ESAAS";
typedef enum: uint32_t
{
	kPrvHRD_AAS_NotSupport = 0,
	kPrvHRD_AAS_Support,
}PrvHRD_AAS;

static const ES_CHAR* kPrvHRD_TET                            = "ESTET";
typedef enum: uint32_t
{
	kPrvHRD_TET_NotSupport = 0,
	kPrvHRD_TET_Support,
}PrvHRD_TET;

static const ES_CHAR* kPrvHRD_InterpreterName                = "ESInterpreterName";

static const ES_CHAR* kPrvHRD_FlatbedType                    = "ESFlatbedType";
typedef enum: uint32_t
{
	kPrvHRD_FlatbedType_A4 = 0,
	kPrvHRD_FlatbedType_A3,
	kPrvHRD_FlatbedType_A3_Plus,
}PrvHRD_FlatbedType;

static const ES_CHAR* kPrvHRD_ADFPixel                       = "ESADFPixel";

static const ES_CHAR* kPrvHRD_ModeDefault                    = "ESModeDefault";
typedef enum: uint32_t
{
	kPrvHRD_ModeDefault_Document = 1,
	kPrvHRD_ModeDefault_Photo,
}PrvHRD_ModeDefault;

static const ES_CHAR* kPrvHRD_DocumentMode                   = "ESDocumentMode";
typedef enum: uint32_t
{
	kPrvHRD_DocumentMode_NotSupport = 0,
	kPrvHRD_DocumentMode_Support,
}PrvHRD_DocumentMode;

static const ES_CHAR* kPrvHRD_PhotoMode                      = "ESPhotoMode";
typedef enum: uint32_t
{
	kPrvHRD_PhotoMode_NotSupport = 0,
	kPrvHRD_PhotoMode_Support,
}PrvHRD_PhotoMode;

static const ES_CHAR* kPrvHRD_SetConfig                      = "ESSetConfig";
typedef enum: uint32_t
{
	kPrvHRD_SetConfig_False = 0,
	kPrvHRD_SetConfig_True,
}PrvHRD_SetConfig;

static const ES_CHAR* kPrvHRD_ProductID                      = "ESProductID";

static const ES_CHAR* kPrvHRD_FWDriverName                   = "ESFWDriverName";

static const ES_CHAR* kPrvHRD_DTRNegaMode                    = "ESDTRNegaMode";
typedef enum: uint32_t
{
	kPrvHRD_DTRNegaMode_NotSupport = 0,
	kPrvHRD_DTRNegaMode_Support,
}PrvHRD_DTRNegaMode;

static const ES_CHAR* kPrvHRD_HolderRange                    = "ESHolderRange";

static const ES_CHAR* kPrvHRD_FilmSizeRange                  = "ESFilmSizeRange";

static const ES_CHAR* kPrvHRD_FilmSize                       = "ESFilmSize";

static const ES_CHAR* kPrvHRD_DropOut                        = "ESDropOut";
typedef enum: uint32_t
{
	kPrvHRD_DropOut_NotSupport = 0,
	kPrvHRD_DropOut_Full,
	kPrvHRD_DropOut_RGB,
}PrvHRD_DropOut;

static const ES_CHAR* kPrvHRD_ColorEnhance                   = "ESColorEnhance";
typedef enum: uint32_t
{
	kPrvHRD_ColorEnhance_NotSupport = 0,
	kPrvHRD_ColorEnhance_Full,
	kPrvHRD_ColorEnhance_RGB,
}PrvHRD_ColorEnhance;

static const ES_CHAR* kPrvHRD_Protocol                       = "ESProtocol";
typedef enum: uint32_t
{
	kPrvHRD_Protocol_Network_TWAIN = 0,
	kPrvHRD_Protocol_Internet_Scan,
}PrvHRD_Protocol;

static const ES_CHAR* kPrvHRD_ADFPreview                     = "ESADFPreview";
typedef enum: uint32_t
{
	kPrvHRD_ADFPreview_NotSupport = 0,
	kPrvHRD_ADFPreview_Support,
}PrvHRD_ADFPreview;

static const ES_CHAR* kPrvHRD_FlatbedOffset                  = "ESFlatbedOffset";
typedef enum: uint32_t
{
	kPrvHRD_FlatbedOffset_Left = 0,
	kPrvHRD_FlatbedOffset_Center,
}PrvHRD_FlatbedOffset;

static const ES_CHAR* kPrvHRD_ADFOffset                      = "ESADFOffset";
typedef enum: uint32_t
{
	kPrvHRD_ADFOffset_Left = 0,
	kPrvHRD_ADFOffset_Center_Auto,
	kPrvHRD_ADFOffset_Center_Manual,
	kPrvHRD_ADFOffset_Right_Manual,
}PrvHRD_ADFOffset;

static const ES_CHAR* kPrvHRD_PrintICELevel                  = "ESPrintICELevel";
typedef enum: uint32_t
{
	kPrvHRD_PrintICELevel_NotSupport = 0,
	kPrvHRD_PrintICELevel_Support,
}PrvHRD_PrintICELevel;

static const ES_CHAR* kPrvHRD_FilmICELevel                   = "ESFilmICELevel";
typedef enum: uint32_t
{
	kPrvHRD_FilmICELevel_NotSupport = 0,
	kPrvHRD_FilmICELevel_Support,
}PrvHRD_FilmICELevel;

static const ES_CHAR* kPrvHRD_TPU2Bit                        = "ESTPU2Bit";
typedef enum: uint32_t
{
	kPrvHRD_TPU2Bit_NotSupport = 0,
	kPrvHRD_TPU2Bit_AFL,
	kPrvHRD_TPU2Bit_8x10,
}PrvHRD_TPU2Bit;

static const ES_CHAR* kPrvHRD_DigitalName                    = "ESDigitalName";

static const ES_CHAR* kPrvHRD_DraftMode                      = "ESDraftMode";
typedef enum: uint32_t
{
	kPrvHRD_DraftMode_NotSupport = 0,
	kPrvHRD_DraftMode_Support,
}PrvHRD_DraftMode;

static const ES_CHAR* kPrvHRD_InputSizeLimit                 = "ESInputSizeLimit";

static const ES_CHAR* kPrvHRD_Gamma                          = "ESGamma";

static const ES_CHAR* kPrvHRD_FringeCorrectionName           = "ESFringeCorrectionName";

static const ES_CHAR* kPrvHRD_DefaultScanMode                = "ESDefaultScanMode";
typedef enum: uint32_t
{
	kPrvHRD_DefaultScanMode_Nomal = 0,
	kPrvHRD_DefaultScanMode_Draft,
}PrvHRD_DefaultScanMode;

static const ES_CHAR* kPrvHRD_EncryptPDF                     = "ESEncryptPDF";
typedef enum: uint32_t
{
	kPrvHRD_EncryptPDF_NotSupport = 0,
	kPrvHRD_EncryptPDF_Support,
}PrvHRD_EncryptPDF;

static const ES_CHAR* kPrvHRD_DDEADF                         = "ESDDEADF";

static const ES_CHAR* kPrvHRD_DDEFlatbed                     = "ESDDEFlatbed";

static const ES_CHAR* kPrvHRD_ADFMessage                     = "ESADFMessage";
typedef enum: uint32_t
{
	kPrvHRD_ADFMessage_NotSupport = 0,
	kPrvHRD_ADFMessage_Support,
}PrvHRD_ADFMessage;

static const ES_CHAR* kPrvHRD_ADFWidthMin                    = "ESADFWidthMin";

static const ES_CHAR* kPrvHRD_ADFHeightMin                   = "ESADFHeightMin";

static const ES_CHAR* kPrvHRD_ADFHeightMax                   = "ESADFHeightMax";

static const ES_CHAR* kPrvHRD_DuplexADFHeightMin             = "ESDuplexADFHeightMin";

static const ES_CHAR* kPrvHRD_DuplexADFHeightMax             = "ESDuplexADFHeightMax";

static const ES_CHAR* kPrvHRD_MBCName                        = "ESMBCName";

static const ES_CHAR* kPrvHRD_RollerCount                    = "ESRollerCount";
typedef enum: uint32_t
{
	kPrvHRD_RollerCount_NotSupport = 0,
	kPrvHRD_RollerCount_Support,
}PrvHRD_RollerCount;

static const ES_CHAR* kPrvHRD_RollerUsefulCount              = "ESRollerUsefulCount";

static const ES_CHAR* kPrvHRD_SeparationRollerUsefulCount    = "ESSeparationRollerUsefulCount";

static const ES_CHAR* kPrvHRD_SeparationPadCount             = "ESSeparationPadCount";
typedef enum: uint32_t
{
	kPrvHRD_SeparationPadCount_NotSupport = 0,
	kPrvHRD_SeparationPadCount_Support,
}PrvHRD_SeparationPadCount;

static const ES_CHAR* kPrvHRD_SeparationPadUsefulCount       = "ESSeparationPadUsefulCount";

static const ES_CHAR* kPrvHRD_CleaningAlertLimitCount        = "ESCleaningAlertLimitCount";

static const ES_CHAR* kPrvHRD_Interrupt                      = "ESInterrupt";
typedef enum: uint32_t
{
	kPrvHRD_Interrupt_NotSupport = 0,
	kPrvHRD_Interrupt_Support,
}PrvHRD_Interrupt;

static const ES_CHAR* kPrvHRD_LampSwitch                     = "ESLampSwitch";
typedef enum: uint32_t
{
	kPrvHRD_LampSwitch_NotSupport = 0,
	kPrvHRD_LampSwitch_Support,
}PrvHRD_LampSwitch;

static const ES_CHAR* kPrvHRD_OccupantCommand                = "ESOccupantCommand";
typedef enum: uint32_t
{
	kPrvHRD_OccupantCommand_NotSupport = 0,
	kPrvHRD_OccupantCommand_Support,
	kPrvHRD_OccupantCommand_AccessControl,
}PrvHRD_OccupantCommand;

static const ES_CHAR* kPrvHRD_ADFCard                        = "ESADFCard";
typedef enum: uint32_t
{
	kPrvHRD_ADFCard_NotSupport = 0,
	kPrvHRD_ADFCard_Support,
	kPrvHRD_ADFCard_Portrait,
	kPrvHRD_ADFCard_Landscape,
}PrvHRD_ADFCard;

static const ES_CHAR* kPrvHRD_BindingMargin                  = "ESBindingMargin";
typedef enum: uint32_t
{
	kPrvHRD_BindingMargin_NotSupport = 0,
	kPrvHRD_BindingMargin_Support,
}PrvHRD_BindingMargin;

static const ES_CHAR* kPrvHRD_PowerOffTimer                  = "ESPowerOffTimer";
typedef enum: uint32_t
{
	kPrvHRD_PowerOffTimer_NotSupport = 0,
	kPrvHRD_PowerOffTimer_Support,
    kPrvHRD_PowerOffTimer_Battery_Support,
}PrvHRD_PowerOffTimer;

static const ES_CHAR* kPrvHRD_SleepTime                      = "ESSleepTime";
typedef enum: uint32_t
{
	kPrvHRD_SleepTime_NotSupport = 0,
	kPrvHRD_SleepTime_Support,
}PrvHRD_SleepTime;

static const ES_CHAR* kPrvHRD_NetworkEnergySave              = "ESNetworkEnergySave";
typedef enum: uint32_t
{
    kPrvHRD_NetworkEnergySave_NotSupport = 0,
    kPrvHRD_NetworkEnergySave_Support,
}PrvHRD_NetworkEnergySave;

static const ES_CHAR* kPrvHRD_CarrierSheet                   = "ESCarrierSheet";
typedef enum: uint32_t
{
	kPrvHRD_CarrierSheet_NotSupport = 0,
	kPrvHRD_CarrierSheet_Support,
	kPrvHRD_CarrierSheet_Auto,										
}PrvHRD_CarrierSheet;

static const ES_CHAR* kPrvHRD_Lever                          = "ESLever";
typedef enum: uint32_t
{
	kPrvHRD_Lever_NotSupport = 0,
	kPrvHRD_Lever_Support,
}PrvHRD_Lever;

static const ES_CHAR* kPrvHRD_SWAutoSize                     = "ESSWAutoSize";
typedef enum: uint32_t
{
	kPrvHRD_SWAutoSize_NotSupport = 0,
	kPrvHRD_SWAutoSize_Support,
}PrvHRD_SWAutoSize;

static const ES_CHAR* kPrvHRD_SWAutoSizeFlatbed                     = "ESSWAutoSizeFlatbed";
typedef enum: uint32_t
{
	kPrvHRD_SWAutoSizeFlatbed_NotSupport = 0,
	kPrvHRD_SWAutoSizeFlatbed_Support_Legacy,
	kPrvHRD_SWAutoSizeFlatbed_Support_Advanced
}PrvHRD_SWAutoSizeFlatbed;

static const ES_CHAR* kPrvHRD_ReduceBirightness = "ESReduceBrightness";



static const ES_CHAR* kPrvHRD_IPv6                           = "ESIPv6";
typedef enum: uint32_t
{
	kPrvHRD_IPv6_NotSupport = 0,
	kPrvHRD_IPv6_Support,
}PrvHRD_IPv6;

static const ES_CHAR* kPrvHRD_ReduceBlurColor                = "ESReduceBlurColor";
typedef enum: uint32_t
{
	kPrvHRD_ReduceBlurColor_NotSupport = 0,
	kPrvHRD_ReduceBlurColor_Support,
}PrvHRD_ReduceBlurColor;

static const ES_CHAR* kPrvHRD_I2BSZ                          = "ESI2BSZ";

static const ES_CHAR* kPrvHRD_CancelCheck                    = "ESCancelCheck";
typedef enum: uint32_t
{
	kPrvHRD_CancelCheck_Unnecessary = 0,
	kPrvHRD_CancelCheck_Necessary,
}PrvHRD_CancelCheck;

static const ES_CHAR* kPrvHRD_ImageFilterName                = "ESImageFilterName";

static const ES_CHAR* kPrvHRD_DDCName                        = "ESDDCName";

static const ES_CHAR* kPrvHRD_DDEName                        = "ESDDEName";

static const ES_CHAR* kPrvHRD_FITName                        = "ESFITName";

static const ES_CHAR* kPrvHRD_DTRName                        = "ESDTRName";

static const ES_CHAR* kPrvHRD_PDFLibName                     = "ESPDFLibName";

static const ES_CHAR* kPrvHRD_PDFExtentionName               = "ESPDFExtentionName";

static const ES_CHAR* kPrvHRD_OCRName                        = "ESOCRName";

static const ES_CHAR* kPrvHRD_IconFileName                   = "ESIconFileName";

static const ES_CHAR* kPrvHRD_DeviceID                       = "ESDeviceID";

static const ES_CHAR* kPrvHRD_TWAINID                        = "ESTWAINID";

static const ES_CHAR* kPrvHRD_EdgeFill                       = "ESEdgeFill";
typedef enum: uint32_t
{
	kPrvHRD_EdgeFill_None = 0,
	kPrvHRD_EdgeFill_White,
	kPrvHRD_EdgeFill_Black,
}PrvHRD_EdgeFill;

static const ES_CHAR* kPrvHRD_EdgeFillLevelTop               = "ESEdgeFillLevelTop";

static const ES_CHAR* kPrvHRD_EdgeFillLevelBottom            = "ESEdgeFillLevelBottom";

static const ES_CHAR* kPrvHRD_EdgeFillLevelLeft              = "ESEdgeFillLevelLeft";

static const ES_CHAR* kPrvHRD_EdgeFillLevelRight             = "ESEdgeFillLevelRight";

static const ES_CHAR* kPrvHRD_CarrierSheetMarginX            = "ESCarrierSheetMarginX";

static const ES_CHAR* kPrvHRD_CarrierSheetMarginY            = "ESCarrierSheetMarginY";

static const ES_CHAR* kPrvHRD_CarrierSheetWidth              = "ESCarrierSheetWidth";

static const ES_CHAR* kPrvHRD_CarrierSheetHeight             = "ESCarrierSheetHeight";

static const ES_CHAR* kPrvHRD_RemovePunchHole                = "ESRemovePunchHole";
typedef enum: uint32_t
{
	kPrvHRD_RemovePunchHole_NotSupport = 0,
	kPrvHRD_RemovePunchHole_Support,
	kPrvHRD_RemovePunchHole_Range,
}PrvHRD_RemovePunchHole;

static const ES_CHAR* kPrvHRD_Cleaning                       = "ESCleaning";
typedef enum: uint32_t
{
	kPrvHRD_Cleaning_NotSupport = 0,
	kPrvHRD_Cleaning_Support,
}PrvHRD_Cleaning;

static const ES_CHAR* kPrvHRD_Calibration                    = "ESCalibration";
typedef enum: uint32_t
{
	kPrvHRD_Calibration_NotSupport = 0,
	kPrvHRD_Calibration_Support,
}PrvHRD_Calibration;

static const ES_CHAR* kPrvHRD_UtilAdjustment                 = "ESUtilAdjustment";
typedef enum: uint32_t
{
	kPrvHRD_UtilAdjustment_NotSupport = 0,
	kPrvHRD_UtilAdjustment_Support,
}PrvHRD_UtilAdjustment;

static const ES_CHAR* kPrvHRD_PushScanEventDestination       = "ESPushScanEventDestination";

static const ES_CHAR* kPrvHRD_VerticalBanding                = "ESVerticalBanding";
typedef enum: uint32_t
{
    kPrvHRD_VerticalBanding_NotSupport = 0,
    kPrvHRD_VerticalBanding_Support,
}PrvHRD_VerticalBanding;

static const ES_CHAR* kPrvHRD_DoubleLens                     = "ESDoubleLens";
typedef enum: uint32_t
{
    kPrvHRD_DoubleLens_NotSupport = 0,
    kPrvHRD_DoubleLens_Support,
}PrvHRD_DoubleLens;

static const ES_CHAR* kPrvHRD_Focus                          = "ESFocus";
typedef enum: uint32_t
{
    kPrvHRD_Focus_NotSupport = 0,
    kPrvHRD_Focus_Support,
}PrvHRD_Focus;

static const ES_CHAR* kPrvHRD_QuietModeType                  = "ESQuietModeType";
typedef enum: uint32_t
{
    kPrvHRD_QuietModeType_NotSupport = 0,
    kPrvHRD_QuietModeType_Support,
    kPrvHRD_QuietModeType_Support_Printer,
}PrvHRD_QuietModeType;

static const ES_CHAR* kPrvHRD_DefaultQuietMode               = "ESDefaultQuietMode";

static const ES_CHAR* kPrvHRD_DefaultDetectDoubleFeed        = "ESDefaultDetectDoubleFeed";

static const ES_CHAR* kPrvHRD_AutoDetectLongPaper            = "ESAutoDetectLongPaper";
typedef enum: uint32_t
{
    kPrvHRD_AutoDetectLongPaper_NotSupport = 0,
    kPrvHRD_AutoDetectLongPaper_Support,
}PrvHRD_AutoDetectLongPaper;

static const ES_CHAR* kPrvHRD_StandardPreset                 = "ESStandardPreset";

static const ES_CHAR* kPrvHRD_DisplayKSize                   = "ESDisplayKSize";
typedef enum: uint32_t
{
    kPrvHRD_DisplayKSize_NotSupport = 0,
    kPrvHRD_DisplayKSize_Support,
}PrvHRD_DisplayKSize;

static const ES_CHAR* kPrvHRD_DisplayFolioSize				 = "ESDisplayFolioSize";
typedef enum : uint32_t
{
	kPrvHRD_DisplayFolioSize_NotSupport = 0,
	kPrvHRD_DisplayFolioSize_Support,
}PrvHRD_DisplayFolioSize;

static const ES_CHAR* kPrvHRD_LandscapeDirection			 = "ESLandscapeDirection";
typedef enum : uint32_t
{
	kPrvHRD_LandscapeDirection_270 = 0,
	kPrvHRD_LandscapeDirection_90,
}PrvHRD_LandscapeDirection;


static const ES_CHAR* kPrvHRD_UtilRestrictFunction			 = "ESUtilRestrictFunction";
typedef enum : uint32_t
{
	kPrvHRD_UtilRestrictFunction_NotSupport = 0,
	kPrvHRD_UtilRestrictFunction_Support,
}PrvHRD_UtilRestrictFunction;

static const ES_CHAR* kPrvHRD_WhitePageSkipColorType =  "ESWhitePageSkipColorType";
typedef enum : uint32_t 
{
	kPrvHRD_WhitePageSkipColorType_Gray = 0,
	kPrvHRD_WhitePageSkipColorType_Color
}PrvHRD_WhitePageSkipColorType;


static const ES_CHAR* kPrvHRD_ESShowGlassDirtyDetectLevel  =  "ESShowGlassDirtyDetectLevel";

static const ES_CHAR* kPrvHRD_AutoColorGrayMono = "ESAutoColorGrayMono";
typedef enum : uint32_t
{
	kPrvHRD_AutoColorGrayMono_NotSupport = 0,
	kPrvHRD_AutoColorGrayMono_Support
}PrvHRD_AutoColorGrayMono;

static const ES_CHAR* kPrvHRD_ClearNotSupport              = "ESImageClearNotSupport";
typedef enum : uint32_t
{
	kPrvHRD_ClearNotSupport_NotSupport = 0,
	kPrvHRD_ClearNotSupport_Support,
}PrvHRD_ClearNotSupport;

static const ES_CHAR* kPrvHRD_TextEnhancementNotSupport              = "ESTextEnhancementNotSupport";
typedef enum : uint32_t
{
	kPrvHRD_TextEnhancementNotSupport_Support = 0,
	kPrvHRD_TextEnhancementNotSupport_NotSupport,
}PrvHRD_TextEnhancementNotSupport;

static const ES_CHAR* const kPrvHRD_ErrorMessageForPassport = "ESErrorMessageForPassport";
typedef enum : uint32_t
{
    kPrvHRD_ErrorMessageForPassport_NotSupport = 0,
    kPrvHRD_ErrorMessageForPassport_Support,
}PrvHRD_ErrorMessageForPassport;


//---------------------------------------------------------
// Hardware Info  Sub Key
//---------------------------------------------------------
static const ES_CHAR* kHRD_SubKey_Width                      = "Width";
static const ES_CHAR* kHRD_SubKey_Height                     = "Height";
static const ES_CHAR* kHRD_SubKey_Main                       = "Main";
static const ES_CHAR* kHRD_SubKey_Sub                        = "Sub";
static const ES_CHAR* kHRD_SubKey_Office                     = "Office";
static const ES_CHAR* kHRD_SubKey_FilmICE                    = "FilmICE";
static const ES_CHAR* kHRD_SubKey_Min                        = "Min";
static const ES_CHAR* kHRD_SubKey_Max                        = "Max";
static const ES_CHAR* kHRD_SubKey_Moire                      = "Moire";
static const ES_CHAR* kHRD_SubKey_PrintICE                   = "PrintICE";
static const ES_CHAR* kHRD_SubKey_Reflection                 = "Reflection";
static const ES_CHAR* kHRD_SubKey_Transparency               = "Transparency";

//---------------------------------------------------------
// 2 in 1
//---------------------------------------------------------
static const ES_CHAR* kPrvHRD_CommandTypeFlatbedOption 	       = "ESCommandTypeFlatbedOption";      /* @í2p R}h^Cv */
static const ES_CHAR* kPrvHRD_I2BSZFlatbedOption               = "ESI2BSZFlatbedOption";            /* @í2p BSZÌl */
static const ES_CHAR* kPrvHRD_TWAINIDFlatbedOption             = "ESTWAINIDFlatbedOption";          /* @í2p STI@í */
static const ES_CHAR* kPrvHRD_FlatbedOption                    = "ESFlatbedOption";                 /* @í2p 2in1 T|[g L³ */


//---------------------------------------------------------
// Parameter Table Key
//---------------------------------------------------------
static const ES_CHAR* kTable_Key_ProfileTable                = "ESProfileTable";
static const ES_CHAR* kTable_Key_GrayBalanceGammaTable       = "ESGrayBalanceGammaTable";
static const ES_CHAR* kTable_Key_USMTable                    = "ESUSMTable";
static const ES_CHAR* kTable_Key_ClipConstTable              = "ESClipConstTable";
static const ES_CHAR* kTable_Key_ClipCorrectTable            = "ESClipCorrectTable";
static const ES_CHAR* kTable_Key_ClipReviseTable             = "ESClipReviseTable";
static const ES_CHAR* kTable_Key_MoireTable                  = "ESMoireTable";
static const ES_CHAR* kTable_Key_ReductionTable              = "ESReductionTable";
static const ES_CHAR* kTable_Key_DustRemovalTable            = "ESDustRemovalTable";
static const ES_CHAR* kTable_Key_ResolutionTable             = "ESResolutionTable";
static const ES_CHAR* kTable_Key_MaxResolutionTable          = "ESMaxResolutionTable";
static const ES_CHAR* kTable_Key_FocusTable                  = "ESFocusTable";
static const ES_CHAR* kTable_Key_SharpTable                  = "ESSharpTable";
static const ES_CHAR* kTable_Key_USMNoiseTable               = "ESUSMNoiseTable";
static const ES_CHAR* kTable_Key_DualUSMNoiseTable           = "ESDualUSMNoiseTable";
static const ES_CHAR* kTable_Key_USMBlurTable                = "ESUSMBlurTable";
static const ES_CHAR* kTable_Key_DualUSMBlurTable            = "ESDualUSMBlurTable";
static const ES_CHAR* kTable_Key_WaitTable                   = "ESWaitTable";
static const ES_CHAR* kTable_Key_DraftResTable               = "ESDraftResTable";
static const ES_CHAR* kTable_Key_DICEDraftResTable           = "ESDICEDraftResTable";
static const ES_CHAR* kTable_Key_PreviewBWtable              = "ESPreviewBWTable";
static const ES_CHAR* kTable_Key_DefaultLUTTable             = "ESDefaultLUTTable";
static const ES_CHAR* kTable_Key_FilterSequenceTable         = "ESFilterSequenceTable";

//---------------------------------------------------------
// Parameter Table Dictionary Key
//---------------------------------------------------------
static const ES_CHAR* kTable_Info_KeyClipConst               = "ClipConst";
static const ES_CHAR* kTable_Info_KeyClipCorrect             = "ClipCorrect";
static const ES_CHAR* kTable_Info_KeyClipRevise              = "ClipRevise";
static const ES_CHAR* kTable_Info_KeyDefaultLUT              = "DefaultLUT";
static const ES_CHAR* kTable_Info_KeyDICEDraftRes            = "DICEDraftRes";
static const ES_CHAR* kTable_Info_KeyDICERes                 = "DICERes";
static const ES_CHAR* kTable_Info_KeyDraftRes                = "DraftRes";
static const ES_CHAR* kTable_Info_KeyDualUSM                 = "DualUSM";
static const ES_CHAR* kTable_Info_KeyDualUSMBlur             = "DualUSMBlur";
static const ES_CHAR* kTable_Info_KeyDualUSMNoise            = "DualUSMNoise";
static const ES_CHAR* kTable_Info_KeyDustRemoval             = "DustRemoval";
static const ES_CHAR* kTable_Info_KeyFocus                   = "Focus";
static const ES_CHAR* kTable_Info_KeyGrayBalanceGamma        = "GrayBalanceGamma";
static const ES_CHAR* kTable_Info_KeyMaxResolution           = "MaxResolution";
static const ES_CHAR* kTable_Info_KeyMoire                   = "Moire";
static const ES_CHAR* kTable_Info_KeyPreviewBW               = "PreviewBW";
static const ES_CHAR* kTable_Info_KeyProfile                 = "Profile";
static const ES_CHAR* kTable_Info_KeyReduction               = "Reduction";
static const ES_CHAR* kTable_Info_KeyResolution              = "Resolution";
static const ES_CHAR* kTable_Info_KeySharp                   = "Sharp";
static const ES_CHAR* kTable_Info_KeyUSMBlur                 = "USMBlur";
static const ES_CHAR* kTable_Info_KeyUSMNoise                = "USMNoise";
static const ES_CHAR* kTable_Info_KeyUSMOperation            = "USMOperation";
static const ES_CHAR* kTable_Info_KeyFilterSequence          = "FilterSequence";


//---------------------------------------------------------
// Convert Table
//---------------------------------------------------------

static const ES_CHAR* kConvertTableKey_ParamTable            = "ParamTable";
static const ES_CHAR* kConvertTableKey_DoubleSideRotate      = "DoubleSideRotate";

static const ES_CHAR* kConvertTableKey_IN                    = "ConditionParam";
static const ES_CHAR* kConvertTableKry_OUT                   = "SetParam";


static const ES_CHAR* kParamTable_Condition_DocumentType     = "DocumentType";
typedef enum: uint32_t
{
    kParamTable_Condition_DocumentType_Reflective = 0,
    kParamTable_Condition_DocumentType_Negative   = 1,
    kParamTable_Condition_DocumentType_Postive    = 2
}ParamTable_Condition_DocumentType;

static const ES_CHAR* kParamTable_Condition_FunctionalUnit   = "FunctionalUnit";
typedef enum: uint32_t
{
    kParamTable_Condition_FunctionalUnit_Flatbled = 0,
    kParamTable_Condition_FunctionalUnit_ADF      = 1,
    kParamTable_Condition_FunctionalUnit_TPU      = 2
}ParamTable_Condition_FunctionalUnit;

static const ES_CHAR* kParamTable_Condition_ImageType        = "ImageType";
typedef enum: uint32_t
{
    kParamTable_Condition_ImageType_Color         = 0,
    kParamTable_Condition_ImageType_Gray          = 1,
    kParamTable_Condition_ImageType_Mono          = 2,
}ParamTable_Condition_ImageType;

static const ES_CHAR* kParamTable_Condition_Direction        = "Direction";
typedef enum: uint32_t
{
    kParamTable_Condition_Direction_Main = 0,
    kParamTable_Condition_Direction_Sub  = 1,
}ParamTable_Condition_Direction;

static const ES_CHAR* kParamTable_Condition_Descreening      = "Descreening";
typedef enum: uint32_t
{
    kParamTable_Condition_Descreening_Off       = 0,
	kParamTable_Condition_Descreening_Low       = 1, 
	kParamTable_Condition_Descreening_Middle    = 2, 
	kParamTable_Condition_Descreening_High      = 3,
}ParamTable_Condition_Descreening;

static const ES_CHAR* kParamTable_Condition_SpeedPriority    = "SpeedPriority";
typedef enum: uint32_t
{
    kParamTable_Condition_SpeedPriority_Normal  = 0,
    kParamTable_Condition_SpeedPriority_High    = 1,
}ParamTable_Condition_SpeedPriority;

static const ES_CHAR* kParamTable_Condition_FilterSequenceType = "FilterSequenceType";
typedef enum: uint32_t
{
    kParamTable_Condition_FilterSequenceType_MainScan          = 0,
    kParamTable_Condition_FilterSequenceType_LivePreviewAdjust = 1,
    kParamTable_Condition_FilterSequenceType_FileFormatConvert = 2,
    kParamTable_Condition_FilterSequenceType_DoubleImageAdjust = 3,
    kParamTable_Condition_FilterSequenceType_RawScan           = 4,
}ParamTable_Condition_FilterSequenceType;

static const ES_CHAR* kParamTable_Condition_GrainReduction   = "GrainReduction";
typedef enum: uint32_t
{
    kParamTable_Condition_GrainReduction_Low    = 1,
    kParamTable_Condition_GrainReduction_Medium = 2,
    kParamTable_Condition_GrainReduction_High   = 3,
}ParamTable_Condition_GrainReduction;



static const ES_CHAR* kPrvHRD_ImageDefectCorrection = "ESImageDefectCorrection";
typedef enum : uint8_t 
{
	kPrvHRD_ImageDefectCorrection_NotSupport = 0,
	kPrvHRD_ImageDefectCorrection_Support
} PrvHRD_ImageDefectCorrection;

static const ES_CHAR* kPrvHRD_OCRPrecision = "ESOCRPrecision";
typedef enum : uint8_t
{
	kPrvHRD_OCRPrecision_NotSupport = 0,
	kPrvHRD_OCRPrecision_Support
}PrvHRD_OCRPrecision;


class ModelInfo {

public:	
	explicit ModelInfo(const SDIDeviceInfo& devInfo); 
	virtual ~ModelInfo();

	template<typename T>
	bool GetValue(const ES_CHAR* key, T& value)
	{
		try {
			if (hardwareInfo_.count(key) != 0)
			{
				value = boost::any_cast<T>(hardwareInfo_[key]);
				return true;
			}
		}catch (...) {
			// ignore exception
			return false;
		}

		return false;
	}

	bool GetPlugInsDirPath(ESString& value) {
		value = EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "PlugIns";
		return true;
	}

	bool GetDtiPath(ESString& value) {

		std::string modelIDLower = modelID_;
		std::transform(modelIDLower.begin(), modelIDLower.end(), modelIDLower.begin(), ::toupper);
		value = EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "Models/"+ modelID_ + "/"+ modelIDLower+".dti";
		return true;
	}

	bool GetTablesPath(ESString& value) {
         value = EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "Tables";
		 return true;
	}

	

	ESNumber GetModelID()
	{
		ESNumber nModelID = 0;
		ESString strModelID;
		GetValue(kPrvHRD_ModelID,strModelID);
		if(!strModelID.empty()){
			nModelID = (UInt16)strtol(strModelID.c_str(), NULL, 16);
		}
		return nModelID;
	}

	void SetBackGroundColorFront(ESDictionary bgColorFront)
	{
		bgColorFront_ = bgColorFront;
	}

	ESDictionary GetBackGroundColorFront()
	{
		return bgColorFront_;
	}

	void SetBackGroundColorBack(ESDictionary bgColorBack)
	{
		bgColorBack_ = bgColorBack;
	}

	ESDictionary GetBackGroundColorBack()
	{
		return bgColorBack_;
	}


	bool GetConvertTable(ESDictionary& dict); 

    /*
	 *  Get ESxxx ID from productid or productName  
	 */	
	static bool GetModelIDFromProductID(int32_t productID, std::string& outModelID);
	static bool GetModelIDFromProductName(const std::string& productName, std::string& outModelID);
	static void GetSha1Hash(const std::string& inValue, std::string& outValue);

	private:
		static bool GetModelIDFromProductID(int32_t productID, std::string hrdDir, std::string& outModelID);
		static void GetModelIDFromProductName(const std::string& productName, std::string hrdDir, std::string& outModelID); 
		static bool GetModelInfoDirPath(const SDIDeviceInfo& devInfo, std::string& outPath);
	
		ESDictionary hardwareInfo_;
		std::string modelID_;

		ESDictionary bgColorFront_;
		ESDictionary bgColorBack_;
};
} // namespace epsonscan
		
