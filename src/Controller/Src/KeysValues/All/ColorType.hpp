#pragma once
#include "Key.hpp"
#include "EpsonScan2.h"
#include "ESImageInfo.h"

namespace epsonscan {

	class ColorType : public Key<SDIInt> 
	{
		public:

		explicit ColorType(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) , 
															 isSupportHardwareColorTypeAuto_(false),
															 scanGammaMode_(0),
															 saveGammaMode_(0)
		{

		}

		std::string GetName() const override 
		{
			return std::string(kSDIColorTypeKey);
		}


		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESNumber GetScanCounterColorType();
		ESNumber GetColorType();
		ESDictionary GetEngineKeyValues() override;

		bool IsHardawaDropout();
		bool IsSoftwareDropout();

		bool IsSupportHardwareColorTypeAuto();

		ESNumber GetScanGammaMode() const
		{
			return scanGammaMode_;
		}

		ESNumber GetSaveGammaMode() const 
		{
			return saveGammaMode_;
		}

		private:
			void GetGammaTable(ESGammaTable& outGammaTable);
			bool isSupportHardwareColorTypeAuto_;
			ESNumber scanGammaMode_;
			ESNumber saveGammaMode_;
	};

}//namespace epsonscan
