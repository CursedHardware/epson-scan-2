#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ScanHeightMax : public Key<SDIInt> 
	{
		public:

		explicit ScanHeightMax(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIMaxScanAreaHeightKey);
		}

		SDIInt ADFValue();
		SDIInt FlatbedValue();
		SDIInt AutoValue();


		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
