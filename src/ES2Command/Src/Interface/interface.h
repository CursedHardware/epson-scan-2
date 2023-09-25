#pragma once

#include "IInterface.h"
#include <memory>

class IInterfaceImpl;

class Interface : public IInterface, public IInterfaceDelegate
{
protected:
    Interface();
 public:
    explicit Interface(ESConnectionType connection_type, std::unique_ptr<IInterfaceImpl> impl);

    //
    // IInterface methods
    //
    virtual void DestroyInstance();

    virtual ESConnectionType ConnectionType() const;

    virtual void SetDelegate(IInterfaceDelegate *delegate);

    virtual ESErrorCode Open();
    virtual bool IsOpened();
    virtual void Close();

    virtual ESErrorCode Read(PESByte outBytes, UInt32 length);
    virtual ESErrorCode Write(PESByte inBytes, UInt32 length);
    virtual ESErrorCode Write(PESByte inBytes, UInt32 length, UInt32 committedReadLength);

    virtual bool IsInterruptSupported();
	virtual void ReceiveInterruptEvent();

    virtual bool IsSupportsExtendedTransfer();
    virtual ESErrorCode StartExtendedTransferWithBlocks(UInt32 blocks, UInt32 bytesPerBlock, UInt32 bytesOfLastBlock);
    virtual ESErrorCode ReadExtendedTransfer(PESByte theBytes, UInt32 length);
    virtual ESErrorCode AbortExtendedTransferWithCode(UInt8 code);

    virtual bool IsAvoidsAckWhileImageTransfer();
    virtual int WaitTimeAfterFinalize();

    //
    // InterfaceDelegate methods
    //
	virtual void CALLBACK DidPressButton(UInt8 buttonNumber);
	virtual void CALLBACK DidRequestStartScanning();
	virtual void CALLBACK DidRequestStopScanning();
	virtual void CALLBACK DidRequestStartOrStop();
	virtual void CALLBACK DidRequestStop();
	virtual void CALLBACK DidReceiveServerError();
	virtual void CALLBACK DidDisconnect();
	virtual void CALLBACK DidRequestPushScanConnection();
	virtual BOOL CALLBACK ShouldPreventTimeout();
	virtual void CALLBACK DidTimeout();
	virtual void CALLBACK IsReservedByHost(IInterface *interface, const ES_CHAR *address);
	virtual BOOL CALLBACK ShouldStopScanning(IInterface* interface);
	virtual void CALLBACK DidRequestGetImageData();
	virtual void CALLBACK DidNotifyStatusChange();

	virtual void CALLBACK DeviceCommunicationError(ESErrorCode err);

private:
    ESConnectionType connection_type_;
    std::unique_ptr<IInterfaceImpl> impl_;
    IInterfaceDelegate *delegate_;
};
