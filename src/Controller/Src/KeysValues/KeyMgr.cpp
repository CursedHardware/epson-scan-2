#include "KeyMgr.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include "FunctionalUnit.hpp"
#include "Resolution.hpp"
#include "ColorType.hpp"
#include "ImageFormat.hpp"
#include "ScanAreaOffsetX.hpp"
#include "ScanAreaOffsetY.hpp"
#include "ScanAreaWidth.hpp"
#include "ScanAreaHeight.hpp"
#include "ScanWidthMax.hpp"
#include "ScanHeightMax.hpp"
#include "ScanHeightMaxInLongPaper.hpp"
#include "Dropout.hpp"
#include "Orientation.hpp"
#include "MultiTiff.hpp"
#include "TiffCompression.hpp"
#include "Brightness.hpp"
#include "Contrast.hpp"
#include "Threshold.hpp"
#include "JpegQuality.hpp"
#include "DuplexType.hpp"
#include "PaperLoaded.hpp"
#include "PagesTobeScanned.hpp"
#include "DoubleFeedDetection.hpp"
#include "DoubleFeedDetectionLevel.hpp"
#include "DoubleFeedDetectionAreaMin.hpp"
#include "DoubleFeedDetectionAreaLength.hpp"
#include "SkipBlankPage.hpp"
#include "PaperDeskew.hpp"
#include "BackgroundRemoval.hpp"
#include "TransferCompression.hpp"
#include "AutoSize.hpp"
#include "Gamma.hpp"
#include "Key.hpp"
#include "PassThroughInt.hpp"
#include "SkipBlankPageOnOff.hpp"
#include "ADFMinSize.hpp"
#include "JpegProgressive.hpp"
#include "PaperEndDetection.hpp"
#include "Is2in1Mode.hpp"
#include "PoweroffTimer.hpp"
#include "PoweroffTimerBattery.hpp"
#include "SleepTimer.hpp"
#include "UnscannableScanParameter.hpp"
#include "UnscannableScanParameterForAFMC.hpp"
#include "ErrorStatus.hpp"
#include "BackgroundColor.hpp"
#include "ModelInfoPassThrough.hpp"
#include "RollerCounter.hpp"
#include "ScanHeightMaxInLongPaperResolutionTable.hpp"
#include "PassThroughString.hpp"
#include "NoiseReductionLevel.hpp"
#include "PaperCreaseReduction.hpp"
#include "TextEnhance.hpp"
#include "TextEnhanceSensitivity.hpp"
#include "ThresholdAdjustment.hpp"
#include "AutoColorPixelLevel.hpp"
#include "AutoColorPixelType.hpp"
#include "OFDSuppoted.hpp"
#include "RemovePunchHole.hpp"

#include "FilmAreaGuide.hpp"
#include "DocumentType.hpp" 
#include "FocusPosition.hpp"
#include "DeficiencyCorrection.hpp"

#include "AutoCropInscribed.hpp"

#include <utils/PathUtils.h>


namespace epsonscan {

	KeyMgr::KeyMgr(std::shared_ptr<ModelInfo> modelInfo, std::shared_ptr<Scanner> scanner) : modelInfo_(modelInfo), scanner_(scanner)
	{
		keyInstaces_.push_back(std::make_shared<FunctionalUnit>(this));
	    keyInstaces_.push_back(std::make_shared<ColorType>(this));
		keyInstaces_.push_back(std::make_shared<Resolution>(this));	
		keyInstaces_.push_back(std::make_shared<ImageFormat>(this));
		keyInstaces_.push_back(std::make_shared<ScanAreaOffsetX>(this));
		keyInstaces_.push_back(std::make_shared<ScanAreaOffsetY>(this));
		keyInstaces_.push_back(std::make_shared<ScanAreaWidth>(this));
		keyInstaces_.push_back(std::make_shared<ScanAreaHeight>(this));
		keyInstaces_.push_back(std::make_shared<ScanWidthMax>(this));		
		keyInstaces_.push_back(std::make_shared<ScanHeightMax>(this));
		keyInstaces_.push_back(std::make_shared<ScanHeightMaxInLongPaper>(this));
		keyInstaces_.push_back(std::make_shared<Dropout>(this));
		keyInstaces_.push_back(std::make_shared<Orientation>(this));
		keyInstaces_.push_back(std::make_shared<MultiTiff>(this));
		keyInstaces_.push_back(std::make_shared<TiffCompression>(this));
		keyInstaces_.push_back(std::make_shared<Contrast>(this));
		keyInstaces_.push_back(std::make_shared<Brightness>(this));
		keyInstaces_.push_back(std::make_shared<Threshold>(this));
		keyInstaces_.push_back(std::make_shared<JpegQuality>(this));
		keyInstaces_.push_back(std::make_shared<DuplexType>(this));
		keyInstaces_.push_back(std::make_shared<PaperLoaded>(this));
		keyInstaces_.push_back(std::make_shared<PagesTobeScanned>(this));
		keyInstaces_.push_back(std::make_shared<DoubleFeedDetectionLevel>(this));
		keyInstaces_.push_back(std::make_shared<DoubleFeedDetection>(this));
		keyInstaces_.push_back(std::make_shared<SkipBlankPage>(this));
		keyInstaces_.push_back(std::make_shared<PaperDeskew>(this));
		keyInstaces_.push_back(std::make_shared<BackgroundRemoval>(this));
		keyInstaces_.push_back(std::make_shared<TransferCompression>(this));
		keyInstaces_.push_back(std::make_shared<AutoSize>(this));
		keyInstaces_.push_back(std::make_shared<Gamma>(this));
		keyInstaces_.push_back(std::make_shared<SkipBlankPageOnOff>(this));
		keyInstaces_.push_back(std::make_shared<JpegProgressive>(this));
		keyInstaces_.push_back(std::make_shared<PaperEndDetection>(this));
		keyInstaces_.push_back(std::make_shared<Is2in1Mode>(this)); 
		keyInstaces_.push_back(std::make_shared<PoweroffTimer>(this));
		keyInstaces_.push_back(std::make_shared<PoweroffTimerBattery>(this));
		keyInstaces_.push_back(std::make_shared<SleepTimer>(this));
		keyInstaces_.push_back(std::make_shared<DoubleFeedDetectionAreaMin>(this));
		keyInstaces_.push_back(std::make_shared<DoubleFeedDetectionAreaLength>(this));
		keyInstaces_.push_back(std::make_shared<UnscannableScanParameter>(this));
		keyInstaces_.push_back(std::make_shared<UnscannableScanParameterForAFMC>(this));
		keyInstaces_.push_back(std::make_shared<ErrorStatus>(this));
		keyInstaces_.push_back(std::make_shared<BackgroundColor>(this));
		keyInstaces_.push_back(std::make_shared<FilmAreaGuide>(this));
		keyInstaces_.push_back(std::make_shared<DocumentType>(this));
		keyInstaces_.push_back(std::make_shared<ScanHeightMaxInLongPaperResolutionTable>(this));
		keyInstaces_.push_back(std::make_shared<ThresholdAdjustment>(this));
		keyInstaces_.push_back(std::make_shared<TextEnhanceSensitivity>(this));
		keyInstaces_.push_back(std::make_shared<TextEnhance>(this));
		keyInstaces_.push_back(std::make_shared<NoiseReductionLevel>(this));
		keyInstaces_.push_back(std::make_shared<PaperCreaseReduction>(this));
		keyInstaces_.push_back(std::make_shared<OFDSuppoted>(this));
		keyInstaces_.push_back(std::make_shared<RemovePunchHole>(this));
		keyInstaces_.push_back(std::make_shared<FocusPosition>(this));

		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIScanCounterKey, kESScanCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIADFScanCounterKey, kESSimplexScanCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIADFDuplexCounterKey, kESDuplexScanCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIADFCarrierSheetCounterKey, kESSpecialDocumentCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDICardScanCounterKey, kESSimplexCardScanCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDICardScanCounterKey, kESSimplexCardScanCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIPaperJamCounterKey, kESPaperJamCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIDoubleFeedCounterKey, kESUltraSonicDoubleFeedCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<RollerCounter<>>(this, kSDIRollerCounterKey, kESPickupRollerCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRollerLifeLimitKey, kESPickupRollerLifeLimit);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRollerNearEndKey, kESPickupRollerNearend);
			keyInstaces_.push_back(key);
		}
		
		{
			auto key = std::make_shared<RollerCounter<>>(this, kSDIRollerKitCounterKey, kESRollerKitCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRollerKitLifeLimitKey, kESRollerKitLifeLimit);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRollerKitNearEndKey, kESRollerKitNearend);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<RollerCounter<>>(this, kSDIRetardRollerCounterKey, kESRetardRollerCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRetardRollerLifeLimitKey, kESRetardRollerLifeLimit);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRetardRollerNearEndKey, kESRetardRollerNearend);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIScannerPositionKey, kESScannerPositionStatus);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIBackgroundColorKey, kESBGColor);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIAFMTimeoutKey, kESAutoFeedingModeTimeout);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ADFMinSize>(this, kSDIADFHeightMinKey, kPrvHRD_ADFHeightMin);
			keyInstaces_.push_back(key);	
		}
		{
			auto key = std::make_shared<ADFMinSize>(this, kSDIADFDuplexHeightMinKey, kPrvHRD_DuplexADFHeightMin);
			keyInstaces_.push_back(key);	
		}
		{
			auto key = std::make_shared<ADFMinSize>(this, kSDIADFWidthMinKey, kPrvHRD_ADFWidthMin);
			keyInstaces_.push_back(key);	
		}
		{
			auto key = std::make_shared<PassThroughInt<bool>>(this, kSDIAFMModeKey, kESAutoFeedingMode);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ModelInfoPassThrough<ESNumber>>(this, kSDIRestrictFunctionKey, kPrvHRD_UtilRestrictFunction);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ModelInfoPassThrough<ESNumber>>(this, kSDIGuidePositionKey, kPrvHRD_ADFOffset);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ModelInfoPassThrough<ESNumber>>(this, kSDICleaningAlertLimitCountKey, kPrvHRD_CleaningAlertLimitCount);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ModelInfoPassThrough<ESNumber>>(this, kSDIADFCardKey, kPrvHRD_ADFCard);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIDirectPowerOnKey, kESDirectPowerOn, true);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDINonConnectPowerOffKey, kESNonConnectPowerOff, true);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIADFPaperProtectionKey, kESADFPaperProtection, true);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIBehaviorWhenDoubleFeedKey, kESBehaviorWhenDoubleFeed, true);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDICleaningWarningNotifyCountKey, kESCleaningWarningNotifyCount, false, true);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRollerKitNotifyCountKey, kESRollerKitNotifyCount,false, true);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIRetardRollerNotifyCountKey, kESRetardRollerNotifyCount);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDISeparationPadNotifyCountKey, kESSeparationPadNotifyCount);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIPickupRollerNotifyCountKey, kESPickupRollerNotifyCount);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDICleaningWarningCounterKey, kESCleaningWarningCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDICleaningWarningNotifyKey, kESCleaningWarningNotify);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDISensorGlassDirtSensitivity, kESSensorGlassDirtSensitivity, true);
			key->SetModelInfoSupportedKey(kPrvHRD_ESShowGlassDirtyDetectLevel);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIPaperProtectionCounterKey, kESPaperProtectionCounter);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIADFDuplexTypeKey, kESDuplexType);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ModelInfoPassThrough<ESNumber>>(this, KSDIScannerKindKey, kPrvHRD_Kind);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughString<std::string>>(this, kSDIFirmwareVersionKey, kESVersion);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughString<std::string>>(this, kSDISerialNumberKey, kESSerialNumber);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<bool>>(this, kSDIAdminLockKey, kESAdminLock);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIDefaultPasswordTypeKey, kESDefaultPasswordType);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<bool>>(this, kSDIAdminLockEnabledKey, kESAdminLockEnabled);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughString<std::string>>(this, kSDIAdminLockPasswordKey, kESAdminLockPassword);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<AutoColorPixelLevel>(this);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<AutoColorPixelType>(this);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<bool>>(this, kSDIContinuousAutoFeedingModeKey, kESContinuousAutoFeedingMode);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<ModelInfoPassThrough<ESNumber>>(this, kSDIADFSpecialErrorMessageForPassportKey, kPrvHRD_ErrorMessageForPassport);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<AutoCropInscribed>(this);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<DeficiencyCorrection>(this);
			keyInstaces_.push_back(key);
		}
		{
			auto key = std::make_shared<PassThroughInt<>>(this, kSDIDeviceErrorCodeKey, kESErrorCode);
			keyInstaces_.push_back(key);
		}

		//
		// create all keys
		//
		for (auto&& keyInst : keyInstaces_) {
			 allKeys_.push_back(keyInst.get()->GetName());		
		}
	}

	void KeyMgr::SetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size)
	{
		std::shared_ptr<IKey> keyInstance = GetKeyInstance(key);
		if (keyInstance) {
			keyInstance.get()->SetValue(valType, value, size);
		}
	}

	void KeyMgr::GetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size)
	{
		std::shared_ptr<IKey> keyInstance = GetKeyInstance(key);
		if (keyInstance) {
			keyInstance.get()->GetValue(valType, value, size);
		}
	}

	void KeyMgr::GetDefault(const std::string& key, SDIValueType valType, void* value, SDIInt size)
	{
		std::shared_ptr<IKey> keyInstance = GetKeyInstance(key);
		if (keyInstance) {
		//	keyInstance.get()->GetDefault(valType, value, size);
		}
	}


	void KeyMgr::Reset()
	{
		for(auto&& key : allKeys_) {
			std::shared_ptr<IKey> keyInstance = GetKeyInstance(key);
			if (keyInstance) {
				keyInstance.get()->Reset();
			}
		}
	}

	void KeyMgr::GetCapability(const std::string& key, SDICapability &capability)
	{
		std::shared_ptr<IKey> keyInstance = GetKeyInstance(key);
		if (keyInstance) {
			keyInstance.get()->GetCapability(capability);
		}
	}

	ESDictionary KeyMgr::GetEngineKeyValues()
	{
		ESDictionary currentValues; // for logging

		commandKeyValueCache_.clear();

		for(auto key : allKeys_) {
			std::shared_ptr<IKey> keyInstance = GetKeyInstance(key);
			if (keyInstance) {
			   ESDictionary dict = keyInstance.get()->GetEngineKeyValues();
			   for (auto dictElem : dict)
			   {
				   auto value = dict[dictElem.first];
				   commandKeyValueCache_[dictElem.first] = value;
			   }

			   // for logging
			   if (keyInstance->GetType() == kSDIValueTypeInt)
			   {
				   SDIInt current = 0;
				   keyInstance->GetValue(kSDIValueTypeInt, &current, sizeof(SDIInt));
				   currentValues[keyInstance->GetName()] = (ESNumber)current;
			   }
			}
		}

		ES_Dump_Json(currentValues, "ControllerValue");


		UpdateOtherEngineSetting(commandKeyValueCache_);
		ResolveEngineSettingInconsistents(commandKeyValueCache_);

		return commandKeyValueCache_;
	}

	void KeyMgr::UpdateOtherEngineSetting(ESDictionary& params)
	{
		params[kESInterruptionEnabled] = (bool)false;
		
		{// workdir
			params[kESWorkFolder] = (ESString)ES_CMN_FUNCS::PATH::ES_GetWorkTempPath();
			if(ES_CMN_FUNCS::PATH::ES_IsExistFolder(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath()) == FALSE)
			{
				ES_CMN_FUNCS::PATH::ES_MakeFolder(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath());
			}
		}
		{// bufufer size

			ESNumber bufferSize = 0;
			modelInfo_->GetValue(kPrvHRD_I2BSZ, bufferSize);
			if (bufferSize > 0)
			{
				params[kPrvHRD_I2BSZ] = bufferSize;
			}
		}
		{// Authentification
			params[kESAuthenticationEnabled] = (bool)false;
		}
	}

	void KeyMgr::ResolveEngineSettingInconsistents(ESDictionary& params)
	{	
		ESNumber colorFormat = 0;
		if(SafeBoostAnyCast_Local(params[kESColorFormat], colorFormat))
		{
			if((ESColorFormat)colorFormat != kESColorFormatMono1)
			{
				params.erase(kESThreshold);
			}
		}
	}

	SDIInt KeyMgr::GetValueInt(std::string key)
	{
		SDIInt intVal = 0;
		GetValue(key, kSDIValueTypeInt, &intVal, sizeof(intVal));
		return intVal;
	}

	void   KeyMgr::SetValueInt(std::string key, SDIInt intVal)
	{
		SetValue(key, kSDIValueTypeInt, &intVal, sizeof(intVal));
	}


	const std::vector<std::string>& KeyMgr::GetAllKeys() const 
	{
		return allKeys_;	
	}
	
	std::shared_ptr<IKey> KeyMgr::GetKeyInstance(const std::string& key)
	{
		for(auto&& keyInst : keyInstaces_) {
			if (keyInst.get()->GetName() == key) {
				return keyInst;	
			}
		}

		return nullptr;
	}


	std::shared_ptr<ModelInfo> KeyMgr::GetModelInfo() {
		return modelInfo_;
	}

	std::shared_ptr<Scanner> KeyMgr::GetScanner() {
		return scanner_;
	}

}// namespace epsonscan
