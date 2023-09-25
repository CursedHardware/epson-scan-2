#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class Resolution : public Key<SDIInt> 
	{
		public:

		explicit Resolution(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIResolutionKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;


		private:
		ESNumber ScanResolutionForDestinationResolution(ESNumber nTargetResolution, ESDictionary& dictResolutionTables);
		SDIInt GetScanResolution(SDIInt inResolution, bool isXResolution);
		ESDictionary GetEngineKeyValues() override;
		ESDictionary  GetSeolutionTable(bool isXResolution);
	};

}//namespace epsonscan
