#pragma once
#include "EpsonScan2.h"
#include "KeyDataProvider.hpp"
#include <ESCommonTypedef.h>
#define BOOST_NO_CXX11_RVALUE_REFERENCES 1
#include <boost/any.hpp>
#include <memory>

namespace epsonscan {

	class IKey
	{
		public:

		explicit IKey(IKeyDataProvider* dataProvider) {}	

		virtual std::string GetName()  const = 0;
		virtual SDIValueType GetType() const = 0;

	  	virtual void SetValue(SDIValueType valType, void* value, SDIInt size) = 0;
		virtual void GetValue(SDIValueType valType, void* value, SDIInt size) = 0;
		virtual void GetCapability(SDICapability& capability) = 0;
		virtual void Reset() = 0;

		virtual ESDictionary GetEngineKeyValues() = 0;
	};

	template<typename T>
	class Key : public IKey 
	{
	   public:
		explicit Key(IKeyDataProvider* dataProvider) : IKey(dataProvider), dataProvider_(dataProvider) {
	
		}

		SDIValueType GetType() const override;

	  	void SetValue(SDIValueType valType, void* value, SDIInt size) override;
		void GetValue(SDIValueType valType, void* value, SDIInt size) override;
		void GetCapability(SDICapability& capability) override {};


		virtual void SetValue(const T& val) = 0;
		virtual void GetValue(T& outVal) = 0;
		
		protected:
			T current_;	
			IKeyDataProvider* dataProvider_;

			void SetDefaultRangeCapability(SDICapability& capability) {
				capability.version = 1;
				capability.capabilityType = kSDICapabilitTypeRange;
				capability.countOfList = 0;
				capability.minValue = 0;
				capability.maxValue = 0;
				capability.allMinValue = 0;
				capability.allMaxValue = 0;
			    capability.supportLevel = kSDISupportLevelAvailable;
			}

			void SetDefaultListCapability(SDICapability& capability) {
				capability.version = 1;
				capability.capabilityType = kSDICapabilitTypeList;
				capability.countOfList = 0;
				capability.countOfAllList = 0;
				capability.minValue = 0;
				capability.maxValue = 0;
				capability.allMinValue = 0;
				capability.allMaxValue = 0;
			    capability.supportLevel = kSDISupportLevelAvailable;
			}

			void AddList(SDICapability& capability, SDIInt value)
			{
				if (capability.countOfList >= SDI_CAPABILITY_LISTMAX)
				{
					return;
				}

				capability.list[capability.countOfList] = value;
				capability.countOfList++;
			}

			void ResetList(SDICapability& capability)
			{
				capability.countOfList = 0;
			}

			void ResetRange(SDICapability& capability)
			{
				capability.minValue = 0;
				capability.maxValue = 0;
			}

			void AddAllList(SDICapability& capability, SDIInt value)
			{
				if (capability.countOfList >= SDI_CAPABILITY_LISTMAX)
				{
					return;
				}			

				capability.allList[capability.countOfAllList] = value;
				capability.countOfAllList++;
			}

			SDIInt GetValueInt(std::string key)
			{
				std::shared_ptr<IKey> keyInstance =  dataProvider_->GetKeyInstance(std::string(key));

				SDIInt intVal = 0;
				keyInstance->GetValue(kSDIValueTypeInt, &intVal, sizeof(intVal));

				return intVal;
			}

			void SetValueInt(std::string key, SDIInt intVal)
			{
				std::shared_ptr<IKey> keyInstance =  dataProvider_->GetKeyInstance(std::string(key));
				keyInstance->SetValue(kSDIValueTypeInt, &intVal, sizeof(intVal));
			}

			void GetCapabilityWithKey(std::string key, SDICapability& capability)
			{
				std::shared_ptr<IKey> keyInstance =  dataProvider_->GetKeyInstance(std::string(key));
				keyInstance->GetCapability(capability);
			}

			double MMtoInch(double mm)
			{
				return mm / 25.4;
			}
	};

   	   template<typename T> SDIValueType Key<T>::GetType() const
	   {
			return kSDIValueTypeInt;		
	   }
}//namespace epsonscan
