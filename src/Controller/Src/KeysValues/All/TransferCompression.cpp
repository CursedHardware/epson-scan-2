#include "TransferCompression.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>


namespace epsonscan {

void TransferCompression::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void TransferCompression::Reset()
{
	current_ = SupportCompressionTransfer();
}

void TransferCompression::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void TransferCompression::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
	ESIndexSet availableTransferCompression;
	capability.supportLevel = kSDISupportLevelNone;

	if (SupportCompressionTransfer() && SupportRAWTransfer())
	{
		AddList(capability, kSDITransferCompressionJPEG);
		AddAllList(capability, kSDITransferCompressionJPEG);

		AddList(capability, kSDITransferCompressionRAW);
		AddAllList(capability, kSDITransferCompressionRAW);


		capability.supportLevel = kSDISupportLevelAvailable;
	}else{
		capability.supportLevel = kSDISupportLevelNone;
	}
}

ESDictionary TransferCompression::GetEngineKeyValues()
{
	auto dict =  ESDictionary();

	if (SupportCompressionTransfer() && SupportRAWTransfer() == false)
	{
		dict[kESImageFormat] = (ESNumber)kESImageFormatJPEG;
	}else if (SupportCompressionTransfer() == false && SupportRAWTransfer())
	{
		dict[kESImageFormat] = (ESNumber)kESImageFormatRaw;
	}else{
		if (current_)
		{
			dict[kESImageFormat] = (ESNumber)kESImageFormatJPEG;
		}else{
			dict[kESImageFormat] = (ESNumber)kESImageFormatRaw;	
		}
	}

	//BufferSize
		ESNumber commandType = kPrvHRD_CommandType_ESCI2;
		dataProvider_->GetModelInfo()->GetValue(kPrvHRD_CommandType, commandType);
		if (commandType == kPrvHRD_CommandType_ESCI2)
		{
			ESNumber bufferSize = 0;
			dataProvider_->GetModelInfo()->GetValue(kPrvHRD_I2BSZ, bufferSize);
			if (bufferSize == 0)
			{
				dict[kESBufferSize] = (ESNumber)1048576;//1MB
			}else{
				dict[kESBufferSize] = (ESNumber)bufferSize;
			}
		}else{
			dict[kESBufferSize] = 262144;//256K
		}

	return dict;
}

bool TransferCompression::SupportCompressionTransfer()
{
	ESIndexSet availableTransferCompression;
	Scanner* scanner = dataProvider_->GetScanner().get();

	if(scanner &&  scanner->GetAvailableValueForKey(kESImageFormat, availableTransferCompression)) 
	{
		if (availableTransferCompression.find(kESImageFormatJPEG) != availableTransferCompression.end())
	   	{
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool TransferCompression::SupportRAWTransfer()
{
	ESIndexSet availableTransferCompression;
	Scanner* scanner = dataProvider_->GetScanner().get();

	if(scanner &&  scanner->GetAvailableValueForKey(kESImageFormat, availableTransferCompression)) 
	{
		if (availableTransferCompression.find(kESImageFormatRaw) != availableTransferCompression.end())
	   	{
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}
	
}//namespace epsonscan
