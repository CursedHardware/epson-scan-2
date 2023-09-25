#pragma once

#include <libusb-1.0/libusb.h>
#include "../interfaceImpl.h"
#include "../interface.h"

class USBInterfaceImpl : public IInterfaceImpl
{
public:
	USBInterfaceImpl();
	virtual ~USBInterfaceImpl();

    virtual void SetDelegate(IInterfaceDelegate *delegate);

	ESErrorCode Init(UInt16 vid, UInt16 pid);
	ESErrorCode Init(UInt16 vid, UInt16 pid, UInt8 busNumber, UInt8 deviceNumber);

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

private:
    bool IsMatchID(UInt16 vid, UInt16 pid, libusb_device* dev);
	bool IsMatchID(UInt16 vid, UInt16 pid, UInt8 busNumber, UInt8 deviceNumber , libusb_device *dev);

	ESErrorCode SetupInterface();

private:
	IInterfaceDelegate *m_delegate;
	libusb_device_handle *m_handle;
	uint16_t m_vid;
	uint16_t m_pid;
	uint8_t  m_busNumber;
	uint8_t  m_deviceNumber;

	int m_cfg;
	int m_if;
	bool m_bKernelDriverDetached;
	int m_bulk_in;
	int m_bulk_out;
    int m_interrupt;

	static bool sm_isInitialized;
	static int  sm_nDefaultTimeout;
	static int  sm_nConnectionCount;
	static libusb_context *sm_ctx;
};
