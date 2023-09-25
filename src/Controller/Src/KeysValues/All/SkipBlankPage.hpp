#include "Key.hpp"
#include "EpsonScan2.h"
#include "DTR.hpp"

namespace epsonscan {

	class SkipBlankPage : public Key<SDIInt> 
	{
		public:

		explicit SkipBlankPage(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) , isSupportHardwareBlankPageLevel_(false)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIBlankPageSkipLevelKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

		bool IsSupportBlankPageLevel();

		private:
			bool isSupportHardwareBlankPageLevel_;
	};

}//namespace epsonscan
