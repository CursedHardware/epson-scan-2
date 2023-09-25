#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class AutoColorPixelType : public Key<SDIInt> 
	{
		public:

		explicit AutoColorPixelType(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return kSDIAutoColorPixelTypeKey;
		}

		void SetValue(const SDIInt& intVal) override
        {
            current_ = intVal;

        }

		void GetValue(SDIInt& outIntVal) override
        {
            outIntVal = current_;
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);

            capability.supportLevel = kSDISupportLevelAvailable;

            AddList(capability, kSDIAutoColorPixelTypeMono);
            AddList(capability, kSDIAutoColorPixelTypeGray);

            ESNumber isAutoColorSupported = 0;


            if(dataProvider_->GetModelInfo()->GetValue(kPrvHRD_AutoColorGrayMono, isAutoColorSupported))
            {
                if (isAutoColorSupported)
                {
                    AddList(capability, kSDIAutoColorPixelTypeAuto);
                }
            }
        }

		void Reset() override
        {
            current_ = kSDIAutoColorPixelTypeMono;
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }
	};

}//epsonscan