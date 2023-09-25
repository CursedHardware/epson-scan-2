////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2Scanner_Capability.cpp
//!
//! @brif     ESCI/2コマンド用スキャナクラス - Capabilityの実装
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ESCI2Scanner.h"
#include "../ESScannerDefs.h"

const bool BOOLEAN_ARRAY_DATA[2] ={ true, false };
const ESIndexSet BOOLEAN_ARRAY( BOOLEAN_ARRAY_DATA, BOOLEAN_ARRAY_DATA+2);

//===============================================================
//!
//! すべての設定可能なキーを取得する
//!
//===============================================================
const ESStringArray& CESCI2Scanner::GetAllKeys()
{
	const ES_CHAR_CPTR KeyList[] = {
		kESBatteryStatus,				// r    ESNumber (ESBatteryStatus)
		kESSensorGlassStatus,
		kESSensorGlassDirtSensitivity,
		kESWarningStatus,
		kESUnavailableScanParameterStatus,
		kESInterruptionEnabled,
		kESLengthPadding,				// r    bool
		kESAutoFeedingMode,
		kESContinuousAutoFeedingMode,
		kESAutoFeedingModeTimeout,
		kESSerialNumber,				// r    ESString
		kESMaxImagePixels,				// r    ST_ES_SIZE_F
		kESMaxScanSizeInLongLength,		// r    ST_ES_SIZE_F
		kESMaxLongLengthTable,			// r
		kESMinScanSize,					// r    ST_ES_SIZE_F
		kESGuidePosition,				// r    ESNumber (ESGuidePosition)
		kESGuidelessADF,				// r	bool
		kESImageFormat,					// rw   ESNumber (ESImageFormat)
		kESColorMatrixMode,				// rw   ESNumber (ESColorMatrixMode)
		kESSkewCorrection,				// rw   bool
		kESJPEGQuality,					// rw   ESNumber
		kESPaperEndDetection,			// rw   bool
		kESOverScan,					// rw   bool,
		kESOverScanForCropping,			// rw   bool,
		kESPassportCarrierSheetScan,	// rw   bool
		kESDetectedDocumentSize,		// r    ST_ES_SIZE_F
		kESDocumentSeparation,			// r    bool
		kESScannerPositionStatus,		// r    bool
		kESCardScanning,				// r    bool
		kESAutoCropping,				// rw   bool
		kESAutoCroppingInscribed,		// rw   bool
		kESCroppableResolutions,		// r ESIndexSet or ST_ES_RANGE or nullptr
		kESCroppingSize,				// rw   ESFloat
		kESEdgeFillColor,				// rw   ESNumber (ESEdgeFillColor)
		kESEdgeFillWidthRight,			// rw   ESFloat
		kESEdgeFillWidthTop,			// rw   ESFloat
		kESEdgeFillWidthLeft,			// rw   ESFloat
		kESEdgeFillWidthBottom,			// rw   ESFloat
		kESCarrierSheetDetection,
		kESMinDoubleFeedDetectionRangeLength,
		kESDoubleFeedDetectionRangeOffset,
		kESDoubleFeedDetectionRangeLength,
		kESLenghtDoubleFeedDetection,
		kESLengthDoubleFeedDetectionLength,
		kESImageDoubleFeedDetection,
		kESImagePaperProtection,
		kESLaminatedPaperMode,
		kESDisableJobContinue,
		kESDisableKeepingCapture,
		kESScanningMode,
		kESFilmType,
		kESBGColor,
		kESClientApplication,
		kESColorCounterType,

		kESPowerSaveTime,				// rw   ESNumber (ENUM_ESCI_POWER_SAVE_TIME)
		kESPowerOffTime,				// rw   ESNumber
		kESPowerOffTime2nd,				// rw   ESNumber
		kESMaxFocus,					// r    ESFloat
		kESMinFocus,					// r    ESFloat
		kESFocus,						// rw   ESFloat
		kESErrorCode,
		kESFirstPCConnectionDate,
		kESSimplexScanCounter,			// rw   ESNumber
		kESDuplexScanCounter,			// rw   ESNumber
		kESSimplexCardScanCounter,		// rw   ESNumber
		kESDuplexCardScanCounter,		// rw   ESNumber
		kESDocumentFeederLamp1Counter,	// rw   ESNumber
		kESDocumentFeederLamp2Counter,	// rw   ESNumber
		kESPaperJamCounter,				// rw   ESNumber
		kESUltraSonicDoubleFeedCounter,	// rw   ESNumber
		kESLengthDoubleFeedCounter,		// rw   ESNumber
		kESImageDoubleFeedCounter,		// rw   ESNumber
		kESPaperProtectionCounter,		// rw   ESNumber
		kESRollerKitCounter,			// rw   ESNumber
		kESRetardRollerCounter,			// rw   ESNumber
		kESPickupRollerCounter,			// rw   ESNumber
		kESSeparationPadCounter,		// rw   ESNumber
		kESSpecialDocumentCounter,		// rw   ESNumber
		kESPassportCarrierSheetCounter,	// rw   ESNumber
		kESScanCounter,					// rw   ESNumber
		kESFlatbedLamp1Counter,			// rw   ESNumber
		kESFlatbedLamp2Counter,			// rw   ESNumber
		kESDocumentTopCorrectionFront,	// rw   ESNumber
		kESDocumentTopCorrectionBack,	// rw   ESNumber
		kESDocumentDriveCorrectionFront,// rw   ESNumber
		kESDocumentDriveCorrectionBack,	// rw   ESNumber
		kESLightIntensityBack,			// rw   ESNumber
		kESDocumentFeederBGLevelFront,
		kESDocumentFeederBGLevelBack,
		kESPickupRollerLifeLimit,
		kESSeparationPadLifeLimit,
		kESRollerKitLifeLimit,
		kESRetardRollerLifeLimit,
		kESPickupRollerNearend,
		kESSeparationPadNearend,
		kESRollerKitNearend,
		kESRetardRollerNearend,
		kESContrast,                        // rw   ESNumber
        kESGammaScale,                      // rw   ESNumber
        kESBrightness,                      // rw   ESNumber
        kESBackgroundRemoval,
        kESSharpnessFilter,
		kESDetectColorTypeEnabled,			// rw   bool
		kESDetectBlankPageEnabled,			// rw   bool
		kESDetectBlankPageLevel,			// rw   ESNumber
		kESSkipImageEnhancement,			// rw   bool
		kESDirectPowerOn,					// rw   ESNumber
		kESNonConnectPowerOff,				// rw   ESNumber
		kESADFPaperProtection,				// rw   ESNumber
		kESBehaviorWhenDoubleFeed,			// rw   ESNumber
		kESCleaningWarningNotifyCount,		// rw   ESNumber
		kESRollerKitNotifyCount,			// rw   ESNumber
		kESRetardRollerNotifyCount,			// rw   ESNumber
		kESSeparationPadNotifyCount,		// rw   ESNumber
		kESPickupRollerNotifyCount,			// rw   ESNumber
		kESCleaningWarningCounter,			// rw   ESNumber
		kESCleaningWarningNotify,			// rw   ESNumber
		kESAdminLock,
		kESAdminLockEnabled,
		kESDefaultPasswordType,
		kESAdminLockPassword,
		NULL
	};
	if ( m_arAllKeys.empty() ){
		const ESStringArray& arParentAllKeys = CESScanner::GetAllKeys();
		m_arAllKeys.insert( m_arAllKeys.end(), arParentAllKeys.begin(), arParentAllKeys.end() );
		m_arAllKeys.insert( m_arAllKeys.end(), KeyList, KeyList+_countof(KeyList)-1 );
	}
	return m_arAllKeys;
}

//===============================================================
//!
//! 指定したキーの能力 ( 設定可能なすべての値、現在設定可能な値、デフォルト値 )を取得する
//!
//===============================================================
ESErrorCode CESCI2Scanner::GetCapabilityForKey( ES_CHAR_CPTR pszKey, ESDictionary& dicResult )
{
	if ( pszKey == NULL || pszKey[0] == L'\0' ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	if ( m_funcCapability.empty() ){
		m_funcCapability[ kESDisableJobContinue                ] = FN_CAPA(&CESCI2Scanner::GetDisableJobContinueCapability         );
		m_funcCapability[ kESDisableKeepingCapture             ] = FN_CAPA(&CESCI2Scanner::GetDisableKeepingCaptureCapability      );
		m_funcCapability[ kESBatteryStatus                     ] = FN_CAPA(&CESCI2Scanner::GetBatteryStatusCapability              );
		m_funcCapability[ kESSensorGlassStatus                 ] = FN_CAPA(&CESCI2Scanner::GetSensorGlassStatusCapability          );
		m_funcCapability[ kESScannerPositionStatus             ] = FN_CAPA(&CESCI2Scanner::GetScannerPositionStatusCapability      );
		m_funcCapability[ kESSensorGlassDirtSensitivity        ] = FN_CAPA(&CESCI2Scanner::GetSensorGlassDirtSensitivityCapability );
		m_funcCapability[ kESWarningStatus                     ] = FN_CAPA(&CESCI2Scanner::GetWarningStatusCapability              );
		m_funcCapability[ kESUnavailableScanParameterStatus    ] = FN_CAPA(&CESCI2Scanner::GetUnavailableScanParameterStatusCapability);
		m_funcCapability[ kESInterruptionEnabled               ] = FN_CAPA(&CESCI2Scanner::GetInterruptionEnabledCapability        );
		m_funcCapability[ kESLengthPadding                     ] = FN_CAPA(&CESCI2Scanner::GetLengthPaddingCapability              );
		m_funcCapability[ kESAutoFeedingMode                   ] = FN_CAPA(&CESCI2Scanner::GetAutoFeedingModeCapability            );
		m_funcCapability[ kESContinuousAutoFeedingMode         ] = FN_CAPA(&CESCI2Scanner::GetContinuousAutoFeedingModeCapability  );
		m_funcCapability[ kESAutoFeedingModeTimeout            ] = FN_CAPA(&CESCI2Scanner::GetAutoFeedingModeTimeoutCapability     );
		m_funcCapability[ kESSerialNumber                      ] = FN_CAPA(&CESCI2Scanner::GetSerialNumberCapability               );
		m_funcCapability[ kESMaxImagePixels                    ] = FN_CAPA(&CESCI2Scanner::GetMaxImagePixelsCapability             );
		m_funcCapability[ kESMaxScanSizeInLongLength           ] = FN_CAPA(&CESCI2Scanner::GetMaxScanSizeInLongLengthCapability    );
		m_funcCapability[ kESMaxLongLengthTable                ] = FN_CAPA(&CESCI2Scanner::GetMaxLongLengthTableCapability         );
		m_funcCapability[ kESMinScanSize                       ] = FN_CAPA(&CESCI2Scanner::GetMinScanSizeCapability                );
		m_funcCapability[ kESGuidePosition                     ] = FN_CAPA(&CESCI2Scanner::GetGuidePositionCapability              );
		m_funcCapability[ kESGuidelessADF                      ] = FN_CAPA(&CESCI2Scanner::GetGuidelessADFCapability               );
		m_funcCapability[ kESImageFormat                       ] = FN_CAPA(&CESCI2Scanner::GetImageFormatCapability                );
		m_funcCapability[ kESColorMatrixMode                   ] = FN_CAPA(&CESCI2Scanner::GetColorMatrixModeCapability            );
		m_funcCapability[ kESLaminatedPaperMode                ] = FN_CAPA(&CESCI2Scanner::GetLaminatedPaperModeCapability         );
		m_funcCapability[ kESSkewCorrection                    ] = FN_CAPA(&CESCI2Scanner::GetSkewCorrectionCapability             );
		m_funcCapability[ kESJPEGQuality                       ] = FN_CAPA(&CESCI2Scanner::GetJPEGQualityCapability                );
		m_funcCapability[ kESPaperEndDetection                 ] = FN_CAPA(&CESCI2Scanner::GetPaperEndDetectionCapabiliy           );
		m_funcCapability[ kESCarrierSheetDetection             ] = FN_CAPA(&CESCI2Scanner::GetCarrierSheetDetectionCapabiliy       );
		m_funcCapability[ kESOverScan                          ] = FN_CAPA(&CESCI2Scanner::GetOverScanCapability                   );
		m_funcCapability[ kESOverScanForCropping               ] = FN_CAPA(&CESCI2Scanner::GetOverScanForCroppingCapability        );
		m_funcCapability[ kESPassportCarrierSheetScan          ] = FN_CAPA(&CESCI2Scanner::GetPassportCarrierSheetCapabiliy        );
		m_funcCapability[ kESMinDoubleFeedDetectionRangeLength ] = FN_CAPA(&CESCI2Scanner::GetMinDoubleFeedDetectionRangeLengthCapability);
		m_funcCapability[ kESDoubleFeedDetectionRangeOffset    ] = FN_CAPA(&CESCI2Scanner::GetDoubleFeedDetectionRangeOffsetCapability   );
		m_funcCapability[ kESDoubleFeedDetectionRangeLength    ] = FN_CAPA(&CESCI2Scanner::GetDoubleFeedDetectionRangeLengthCapability   );
		m_funcCapability[ kESLenghtDoubleFeedDetection         ] = FN_CAPA(&CESCI2Scanner::GetLenghtDoubleFeedDetectionCapability        );
		m_funcCapability[ kESLengthDoubleFeedDetectionLength   ] = FN_CAPA(&CESCI2Scanner::GetLengthDoubleFeedDetectionLengthCapability  );
		m_funcCapability[ kESImageDoubleFeedDetection          ] = FN_CAPA(&CESCI2Scanner::GetImageDoubleFeedDetectionCapability   );
		m_funcCapability[ kESImagePaperProtection              ] = FN_CAPA(&CESCI2Scanner::GetImagePaperProtectionCapability       );
		m_funcCapability[ kESDetectedDocumentSize              ] = FN_CAPA(&CESCI2Scanner::GetDetectedDocumentSizeCapability       );
		m_funcCapability[ kESDocumentSeparation                ] = FN_CAPA(&CESCI2Scanner::GetDocumentSeparationCapability         );
		m_funcCapability[ kESCardScanning                      ] = FN_CAPA(&CESCI2Scanner::GetCardScanningCapability               );
		m_funcCapability[ kESAutoCropping                      ] = FN_CAPA(&CESCI2Scanner::GetAutoCroppingCapability               );
		m_funcCapability[ kESAutoCroppingInscribed             ] = FN_CAPA(&CESCI2Scanner::GetAutoCroppingInscribedCapability      );
		m_funcCapability[ kESCroppableResolutions              ] = FN_CAPA(&CESCI2Scanner::GetCroppableResolutionsCapability       );
		m_funcCapability[ kESCroppingSize                      ] = FN_CAPA(&CESCI2Scanner::GetCroppingSizeCapability               );
		m_funcCapability[ kESFilmType                          ] = FN_CAPA(&CESCI2Scanner::GetFilmTypeCapability                   );
		m_funcCapability[ kESEdgeFillColor                     ] = FN_CAPA(&CESCI2Scanner::GetEdgeFillColorCapability              );
		m_funcCapability[ kESEdgeFillWidthRight                ] = FN_CAPA(&CESCI2Scanner::GetEdgeFillWidthCapability              );
		m_funcCapability[ kESEdgeFillWidthTop                  ] = FN_CAPA(&CESCI2Scanner::GetEdgeFillWidthCapability              );
		m_funcCapability[ kESEdgeFillWidthLeft                 ] = FN_CAPA(&CESCI2Scanner::GetEdgeFillWidthCapability              );
		m_funcCapability[ kESEdgeFillWidthBottom               ] = FN_CAPA(&CESCI2Scanner::GetEdgeFillWidthCapability              );
		m_funcCapability[ kESPowerSaveTime                     ] = FN_CAPA(&CESCI2Scanner::GetPowerSaveTimeCapability              );
		m_funcCapability[ kESPowerOffTime                      ] = FN_CAPA(&CESCI2Scanner::GetPowerOffTimeCapability               );
		m_funcCapability[ kESPowerOffTime2nd                   ] = FN_CAPA(&CESCI2Scanner::GetPowerOffTime2ndCapability            );
		m_funcCapability[ kESMaxFocus                          ] = FN_CAPA(&CESCI2Scanner::GetMaxFocusCapability                   );
		m_funcCapability[ kESMinFocus                          ] = FN_CAPA(&CESCI2Scanner::GetMinFocusCapability                   );
		m_funcCapability[ kESFocus                             ] = FN_CAPA(&CESCI2Scanner::GetFocusCapability                      );	
		m_funcCapability[ kESFirstPCConnectionDate             ] = FN_CAPA(&CESCI2Scanner::GetFirstPCConnectionDateCapability      );
		m_funcCapability[ kESSimplexScanCounter                ] = FN_CAPA(&CESCI2Scanner::GetSimplexScanCounterCapability         );
		m_funcCapability[ kESDuplexScanCounter                 ] = FN_CAPA(&CESCI2Scanner::GetDuplexScanCounterCapability          );
		m_funcCapability[ kESDocumentFeederLamp1Counter        ] = FN_CAPA(&CESCI2Scanner::GetDocumentFeederLamp1CounterCapability );
		m_funcCapability[ kESDocumentFeederLamp2Counter        ] = FN_CAPA(&CESCI2Scanner::GetDocumentFeederLamp2CounterCapability );
		m_funcCapability[ kESPaperJamCounter                   ] = FN_CAPA(&CESCI2Scanner::GetPaperJamCounterCapability            );
		m_funcCapability[ kESUltraSonicDoubleFeedCounter       ] = FN_CAPA(&CESCI2Scanner::GetUltraSonicDoubleFeedCounterCapability);
		m_funcCapability[ kESLengthDoubleFeedCounter           ] = FN_CAPA(&CESCI2Scanner::GetLengthDoubleFeedCounterCapability    );
		m_funcCapability[ kESImageDoubleFeedCounter            ] = FN_CAPA(&CESCI2Scanner::GetImageDoubleFeedCounterCapability     );
		m_funcCapability[ kESPaperProtectionCounter            ] = FN_CAPA(&CESCI2Scanner::GetPaperProtectionCounterCapability     );
		m_funcCapability[ kESRollerKitCounter                  ] = FN_CAPA(&CESCI2Scanner::GetRollerKitCounterCapability           );
		m_funcCapability[ kESRetardRollerCounter               ] = FN_CAPA(&CESCI2Scanner::GetRetardRollerCounterCapability        );
		m_funcCapability[ kESPickupRollerCounter               ] = FN_CAPA(&CESCI2Scanner::GetPickupRollerCounterCapability        );
		m_funcCapability[ kESSeparationPadCounter              ] = FN_CAPA(&CESCI2Scanner::GetSeparationPadCounterCapability       );
		m_funcCapability[ kESSpecialDocumentCounter            ] = FN_CAPA(&CESCI2Scanner::GetSpecialDocumentCounterCapability     );
		m_funcCapability[ kESPassportCarrierSheetCounter       ] = FN_CAPA(&CESCI2Scanner::GetPassportCarrierSheetCounterCapability);
		m_funcCapability[ kESScanCounter                       ] = FN_CAPA(&CESCI2Scanner::GetScanCounterCapability                );
		m_funcCapability[ kESSimplexCardScanCounter            ] = FN_CAPA(&CESCI2Scanner::GetSimplexCardScanCounterCapability     );
		m_funcCapability[ kESDuplexCardScanCounter             ] = FN_CAPA(&CESCI2Scanner::GetDuplexCardScanCounterCapability      );
		m_funcCapability[ kESFlatbedLamp1Counter               ] = FN_CAPA(&CESCI2Scanner::GetFlatbedLamp1CounterCapability        );
		m_funcCapability[ kESFlatbedLamp2Counter               ] = FN_CAPA(&CESCI2Scanner::GetFlatbedLamp2CounterCapability        );
		m_funcCapability[ kESDocumentTopCorrectionFront        ] = FN_CAPA(&CESCI2Scanner::GetDocumentTopCorrectionFrontCapability );
		m_funcCapability[ kESDocumentTopCorrectionBack         ] = FN_CAPA(&CESCI2Scanner::GetDocumentTopCorrectionBackCapability  );
		m_funcCapability[ kESDocumentDriveCorrectionFront      ] = FN_CAPA(&CESCI2Scanner::GetDocumentDriveCorrectionFrontCapability);
		m_funcCapability[ kESDocumentDriveCorrectionBack       ] = FN_CAPA(&CESCI2Scanner::GetDocumentDriveCorrectionBackCapability);
		m_funcCapability[ kESLightIntensityBack                ] = FN_CAPA(&CESCI2Scanner::GetLightIntensityBackCapability         );
		m_funcCapability[ kESDocumentFeederBGLevelFront        ] = FN_CAPA(&CESCI2Scanner::GetDocumentFeederBGLevelFrontCapability );
		m_funcCapability[ kESDocumentFeederBGLevelBack         ] = FN_CAPA(&CESCI2Scanner::GetDocumentFeederBGLevelBackCapability  );
		m_funcCapability[ kESPickupRollerLifeLimit             ] = FN_CAPA(&CESCI2Scanner::GetPickupRollerLifeLimitCapability      );
		m_funcCapability[ kESRollerKitLifeLimit                ] = FN_CAPA(&CESCI2Scanner::GetRollerKitLifeLimitCapability         );
		m_funcCapability[ kESRetardRollerLifeLimit             ] = FN_CAPA(&CESCI2Scanner::GetRetardRollerLifeLimitCapability      );
		m_funcCapability[ kESSeparationPadLifeLimit            ] = FN_CAPA(&CESCI2Scanner::GetSeparationPadLifeLimitCapability     );
		m_funcCapability[ kESPickupRollerNearend               ] = FN_CAPA(&CESCI2Scanner::GetPickupRollerNearendCapability        );
		m_funcCapability[ kESRollerKitNearend                  ] = FN_CAPA(&CESCI2Scanner::GetRollerKitNearendCapability           );
		m_funcCapability[ kESRetardRollerNearend               ] = FN_CAPA(&CESCI2Scanner::GetRetardRollerNearendCapability        );
		m_funcCapability[ kESSeparationPadNearend              ] = FN_CAPA(&CESCI2Scanner::GetSeparationPadNearendCapability       );
		m_funcCapability[ kESScanningMode                      ] = FN_CAPA(&CESCI2Scanner::GetScanningModeCapability               );
		m_funcCapability[ kESBGColor                           ] = FN_CAPA(&CESCI2Scanner::GetBGColorCapability                    );
		m_funcCapability[ kESColorCounterType                  ] = FN_CAPA(&CESCI2Scanner::GetColorCounterTypeCapability           );

        m_funcCapability[ kESContrast                          ] = FN_CAPA(&CESCI2Scanner::GetContrastCapablity                    );
        m_funcCapability[ kESGammaScale                        ] = FN_CAPA(&CESCI2Scanner::GetGammaScaleCapablity                  );
        m_funcCapability[ kESBrightness                        ] = FN_CAPA(&CESCI2Scanner::GetBrightnessCapablity                  );
        m_funcCapability[ kESBackgroundRemoval                 ] = FN_CAPA(&CESCI2Scanner::GetBackgroundRemovalCapablity           );
        m_funcCapability[ kESSharpnessFilter                   ] = FN_CAPA(&CESCI2Scanner::GetSharpnessFilterCapablity             );
		m_funcCapability[ kESDetectColorTypeEnabled            ] = FN_CAPA(&CESCI2Scanner::GetDetectColorTypeCapablity             );
		m_funcCapability[ kESDetectBlankPageEnabled            ] = FN_CAPA(&CESCI2Scanner::GetDetectBlankPageCapablity             );
		m_funcCapability[ kESDetectBlankPageLevel              ] = FN_CAPA(&CESCI2Scanner::GetDetectBlankPageLevelCapablity        );
		m_funcCapability[ kESSkipImageEnhancement              ] = FN_CAPA(&CESCI2Scanner::GetSkipImageCapablity                   );
        m_funcCapability[ kESADFLoadSupported                   ] = FN_CAPA(&CESCI2Scanner::GetADFLoadCapablity             );

		m_funcCapability[ kESErrorCode                         ] = FN_CAPA(&CESCI2Scanner::GetErrorCodeCapability                  );

		m_funcCapability[ kESDirectPowerOn                     ] = FN_CAPA(&CESCI2Scanner::GetDirectPowerOnCapability              );
		m_funcCapability[ kESNonConnectPowerOff                ] = FN_CAPA(&CESCI2Scanner::GetNonConnectPowerOffCapability         );
		m_funcCapability[ kESADFPaperProtection                ] = FN_CAPA(&CESCI2Scanner::GetADFPaperProtectionCapability         );
		m_funcCapability[ kESBehaviorWhenDoubleFeed            ] = FN_CAPA(&CESCI2Scanner::GetBehaviorWhenDoubleFeedCapability     );
		m_funcCapability[ kESCleaningWarningNotifyCount        ] = FN_CAPA(&CESCI2Scanner::GetCleaningWarningNotifyCountCapability );
		m_funcCapability[ kESRollerKitNotifyCount              ] = FN_CAPA(&CESCI2Scanner::GetRollerKitNotifyCountCapability);
		m_funcCapability[ kESRetardRollerNotifyCount           ] = FN_CAPA(&CESCI2Scanner::GetRetardRollerNotifyCountCapability);
		m_funcCapability[ kESSeparationPadNotifyCount          ] = FN_CAPA(&CESCI2Scanner::GetSeparationPadNotifyCountCapability);
		m_funcCapability[ kESPickupRollerNotifyCount           ] = FN_CAPA(&CESCI2Scanner::GetPickupRollerNotifyCountCapability);
		m_funcCapability[ kESCleaningWarningCounter            ] = FN_CAPA(&CESCI2Scanner::GetCleaningWarningCounterCapability     );
		m_funcCapability[ kESCleaningWarningNotify             ] = FN_CAPA(&CESCI2Scanner::GetCleaningWarningNotifyCapability      );

		m_funcCapability[ kESAdminLock                         ] = FN_CAPA(&CESCI2Scanner::GetAdminLockCapability                  );
		m_funcCapability[ kESAdminLockEnabled                  ] = FN_CAPA(&CESCI2Scanner::GetAdminLockEnabledCapability           );
		m_funcCapability[ kESAdminLockPassword                 ] = FN_CAPA(&CESCI2Scanner::GetAdminLockPasswordCapability          );
		m_funcCapability[ kESDefaultPasswordType               ] = FN_CAPA(&CESCI2Scanner::GetDefaultPasswordTypeCapability        );
	}
	if ( m_funcCapability.count( pszKey ) != 0 ){
		(m_funcCapability[ pszKey ])( this, dicResult );
		return kESErrorNoError;
	}
	return CESScanner::GetCapabilityForKey( pszKey, dicResult );
}

//===============================================================
//!
//! 優先するキーを取得
//!
//===============================================================
ESStringArray CESCI2Scanner::GetPriorKeys()
{
	const ES_CHAR_CPTR KeyList[] ={
		kESScanArea,
		kESScanAreaInPixel,
		kESScanSize,
		kESScanSizeInPixel,
		kESAutoCropping,
		kESAutoCroppingInscribed,
		kESColorMatrixMode,
		kESColorMatrix,
		kESImageFormat,
		NULL
	};

	static ESStringArray s_arPriorKeys;
	if ( s_arPriorKeys.empty() ){
		s_arPriorKeys = CESScanner::GetPriorKeys();
		s_arPriorKeys.insert( s_arPriorKeys.end(), &KeyList[0], &KeyList[ _countof(KeyList)-1 ] );
	}
	return s_arPriorKeys;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void CESCI2Scanner::GetVersionCapability( ESDictionary& dicResult )
{
	if ( !GetVersion().empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = GetVersion();
	}
}
void CESCI2Scanner::GetScanSizeCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}
void CESCI2Scanner::GetColorMatrixCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
}
void CESCI2Scanner::GetDisableJobContinueCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
}
void CESCI2Scanner::GetDisableKeepingCaptureCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
}
void CESCI2Scanner::GetBatteryStatusCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
}

void CESCI2Scanner::GetSensorGlassStatusCapability(ESDictionary& dicResult)
{
	if (IsSensorGlassStatusSupported()) {
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}

void CESCI2Scanner::GetScannerPositionStatusCapability(ESDictionary& dicResult)
{
	if (IsScannerPositionStatusSupported()){
		ESIndexSet values;
		values.insert(kESScannerPositionStatusFlat);
		values.insert(kESScannerPositionStatusTilt);
		values.insert(kESScannerPositionStatusInvalid);

		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
	}
}

void CESCI2Scanner::GetSensorGlassDirtSensitivityCapability(ESDictionary& dicResult)
{
	ESIndexSet values = GetSupportedSensorGlassDirtSensitivities();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES] = values;
	}
}

void CESCI2Scanner::GetInterruptionEnabledCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
}

void CESCI2Scanner::GetWarningStatusCapability(ESDictionary& dicResult)
{
	dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
}

void CESCI2Scanner::GetLengthPaddingCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
}

void CESCI2Scanner::GetAutoFeedingModeCapability( ESDictionary& dicResult )
{
	if ( IsAutoFeedingModeSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESCI2Scanner::GetContinuousAutoFeedingModeCapability( ESDictionary& dicResult )
{
	if ( IsContinuousAutoFeedingModeSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESCI2Scanner::GetAutoFeedingModeTimeoutCapability( ESDictionary& dicResult )
{
	const int AFM_TIMEOUT_MAX = 30;  // minutes

	if ( IsAutoFeedingModeSupported() ){
		ST_ES_RANGE values;
		values.nMin  = 0;
		values.nMax  = AFM_TIMEOUT_MAX;
		values.nStep = 1;

		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]       = values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ] = values;
	}
}

void CESCI2Scanner::GetSerialNumberCapability( ESDictionary& dicResult )
{
	if ( !GetSerialNumber().empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = GetSerialNumber();
	}
}

void CESCI2Scanner::GetMaxImagePixelsCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = GetMaxImagePixels();
}
void CESCI2Scanner::GetMaxScanSizeInLongLengthCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = GetMaxScanSizeInLongLength();
}
void CESCI2Scanner::GetMaxLongLengthTableCapability(ESDictionary& dicResult)
{
	if (GetMaxLongLengthTable().size()) {
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}

void CESCI2Scanner::GetMinScanSizeCapability( ESDictionary& dicResult )
{
	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = GetMinScanSize();
}
void CESCI2Scanner::GetGuidePositionCapability( ESDictionary& dicResult )
{
	ESIndexSet values;
	values.insert ( kESGuidePositionLeft );
	values.insert ( kESGuidePositionCenter );
	values.insert ( kESGuidePositionRight );

	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
}

void CESCI2Scanner::GetGuidelessADFCapability(ESDictionary& dicResult)
{
	dicResult[ES_CAPABILITY_KEY_ALLVALUES] = BOOLEAN_ARRAY;
}

void CESCI2Scanner::GetImageFormatCapability(ESDictionary& dicResult)
{
	ESIndexSet values = GetSupportedImageFormats();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESImageFormatRaw;
	}
}
void CESCI2Scanner::GetColorMatrixModeCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedColorMatrixModes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESColorMatrixUnit;
	}
}
void CESCI2Scanner::GetLaminatedPaperModeCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedLaminatedPaperModes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESLaminatedPaperModeOff;
	}
}
void CESCI2Scanner::GetSkewCorrectionCapability( ESDictionary& dicResult )
{
	if ( IsSkewCorrectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetScanningModeCapability( ESDictionary& dicResult )
{
	ESIndexSet values;
	values.insert( kESScanningModeHighSpeed );
	values.insert( kESScanningModeNormal );

	dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
	dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESScanningModeNormal;
}

void CESCI2Scanner::GetFilmTypeCapability(ESDictionary& dicResult)
{

	ESAny values = GetSupportedFilmType();

	if (!values.empty()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}



void CESCI2Scanner::GetMaxFocusCapability(ESDictionary& dicResult)
{

	if (IsManualFocusSupported()) {
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= GetMaxFocus();
	}
}

void CESCI2Scanner::GetMinFocusCapability(ESDictionary& dicResult)
{

	if (IsManualFocusSupported()) {
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= GetMinFocus();
	}
}

void CESCI2Scanner::GetFocusCapability(ESDictionary& dicResult)
{
	if (IsManualFocusSupported()) {
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESFloat)0.0f;
	}
}



void CESCI2Scanner::GetJPEGQualityCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedJPEGQuality();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)100;

		if ( GetImageFormat() == kESImageFormatJPEG && GetBitsPerSample() != 1 ){
			dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		}
	}
}
void CESCI2Scanner::GetPaperEndDetectionCapabiliy( ESDictionary& dicResult )
{
	if ( IsPaperEndDetectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetCarrierSheetDetectionCapabiliy( ESDictionary& dicResult )
{
	if ( IsCarrierSheetDetectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
	}
}
void CESCI2Scanner::GetOverScanCapability( ESDictionary& dicResult )
{
	if ( IsOverScanSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetOverScanForCroppingCapability( ESDictionary& dicResult )
{
	if ( IsOverScanSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetPassportCarrierSheetCapabiliy(ESDictionary& dicResult)
{
	if (IsPassportCarrierSheetSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetMinDoubleFeedDetectionRangeLengthCapability( ESDictionary& dicResult )
{
	if ( IsDoubleFeedDetectionRangeSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}
void CESCI2Scanner::GetDoubleFeedDetectionRangeOffsetCapability( ESDictionary& dicResult )
{
	if ( IsDoubleFeedDetectionRangeSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
	}
}
void CESCI2Scanner::GetDoubleFeedDetectionRangeLengthCapability( ESDictionary& dicResult )
{
	if ( IsDoubleFeedDetectionRangeSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
	}
}
void CESCI2Scanner::GetLenghtDoubleFeedDetectionCapability( ESDictionary& dicResult )
{
	if ( IsLengthDoubleFeedDetectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetLengthDoubleFeedDetectionLengthCapability( ESDictionary& dicResult )
{
	if ( IsLengthDoubleFeedDetectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
	}
}
void CESCI2Scanner::GetImageDoubleFeedDetectionCapability( ESDictionary& dicResult )
{
	if ( IsImageDoubleFeedDetectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetImagePaperProtectionCapability( ESDictionary& dicResult )
{
	if ( IsImagePaperProtectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetDetectedDocumentSizeCapability( ESDictionary& dicResult )
{
	if ( IsDocumentSizeDetectionSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}

void CESCI2Scanner::GetDocumentSeparationCapability( ESDictionary& dicResult )
{
	if ( IsDocumentSeparationSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESCI2Scanner::GetUnavailableScanParameterStatusCapability(ESDictionary& dicResult)
{
	if (IsUnavailableScanParameterStatusSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}

void CESCI2Scanner::GetCardScanningCapability( ESDictionary& dicResult )
{
	if ( IsCardScanningSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ] = BOOLEAN_ARRAY;
	}
}
void CESCI2Scanner::GetAutoCroppingCapability( ESDictionary& dicResult )
{
	if ( IsAutoCroppingSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetAutoCroppingInscribedCapability( ESDictionary& dicResult )
{
	if ( IsAutoCroppingInscribedSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetCroppableResolutionsCapability( ESDictionary& dicResult )
{
	ESAny values = GetCroppableResolutions();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
	}
}
void CESCI2Scanner::GetCroppingSizeCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedCroppingSizes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESFloat)0.0f;
	}
}

void CESCI2Scanner::GetEdgeFillColorCapability( ESDictionary& dicResult )
{
	ESIndexSet values = GetSupportedEdgeFillColors();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESEdgeFillColorWhite;
	}
}
void CESCI2Scanner::GetEdgeFillWidthCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedEdgeFillWidth();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESFloat)0.0f;
	}
}
void CESCI2Scanner::GetPowerSaveTimeCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedPowerSaveTimes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)15;
	}
}
void CESCI2Scanner::GetPowerOffTimeCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedPowerOffTimes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)15;
	}
}
void CESCI2Scanner::GetPowerOffTime2ndCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedPowerOffTimes2nd();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)15;
	}
}
void CESCI2Scanner::GetFirstPCConnectionDateCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedFirstPCConnectionDates();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
	}
}
void CESCI2Scanner::GetSimplexScanCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedSimplexScanCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetDuplexScanCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDuplexScanCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetDocumentFeederLamp1CounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDocumentFeederLamp1Counters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetDocumentFeederLamp2CounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDocumentFeederLamp2Counters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetPaperJamCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedPaperJamCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetUltraSonicDoubleFeedCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedUltraSonicDoubleFeedCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetLengthDoubleFeedCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedLengthDoubleFeedCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetImageDoubleFeedCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedImageDoubleFeedCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetPaperProtectionCounterCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedPaperProtectionCounters();
	if (!values.empty()) {
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES] = values;
	}
}
void CESCI2Scanner::GetRollerKitCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedRollerKitCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetRetardRollerCounterCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedRetardRollerCounters();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES] = values;
	}
}
void CESCI2Scanner::GetPickupRollerCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedPickupRollerCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetSeparationPadCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedSeparationPadCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetSpecialDocumentCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedSpecialDocumentCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetPassportCarrierSheetCounterCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedPassportCarrierSheetCounters();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES] = values;
	}
}
void CESCI2Scanner::GetScanCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedScanCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetSimplexCardScanCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedSimplexCardScanCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetDuplexCardScanCounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDuplexCardScanCounters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetFlatbedLamp1CounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedFlatbedLamp1Counters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetFlatbedLamp2CounterCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedFlatbedLamp2Counters();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}

void CESCI2Scanner::GetDocumentTopCorrectionFrontCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDocumentTopCorrectionsFront();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}

void CESCI2Scanner::GetDocumentTopCorrectionBackCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDocumentTopCorrectionsBack();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}

void CESCI2Scanner::GetDocumentDriveCorrectionFrontCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDocumentDriveCorrectionsFront();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}

void CESCI2Scanner::GetDocumentDriveCorrectionBackCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedDocumentDriveCorrectionsBack();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}

void CESCI2Scanner::GetLightIntensityBackCapability( ESDictionary& dicResult )
{
	ESAny values = GetSupportedLightIntensitiesBack();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
	}
}
void CESCI2Scanner::GetDocumentFeederBGLevelFrontCapability( ESDictionary& dicResult )
{
	if ( IsDocumentFeederBGLevelFrontSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}
void CESCI2Scanner::GetDocumentFeederBGLevelBackCapability( ESDictionary& dicResult )
{
	if ( IsDocumentFeederBGLevelBackSupported() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}
void CESCI2Scanner::GetPickupRollerLifeLimitCapability(ESDictionary& dicResult)
{
	if (IsPickupRollerLifeLimitSupported()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}
void CESCI2Scanner::GetRollerKitLifeLimitCapability(ESDictionary& dicResult)
{
	if (IsRollerKitLifeLimitSupported()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}
void CESCI2Scanner::GetRetardRollerLifeLimitCapability(ESDictionary& dicResult)
{
	if (IsRetardRollerLifeLimitSupported()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}
void CESCI2Scanner::GetSeparationPadLifeLimitCapability(ESDictionary& dicResult)
{
	if (IsSeparationPadLifeLimitSupported()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}
void CESCI2Scanner::GetPickupRollerNearendCapability(ESDictionary& dicResult)
{
	if (IsPickupRollerNearendSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}
void CESCI2Scanner::GetRollerKitNearendCapability(ESDictionary& dicResult)
{
	if (IsRollerKitNearendSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}
void CESCI2Scanner::GetRetardRollerNearendCapability(ESDictionary& dicResult)
{
	if (IsRetardRollerNearendSupported()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}
void CESCI2Scanner::GetSeparationPadNearendCapability(ESDictionary& dicResult)
{
	if (IsSeparationPadNearendSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}
void CESCI2Scanner::GetBGColorCapability(ESDictionary& dicResult)
{
	ESIndexSet values = GetSupportedBGColors();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
		dicResult[ES_CAPABILITY_KEY_DEFAULT] = (ESNumber)kESBGColorWhite;
		if (IsFeederEnabled()) {
			dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES] = values;
		}
	}
}

void CESCI2Scanner::GetClientApplicationCapability(ESDictionary& dicResult)
{
    ESIndexSet values = GetSupportedClientApplications();
    if (!values.empty()){
        dicResult[ES_CAPABILITY_KEY_ALLVALUES] = values;
        dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES] = values;
    }
}


void CESCI2Scanner::GetBrightnessCapablity( ESDictionary& dicResult )
{
    ESAny anyRange = GetSupportedBrightness();
    if ( !anyRange.empty() ){
        dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]        = anyRange;
        dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]        = anyRange;
    }
}

void CESCI2Scanner::GetContrastCapablity( ESDictionary& dicResult )
{
    ESAny anyRange = GetSupportedContrast();
    if ( !anyRange.empty() ){
        dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]        = anyRange;
        dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]        = anyRange;
    }
}
void CESCI2Scanner::GetGammaScaleCapablity( ESDictionary& dicResult )
{
    ESAny anyRange = GetSupportedGammaScale();
    if ( !anyRange.empty() ){
        dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]        = anyRange;
        dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]        = anyRange;
    }
}

void CESCI2Scanner::GetBackgroundRemovalCapablity(ESDictionary& dicResult)
{
    ESAny values = GetSupportedBackGroundRemoval();
    if (!values.empty()){
        dicResult[ES_CAPABILITY_KEY_ALLVALUES]            = values;
        dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]    = values;
    }
}
void CESCI2Scanner::GetSharpnessFilterCapablity(ESDictionary& dicResult)
{
    ESAny values = GetSupportedSharpnessLevel();
    if (!values.empty()){
        dicResult[ES_CAPABILITY_KEY_ALLVALUES]            = values;
        dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]    = values;
    }
}

void CESCI2Scanner::GetDetectBlankPageCapablity(ESDictionary& dicResult)
{
	if (IsDetectBlankPageSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetDetectBlankPageLevelCapablity(ESDictionary& dicResult)
{
	ESAny values = GetSupportedDetectBlankPageLevel();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}
void CESCI2Scanner::GetDetectColorTypeCapablity(ESDictionary& dicResult)
{
	if (IsDetectColorTypeSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}
void CESCI2Scanner::GetSkipImageCapablity(ESDictionary& dicResult)
{
	if (IsSkipImageSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetADFLoadCapablity(ESDictionary& dicResult)
{
    bool isADFLoad = IsAdfLoadSupported();
    if (isADFLoad){
        dicResult[ES_CAPABILITY_KEY_ALLVALUES]            = nullptr;
        dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]    = nullptr;
    }
}

void CESCI2Scanner::GetDirectPowerOnCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedDirectPowerOn();
	if (!values.empty()){
		const ESIndexSet *p = SAFE_ANY_DATA_CPTR(values, ESIndexSet);
		if (p && p->size()) {
			dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
			dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
		}
	}
}

void CESCI2Scanner::GetNonConnectPowerOffCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedNonConnectPowerOff();
	if (!values.empty()){
		const ESIndexSet *p = SAFE_ANY_DATA_CPTR(values, ESIndexSet);
		if (p && p->size()) {
			dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
			dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
		}
	}
}

void CESCI2Scanner::GetADFPaperProtectionCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedADFPaperProtection();
	if (!values.empty()){
		const ESIndexSet *p = SAFE_ANY_DATA_CPTR(values, ESIndexSet);
		if (p && p->size()) {
			dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
			dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
		}
	}
}

void CESCI2Scanner::GetBehaviorWhenDoubleFeedCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedBehaviorWhenDoubleFeed();
	if (!values.empty()){
		const ESIndexSet *p = SAFE_ANY_DATA_CPTR(values, ESIndexSet);
		if (p && p->size()) {
			dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
			dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
		}
	}
}

void CESCI2Scanner::GetCleaningWarningNotifyCountCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedCleaningWarningNotifyCount();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}

void CESCI2Scanner::GetRollerKitNotifyCountCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedRollerKitNotifyCount();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}

void CESCI2Scanner::GetRetardRollerNotifyCountCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedRetardRollerNotifyCount();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}

void CESCI2Scanner::GetSeparationPadNotifyCountCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedSeparationPadNotifyCount();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}

void CESCI2Scanner::GetPickupRollerNotifyCountCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedPickupRollerNotifyCount();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}

void CESCI2Scanner::GetCleaningWarningCounterCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedCleaningWarningCounter();
	if (!values.empty()){
		dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
		dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
	}
}
void CESCI2Scanner::GetCleaningWarningNotifyCapability(ESDictionary& dicResult)
{
	ESAny values = GetSupportedCleaningWarningNotify();
	if (!values.empty()){
		const ESIndexSet *p = SAFE_ANY_DATA_CPTR(values, ESIndexSet);
		if (p && p->size()) {
			dicResult[ES_CAPABILITY_KEY_ALLVALUES]			= values;
			dicResult[ES_CAPABILITY_KEY_AVAILABLEVALUES]	= values;
		}
	}
}

void CESCI2Scanner::GetAdminLockCapability(ESDictionary& dicResult)
{
	if (IsAdminLockSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetAdminLockEnabledCapability(ESDictionary& dicResult)
{
	if (IsAdminLockSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= BOOLEAN_ARRAY;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= false;
	}
}

void CESCI2Scanner::GetDefaultPasswordTypeCapability(ESDictionary& dicResult)
{
	if (IsDefaultPasswordTypeSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
	}
}

void CESCI2Scanner::GetAdminLockPasswordCapability(ESDictionary& dicResult)
{
	if (IsAdminLockSupported()){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= nullptr;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= nullptr;
	}
}

void CESCI2Scanner::GetColorCounterTypeCapability(ESDictionary& dicResult)
{
	ESIndexSet values = GetSupportedColorCounterTypes();
	if ( !values.empty() ){
		dicResult[ ES_CAPABILITY_KEY_ALLVALUES ]		= values;
		dicResult[ ES_CAPABILITY_KEY_AVAILABLEVALUES ]	= values;
		dicResult[ ES_CAPABILITY_KEY_DEFAULT ]			= (ESNumber)kESColorCounterTypePref;
	}
}

void CESCI2Scanner::GetErrorCodeCapability( ESDictionary& dicResult )
{
	if (IsErrorCodeSupported()) {
		dicResult[ES_CAPABILITY_KEY_ALLVALUES] = nullptr;
	}
}