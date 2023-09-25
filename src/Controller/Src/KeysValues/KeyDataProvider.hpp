#pragma once
#include <memory>
#include <ESCommonTypedef.h>

namespace epsonscan 
{

	class ModelInfo;
	class Scanner;
	class IKey;
	class IKeyDataProvider
	{
		public:
			virtual std::shared_ptr<ModelInfo> GetModelInfo() = 0;
			virtual std::shared_ptr<Scanner>   GetScanner()	  = 0;
			virtual	std::shared_ptr<IKey> GetKeyInstance(const std::string& key) = 0;
			virtual ESNumber GetCommandValueAsNumber(ESString key) = 0;
	};

}//namespace epsonscan
