#pragma once

#include "IInterface.h"

class IInterfaceImpl
{
public:
    virtual ~IInterfaceImpl(){}

    virtual void SetDelegate(IInterfaceDelegate *delegate) = 0;

    virtual ESErrorCode Open() = 0;
    virtual bool IsOpened() = 0;
    virtual void Close() = 0;

    virtual ESErrorCode Read(PESByte outBytes, UInt32 length) = 0;
    virtual ESErrorCode Write(PESByte inBytes, UInt32 length) = 0;
    virtual ESErrorCode Write(PESByte inBytes, UInt32 length, UInt32 committedReadLength) = 0;

    virtual bool IsInterruptSupported() = 0;
    virtual void ReceiveInterruptEvent() = 0;

    virtual bool IsSupportsExtendedTransfer() = 0;
    virtual ESErrorCode StartExtendedTransferWithBlocks(UInt32 blocks, UInt32 bytesPerBlock, UInt32 bytesOfLastBlock) = 0;
    virtual ESErrorCode ReadExtendedTransfer(PESByte theBytes, UInt32 length) = 0;
    virtual ESErrorCode AbortExtendedTransferWithCode(UInt8 code) = 0;

    virtual bool IsAvoidsAckWhileImageTransfer() = 0;
    virtual int WaitTimeAfterFinalize() = 0;
};
