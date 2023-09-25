#pragma once
#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class FunctionalUnit : public Key<SDIInt> 
	{
		public:

		explicit FunctionalUnit(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIFunctionalUnitKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		bool ADFAvailable();
		bool FlatbedAvailable();
		bool TransparentAvailable();
		bool TransarentAreaGuideAvailable();

		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
