#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class AutoColorPixelLevel : public Key<SDIInt> 
	{
		public:

		explicit AutoColorPixelLevel(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return kSDIAutoColorPixelLevelKey;
		}

		void SetValue(const SDIInt& intVal) override
        {
            current_ = intVal;
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultRangeCapability(capability);

            capability.supportLevel = kSDISupportLevelNone;

            ESNumber isAutoColorSupported = 0;

            if(dataProvider_->GetModelInfo()->GetValue(kPrvHRD_AutoColorGrayMono, isAutoColorSupported))
            {
                if (isAutoColorSupported)
                {
                    capability.supportLevel = kSDISupportLevelAvailable;
                }
            }
        }

		void GetValue(SDIInt& outIntVal) override
        {
            outIntVal = current_;
        }

		void Reset() override
        {
            current_ = 2;
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }
	};

}//epsonscan