#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class Contrast : public Key<SDIInt> 
	{
	public:
		explicit Contrast(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider), isSupportContarastCommand_(false)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIContrastKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;
		bool IsSupportContrastCommand();

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

	private:
		bool isSupportContarastCommand_;
	};

}//namespace epsonscan
