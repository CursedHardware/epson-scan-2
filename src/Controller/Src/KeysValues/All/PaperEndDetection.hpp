#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class PaperEndDetection : public Key<SDIInt> 
	{
		public:

		explicit PaperEndDetection(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIPaperEndDetectionKey);
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

                capability.supportLevel = kSDISupportLevelNone;
                ESIndexSet available;

                Scanner* scanner = dataProvider_->GetScanner().get();
                if(scanner &&  scanner->GetAvailableValueForKey(kESPaperEndDetection, available)) {

                    if (available.find(true) != available.end())
                    {
                        AddList(capability, true);
                        AddList(capability, false);

                        AddAllList(capability, true);
                        AddAllList(capability, false);

                        capability.supportLevel = kSDISupportLevelAvailable;
                    }else{
                        AddList(capability, false);
                        AddAllList(capability, false);
                    }
                }	

                //
                // Functional Unit
                // 
                if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
                {
                    if (capability.supportLevel == kSDISupportLevelAvailable)
                    { 
                         capability.supportLevel = kSDISupportLevelUnavailable;
                    }
                    else
                    {
                        capability.supportLevel = kSDISupportLevelNone;
                    }
                    ResetList(capability);
                }
            }

		void Reset() override
        {
	        current_  = false;

        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }
	};

}//namespace epsonscan
