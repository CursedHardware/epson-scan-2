#include "interface.h"
#include "interfaceImpl.h"
#include "interfaceFactory.h"

ESErrorCode CreateInterface(const ES_CHAR *pszJSON, IInterface** ppInterface)
{
    return InterfaceFactory::Create(pszJSON, ppInterface);
}
    

Interface::Interface(ESConnectionType connection_type, std::unique_ptr<IInterfaceImpl> impl)
: connection_type_(connection_type)
, impl_(std::move(impl))
, delegate_(nullptr)
{
	if(!impl_){
		throw std::invalid_argument("implimentation class is null.");
	}
    impl_->SetDelegate(this);
}

void Interface::DestroyInstance()
{
    delegate_ = nullptr;
    impl_ = nullptr;
    delete this;
}

ESConnectionType Interface::ConnectionType() const
{
    return connection_type_;
}

void Interface::SetDelegate(IInterfaceDelegate *delegate)
{
    delegate_ = delegate;
}

ESErrorCode Interface::Open()
{
    return impl_->Open();
}

bool Interface::IsOpened()
{
    return impl_->IsOpened();
}

void Interface::Close()
{
     impl_->Close();
}

ESErrorCode Interface::Read(PESByte outBytes, UInt32 length)
{
    return impl_->Read(outBytes, length);
}

ESErrorCode Interface::Write(PESByte inBytes, UInt32 length)
{
    return impl_->Write(inBytes, length);
}

ESErrorCode Interface::Write(PESByte inBytes, UInt32 length, UInt32 committedReadLength)
{
    return impl_->Write(inBytes, length, committedReadLength);
}

bool Interface::IsInterruptSupported()
{
    return impl_->IsInterruptSupported();
}

void Interface::ReceiveInterruptEvent()
{
    impl_->ReceiveInterruptEvent();
}

bool Interface::IsSupportsExtendedTransfer()
{
    return impl_->IsSupportsExtendedTransfer();
}

ESErrorCode Interface::StartExtendedTransferWithBlocks(UInt32 blocks, UInt32 bytesPerBlock, UInt32 bytesOfLastBlock)
{
    return impl_->StartExtendedTransferWithBlocks(blocks, bytesPerBlock, bytesOfLastBlock);
}

ESErrorCode Interface::ReadExtendedTransfer(PESByte theBytes, UInt32 length)
{
    return impl_->ReadExtendedTransfer(theBytes, length);
}

ESErrorCode Interface::AbortExtendedTransferWithCode(UInt8 code)
{
    return impl_->AbortExtendedTransferWithCode(code);
}

bool Interface::IsAvoidsAckWhileImageTransfer()
{
    return impl_->IsAvoidsAckWhileImageTransfer();
}
int Interface::WaitTimeAfterFinalize()
{
    return impl_->WaitTimeAfterFinalize();
}

void CALLBACK Interface::DidPressButton(UInt8 buttonNumber)
{
    if(delegate_) {
        delegate_->DidPressButton(buttonNumber);
    }
}

void CALLBACK Interface::DidRequestStartScanning()
{
    if (delegate_){
        delegate_->DidRequestStartScanning();
    }
}

void CALLBACK Interface::DidRequestStopScanning()
{
    if (delegate_){
        delegate_->DidRequestStopScanning();
    }
}

void CALLBACK Interface::DidRequestStartOrStop()
{
    if(delegate_){
        delegate_->DidRequestStartOrStop();
    }
}

void CALLBACK Interface::DidRequestStop()
{
    if(delegate_){
        delegate_->DidRequestStop();
    }
}

void CALLBACK Interface::DidReceiveServerError()
{
    if(delegate_){
        delegate_->DidReceiveServerError();
    }
}

void CALLBACK Interface::DidDisconnect()
{
    if(delegate_){
        delegate_->DidDisconnect();
    }
}

void CALLBACK Interface::DidRequestPushScanConnection()
{
    if(delegate_){
       delegate_->DidRequestPushScanConnection();
    }
}

BOOL CALLBACK Interface::ShouldPreventTimeout()
{
    if(delegate_){
        return delegate_->ShouldPreventTimeout();
    }
    return FALSE;
}

void CALLBACK Interface::DidTimeout()
{
    if(delegate_){
        delegate_->DidTimeout();
    }
}

void CALLBACK Interface::IsReservedByHost(IInterface *interface, const ES_CHAR *address)
{
    if(delegate_){
        delegate_->IsReservedByHost(this, address);
    }
}

BOOL CALLBACK Interface::ShouldStopScanning(IInterface* interface)
{
    if(delegate_){
        return delegate_->ShouldStopScanning(this);
    }
    return FALSE;
}

void CALLBACK Interface::DidRequestGetImageData()
{
    if(delegate_){
        delegate_->DidRequestGetImageData();
    }
}

void CALLBACK Interface::DidNotifyStatusChange()
{
    if(delegate_){
        delegate_->DidNotifyStatusChange();
    }
}

void CALLBACK Interface::DeviceCommunicationError(ESErrorCode err)
{
    if(delegate_){
        delegate_->DeviceCommunicationError(err);
    }
}
