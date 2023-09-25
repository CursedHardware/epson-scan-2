#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class AutoCropInscribed : public Key<SDIInt> 
	{
		public:

		explicit AutoCropInscribed(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIAutoCroppingInscribedKey);
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
