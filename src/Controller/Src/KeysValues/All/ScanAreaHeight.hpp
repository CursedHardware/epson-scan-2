#include "Key.hpp"
#include "EpsonScan2.h"
#include <ES2Command/ES2CommandAPI.h>

namespace epsonscan {

	class ScanAreaHeight : public Key<SDIInt> 
	{
		public:

		explicit ScanAreaHeight(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIScanAreaHeightKey);
		}

		void UpdateScanAreaWithDetectedSize(ESFunctionalUnitType funcUnit, ESDictionary& dict);
		bool IsOverScanForCroppingAvailable();

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;
	};

}//namespace epsonscan
