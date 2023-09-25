#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class DoubleFeedDetectionAreaLength : public Key<SDIInt> 
	{
		public:

		explicit DoubleFeedDetectionAreaLength(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIDoubleFeedDetectionAreaLengthKey);
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

            auto maxAreaHeight = GetValueInt(kSDIMaxScanAreaHeightKey);

	        ESFloat rangeValue;
            Scanner* scanner = dataProvider_->GetScanner().get();
	        if(scanner &&  scanner->GetValueForKey(kESDoubleFeedDetectionRangeLength, rangeValue)) 
            {
                capability.supportLevel = kSDISupportLevelAvailable;
                capability.minValue = 100;
                capability.maxValue = maxAreaHeight;
            }
        }

		void Reset() override
        {
            current_ = 1550;
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();	        
            dict[kESDoubleFeedDetectionRangeLength] = (ESFloat)(current_ / 100.0);
            return dict;
        }
	};

}//namespace epsonscan