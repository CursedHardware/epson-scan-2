#pragma once

#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ThresholdAdjustment : public Key<SDIInt> 
	{
		public:

		explicit ThresholdAdjustment(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDITextEnhanceThresholdAdjustmentKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
