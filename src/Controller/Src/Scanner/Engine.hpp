#pragma once
#include "EpsonScan2.h"
#include "Image.hpp"
#include "Utility.hpp"
#include "Error.hpp"
#include <ES2Command/IESScanner.h>
#include <ESResultString.h>
#include <ESJsonUtils.h>

namespace epsonscan {

	enum EngineEventType
	{
		kEngineEventTypeDataReceive,
		kEngineEventTypeScanEnd,
		kEngineEventTypeDisconnect,
		kEngineEventTypePushScan,
		kEngineEventTypeCancel,
		kEngineEventTypeStartContinuousScanInAFM,
		kEngineEventTypeStopContinuousScanInAFM
	};

	template <typename T>
	static bool SafeBoostAnyCast_Local(const ESAny &anyIn, T &outputValue)
	{
		try
		{
			outputValue = boost::any_cast<T>(anyIn);
			return true;
		}
		catch (...)
		{
		}
		return false;
	}

	// T型でデータが取れた場合はtrueを返す。castに失敗した場合やそもそも値が取れなかった場合はfalseを返す
	template <typename T>
	static bool GetValueForKeyFromDictionary(ESDictionary &dict, const char *key, T &value)
	{
		if (dict.find(key) != dict.end())
		{
			return SafeBoostAnyCast_Local(dict[key], value);
		}
		return false;
	}

	static SDIError ExchangeError(ESErrorCode eErrorCode)
	{

		SDI_TRACE_LOG("eErrorCode = %d", eErrorCode);

		switch(eErrorCode){

        case kESErrorNoError:
            return kSDIErrorNone;
            break;

        case kESErrorNoPermission:
            return kSDIErrorNoPermission;
            break;

        case kESErrorFatalError:
            return kSDIErrorDeviceFatalError;
            break;

        case kESErrorMemoryError:
            return kSDIErrorNoMemory;
            break;

        case kESErrorFileReadError:
            return kSDIErrorUnknownError;
            break;

        case kESErrorFileWriteError:
            return kSDIErrorWriteFile;
            break;

        case kESErrorSequenceError:
            return kSDIErrorUnknownError;
            break;

        case kESErrorScanAreaTooLargeError:
            return kSDIErrorScanAreaTooLarge;

        case kESErrorDataReceiveFailure:
            return kSDIErrorDataReceive;
            break;

        case kESErrorDataSendFailure:
            return kSDIErrorDataSend;
            break;

        case kESErrorInvalidResponse:
            return kSDIErrorInvalidResponse;
            break;

        case kESErrorDeviceInUse:
            return kSDIErrorDeviceInUse;
            break;

        case kESErrorDeviceInBusy:
            return kSDIErrorDeviceInBusy;
            break;

        case kESErrorPaperEmpty:
            return kSDIErrorPaperEmpty;
            break;

        case kESErrorPaperDoubleFeed:
            return kSDIErrorPaperDoubleFeed;
            break;

		case kESErrorPaperDoubleFeed2:
			return kSDIErrorPaperDoubleFeed2;
            break;

        case kESErrorPaperJam:
            return kSDIErrorPaperJam;
            break;

        case kESErrorCoverOpen:
            return kSDIErrorCoverOpen;
            break;

        case kESErrorTrayClose:
            return kSDIErrorTrayClose;
            break;

        case kESErrorLampError:
            return kSDIErrorLamp;
            break;

        case kESErrorAuthFailure:
            return kSDIErrorAuthFailure;
            break;

        case kESErrorLowBattery:
            return kSDIErrorLowBattery;
            break;

        case kESErrorDeviceOpenError:
            return kSDIErrorDeviceOpen;
            break;

        case kESErrorFocusError:
            return kSDIErrorUnknownError;
            break;

		case kESErrorDeviceFatalError:
			return kSDIErrorDeviceFatalError;
			break;

		case kESErrorCRSheetSensorError: 
			return kSDIErrorCRSheetSensorError;
			break;

		case kESErrorPaperProtect:
			return kSDIErrorPaperProtect;
			break;

		case kESErrorSeparationLeverError:
			return kSDIErrorSepLeverChangedInterruptedly;
			break;

		case kESErrorPoseError: 
		 	return kSDIErrorDeviceFormChangedInterruptedly;
			break;

		case kESErrorETSensorError: 
			return kSDIErrorETSensorError;
			break;

		case kESErrorUserAuthEnabled:
			return kSDIErrorUserAuthEnabled;
			break;

		case kESErrorPaperRemoveError:
			return kSDIErrorPaperRemoveError;
			break;
		
		case kESErrorSkewDetectError:
			return kSDIErrorSkewDetectError;
			break;

		case kESErrorPESensorError:
			return kSDIErrorPESensorError;
			break;

		default:
            return kSDIErrorUnknownError;
            break;

	}

		return kSDIErrorNone ;
	}
		

	typedef std::function<void(EngineEventType eventType, Image*, SDIError error)> EngineCallBack;
	class Engine : public IESScannerDelegate {
		public:
			explicit Engine(ESCommandType comType); 
			virtual ~Engine();

			bool InitWithDeviceInfoDict(const char *deviceInfo); 
	
		 	SDIError Open();	
			SDIError Close();
			SDIError ScanInBackground(); 
			SDIError Cancel();
			SDIError StartJobInMode(ESJobMode eJobMode);
			SDIError StopJobInMode(ESJobMode eJobMode);

			SDIError RequestSavingPendingImages();

			template <class T>
			bool	GetValueForKey(const char *key, T &value){
	
				if (engine_) {
				CESResultString		str ;
				call_GetValueForKey(key, str);
				if (str.Get()) {
					ESDictionary dict;
					ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(str.Get()), dict);
					if (dict.find(key) != dict.end()) {
						if (dict[key].type() != typeid(nullptr) && !dict[key].empty()) {
							return GetValueForKeyFromDictionary(dict, key, value);
						}
					}
				}
				}
				return false;
			}

			template <class T>
			bool GetAvailableValueForKey(const char *key, T &value)
			{
				bool bRetval = false;
				if (key && engine_)
				{
					CESResultString str;
					call_GetAvailableValuesForKey(key, str);
					if (str.Get())
					{
						ESDictionary dict;
						ES_CMN_FUNCS::JSON::JSONtoDictionary(str.Get(), dict);

						if (dict.find(key) != dict.end())
						{
							if (dict[key].type() != typeid(nullptr) && !dict[key].empty())
							{
								bRetval = GetValueForKeyFromDictionary(dict, key, value);
							}
						}
					}
				}
				return bRetval;
			}

			bool GetAvailableValueForKeyDict(const char *key, ESDictionary &value)
			{
				bool bRetval = false;
				if (key && engine_)
				{
					CESResultString str;
					call_GetAvailableValuesForKey(key, str);
					if (str.Get())
					{
						ES_CMN_FUNCS::JSON::JSONtoDictionary(str.Get(), value);
						bRetval  = true;
					}
				}
				return bRetval;
			}
			template <class T>
			bool GetAvailableValueForKey(const char *key, T &minValue, T &maxValue)
			{
				bool bRetval = false;
				{
					if (key && engine_)
					{
						CESResultString str;
						call_GetAvailableValuesForKey(key, str);
						if (str.Get())
						{
							ESDictionary dict;
							ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(str.Get()), dict);

							if (dict.find(key) != dict.end())
							{
								if (dict[key].type() == typeid(nullptr) || dict[key].empty())
								{
									return false;
								}

								if (typeid(T) == typeid(ESNumber))
								{
									if (dict[key].type() == typeid(ESIndexSet))
									{
										ESIndexSet tmp;
										if (SafeBoostAnyCast_Local(dict[key], tmp) && tmp.size())
										{
											minValue = *(std::min_element(tmp.begin(), tmp.end()));
											maxValue = *(std::max_element(tmp.begin(), tmp.end()));
											bRetval = true;
										}
									}
									else if (dict[key].type() == typeid(ESIndexArray))
									{
										ESIndexArray tmp;
										if (SafeBoostAnyCast_Local(dict[key], tmp) && tmp.size())
										{
											minValue = *(std::min_element(tmp.begin(), tmp.end()));
											maxValue = *(std::max_element(tmp.begin(), tmp.end()));
											bRetval = true;
										}
									}
									else if (dict[key].type() == typeid(ST_ES_RANGE))
									{
										ST_ES_RANGE tmp;
										if (SafeBoostAnyCast_Local(dict[key], tmp))
										{
											minValue = tmp.nMin;
											maxValue = tmp.nMax;
											bRetval = true;
										}
									}
								}
								else if (typeid(T) == typeid(ESFloat))
								{
									if (dict[key].type() == typeid(ESFloatArray))
									{
										ESFloatArray tmp;
										if (SafeBoostAnyCast_Local(dict[key], tmp) && tmp.size())
										{
											minValue = *(std::min_element(tmp.begin(), tmp.end()));
											maxValue = *(std::max_element(tmp.begin(), tmp.end()));
											bRetval = true;
										}
									}
									else if (dict[key].type() == typeid(ST_ES_RANGE_F))
									{
										ST_ES_RANGE_F tmp;
										if (SafeBoostAnyCast_Local(dict[key], tmp))
										{
											minValue = tmp.fMin;
											maxValue = tmp.fMax;
											bRetval = true;
										}
									}
								}

								if (!bRetval)
								{
									ES_REQUIRE(false, "unable to getvalue");
								}
							}
						}
					}
				}
				return bRetval;
			}

			void  SetValueForKey(const ESString& strKey, const ESAny& anyValue);
			void  SetValuesForKeysWithDictionary(const ESDictionary& dictionary); 

			void SetCallBackReceiver(EngineCallBack callBack) {
				callBack_ = callBack;	
			}

			void Reset()
			{
				if (engine_)
				{
					engine_->Reset();
				}
			}

		   SDIError UnlockAdministratorLock()
		   {
			   return ExchangeError(engine_->UnlockAdministratorLock());
		   }

		   SDIError LockAdministratorLock() 
		   {
			   return ExchangeError(engine_->LockAdministratorLock());
		   }

		private:
			EngineCallBack callBack_;

			void LoadSDModule(ESCommandType comType);
			void call_GetValueForKey(const char *key, CESResultString &str); 
			void call_GetAvailableValuesForKey(const char *key, CESResultString &str);
			void* module_;
			IESScanner* engine_;
			bool isAfmEnabled_;

		void CALLBACK ScannerWillBeginContinuousScanning(IESScanner* pScanner) override;
		void CALLBACK ScannerDidEndContinuousScanning(IESScanner* pScanenr) override;
		void CALLBACK ScannerWillScanToScannedImage( IESScanner* pScanner, IESScannedImage* pESImage ) override;
		void CALLBACK ScannerDidScanToScannedImage( IESScanner* pScanner, IESScannedImage* pESImage ) override;
		void CALLBACK ScannerWillCancelScanning( IESScanner* pScanner ) override;
		void CALLBACK ScannerDidCancelScanning( IESScanner* pScanner) override;
		void CALLBACK ScannerDidCompleteScanningWithError( IESScanner* pScanner, ESErrorCode err) override;
		void CALLBACK ScannerDidInterruptScanningWithError(IESScanner* pcScanner ,ESErrorCode err) override;
		void CALLBACK ScannerDidEncounterDeviceCommunicationError( IESScanner* pScanner, ESErrorCode err ) override;
		void CALLBACK ScannerWillWarmUp( IESScanner* pScanner ) override;
		void CALLBACK ScannerDidWarmUp( IESScanner* pScanner ) override;
		void CALLBACK NetworkScannerDidRequestStartScanning( IESScanner* pScanner ) override;
		void CALLBACK NetworkScannerDidRequestStopScanning( IESScanner* pScanner ) override;
		void CALLBACK ScannerDidDisconnect( IESScanner* pScanner ) ;
		void CALLBACK NetworkScannerDidReceiveServerError(IESScanner* pScanner) ;
		BOOL CALLBACK NetworkScannerShouldPreventTimeout( IESScanner* pScanner ) ;
		void CALLBACK NetworkScannerDidTimeout(  IESScanner* pScanner ) ;
		void CALLBACK ScannerIsReservedByHost( IESScanner* pScanner, const ES_CHAR* pszAddress ) ;
		void CALLBACK ScannerDidPressButton( UInt8 un8ButtonNumber ) ;
		void CALLBACK ScannerDidRequestStop(IESScanner *pScanner);
		void CALLBACK ScannerDidRequestPushScanConnection(IESScanner *pScanner);
		void CALLBACK ScannerDidNotifyStatusChange(IESScanner *pScanner) ;
	};

} // namespace epsonscan
