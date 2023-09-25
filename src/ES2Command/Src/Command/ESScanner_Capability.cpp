////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScanner_Capability.cpp
//!
//! @brif     スキャナーコマンド用ベースクラス - Capabilityの実装
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ESScanner.h"
#include "ESScannerDefs.h"

const bool BOOLEAN_ARRAY_DATA[2] ={ true, false };
const ESIndexSet BOOLEAN_ARRAY( BOOLEAN_ARRAY_DATA, BOOLEAN_ARRAY_DATA+2);


//===============================================================
//!
//! すべての設定可能なキーを取得する
//!
//===============================================================
const ESStringArray& CESScanner::GetAllKeys()
{
	const ES_CHAR_CPTR KeyList[] = {
		kESVersion,					// r    ESString
		kESWorkFolder,				// rw   ESString
		kESProductName,				// r    ESString
		kESErrorStatus,				// r    ESNumber (ESErrorCode)
		kESAuthentication,			// r    bool
		kESAuthenticationEnabled,	// rw   bool
		kESAuthUsername,			// rw   ESString
		kESAuthPassword,			// rw   ESString
		kESFunctionalUnitType,		// rw   ESNumber (ESFunctionalUnitType)
		kESXResolution,				// rw   ESNumber
		kESYResolution,				// rw   ESNumber
		kESOpticalResolution,		// r    ESNumber
		kESMaxScanSize,				// r    ST_ES_SIZE_F
		kESScanArea,				// rw   ST_ES_RECT_F
		kESScanAreaInPixel,			// rw   ST_ES_RECT_UN32
		kESScanSize,				// rw   ST_ES_SIZE_F
		kESScanSizeInPixel,			// rw   ST_ES_SIZE_UN32
		kESXOffsetMargin,			// rw   ESFloat
		kESYOffsetMargin,			// rw   ESFloat
		kESColorFormat,				// rw   ESNumber (ESColorFormat)
		kESColorMatrix,				// rw   ESFloatArray(3 x 3 Floats) or nullptr(get only)
		kESBitsPerPixel,			// r    ESNumber
		kESGammaMode,				// rw   ESNumber (ESGammaMode)
		kESGammaTableMono,			// rw   ESIndexArray(256 array) or nullptr(get only)
		kESGammaTableRed,			// rw   ESIndexArray(256 array) or nullptr(get only)
		kESGammaTableGreen,			// rw   ESIndexArray(256 array) or nullptr(get only)
		kESGammaTableBlue,			// rw   ESIndexArray(256 array) or nullptr(get only)
		kESThreshold,				// rw   ESNumber
		kESQuietMode,				// rw   ESNumber (ESQuietMode)
		kESBufferSize,				// rw   ESNumber
		kESDuplexType,				// r    ESNumber (ESDuplexType)
		kESDuplex,					// rw   bool
		kESDoubleFeedDetection,		// rw   ESNumber (ESDoubleFeedDetection)
		kESPagesToBeScanned,		// rw   ESNumber
		kESDocumentLoaded,			// r    bool
		kESCleaning,				// r    bool
		kESCalibration,				// r    bool
		NULL
	};
	if ( m_arAllKeys.empty() ){
		m_arAllKeys.insert( m_arAllKeys.end(), KeyList, KeyList+_countof(KeyList)-1 );
	}
	return m_arAllKeys;
}

//===============================================================
//!
//! 指定したキーの能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )を取得します。
//!
//===============================================================
ESErrorCode CESScanner::GetCapabilityForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult )
{
	if ( pszKey == NULL || pszKey[0] == L'\0' ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	if ( m_funcCapability.empty() ){
		m_funcCapability[ kESVersion               ] = FN_CAPA(&CESScanner::GetVersionCapability               );
		m_funcCapability[ kESWorkFolder            ] = FN_CAPA(&CESScanner::GetWorkFolderCapability            );
		m_funcCapability[ kESErrorStatus           ] = FN_CAPA(&CESScanner::GetErrorStatusCapability           );
		m_funcCapability[ kESAuthentication        ] = FN_CAPA(&CESScanner::GetAuthenticationCapability        );
		m_funcCapability[ kESAuthenticationEnabled ] = FN_CAPA(&CESScanner::GetAuthenticationEnabledCapability );
		m_funcCapability[ kESAuthUsername          ] = FN_CAPA(&CESScanner::GetAuthUserNameCapability          );
		m_funcCapability[ kESAuthPassword          ] = FN_CAPA(&CESScanner::GetAuthPasswordCapability          );
		m_funcCapability[ kESProductName           ] = FN_CAPA(&CESScanner::GetProductNameCapability           );
		m_funcCapability[ kESFunctionalUnitType    ] = FN_CAPA(&CESScanner::GetFunctionalUnitTypeCapability    );
		m_funcCapability[ kESXResolution           ] = FN_CAPA(&CESScanner::GetXResolutionCapability           );
		m_funcCapability[ kESYResolution           ] = FN_CAPA(&CESScanner::GetYResolutionCapability           );
		m_funcCapability[ kESOpticalResolution     ] = FN_CAPA(&CESScanner::GetOpticalResolutionCapability     );
		m_funcCapability[ kESMaxScanSize           ] = FN_CAPA(&CESScanner::GetMaxScanSizeCapability           );
		m_funcCapability[ kESScanArea              ] = FN_CAPA(&CESScanner::GetScanAreaCapability              );
		m_funcCapability[ kESScanAreaInPixel       ] = FN_CAPA(&CESScanner::GetScanAreaInPixelCapability       );
		m_funcCapability[ kESScanSize              ] = FN_CAPA(&CESScanner::GetScanSizeCapability              );
		m_funcCapability[ kESScanSizeInPixel       ] = FN_CAPA(&CESScanner::GetScanSizeInPixelCapability       );
		m_funcCapability[ kESXOffsetMargin         ] = FN_CAPA(&CESScanner::GetXOffsetMarginCapability         );
		m_funcCapability[ kESYOffsetMargin         ] = FN_CAPA(&CESScanner::GetYOffsetMarginCapability         );
		m_funcCapability[ kESColorFormat           ] = FN_CAPA(&CESScanner::GetColorFormatCapability           );
		m_funcCapability[ kESColorMatrix           ] = FN_CAPA(&CESScanner::GetColorMatrixCapability           );
		m_funcCapability[ kESBitsPerPixel          ] = FN_CAPA(&CESScanner::GetBitsPerPixelCapability          );
		m_funcCapability[ kESGammaMode             ] = FN_CAPA(&CESScanner::GetGammaModeCapability             );
		m_funcCapability[ kESGammaTableMono        ] = FN_CAPA(&CESScanner::GetGammaTableCapability            );
		m_funcCapability[ kESGammaTableRed         ] = FN_CAPA(&CESScanner::GetGammaTableCapability            );
		m_funcCapability[ kESGammaTableGreen       ] = FN_CAPA(&CESScanner::GetGammaTableCapability            );
		m_funcCapability[ kESGammaTableBlue        ] = FN_CAPA(&CESScanner::GetGammaTableCapability            );
		m_funcCapability[ kESThreshold             ] = FN_CAPA(&CESScanner::GetThresholdCapability             );
		m_funcCapability[ kESQuietMode             ] = FN_CAPA(&CESScanner::GetQuietModeCapability             );
		m_funcCapability[ kESBufferSize            ] = FN_CAPA(&CESScanner::GetBufferSizeCapability            );
		m_funcCapability[ kESDuplexType            ] = FN_CAPA(&CESScanner::GetDuplexTypeCapability            );
		m_funcCapability[ kESDuplex                ] = FN_CAPA(&CESScanner::GetDuplexCapability                );
		m_funcCapability[ kESDoubleFeedDetection   ] = FN_CAPA(&CESScanner::GetDoubleFeedDetectionCapability   );
		m_funcCapability[ kESPagesToBeScanned      ] = FN_CAPA(&CESScanner::GetPagesToBeScannedCapability      );
		m_funcCapability[ kESDocumentLoaded        ] = FN_CAPA(&CESScanner::GetDocumentLoadedCapability        );
		m_funcCapability[ kESCleaning              ] = FN_CAPA(&CESScanner::GetCleaningCapability              );
		m_funcCapability[ kESCalibration           ] = FN_CAPA(&CESScanner::GetCalibrationCapability           );
	}
	if ( m_funcCapability.count( pszKey ) != 0 ){
		(m_funcCapability[ pszKey ])( this, dicResult );
		return kESErrorNoError;
	}
	return kESErrorFatalError;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//

void CESScanner::GetWorkFolderCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]       = nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ] = nullptr;
}

void CESScanner::GetErrorStatusCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = nullptr;
}

void CESScanner::GetAuthenticationCapability( ESDictionary& dicResult )
{
	if ( IsAuthenticationSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESScanner::GetAuthenticationEnabledCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
}

void CESScanner::GetAuthUserNameCapability( ESDictionary& dicResult )
{
	if ( IsAuthenticationSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]        = nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
	}
}

void CESScanner::GetAuthPasswordCapability( ESDictionary& dicResult )
{
	if ( IsAuthenticationSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
	}
}

void CESScanner::GetProductNameCapability( ESDictionary& dicResult )
{
	if ( !GetProductName().empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = GetProductName();
	}
}

void CESScanner::GetFunctionalUnitTypeCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedFunctionalUnitTypes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESFunctionalUnitFlatbed;
	}
}

void CESScanner::GetXResolutionCapability( ESDictionary& dicResult )
{
//	ESIndexSet values = GetSupportedXResolutions();
	ESAny values = GetSupportedXResolutions();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)300;
	}
}

void CESScanner::GetYResolutionCapability( ESDictionary& dicResult )
{
//	ESIndexSet values = GetSupportedYResolutions();
	ESAny values = GetSupportedYResolutions();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)300;
	}
}

void CESScanner::GetOpticalResolutionCapability( ESDictionary& dicResult )
{
	ESNumber value = GetOpticalResolution();
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= value;
}

void CESScanner::GetMaxScanSizeCapability( ESDictionary& dicResult )
{
	ST_ES_SIZE_F value = GetMaxScanSize();
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= value;
}

void CESScanner::GetScanAreaCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetScanAreaInPixelCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetScanSizeCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetScanSizeInPixelCapability		( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetXOffsetMarginCapability			( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetYOffsetMarginCapability			( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetColorFormatCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedColorFormats();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESColorFormatRGB24;
	}
}

void CESScanner::GetBitsPerPixelCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
//	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}


void CESScanner::GetGammaModeCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedGammaModes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESGammaMode10;
	}
}

void CESScanner::GetGammaTableCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetThresholdCapability( ESDictionary& dicResult )
{
	ESAny anyRange = GetSupportedThreshold();
	if ( !anyRange.empty() ){

		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= anyRange;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)128;

		if ( GetBitsPerSample() == 1 ){
			dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= anyRange;
		}
	}
}

void CESScanner::GetQuietModeCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedQuietModes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESQuietModeOff;
	}
}

void CESScanner::GetBufferSizeCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetDuplexTypeCapability( ESDictionary& dicResult )
{
	if ( IsDuplexSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]	= (ESNumber)GetDuplexType();
	}
}

void CESScanner::GetDuplexCapability( ESDictionary& dicResult )
{
	if ( IsDuplexSupported() ){

		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]		= false;

		if ( IsFeederEnabled() ){
			dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ] = BOOLEAN_ARRAY;
		}
	}
}

void CESScanner::GetDoubleFeedDetectionCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedDoubleFeedDetections();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESDoubleFeedDetectionDisable;

		if ( IsFeederEnabled() ){
			dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		}
	}
}

void CESScanner::GetPagesToBeScannedCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}

void CESScanner::GetDocumentLoadedCapability( ESDictionary& dicResult )
{
	if ( IsFeederSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESScanner::GetCleaningCapability( ESDictionary& dicResult )
{
	if ( IsCleaningSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESScanner::GetCalibrationCapability( ESDictionary& dicResult )
{
	if ( IsCalibrationSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}
