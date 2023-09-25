#include "Key.hpp"
#include "Scanner.hpp"
#include "EpsonScan2.h"

namespace epsonscan {
    
    class Gamma : public Key<SDIInt> 
	{
		public:

		explicit Gamma(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) , isSupportGammaCommand_(false)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIGammaKey);
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

        	capability.supportLevel = kSDISupportLevelAvailable;

            if (GetValueInt(kSDIColorTypeKey) == kSDIColorTypeMono1)
            {
                capability.minValue = 0;
                capability.maxValue = 0;
                capability.supportLevel= kSDISupportLevelUnavailable;
            }else
            {
                capability.minValue = 0;
                capability.maxValue = 30;
                capability.supportLevel= kSDISupportLevelAvailable;
            }
            
            capability.allMinValue = 0;
            capability.allMaxValue = 30;


        }

		void Reset() override
        {
        	current_  = 22;

            ESNumber currentGamma;
            Scanner* scannerSupportGamma = dataProvider_->GetScanner().get();
            if(scannerSupportGamma &&  scannerSupportGamma->GetValueForKey(kESBrightness, currentGamma)) {
                isSupportGammaCommand_ = true;
            }else{
                isSupportGammaCommand_ = false;
            }
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
	        if(isSupportGammaCommand_) {
		        dict[kESGammaScale] = (ESNumber)current_;
	        }

            return dict;
        }

        bool IsSupportGammaCommand()
        {
	        return isSupportGammaCommand_;
        }
private:

        bool isSupportGammaCommand_;
	};
}//namespace epsonscan
