#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class PaperDeskew : public Key<SDIInt> 
	{
		public:

		explicit PaperDeskew(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIPaperDeskewKey);
		}

		bool IsAutoDeskewForWhiteBGEnabled();

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;
		void GetADFCapability(SDICapability &capability);

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

		std::unique_ptr<SDICapability> adf_;
	};

}//namespace epsonscan
