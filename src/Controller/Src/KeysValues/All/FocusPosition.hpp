#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class FocusPosition : public Key<SDIInt> 
	{
		public:

		explicit FocusPosition(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return kSDIFocusPositionKey;
		}

		void SetValue(const SDIInt& intVal) override
        {
            Scanner* scanner = dataProvider_->GetScanner().get();
						
			SDICapability cap;
			GetCapability(cap);
			if (cap.supportLevel != kSDISupportLevelNone && cap.minValue <= intVal && cap.maxValue >= intVal)
			{
				current_ = intVal;
			}else{
				current_ = 0;
			}
			
        }

		void GetValue(SDIInt& outIntVal) override
        {
            outIntVal = current_;
        }

		void GetCapability(SDICapability &capability) override
        {    

			if (cache_)
			{
				capability = *cache_;
				return;
			}
			std::unique_ptr<SDICapability> cap(new SDICapability());
        	SetDefaultRangeCapability(*cap);

			ESNumber focusSupport = 0;
			dataProvider_->GetModelInfo()->GetValue(kPrvHRD_Focus, focusSupport);
						
            Scanner* scanner = dataProvider_->GetScanner().get();
			if (scanner)
			{
				ESFloat min = 0;
				ESFloat max = 0;
				if(scanner &&  scanner->GetAvailableValueForKey(kESFocus, min, max) && focusSupport) {
					cap->supportLevel = kSDISupportLevelAvailable;

					scanner->GetValueForKey(kESMaxFocus, max);
					scanner->GetValueForKey(kESMinFocus, min);

					cap->allMinValue = min * 10;
					cap->allMaxValue = max * 10;	
					cap->minValue = min * 10;
					cap->maxValue = max * 10;	
				}else{
					cap->supportLevel = kSDISupportLevelNone;
				}
			}	

			capability = *cap;
			cache_.swap(cap);
        }

		void Reset() override
        {
			current_ = 0;
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();

			SDICapability cap;
			GetCapability(cap);
			if (cap.supportLevel != kSDISupportLevelNone && cap.minValue <= current_ && cap.maxValue >= current_)
			{
				ESFloat setVal = current_ / 10.0;
				dict[kESFocus] = setVal;
			}

            return dict;
        }

		private :
				std::unique_ptr<SDICapability> cache_;
	};

}//epsonscan