#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class DuplexType : public Key<SDIInt> 
	{
		public:

		explicit DuplexType(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIDuplexTypeKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
		bool hasCache_;
		bool isDuplexAvailable_;
	};

}//namespace epsonscan
