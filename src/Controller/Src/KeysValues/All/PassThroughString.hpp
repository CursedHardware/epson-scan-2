#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

    template<typename ORIGIN_TYPE = std::string>
	class PassThroughString : public Key<std::string> 
	{
		public:

   
		explicit PassThroughString(IKeyDataProvider* dataProvider, 
                                std::string sdiKey, 
                                std::string comKey
                                ) :
                                 Key<std::string>(dataProvider),
                                 sdiKeyName_(sdiKey),
                                 commandkeyName_(comKey)
		{

		}

		std::string GetName() const override 
		{
			return sdiKeyName_;
		}

		void SetValue(const std::string & intVal) override
        {
            dataProvider_->GetScanner()->SetValueForKey(commandkeyName_.c_str(), (ORIGIN_TYPE)intVal);
        }

		void GetValue(std::string & outIntVal) override
        {
            Scanner* scanner = dataProvider_->GetScanner().get();
            ORIGIN_TYPE value;

			SDI_TRACE_LOG("[Test] GetValue");

            if (scanner->Is2in1Mode())
            {
                SDI_TRACE_LOG("[Test] GetValue 2 in 1");
                scanner->GetValueForKey(commandkeyName_.c_str(), value, kESFunctionalUnitDocumentFeeder);
            }else{
                scanner->GetValueForKey(commandkeyName_.c_str(), value);
            }
            
            current_ = value;
            outIntVal = current_;
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);
                               
            ORIGIN_TYPE value;
            Scanner* scanner = dataProvider_->GetScanner().get();

            capability.supportLevel = kSDISupportLevelAvailable;

            if (scanner->Is2in1Mode())
            {
                SDI_TRACE_LOG("[Test] GetValue 2 in 1");
                if(scanner->GetValueForKey(commandkeyName_.c_str(), value, kESFunctionalUnitDocumentFeeder) == false)
                {
                    capability.supportLevel = kSDISupportLevelNone;
                }
            }else{
                if(scanner->GetValueForKey(commandkeyName_.c_str(), value) == false)
                {
                    capability.supportLevel = kSDISupportLevelNone;
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