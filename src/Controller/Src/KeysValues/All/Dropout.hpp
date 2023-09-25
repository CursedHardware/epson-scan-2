#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class Dropout : public Key<SDIInt> 
	{
		public:

		explicit Dropout(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIDropoutColorKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

		bool hasCache_;
		bool isDropoutAvailable_;
	};

}//namespace epsonscan
