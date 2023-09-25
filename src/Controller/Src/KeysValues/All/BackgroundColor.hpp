#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class BackgroundColor : public Key<SDIInt> 
	{
		public:

		explicit BackgroundColor(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIBackgroundColorKey);
		}

		void SetValue(const SDIInt& intVal) override
		{
			SDICapability cap;
			GetCapability(cap);

			if (cap.supportLevel != kSDISupportLevelNone)
			{
				current_ = intVal;
			}
		}

		void GetValue(SDIInt& outIntVal) override
		{			
			outIntVal = current_;
		}	

		void GetCapability(SDICapability &capability) override
		{
			SetDefaultListCapability(capability);
			ESIndexSet available;

				
			Scanner* scanner = dataProvider_->GetScanner().get();
			if(scanner &&  scanner->GetAvailableValueForKey(kESBGColor, available)) {
				AddList(capability, kSDIBackgroundColorBlack);
				AddAllList(capability, kSDIBackgroundColorBlack);

				AddList(capability, kSDIBackgroundColorWhite);
				AddAllList(capability, kSDIBackgroundColorWhite);

				capability.supportLevel = kSDISupportLevelAvailable;
			}else{
				capability.supportLevel = kSDISupportLevelNone;
			}
		}

		void Reset() override
		{

			SDICapability cap;
			GetCapability(cap);

			if (cap.supportLevel != kSDISupportLevelNone)
			{
				current_ = kSDIBackgroundColorBlack;
			}else{
				current_ = kSDIBackgroundColorNone;
			}
		}

		ESDictionary GetEngineKeyValues() override
		{

			auto dict =  ESDictionary();

			SDICapability cap;
			GetCapability(cap);

			if (cap.supportLevel != kSDISupportLevelAvailable)
			{
				return dict;
			}
			
			if (current_ == kSDIBackgroundColorBlack)
			{
				dict[kESBGColor] = (ESNumber)kESBGColorBlack;
			}

			if (current_ == kSDIBackgroundColorWhite)
			{
				dict[kESBGColor] = (ESNumber)kESBGColorWhite;
			}

			return dict;
		}
	};

}//namespace epsonscan
