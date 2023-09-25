#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class SkipBlankPageOnOff : public Key<SDIInt> 
	{
		public:

		explicit SkipBlankPageOnOff(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) , isSupportHardwareBlankPageOnOff_(false)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIBlankPageSkipKey);
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

				bool hasAdf = false;

				ESIndexSet functionalUnits = dataProvider_->GetScanner().get()->GetSupportedFunctionalUnit();
				if (functionalUnits.find(kESFunctionalUnitDocumentFeeder) != functionalUnits.end())
				{
					hasAdf = true;
				}

				if ((DTR::IsDTRAvailable()  || isSupportHardwareBlankPageOnOff_)  && hasAdf)
				{	
					if (GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitFlatbed)
					{
						capability.supportLevel = kSDISupportLevelUnavailable;
					}else{
						capability.supportLevel = kSDISupportLevelAvailable;	
					}
				}else{
					capability.supportLevel = kSDISupportLevelNone;
				}
        }

		void Reset() override
        {
            current_ = 0;

			bool currentBlankPageOnOff = false;
			Scanner* scannerSupportBlankPageOnOff = dataProvider_->GetScanner().get();
			if(scannerSupportBlankPageOnOff && scannerSupportBlankPageOnOff->GetValueForKey(kESDetectBlankPageEnabled, currentBlankPageOnOff)) {
				isSupportHardwareBlankPageOnOff_ = true;
			}else{
				isSupportHardwareBlankPageOnOff_ = false;
			}
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
			if(isSupportHardwareBlankPageOnOff_ == true && GetValueInt(kSDIFunctionalUnitKey) == kSDIFunctionalUnitDocumentFeeder)
			{
				dict[kESDetectBlankPageEnabled] = (bool)current_;
			}
            return dict;
        }

		bool IsSupportHardwareBlankPageOnOff()
		{
			return isSupportHardwareBlankPageOnOff_;
		}

		private:
			bool isSupportHardwareBlankPageOnOff_;
	};

}//namespace epsonscan