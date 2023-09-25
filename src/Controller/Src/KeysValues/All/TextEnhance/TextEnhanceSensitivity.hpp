#pragma once
#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class TextEnhanceSensitivity : public Key<SDIInt> 
	{
		public:

		explicit TextEnhanceSensitivity(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDITextEnhanceSensitivityKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
