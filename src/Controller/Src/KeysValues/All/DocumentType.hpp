#pragma once
#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class DocumentType : public Key<SDIInt> 
	{
		public:

		explicit DocumentType(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIDocumentTypeKey);
		}

		bool IsNegativeFilm()
		{
			return (current_ == kSDIDocumentTypeColorNegativeFilm || current_ == kSDIDocumentTypeMonoNegativeFilm);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;


		ESDictionary GetEngineKeyValues() override;

		std::unique_ptr<SDICapability> cache_;
	};

}//namespace epsonscan
