#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

    template<typename ORIGIN_TYPE = ESNumber>
	class ModelInfoPassThrough : public Key<SDIInt> 
	{
		public:

		explicit ModelInfoPassThrough(IKeyDataProvider* dataProvider, std::string sdiKey, std::string comKey) :
                 Key<SDIInt>(dataProvider),
                 sdiKeyName_(sdiKey),
                 modelInfoKeyName_(comKey)
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
            ORIGIN_TYPE value = 0;
            ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();

            modelInfo->GetValue(modelInfoKeyName_.c_str(), value);
            outIntVal = value;
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