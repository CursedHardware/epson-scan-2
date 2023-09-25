#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class DoubleFeedDetectionAreaMin : public Key<SDIInt> 
	{
		public:

		explicit DoubleFeedDetectionAreaMin(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIDoubleFeedDetectionAreaMinKey);
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
	        SetDefaultRangeCapability(capability);
          	capability.supportLevel = kSDISupportLevelNone;
	        ESFloat minValue;
            auto maxAreaHeight = GetValueInt(kSDIMaxScanAreaHeightKey);

            Scanner* scanner = dataProvider_->GetScanner().get();
	        if(scanner &&  scanner->GetValueForKey(kESDoubleFeedDetectionRangeOffset, minValue)) 
            {
                capability.supportLevel = kSDISupportLevelAvailable;
                capability.maxValue = maxAreaHeight - 100;
                capability.minValue = 0;
            }
        }

		void Reset() override
        {
            current_ = 0;
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();	        
            dict[kESDoubleFeedDetectionRangeOffset] = (ESFloat)(current_ / 100.0);
            return dict;
        }
	};

}//namespace epsonscan