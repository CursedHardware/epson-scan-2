#include "Key.hpp"
#include "EpsonScan2.h"
#include "GetOrientation.hpp"
#include <utils/PathUtils.h>

namespace epsonscan {

	class OFDSuppoted : public Key<SDIInt> 
	{
		public:

		explicit OFDSuppoted(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return kSDIOFDSupportedKey;
		}

		void SetValue(const SDIInt& intVal) override
        {
            
        }

		bool IsOfdPluginAvailable()
		{
  		   return ES_CMN_FUNCS::PATH::ES_IsExistFile("/opt/epson/epsonscan2-ofd-component/libepsonscan2-ofd-component.so");
		}

		void GetValue(SDIInt& outIntVal) override
        {
            outIntVal = IsOFDAvailable();
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);

			if (IsOFDAvailable())
			{
				capability.supportLevel = kSDISupportLevelAvailable;
			}else{
				capability.supportLevel = kSDISupportLevelNone;
			}
        }

		bool IsOFDAvailable()
		{
		   SDIScannerKind kind = (SDIScannerKind)GetValueInt(KSDIScannerKindKey);
			if (kind != kSDIKindSFDocument && kind != kSDIKindSFPhoto)
			{
				return false;
			}

			return  GetOrientation::IsPluginAvailable() && IsOfdPluginAvailable();
		}

		void Reset() override
        {
			current_ =   IsOFDAvailable();
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }
	};

}//epsonscan