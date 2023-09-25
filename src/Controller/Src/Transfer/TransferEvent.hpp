#pragma once
#include <Image.hpp>
#include <Utility.hpp>
#include "EpsonScan2.h"

namespace epsonscan {

enum EventType
{
   kEventTypePaperJam,
   kEventTypeImage,
   kEventTypeComplete,
   kEventTypeCancel,
   kEventTypeAFMStartConituousScan,
   kEventTypeAFMStopContinuousScan
};

class TransferEvent
{
public:
    TransferEvent(EventType type, Image* image, SDIError error) : type_(type), image_(image), error_(error)
    {
        SDI_TRACE_LOG("Enter");
        if (image_) 
        {
            image_->AddReference();
        }
        SDI_TRACE_LOG("Leave");
    }
    
    TransferEvent(EventType type, SDIError error) : type_(type), image_(nullptr), error_(error)
    {
         SDI_TRACE_LOG("Enter");
         SDI_TRACE_LOG("Leave");
    }

    TransferEvent(const TransferEvent& transferEvent)
    {
        image_ =  transferEvent.GetImage();
        if(image_){
            image_->AddReference();
        }
        type_ = transferEvent.GetType();
        error_ = transferEvent.GetError();
    }

    virtual ~TransferEvent()
    {
        SDI_TRACE_LOG("Destroy TransferEvent");
        Release();
    }

    void Release()
    {
        if (image_)
        {
            image_->Release();
        }
    }

	Image* GetImage() const
	{
		return image_;
	}

    EventType GetType() const
    {
        return type_;
    }

    void SetError(SDIError error)
    {
        error_ = error; 
    }

    SDIError GetError() const 
    {
        return error_;
    }

private:
    Image* image_;
    EventType type_;
    SDIError error_;
};

} //epsonscan
