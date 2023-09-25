#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ScanHeightMaxInLongPaper : public Key<SDIInt> 
	{
		public:

		explicit ScanHeightMaxInLongPaper(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIMaxScanAreaHeightInLongPaperKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESNumber ADFMaxScanAreaHeightInHRD();
		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
