#include "Key.hpp"
#include "EpsonScan2.h"
#include "ModelInfo.hpp"

namespace epsonscan {

    template<typename ORIGIN_TYPE = ESNumber>
	class RollerCounter : public Key<SDIInt> 
	{
		public:

		explicit RollerCounter(IKeyDataProvider* dataProvider, std::string sdiKey, std::string comKey) :
                               Key<SDIInt>(dataProvider),
                               sdiKeyName_(sdiKey),
                               commandkeyName_(comKey)
		{

		}

		std::string GetName() const override 
		{
			return sdiKeyName_;
		}

		void SetValue(const SDIInt& intVal) override
        {
            dataProvider_->GetScanner()->SetValueForKey(commandkeyName_.c_str(), (ORIGIN_TYPE)intVal);
        }

		void GetValue(SDIInt& outIntVal) override
        {
            Scanner* scanner = dataProvider_->GetScanner().get();
            ORIGIN_TYPE value = 0;


			SDI_TRACE_LOG("[Test] GetValue");

            if (scanner->Is2in1Mode())
            {
                SDI_TRACE_LOG("[Test] GetValue 2 in 1");
                scanner->GetValueForKey(commandkeyName_.c_str(), value, kESFunctionalUnitDocumentFeeder);
            }else{
                scanner->GetValueForKey(commandkeyName_.c_str(), value);
            }

            scanner->GetValueForKey(commandkeyName_.c_str(), value);
            current_ = value;
            outIntVal = current_;
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);

            Scanner* scanner = dataProvider_->GetScanner().get();
            ORIGIN_TYPE value = 0;
            if (scanner->Is2in1Mode())
            {
                if(scanner->GetValueForKey(commandkeyName_.c_str(), value, kESFunctionalUnitDocumentFeeder) == false)
                {
                    capability.supportLevel = kSDISupportLevelNone;
                }else{
                    
                    ESNumber isSupprted = 0;

                    ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();
                    modelInfo->GetValue(kPrvHRD_RollerCount, isSupprted);
                    if (isSupprted)
                    {
                        capability.supportLevel = kSDISupportLevelAvailable;
                    }else{
                        capability.supportLevel = kSDISupportLevelNone;
                    }
                }
            }else{
                if(scanner->GetValueForKey(commandkeyName_.c_str(), value) == false)
                {
                    capability.supportLevel = kSDISupportLevelNone;
                }else{
                    ESNumber isSupprted = 0;

                    ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();
                    modelInfo->GetValue(kPrvHRD_RollerCount, isSupprted);
                    if (isSupprted)
                    {
                        capability.supportLevel = kSDISupportLevelAvailable;
                    }else{
                        capability.supportLevel = kSDISupportLevelNone;
                    }
                }
            }
        }

		void Reset() override
        {
        }

		ESDictionary GetEngineKeyValues() override
        {
            auto dict =  ESDictionary();
            return dict;
        }

        std::string sdiKeyName_;
        std::string commandkeyName_;
	};

}//epsonscan