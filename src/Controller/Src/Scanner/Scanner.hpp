#pragma once
#include "EpsonScan2.h"
#include "Engine.hpp"
#include <memory>

namespace epsonscan {

static const uint32_t kEpsonUSBVID =  0x04b8;

static const uint32_t k2in1FlatbedOptionVID[]  = {0x13C, 0x13D, 0x13E, 0x13F, 0x00 };

class ModelInfo;

typedef std::function<void(EngineEventType eventType, Image*, SDIError error)> ScannerCallBack;
class Scanner 
{
		public:
			Scanner(const SDIDeviceInfo& devInfo, std::shared_ptr<ModelInfo> modelInfo);
			virtual ~Scanner();
	
			SDIError Open();
			SDIError Close();	
			SDIError Scan();
			SDIError Cancel();
			SDIError StartJobInMode(ESJobMode eJobMode);
			SDIError StopJobInMode(ESJobMode eJobMode);


			//template <class T>
			//bool	GetValueForKey(const char *key, T &value);
			
			template <class T>
			bool GetValueForKey(const char *key, T &value)
			{

				if (isOpened_ == false)
				{
					SDI_TRACE_LOG("scanner is disconnected");
					throw RuntimeError("scanner is disconnected", kSDIErrorDataReceive);
				}

				if (currentEngine_) {
					return currentEngine_->GetValueForKey(key, value);
				}

				return true;
			}

			template <class T>
			bool GetValueForKey(const char *key, T &value, ESFunctionalUnitType funcUnitToSet)
			{

				if (isOpened_ == false)
				{
					SDI_TRACE_LOG("scanner is disconnected");
					throw RuntimeError("scanner is disconnected", kSDIErrorDataReceive);
				}

				ESNumber funcUnitBeforeChange = kESFunctionalUnitFlatbed;
			    if (supportedFunctionalUnit_.find(funcUnitToSet) != supportedFunctionalUnit_.end())
				{
					GetValueForKey(kESFunctionalUnitType, funcUnitBeforeChange);
					SetValueForKey(kESFunctionalUnitType, (ESNumber)funcUnitToSet);
				}
				bool result = GetValueForKey(key, value);
				
				 if (supportedFunctionalUnit_.find(funcUnitToSet) != supportedFunctionalUnit_.end())
				{
					SetValueForKey(kESFunctionalUnitType, funcUnitBeforeChange);
				}

				return result;
			}


			template <class T>
			bool GetAvailableValueForKey(const char *key, T &value, ESFunctionalUnitType funcUnitToSet)
			{
				ESNumber funcUnitBeforeChange = kESFunctionalUnitFlatbed;
					
			    if (supportedFunctionalUnit_.find(funcUnitToSet) != supportedFunctionalUnit_.end())
				{
					GetValueForKey(kESFunctionalUnitType, funcUnitBeforeChange);
					SetValueForKey(kESFunctionalUnitType, (ESNumber)funcUnitToSet);
				}

				bool result = GetAvailableValueForKey(key, value);
			    if (supportedFunctionalUnit_.find(funcUnitToSet) != supportedFunctionalUnit_.end())
				{
					SetValueForKey(kESFunctionalUnitType, funcUnitBeforeChange);
				}

				return result;
			}

			template <class T>
			bool GetAvailableValueForKey(const char *key, T &value)
			{

				if (isOpened_ == false)
				{
					SDI_TRACE_LOG("scanner is disconnected");
					throw RuntimeError("scanner is disconnected", kSDIErrorDataReceive);
				}

				if (engineSecond_ &&
					std::string(key) == kESFunctionalUnitType)
				{	
					value.insert(kESFunctionalUnitFlatbed);
					value.insert(kESFunctionalUnitDocumentFeeder);
					return true;
				}

				if (currentEngine_)
				{
					return currentEngine_->GetAvailableValueForKey(key, value);
				}

				return true;
			}

			bool GetAvailableValueForKey(const char *key, ESIndexArray &value)
			{

				if (isOpened_ == false)
				{
					SDI_TRACE_LOG("scanner is disconnected");
					throw RuntimeError("scanner is disconnected", kSDIErrorDataReceive);
				}

				if (currentEngine_)
				{
					return currentEngine_->GetAvailableValueForKey(key, value);
				}

				return true;
			}

			bool GetAvailableValueForKeyDict(const char *key, ESDictionary& value)
			{

				if (isOpened_ == false)
				{
					SDI_TRACE_LOG("scanner is disconnected");
					throw RuntimeError("scanner is disconnected", kSDIErrorDataReceive);
				}

				if (currentEngine_)
				{
					return currentEngine_->GetAvailableValueForKeyDict(key, value);
				}

				return true;
			}


			template <class T>
			bool GetAvailableValueForKey(const char *key, T &minValue, T &maxValue, ESFunctionalUnitType funcUnitToSet)
			{
				ESNumber funcUnitBeforeChange = kESFunctionalUnitFlatbed;
					
			    if (supportedFunctionalUnit_.find(funcUnitToSet) != supportedFunctionalUnit_.end())
				{
					GetValueForKey(kESFunctionalUnitType, funcUnitBeforeChange);
					SetValueForKey(kESFunctionalUnitType, (ESNumber)funcUnitToSet);
				}

				bool result = GetAvailableValueForKey(key, minValue, maxValue);
			    if (supportedFunctionalUnit_.find(funcUnitToSet) != supportedFunctionalUnit_.end())
				{
					SetValueForKey(kESFunctionalUnitType, funcUnitBeforeChange);
				}

				return result;
			}

			template <class T>
			bool GetAvailableValueForKey(const char *key, T &minValue, T &maxValue)
			{

				if (isOpened_ == false)
				{
					SDI_TRACE_LOG("scanner is disconnected");
					throw RuntimeError("scanner is disconnected", kSDIErrorDataReceive);
				}

				if (engine_)
				{
					currentEngine_->GetAvailableValueForKey(key, minValue, maxValue);
				}

				return true;
			}

			void  SetValueForKey(const ESString& strKey, const ESAny& anyValue);
			void  SetValuesForKeysWithDictionary(const ESDictionary& dictionary); 

			void EngineCallBackReceiver(EngineEventType eventType, Image* image, SDIError error);

			void SetCallBackReceiver(ScannerCallBack callBack)
			{
				callBack_ = callBack;	
			}

			void Reset()
			{
				if (currentEngine_)
				{
					currentEngine_->Reset();
				}
			}

			bool Is2in1Mode()
			{
				return engineSecond_ != nullptr;
			}

			bool GetIsOpened()
			{
				return isOpened_;
			}

			ESIndexSet GetSupportedFunctionalUnit()
			{
				return supportedFunctionalUnit_;
			}


		   SDIError UnlockAdministratorLock()
		   {
			   return currentEngine_->UnlockAdministratorLock();
		   }

		   SDIError LockAdministratorLock() 
		   {
			   return currentEngine_->LockAdministratorLock();
		   }

		private:

			bool Is2in1Supporetd();
			void InitSecondEngine();

			std::shared_ptr<Engine> MakeEngine(ESCommandType comType, const SDIDeviceInfo& devInfo);

			ESCommandType GetComTypeForFirst();
			ESCommandType GetComTypeForSecond();

			void UpdateModelInfoUsingScanner();

			std::shared_ptr<Engine> currentEngine_;
			std::shared_ptr<Engine> engine_;
			std::shared_ptr<Engine> engineSecond_;
			std::shared_ptr<ModelInfo> modelInfo_;
			ScannerCallBack callBack_;

			SDIFunctionalUnit funcUnitFor2in1_ = kSDIFunctionalUnitDocumentFeeder;
			ESIndexSet supportedFunctionalUnit_;
			bool isOpened_ = false;

			SDIDeviceInfo devInfo_;

};

}//namespace epsonscan
