#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class JpegProgressive : public Key<SDIInt> 
	{
		public:

		explicit JpegProgressive(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIJpegProgressiveKey);
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

        	AddList(capability, true);
        	AddList(capability, false);

        	AddAllList(capability, true);
        	AddAllList(capability, false);
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
