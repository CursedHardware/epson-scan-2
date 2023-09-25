#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class MultiTiff : public Key<SDIInt> 
	{
		public:

		explicit MultiTiff(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIMultiTiffEnabledKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
