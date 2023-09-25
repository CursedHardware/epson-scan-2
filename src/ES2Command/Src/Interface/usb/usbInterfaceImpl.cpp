#include "usbInterfaceImpl.h"
#include "Utils/ESDebugLog.h"

bool USBInterfaceImpl::sm_isInitialized    = false;
int  USBInterfaceImpl::sm_nDefaultTimeout  = 60 * 1000; // 60sec
int USBInterfaceImpl::sm_nConnectionCount  = 0;
libusb_context* USBInterfaceImpl::sm_ctx   = nullptr;

USBInterfaceImpl::USBInterfaceImpl() 
: m_handle(nullptr)
, m_vid(0)
, m_pid(0)
, m_cfg(-1)
, m_if(-1)
, m_bKernelDriverDetached(false)
, m_bulk_in(-1)
, m_bulk_out(-1)
, m_interrupt(-1)
, m_delegate(nullptr)
, m_deviceNumber(0)
, m_busNumber(0)
{
    if (!sm_isInitialized){
        int res = libusb_init(&sm_ctx);
        sm_isInitialized = !res;

        if (res){
            sm_ctx = nullptr;
            throw std::runtime_error("unable to initialise USB support");
        }

        libusb_set_debug(sm_ctx, 3);
    }
    ++sm_nConnectionCount;
}

USBInterfaceImpl::~USBInterfaceImpl()
{
	try {
		Close();

		if (0 == --sm_nConnectionCount) {
			if (sm_ctx)
			{
				libusb_exit(sm_ctx);
				sm_ctx = nullptr;
				sm_isInitialized = false;
			}
		}
	} catch(...){}
}

void USBInterfaceImpl::SetDelegate(IInterfaceDelegate *delegate)
{
    m_delegate = delegate;
}

ESErrorCode USBInterfaceImpl::Init(UInt16 vid, UInt16 pid, UInt8 busNumber, UInt8 deviceNumber)
{
	libusb_device **haystack;
	ssize_t cnt = libusb_get_device_list(sm_ctx, &haystack);

	m_vid = m_pid = 0;
	for (ssize_t i = 0; i < cnt; i++){
		if ( IsMatchID(vid, pid, busNumber, deviceNumber, haystack[i]) ){
			m_vid = vid;
			m_pid = pid;
            m_deviceNumber = deviceNumber;
            m_busNumber = busNumber;
			break;
		}
	}
	libusb_free_device_list(haystack, 1);

	return ( (m_vid!=0 && m_pid!=0) ? kESErrorNoError : kESErrorFatalError);
}

ESErrorCode USBInterfaceImpl::Init(UInt16 vid, UInt16 pid)
{
	libusb_device **haystack;
	ssize_t cnt = libusb_get_device_list(sm_ctx, &haystack);

	m_vid = m_pid = 0;
	for (ssize_t i = 0; i < cnt; i++){
		if ( IsMatchID(vid, pid, haystack[i]) ){
			m_vid = vid;
			m_pid = pid;
			break;
		}
	}
	libusb_free_device_list(haystack, 1);

	return ( (m_vid!=0 && m_pid!=0) ? kESErrorNoError : kESErrorFatalError);
}

bool USBInterfaceImpl::IsMatchID(UInt16 vid, UInt16 pid, libusb_device *dev)
{
	struct libusb_device_descriptor dd;
	int res = libusb_get_device_descriptor (dev, &dd);
	if (res || vid != dd.idVendor || pid != dd.idProduct){
		return false;
	}
	return true;
}

bool USBInterfaceImpl::IsMatchID(UInt16 vid, UInt16 pid, UInt8 busNumber, UInt8 deviceNumber , libusb_device *dev)
{
	struct libusb_device_descriptor dd;
	int res = libusb_get_device_descriptor (dev, &dd);
	if (res || vid != dd.idVendor || pid != dd.idProduct){
		return false;
	}
    if (libusb_get_bus_number(dev) != busNumber)
    {
        return false;
    }

    if (libusb_get_device_address(dev) != deviceNumber)
    {
        return false;
    }

	return true;
}

ESErrorCode USBInterfaceImpl::Open()
{
    ES_LOG_TRACE_FUNC();

    const int RETRY_OPENDEVICE = 5;
    const unsigned int INTERVAL_RETRY = 250;

    ESErrorCode err = kESErrorNoError;

    if (m_vid ==0 || m_pid ==0){
        err = kESErrorFatalError;
        goto BAIL;
    }

    {
        if (m_busNumber == 0 && m_deviceNumber == 0)
        {
            // For mutual exclusive access for IPP over USB.
            // Since ippusbd exits if driver tries USBDeviceOpen, then retry.
            for (int nRetry = 0; nRetry < RETRY_OPENDEVICE; nRetry++) {
                m_handle = libusb_open_device_with_vid_pid(sm_ctx, m_vid, m_pid);
                if (m_handle) {
                    break;
                }
                Sleep(INTERVAL_RETRY);
            }
        }else{
          libusb_device **haystack;
          ssize_t cnt = libusb_get_device_list(sm_ctx, &haystack);
          for (ssize_t i = 0; i < cnt; i++){
             if ( IsMatchID(m_vid, m_pid, m_busNumber, m_deviceNumber , haystack[i]) ){
               // For mutual exclusive access for IPP over USB.
               // Since ippusbd exits if driver tries USBDeviceOpen, then retry.
                for (int nRetry = 0; nRetry < RETRY_OPENDEVICE; nRetry++) {
                    int result = libusb_open(haystack[i], &m_handle);
                    if (result == 0) {
                        break;
                    }
                    Sleep(INTERVAL_RETRY);
                }
             }
         }
         libusb_free_device_list(haystack, 1);
       }
    }

    if (!m_handle){
        err = kESErrorFatalError;
        goto BAIL;
    }

    if (SetupInterface() != kESErrorNoError){
        err = kESErrorDeviceOpenError;
        goto BAIL;
    }

BAIL:
#ifdef WIN32
	ES_TRACE_LOG( ES_STRING("LEAVE %s"), __FUNCTIONW__ );
#else
	ES_TRACE_LOG( ES_STRING("LEAVE %s"), __FUNCTION__ );
#endif

    return err;
}

bool USBInterfaceImpl::IsOpened()
{
	return m_handle ? true : false;
}

void USBInterfaceImpl::Close(void)
{
    ES_LOG_TRACE_FUNC();

	if(m_if != -1){
		libusb_release_interface(m_handle, m_if);

		if (m_bKernelDriverDetached){
			libusb_attach_kernel_driver(m_handle, m_if);
		}
		m_cfg = -1;
		m_if = -1;
		m_bulk_in = -1;
		m_bulk_out = -1; 
        m_interrupt = -1;
	}
	if(m_handle){
		libusb_close(m_handle);
		m_handle = nullptr;
	}

#ifdef WIN32
	ES_TRACE_LOG( ES_STRING("LEAVE %s"), __FUNCTIONW__ );
#else
	ES_TRACE_LOG( ES_STRING("LEAVE %s"), __FUNCTION__ );
#endif
}

ESErrorCode USBInterfaceImpl::SetupInterface()
{
    libusb_device* dev = libusb_get_device(m_handle);
    if (!dev){
        return kESErrorFatalError;
    }

    struct libusb_device_descriptor dd;
    int res = libusb_get_device_descriptor(dev, &dd);
    if (res){
        return kESErrorFatalError;
    }

    uint8_t nNumCfg = dd.bNumConfigurations;

    int nCuffentCfg;
    res = libusb_get_configuration(m_handle, &nCuffentCfg);
    if (res){
        return kESErrorFatalError;
    }

    for(uint8_t nCfgIndex = 0; nCfgIndex < nNumCfg; ++nCfgIndex){

        m_cfg = -1;
        struct libusb_config_descriptor *cd = nullptr;
        res = libusb_get_config_descriptor(dev, nCfgIndex, &cd);
        if (res){
            libusb_free_config_descriptor(cd);
            return kESErrorFatalError;
        }

        if (nCuffentCfg != cd->bConfigurationValue){
            res = libusb_set_configuration(m_handle, 1);
            if (res){
                libusb_free_config_descriptor(cd);
                return kESErrorFatalError;
            }
        }

        uint8_t nNumIf = cd->bNumInterfaces;
        for(uint8_t nIfIndex = 0; nIfIndex < cd->bNumInterfaces; ++nIfIndex){

            m_if = -1;
            for(uint8_t a = 0; a < cd->interface[nIfIndex].num_altsetting; ++a){   
                const struct libusb_interface_descriptor *ifd = &cd->interface[nIfIndex].altsetting[a]; 

                if (ifd->bInterfaceClass != 0xFF ||
                    // ifd->bInterfaceSubClass == 0xFF || // requested subclass
                    ifd->bInterfaceProtocol != 0xFF) {
                    continue;
                }

                bool bKernelDriverDetached = false;
                if ( libusb_kernel_driver_active(m_handle, ifd->bInterfaceNumber) == 1){
                    res = libusb_detach_kernel_driver(m_handle, ifd->bInterfaceNumber); 
                    if (res) {
                        libusb_free_config_descriptor(cd);
                        return kESErrorFatalError;
                    }
                    bKernelDriverDetached = true;
                }
                res = libusb_claim_interface(m_handle, ifd->bInterfaceNumber);
                if (res){
                    if (bKernelDriverDetached){
                        libusb_attach_kernel_driver(m_handle, ifd->bInterfaceNumber);
                    }
                    libusb_free_config_descriptor(cd);
                    return kESErrorFatalError;
                }

                m_bulk_in = -1; m_bulk_out = -1; m_interrupt = -1;
                for (int n = 0; n < ifd->bNumEndpoints; ++n) {
                    const struct libusb_endpoint_descriptor *ep = &ifd->endpoint[n];

                    if (LIBUSB_TRANSFER_TYPE_BULK == (LIBUSB_TRANSFER_TYPE_MASK & ep->bmAttributes)){
                        if (LIBUSB_ENDPOINT_DIR_MASK & ep->bEndpointAddress) {
                            m_bulk_in = ep->bEndpointAddress;
                        } else{
                            m_bulk_out = ep->bEndpointAddress;
                        }
                    } 
                    else if(LIBUSB_TRANSFER_TYPE_INTERRUPT == (LIBUSB_TRANSFER_TYPE_MASK & ep->bmAttributes)){
                        m_interrupt = ep->bEndpointAddress;
                    }
                }

                if (m_bulk_in != -1 && m_bulk_out != -1){
                    m_if = ifd->bInterfaceNumber;
                    m_bKernelDriverDetached = bKernelDriverDetached;
                    break;
                }
                
                libusb_release_interface(m_handle, ifd->bInterfaceNumber);
                if (bKernelDriverDetached){
                    libusb_attach_kernel_driver(m_handle, ifd->bInterfaceNumber);
                }
            }
            if (m_if != -1){
                m_cfg = cd->bConfigurationValue;
                break;
            }
        }

        libusb_free_config_descriptor(cd);
        if (m_cfg != -1){
            break;
        }
    }

    return ( (m_if!=-1 && m_bulk_in!=-1 && m_bulk_out!=-1) ? kESErrorNoError : kESErrorFatalError);
}

ESErrorCode USBInterfaceImpl::Read(PESByte outBytes, UInt32 length)
{
    ESErrorCode ret = kESErrorNoError;
    if (IsOpened()){
        int transferred;
        int err = libusb_bulk_transfer (m_handle, m_bulk_in, outBytes, length,
                                        &transferred, sm_nDefaultTimeout);

        if (LIBUSB_ERROR_PIPE == err) {
            err = libusb_clear_halt (m_handle, m_bulk_in);
        }
        if (err){
            ret = kESErrorDataReceiveFailure;
        }
    } else {
        ret = kESErrorDataReceiveFailure;
    }
    return ret;
}

ESErrorCode USBInterfaceImpl::Write(PESByte inBytes, UInt32 length)
{
    ESErrorCode ret = kESErrorNoError;
    if (IsOpened()){
        int transferred;
        int err = libusb_bulk_transfer (m_handle, m_bulk_out, inBytes, length,
                                        &transferred, sm_nDefaultTimeout);

        if (LIBUSB_ERROR_PIPE == err){
            err = libusb_clear_halt (m_handle, m_bulk_out);
        }
        if (err){
            ret = kESErrorDataSendFailure;
        }
    } else {
        ret = kESErrorDataSendFailure;
    }
    return ret;
}

ESErrorCode USBInterfaceImpl::Write(PESByte inBytes, UInt32 length, UInt32 committedReadLength)
{
    return Write(inBytes, length);
}

bool USBInterfaceImpl::IsInterruptSupported()
{
    return (m_interrupt != -1);
}

// USB Interruptリクエスト種別
#define EP_INTERRPT_IDX_EVENT_ID				0x01
#define EP_INTERRPT_IDX_REQUEST_STOP			0x02
#define EP_INTERRPT_IDX_REQUEST_DISCONNECT		0x03
#define EP_INTERRPT_IDX_REQUEST_PUSHSCAN		0x04
#define EP_INTERRPT_IDX_REQUEST_GET_IMAGEDATA	0x05
#define EP_INTERRPT_IDX_NOTIFY_STATUS_CHANGE	0x83

void USBInterfaceImpl::ReceiveInterruptEvent()
{
    const int INTERRUPT_TIMEOUT = 1000;

    if (!IsOpened() || !IsInterruptSupported()){
        return;
    }

    unsigned char bytes[8] = { 0 };
    int transferred;
    int err = libusb_interrupt_transfer(m_handle, m_interrupt, bytes, 8, &transferred, INTERRUPT_TIMEOUT);
    if (err < 0){
        if (err != LIBUSB_ERROR_TIMEOUT) {
            ES_INFO_LOG(ES_STRING("Interrupt read error %d"), err);

            ESErrorCode err = kESErrorDataReceiveFailure;
            if ( m_delegate ){
                m_delegate->DeviceCommunicationError( err );
            }
        }
        return;
    }

    ES_INFO_LOG(ES_STRING("Interrupt read EventType:0x%X EventId:0x%X"), bytes[0], bytes[1]);
    
    switch(bytes[0])
    {
        case EP_INTERRPT_IDX_EVENT_ID:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is Pull Scan Request"));
                if ( m_delegate ){
                    m_delegate->DidPressButton( bytes[1] );
                }
            }
            break;
        case EP_INTERRPT_IDX_REQUEST_STOP:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is Stop Request"));
                if ( m_delegate ){
                    m_delegate->DidRequestStop();
                }
            }
            break;
        case EP_INTERRPT_IDX_REQUEST_DISCONNECT:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is Disconnect"));
                if ( m_delegate ){
                    m_delegate->DidDisconnect();
                }
            }
            break;    
        case EP_INTERRPT_IDX_REQUEST_PUSHSCAN:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is Push Scan"));
                if ( m_delegate ){
                    m_delegate->DidRequestPushScanConnection();
                }
            }
            break;
        case EP_INTERRPT_IDX_REQUEST_GET_IMAGEDATA:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is Get Image"));
                if ( m_delegate ){
                    m_delegate->DidRequestGetImageData();
                }
            }
            break;
        case EP_INTERRPT_IDX_NOTIFY_STATUS_CHANGE:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is Status Change"));
                if ( m_delegate ){
                    m_delegate->DidNotifyStatusChange();
                }
            }
            break;
        default:
            {
                ES_INFO_LOG(ES_STRING("Interrupt received data is not valid"));
            }
            break;
    }
}

bool USBInterfaceImpl::IsSupportsExtendedTransfer()
{
    return false;
}
ESErrorCode USBInterfaceImpl::StartExtendedTransferWithBlocks(UInt32 blocks, UInt32 bytesPerBlock, UInt32 bytesOfLastBlock)
{
	return kESErrorFatalError;
}
ESErrorCode USBInterfaceImpl::ReadExtendedTransfer(PESByte theBytes, UInt32 length)
{
	return kESErrorFatalError;
}
ESErrorCode USBInterfaceImpl::AbortExtendedTransferWithCode(UInt8 code)
{
	return kESErrorFatalError;
}
bool USBInterfaceImpl::IsAvoidsAckWhileImageTransfer()
{
	return false;
}
int USBInterfaceImpl::WaitTimeAfterFinalize()
{
	return 0;
}
