#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ScanHeightMaxInLongPaperResolutionTable : public Key<std::string> 
	{
		public:

		explicit ScanHeightMaxInLongPaperResolutionTable(IKeyDataProvider* dataProvider) : Key<std::string>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIMaxScanAreaHeightInLongPaperResolutionTableKey);
		}

		void SetValue(const std::string& intVal) override;
		void GetValue(std::string& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;
		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
