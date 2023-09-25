#include "Key.hpp"
#include "EpsonScan2.h"

namespace epsonscan {

	class TransferCompression : public Key<SDIInt> 
	{
		public:

		explicit TransferCompression(IKeyDataProvider* dataProvider) : Key<SDIInt>(dataProvider) 
		{

		}

		std::string GetName() const override 
		{;
			return std::string(kSDITransferCompressionKey);
		}

		void SetValue(const SDIInt& intVal) override;
		void GetValue(SDIInt& outIntVal) override;
		void GetCapability(SDICapability &capability) override;

		void Reset() override;

		ESDictionary GetEngineKeyValues() override;

		bool SupportCompressionTransfer();
		bool SupportRAWTransfer();
	};

}//namespace epsonscan
