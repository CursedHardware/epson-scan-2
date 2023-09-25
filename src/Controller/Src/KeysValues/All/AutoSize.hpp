#pragma once
#include "Key.hpp"
#include "EpsonScan2.h"
#include <ES2Command/ES2CommandAPI.h>

namespace epsonscan {

	class AutoSize : public Key<SDIInt> 
	{
		public:

		explicit AutoSize(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) , processSoftwareAutoCrop_(false)
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDIAutoSizeKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		bool IsHardwareAvailable();
		bool IsCRPAvailable();
		bool IsDetectedDocumentSizeAvailable(ESFunctionalUnitType funcUnit);

		bool IsAdvancedSotwareAutoCropEnabled();

		bool IsSoftwareAvailable();
		bool IsLongPaperAvailable();


		ESDictionary GetEngineKeyValues() override;

		bool GetProcessSoftwareAutoCrop()
		{
			return processSoftwareAutoCrop_;
		}

		bool processSoftwareAutoCrop_;
	};

}//namespace epsonscan
