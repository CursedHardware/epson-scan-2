#pragma once
#define BOOST_NO_CXX11_RVALUE_REFERENCES 1
#include <ESCommonTypedef.h>
#include "EpsonScan2.h"
#include "KeyDataProvider.hpp"
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <boost/optional.hpp>

#include <unistd.h>

namespace epsonscan {

class ModelInfo;
class Scanner;
class IKey;
class KeyMgr : public IKeyDataProvider
{
	public:

		KeyMgr(std::shared_ptr<ModelInfo> modelInfo, std::shared_ptr<Scanner> scanner);

		void Reset();
		void SetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size);
		void GetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size);
		void GetDefault(const std::string& key, SDIValueType valType, void* value, SDIInt size);
		void GetCapability(const std::string& key, SDICapability &capability);

		SDIInt GetValueInt(std::string key);
		void   SetValueInt(std::string key, SDIInt intVal);

		ESDictionary GetEngineKeyValues();

		const std::vector<std::string>& GetAllKeys() const;

		std::shared_ptr<ModelInfo> GetModelInfo(); 
		std::shared_ptr<Scanner> GetScanner(); 

		ESNumber GetCommandValueAsNumber(ESString key) 
		{
			return boost::any_cast<ESNumber>(commandKeyValueCache_.at(key));
		}

		bool GetCommandValueAsBool(ESString key) 
		{
			try
			{
				return boost::any_cast<bool>(commandKeyValueCache_.at(key));
			}
			catch(const std::exception& e)
			{
				return false;
			}			
		}

		std::shared_ptr<IKey> GetKeyInstance(const std::string& key);

	private:

		void UpdateOtherEngineSetting(ESDictionary& params);
		void ResolveEngineSettingInconsistents(ESDictionary& params);


		std::shared_ptr<ModelInfo> modelInfo_;
		std::shared_ptr<Scanner>   scanner_;

		std::vector<std::shared_ptr<IKey>> keyInstaces_;
		std::vector<std::string> allKeys_;

		ESDictionary commandKeyValueCache_;
};

}//nemespace epsonscan
