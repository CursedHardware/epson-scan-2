#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class DoubleFeedDetectionLevel : public Key<SDIInt> 
	{
		public:

		explicit DoubleFeedDetectionLevel(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider), adf_(nullptr)
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIDoubleFeedDetectionLevelKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;
		void GetADFCapability(SDICapability &capability);

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

		std::unique_ptr<SDICapability> adf_;
	};

}//namespace epsonscan
