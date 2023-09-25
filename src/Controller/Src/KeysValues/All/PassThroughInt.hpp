#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

    template<typename ORIGIN_TYPE = ESNumber>
	class PassThroughInt : public Key<SDIInt> 
	{
		public:

        std::string modelInfoSupportedKey_;

        void SetModelInfoSupportedKey(std::string key)
        {
            modelInfoSupportedKey_ = key;
        }


		explicit PassThroughInt(IKeyDataProvider* dataProvider, 
                                std::string sdiKey, 
                                std::string comKey, 
                                bool isList = false,
                                bool isRange = false
                                ) :
                                 Key<SDIInt>(dataProvider),
                                 sdiKeyName_(sdiKey),
                                 commandkeyName_(comKey),
                                 isList_(isList),
                                 isRange_(isRange)
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
            
            current_ = value;
            outIntVal = current_;
        }

		void GetCapability(SDICapability &capability) override
        {    
        	SetDefaultListCapability(capability);

            Scanner* scanner = dataProvider_->GetScanner().get();
            ORIGIN_TYPE value = 0;

            if (modelInfoSupportedKey_.length() > 0)
            {
                ESNumber isSupprted  =  0;
                ModelInfo* modelInfo = dataProvider_->GetModelInfo().get();
                modelInfo->GetValue(modelInfoSupportedKey_.c_str(), isSupprted);
                if (isSupprted == 0)
                {
                    capability.supportLevel = kSDISupportLevelNone;
                    return;  
                }      
            }

            if (scanner->Is2in1Mode())
            {
                if(scanner->GetValueForKey(commandkeyName_.c_str(), value, kESFunctionalUnitDocumentFeeder) == false)
                {
                    capability.supportLevel = kSDISupportLevelNone;
                    return;
                }else{
                    capability.supportLevel = kSDISupportLevelAvailable;
                }
            }else{
                if(scanner->GetValueForKey(commandkeyName_.c_str(), value) == false)
                {
                    capability.supportLevel = kSDISupportLevelNone;
                    return;
                }else{
                    capability.supportLevel = kSDISupportLevelAvailable;
                }
            }

            if (isList_)
            {
                ResetList(capability);
                ESIndexSet availabeValue;
                Scanner* scanner = dataProvider_->GetScanner().get();

                if (scanner->Is2in1Mode())
                {
                    if(scanner &&  scanner->GetAvailableValueForKey(commandkeyName_.c_str(), availabeValue, kESFunctionalUnitDocumentFeeder)) {
                        for (ESNumber value : availabeValue)
                        {
                            AddAllList(capability, value);
                            AddList(capability ,value);
                        }
                    }
                }else{
                    if(scanner &&  scanner->GetAvailableValueForKey(commandkeyName_.c_str(), availabeValue)) {
                        for (ESNumber value : availabeValue)
                        {
                            AddAllList(capability, value);
                            AddList(capability ,value);
                        }
                    }
                }
            }

            if (isRange_)
            {
                SetDefaultRangeCapability(capability);

                Scanner* scanner = dataProvider_->GetScanner().get();

                ESNumber min = 0;
                ESNumber max = 0;

                if (scanner->Is2in1Mode())
                {

                    if(scanner &&  scanner->GetAvailableValueForKey(commandkeyName_.c_str(), min, max, kESFunctionalUnitDocumentFeeder)) {
                        capability.minValue = min;
                        capability.allMinValue = min;
                        capability.maxValue = max;
                        capability.allMaxValue = max;
                    }
                }else{
                    if(scanner &&  scanner->GetAvailableValueForKey(commandkeyName_.c_str(), min, max)) {
                        capability.minValue = min;
                        capability.allMinValue = min;
                        capability.maxValue = max;
                        capability.allMaxValue = max;
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
        bool isList_;
        bool isRange_;
	};

}//epsonscan