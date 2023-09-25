#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class FilmAreaGuide : public Key<SDIInt> 
	{
		public:

		explicit FilmAreaGuide(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return kSDIFilmAreaGuideKey;
		}

		void SetValue(const SDIInt& intVal) override
        {
            Scanner* scanner = dataProvider_->GetScanner().get();
			if (intVal)
			{
				ESIndexSet availableFunctionalUnit;
				if(scanner &&  scanner->GetAvailableValueForKey(kESFunctionalUnitType, availableFunctionalUnit)) {
					if (availableFunctionalUnit.find(kESFunctionalUnitTPUAreaGuide) != availableFunctionalUnit.end()) {
						current_ = intVal;
					}
				}
			}else{
				current_ = intVal;
			}

        }

		void GetValue(SDIInt& outIntVal) override
        {
            outIntVal = (bool)current_;
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);
						
			ESIndexSet availableFunctionalUnit;
			Scanner* scanner = dataProvider_->GetScanner().get();
			if(scanner &&  scanner->GetAvailableValueForKey(kESFunctionalUnitType, availableFunctionalUnit)) {
				if (availableFunctionalUnit.find(kESFunctionalUnitTPUAreaGuide) != availableFunctionalUnit.end()) {
					AddList(capability, true);
					AddList(capability, false);

					AddAllList(capability, true);
					AddAllList(capability, false);	
				}
			}		
        }

		void Reset() override
        {
			current_ = false;
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }
	};

}//epsonscan