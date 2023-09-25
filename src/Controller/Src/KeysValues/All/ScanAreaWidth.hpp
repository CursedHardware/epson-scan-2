#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ScanAreaWidth : public Key<SDIInt> 
	{
		public:

		explicit ScanAreaWidth(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIScanAreaWidthKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
	};


}//namespace epsonscan
