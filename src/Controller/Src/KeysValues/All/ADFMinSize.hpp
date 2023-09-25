#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class ADFMinSize : public Key<SDIInt> 
	{
		public:

		explicit ADFMinSize(IKeyDataProvider* dataProvider, std::string sdiKey, std::string modelInfoKey) :
                 Key<SDIInt>(dataProvider),
                 sdiKeyName_(sdiKey),
                 modelInfoKeyName_(modelInfoKey)
		{

		}

		std::string GetName() const override 
		{
			return sdiKeyName_;
		}

		void SetValue(const SDIInt& intVal) override
        {

        }

		void GetValue(SDIInt& outIntVal) override
        {
            ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();
            ESNumber maxSizeInMM = 0;
            modelInfo->GetValue(modelInfoKeyName_.c_str(), maxSizeInMM);
            current_ = MMtoInch(maxSizeInMM/10.0) * 100;
            outIntVal = current_;
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

        std::string sdiKeyName_;
        std::string modelInfoKeyName_;
	};

}//epsonscan