#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class Is2in1Mode : public Key<SDIInt> 
	{
		public:

		explicit Is2in1Mode(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return kSDI2in1ModeKey;
		}

		void SetValue(const SDIInt& intVal) override
        {

        }

		void GetValue(SDIInt& outIntVal) override
        {
            Scanner* scanner = dataProvider_->GetScanner().get();
			outIntVal = (SDIInt)scanner->Is2in1Mode();
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);
        }

		void Reset() override
        {
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }
	};

}//epsonscan