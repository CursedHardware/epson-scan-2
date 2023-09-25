#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class Brightness : public Key<SDIInt> 
	{
	public:
		explicit Brightness(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) , isSupportBrigthenessCommand_(false)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIBrightnessKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;
		bool IsSupportBrightnessCommand();

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

	private:
			bool isSupportBrigthenessCommand_;
	
	};

}//namespace epsonscan
