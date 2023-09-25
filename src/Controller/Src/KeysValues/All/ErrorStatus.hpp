#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ErrorStatus : public Key<SDIInt> 
	{
		public:

		explicit ErrorStatus(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return kSDIErrorStatusKey;
		}

		void SetValue(const SDIInt& intVal) override
        {

        }

		void GetValue(SDIInt& outIntVal) override
        {
            Scanner* scanner = dataProvider_->GetScanner().get();
            ESNumber value = 0;
            scanner->GetValueForKey(kESErrorStatus, value);

            outIntVal = ExchangeError((ESErrorCode)value);
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