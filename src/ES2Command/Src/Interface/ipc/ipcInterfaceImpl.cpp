#include <cstdio>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <string>
#include <csignal>
#include <unistd.h>
#include <thread>
#include <algorithm>
#include <errno.h>

#include "ipcInterfaceImpl.h"
#include "CommonUtility/utils/PathUtils.h"
#include "Utils/ESDebugLog.h"

namespace ipc {

const double seconds = 1.0;
double IPCInterfaceImpl::default_timeout_ = 60 * seconds;

void kill_(pid_t pid_, int port_, int socket_, std::string name_);

static bool delay_elapsed (double t_sec)
{
    struct timespec t;
    t.tv_sec  =  t_sec;
    t.tv_nsec = (t_sec - t.tv_sec) * 1000000000;
    return 0 == nanosleep (&t, 0);
}

static void set_timeout (int socket, double t_sec)
{
    if (0 > socket) return;

    struct timeval t;
    t.tv_sec  =  t_sec;
    t.tv_usec = (t_sec - t.tv_sec) * 1000000;

    errno = 0;
    if (0 > setsockopt (socket, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof (t)))
    {
        ES_ERROR_LOG(ES_STRING("socket option: %s"), ESString(strerror(errno)).c_str());
    }
    errno = 0;
    if (0 > setsockopt (socket, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof (t)))
    {
        ES_ERROR_LOG(ES_STRING("socket option: %s"), ESString(strerror(errno)).c_str());
    }
    errno = 0;
    int flag = 1;
    setsockopt (socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof (flag));
}

IPCInterfaceImpl::IPCInterfaceImpl(std::string name)
: name_(name)
, pid_(-1)
, port_(-1)
, socket_(-1)
, id_(0)
, timeout_s_(default_timeout_)
, isExtendedTransferSupported_(false)
, isInterruptSupported_(false)
, delegate_(nullptr)
, interrupt_(nullptr)
{
    if(name_.empty()){
        throw std::runtime_error("not executable");
    }
}

IPCInterfaceImpl::~IPCInterfaceImpl()
{
    Close();
}

ESErrorCode IPCInterfaceImpl::Init(std::string udi, ESNumber nConnectionTimeout_s, ESNumber nTimeout_s)
{
    udi_ = udi;
    return  kESErrorNoError;
}

void IPCInterfaceImpl::SetDelegate(IInterfaceDelegate *delegate)
{
    delegate_ = delegate;
}

ESErrorCode IPCInterfaceImpl::Open()
{
    using namespace ES_CMN_FUNCS::PATH;

    std::lock_guard<std::recursive_mutex> lock(mtx_);

    if (IsOpened()){
        return kESErrorNoError;
    }

    {
        if (!fork_()) {
            ES_ERROR_LOG(ES_STRING("cannot fork"));
            Close();
            return kESErrorDeviceOpenError;
        }

        int tries_left = 5;
        while (!connect_()
            && 0 < --tries_left
            && delay_elapsed (1.0 * seconds));

        if (!tries_left){
            ES_ERROR_LOG(ES_STRING("cannot connect"));
            Close();
            return kESErrorDeviceOpenError;
        }
    }    
    
    try{
		std::string work_path = EPSON_WORK_PATH;

        if (!work_path.empty() && work_path[work_path.length()-1] != '/'){
            work_path += '/';
        }
        std::string dat_file(work_path);
        dat_file += IPC_INTERRUPT_DATA_FILE;

        if (!work_path.empty() && !ES_IsExistFolder(work_path)){
            ES_MakeFolder(work_path);
        }
        if(!ES_IsExistFile(dat_file)){
            FILE *fp;
            fp = fopen(dat_file.c_str(),"w");
            fclose(fp);
        }

        interrupt_.reset(new ipc_interrupt(delegate_, dat_file, IPC_SHARED_ID, IPC_SEMAHORE_KEY));
        interrupt_->start();

        ES_TRACE_LOG(ES_STRING("sem_key = %d"), interrupt_ ? interrupt_->sem_key() : -1);
    }catch(const std::runtime_error& e){
        ES_ERROR_LOG(ES_STRING("ipc_interrupt %s"), ESString(e.what()).c_str());
        if (interrupt_){
            interrupt_->stop();
            interrupt_.reset(nullptr);
        }
    }catch(...){
        ES_ERROR_LOG(ES_STRING("ipc_interrupt initialize unknown exception."));
        if (interrupt_){
            interrupt_->stop();
            interrupt_.reset(nullptr);
        }
    }

    if (!open_(interrupt_ ? interrupt_->sem_key() : -1)){
        ES_ERROR_LOG(ES_STRING("Failed open device."));
        Close();
        return kESErrorDeviceOpenError;
    }
    if (!get_status_()){
        ES_ERROR_LOG(ES_STRING("Failed get status."));
        Close();
        return kESErrorDeviceOpenError;
    }
    if (!IsInterruptSupported()) {
        if (interrupt_){
            interrupt_->stop();
            interrupt_.reset(nullptr);
        }
    } else {
        if (!interrupt_){
            isInterruptSupported_ = false;
        }
    }

    return kESErrorNoError;
}

bool IPCInterfaceImpl::IsOpened()
{
    return (id_ > 0) ? true : false;
}

void IPCInterfaceImpl::Close(void)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);

	try
	{
		if (IsOpened())
		{
			ipc_header hdr{ 0 };
			hdr_token(hdr, id_);
			hdr_type(hdr, ipc_hdr_type_close);
			streamsize n = send_message_(hdr, nullptr);
			if (0 > n)
			{
				ES_ERROR_LOG(ES_STRING("%s : failure closing connexion"), ESString(name_).c_str());
			}
			id_ = 0;
		}

		if (interrupt_) {
			interrupt_->stop();
			interrupt_.reset(nullptr);
		}

		if (pid_ > 0) {
			std::thread(kill_, pid_, port_, socket_, name_).join();
			pid_ = -1; port_ = -1; socket_ = -1;
		}
        {
            std::string work_path = EPSON_WORK_PATH;
            if (!work_path.empty() && work_path[work_path.length()-1] != '/'){
                work_path += '/';
            }
            std::string dat_file(work_path);
            dat_file += IPC_INTERRUPT_DATA_FILE;

            if(ES_CMN_FUNCS::PATH::ES_IsExistFile(dat_file)){
                try {
                    remove(dat_file.c_str());
                }
                catch(...) {}
            }
        }

	}
	catch(...){}
}

bool IPCInterfaceImpl::connect_()
{
    errno = 0;
    socket_ = socket (AF_INET, SOCK_STREAM, 0);
    if (0 > socket_) {
         ES_ERROR_LOG(ES_STRING("socket: %s"), ESString(strerror(errno)).c_str());
        return false;
    }

    set_timeout (socket_, 10 * seconds);

    struct sockaddr_in addr;
    memset (&addr, 0, sizeof (addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons (port_);
    addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    if (0 != connect (socket_, (struct sockaddr *) &addr, sizeof (addr)))
    {
        ES_ERROR_LOG(ES_STRING("connect: %s"), ESString(strerror(errno)).c_str());
        return false;
    }
    ES_TRACE_LOG(ES_STRING("connect_ success"));
    return true;
}

bool IPCInterfaceImpl::fork_()
{
    ES_LOG_TRACE_FUNC();

    errno = 0;
    int pipe_fd[2] ={};
    if (pipe(pipe_fd) < 0){
        ES_ERROR_LOG(ES_STRING("pipe: %s"), ESString(strerror(errno)).c_str());
        return false;
    }

    pid_ = fork();
    if (pid_ == 0) {
        signal (SIGTERM, SIG_IGN);
        signal (SIGINT , SIG_IGN);

        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) >= 0){
            if (execl(name_.c_str(), name_.c_str(), NULL) < 0){
                ES_ERROR_LOG(ES_STRING("%s[%xd] : %s"), ESString(name_).c_str(), getpid(), ESString(strerror(errno)).c_str());
            }
        } else {
            ES_ERROR_LOG(ES_STRING("%s[%xd] : %s"), ESString(name_).c_str(), getpid(), ESString(strerror(errno)).c_str());
        }

        // notify the parent process that we're done here
        write (pipe_fd[1], "-1\n", strlen("-1\n"));
        fsync (pipe_fd[1]);

        close (pipe_fd[1]);
        exit (EXIT_FAILURE);
    }

    bool result = true;   
    if (pid_ < 0){
        ES_ERROR_LOG(ES_STRING("fork: %s"), ESString(strerror(errno)).c_str());
        result = false;
    } else {
        // Check whether child process has (unexpectedly) exited.  We
        // don't want to have zombies in our closet ;-)
        pid_t w = waitpid (pid_, NULL, WNOHANG);
        if (w == -1) {
            ES_WARM_LOG(ES_STRING("waitpid: %s"), ESString(strerror(errno)).c_str());
        }
        else if (w != 0) {
            ES_ERROR_LOG(ES_STRING("%s[%d]: exited prematurely"), ESString(name_).c_str(), pid_);
            result = false;
        } else {
            FILE *fp = fdopen (pipe_fd[0], "rb");
            if (fp) {
                if (fscanf (fp, "%d", &port_) != 1){
                    ES_ERROR_LOG(ES_STRING("fscanf: %d, %s"), port_, ESString(strerror(errno)).c_str());
                } else {
                    ES_INFO_LOG(ES_STRING("port = %d"), port_);
                }
                fclose (fp);
            } else {
                ES_ERROR_LOG(ES_STRING("fdopen: %s"), ESString(strerror(errno)).c_str());
            }
        }
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    if (port_ < 0){
        result = false;
    }

    return result;
}

bool IPCInterfaceImpl::recv_reply(uint32_t* token)
{
    ipc_header hdr{0};
    hdr_token(hdr, id_);
    char *placeholder = nullptr;

    streamsize n = recv_message_(hdr, placeholder);
    if (placeholder) delete [] placeholder;
    if (token) *token = hdr_token(hdr);
    if (hdr_error(hdr) || n < 0) {
        ES_ERROR_LOG(ES_STRING("recv_reply err.(%d, %d, n)"), hdr_error(hdr), hdr_token(hdr), n);
        return false;
    }
    return true;
}

bool IPCInterfaceImpl::open_(key_t sem_key)
{
    ES_LOG_TRACE_FUNC();

    ipc_header hdr{0};
    hdr_type(hdr, ipc_hdr_type_open);
    if (sem_key > 0){
        hdr_extension(hdr, static_cast<uint32_t>(sem_key));
    }
    hdr_size(hdr, udi_.length());
    streamsize n = send_message_(hdr, udi_.c_str());
    if (n == hdr_size(hdr))
    {
        if (recv_reply(&id_)){
            ES_TRACE_LOG(ES_STRING("opened ipc::connexion to: %s"), udi_.c_str());
            set_timeout (socket_, default_timeout_);
            return true;
        } else {
            ES_ERROR_LOG(ES_STRING("recv_reply err"));
        }
    } else {
        ES_ERROR_LOG(ES_STRING("header size is invalid.(%lld:%d)"), n, hdr_size(hdr));
    }
    return false;
}

bool IPCInterfaceImpl::get_status_(uint32_t status_type, uint32_t& stat)
{
    set_timeout (socket_, timeout_s_);

    ipc_header hdr{0};
    hdr_token(hdr, id_);
    hdr_type(hdr, ipc_hdr_type_status);
    hdr_extension(hdr, status_type);
    streamsize n = send_message_(&hdr, sizeof(hdr));
	if (n <= 0) return false;

    n = recv_message_(&hdr, sizeof(hdr));
	if (n <= 0 || hdr_error(hdr) || hdr_size(hdr) != sizeof(stat)) return false;

    uint32_t stat_;
	n = recv_message_(&stat_, sizeof(stat_));
	if (n < 0) return false;
    stat = htonl(stat_);

    return true;
}

bool IPCInterfaceImpl::get_status_()
{
    uint32_t opt_value = 0;
    if(!get_status_(status_interrupt_supported, opt_value)){
        return false;
    }
    isInterruptSupported_ = static_cast<bool>(opt_value);
    ES_TRACE_LOG(ES_STRING("InterruptSupported %s"), isInterruptSupported_ ? ES_STRING("True") : ES_STRING("False"));

    opt_value = 0;
    if(!get_status_(status_extended_transfer_supported, opt_value)){
        return false;
    }
    isExtendedTransferSupported_ = static_cast<bool>(opt_value);
    ES_TRACE_LOG(ES_STRING("ExtendedTransferSupported %s"), isExtendedTransferSupported_ ? ES_STRING("True") : ES_STRING("False"));

    return true;
}

ESErrorCode IPCInterfaceImpl::Read(PESByte outBytes, UInt32 length)
{
    if (!outBytes) {return kESErrorInvalidParameter;}

    set_timeout (socket_, timeout_s_);

    ipc_header hdr{0};
    hdr_token(hdr, id_);
    hdr_type(hdr, ipc_hdr_type_read);
    hdr_extension(hdr, length);
    streamsize n = send_message_(&hdr, sizeof(hdr));
    if (n <= 0){
        ES_ERROR_LOG(ES_STRING("failer send ipc header : %ld"), n);
        return kESErrorDataSendFailure;
    }
    {
        ipc_header hdr{0};
        hdr_token(hdr, id_);
        char *reply = nullptr;
        recv_message_(hdr, reply);

        if (!hdr_error(hdr) && length == hdr_size(hdr) && reply)
        {
            memcpy_s(outBytes, length, reply, hdr_size(hdr));
            delete [] reply;

            return kESErrorNoError;
        }
        delete [] reply;
    }

    ES_ERROR_LOG(ES_STRING("failer read"));

    return kESErrorDataReceiveFailure;
}

ESErrorCode IPCInterfaceImpl::Write(PESByte inBytes, UInt32 length)
{
    return Write(inBytes, length, 0);
}

ESErrorCode IPCInterfaceImpl::Write(PESByte inBytes, UInt32 length, UInt32 committedReadLength)
{
    ipc_header hdr{0};
    hdr_token(hdr, id_);
    hdr_type(hdr, ipc_hdr_type_write);
    hdr_size(hdr, length);
    hdr_extension(hdr, committedReadLength);
    
    set_timeout (socket_, timeout_s_);
    streamsize n = send_message_(hdr, reinterpret_cast< const char* >(inBytes));
    if (n <= 0 || !recv_reply()){
        ES_ERROR_LOG(ES_STRING("failer ipc write : %ld"), n);
        return kESErrorDataSendFailure;
    }
    return kESErrorNoError;
}

bool IPCInterfaceImpl::IsInterruptSupported()
{
    return isInterruptSupported_;
}

void IPCInterfaceImpl::ReceiveInterruptEvent()
{
    if (interrupt_){
        interrupt_->NotifyInterruptEvent();
    }
}

bool IPCInterfaceImpl::IsSupportsExtendedTransfer()
{
    return isExtendedTransferSupported_;
}

ESErrorCode IPCInterfaceImpl::StartExtendedTransferWithBlocks(UInt32 blocks, UInt32 bytesPerBlock, UInt32 bytesOfLastBlock)
{
    if (!IsSupportsExtendedTransfer()){ return kESErrorFatalError; }

    ext_transfer_block_info exi;
    exi._blocks = blocks;
    exi._bytes_per_block = bytesPerBlock;
    exi._bytes_of_lastblock = bytesOfLastBlock;
    hton(exi);

    ipc_header hdr{0};
    hdr_token(hdr, id_);
    hdr_type(hdr, ipc_hdr_type_ext_transfer_start_with_block);
    hdr_size(hdr, sizeof(exi));

    set_timeout (socket_, timeout_s_);
    streamsize n = send_message_(hdr, reinterpret_cast< const char* >(&exi));
    if (n <= 0 || !recv_reply()){
        ES_ERROR_LOG(ES_STRING("failer ipc StartExtendedTransferWithBlocks : %ld"), n);
        return kESErrorDataSendFailure;
    }
	return kESErrorNoError;
}

ESErrorCode IPCInterfaceImpl::ReadExtendedTransfer(PESByte outBytes, UInt32 length)
{
    if (!IsSupportsExtendedTransfer()){ return kESErrorFatalError; }
    if (!outBytes) { return kESErrorInvalidParameter;}

    ipc_header hdr{0};
    hdr_token(hdr, id_);
    hdr_type(hdr, ipc_hdr_type_ext_transfer_read);
    hdr_extension(hdr, length);

    set_timeout (socket_, timeout_s_);
    streamsize n = send_message_(&hdr, sizeof(hdr));
    if (n <= 0){
        ES_ERROR_LOG(ES_STRING("failer send ipc header : %ld"), n);
        return kESErrorDataSendFailure;
    }
    {
        ipc_header hdr{0};
        hdr_token(hdr, id_);
        char *reply = nullptr;

        recv_message_(hdr, reply);

        if (!hdr_error(hdr) && length == hdr_size(hdr) && reply)
        {
            memcpy_s(outBytes, length, reply, hdr_size(hdr));
            delete [] reply;

            return kESErrorNoError;
        }
        delete [] reply;
    }

    ES_ERROR_LOG(ES_STRING("failer ReadExtendedTransfer error"));

    return kESErrorDataReceiveFailure;
}

ESErrorCode IPCInterfaceImpl::AbortExtendedTransferWithCode(UInt8 code)
{
    if (!IsSupportsExtendedTransfer()){ return kESErrorFatalError; }

    ipc_header hdr{0};
    hdr_token(hdr, id_);
    hdr_type(hdr, ipc_hdr_type_ext_transfer_abort);
    hdr_size(hdr, sizeof(code));

    set_timeout (socket_, timeout_s_);
    streamsize n = send_message_(hdr, reinterpret_cast< const char* >(&code));
    if (n <= 0 || !recv_reply()){
        ES_ERROR_LOG(ES_STRING("failer ipc AbortExtendedTransferWithCode : %ld"), n);
        return kESErrorDataSendFailure;
    }
	return kESErrorNoError;
}

streamsize IPCInterfaceImpl::send_message_(ipc_header hdr, const char* payload)
{
    streamsize n = 0;

    n = send_message_(&hdr, sizeof(hdr));
    if (0 >= n) return -1;

    if (hdr_size(hdr) <= 0) return 0;
    if (!payload) return -1;

    n = send_message_(payload, hdr_size(hdr));

    return n;
}

streamsize IPCInterfaceImpl::recv_message_(ipc_header& hdr, char *&payload)
{
    streamsize n = recv_message_(&hdr, sizeof(hdr));

    if (0 > n) return n;

    if (hdr_size(hdr) <= 0) return 0;

    char *buf = new char[hdr_size(hdr)];
    n = recv_message_(buf, hdr_size(hdr));
    payload = buf;

    return n;
}

streamsize IPCInterfaceImpl::send_message_(const void *data, streamsize size)
{
    if (size <= 0) return -1;

    streamsize n = 0;
    streamsize t = 1;
    const char* p = reinterpret_cast< const char* >(data);

    while (n < size && t > 0) 
    {
        t = write(socket_, p + n, size - n);
        if (0 > t) {
            return -1;
        } else {
            n += t;
        }
    }
    return n;
}

streamsize IPCInterfaceImpl::recv_message_(void *data, streamsize size)
{
    if (size <= 0) return -1;

    streamsize n = 0;
    streamsize t = 1;
    char*  p = reinterpret_cast< char* > (data);

    while (n < size && t > 0)
    {
        t = read(socket_, p + n, size - n);
        if (0 > t) {
            return -1;
        } else {
            n += t;
        }
    }

    return n;
}

streamsize IPCInterfaceImpl::write(int fd, const void *buf, streamsize count)
{
  sigset_t current, blocked;
  sigemptyset (&blocked);
  sigaddset (&blocked, SIGTERM);
  sigaddset (&blocked, SIGINT );
  sigprocmask (SIG_BLOCK, &blocked, &current);

  errno = 0;
  streamsize rv = ::write (fd, buf, count);
  if (0 > rv){
    ES_ERROR_LOG(ES_STRING("write failed: %s"), ESString(strerror(errno)).c_str());
  }
  sigprocmask (SIG_SETMASK, &current, NULL);

  return rv;
}

streamsize IPCInterfaceImpl::read (int fd, void *buf, streamsize count)
{
  sigset_t current, blocked;
  sigemptyset (&blocked);
  sigaddset (&blocked, SIGTERM);
  sigaddset (&blocked, SIGINT );
  sigprocmask (SIG_BLOCK, &blocked, &current);

  errno = 0;
  streamsize rv = ::read (fd, buf, count);
  if (0 > rv){
    ES_ERROR_LOG(ES_STRING("read failed: %s"), ESString(strerror(errno)).c_str());
  }

  sigprocmask (SIG_SETMASK, &current, NULL);

  return rv;
}

bool IPCInterfaceImpl::IsAvoidsAckWhileImageTransfer()
{
	return false;
}
int IPCInterfaceImpl::WaitTimeAfterFinalize()
{
	return 0;
}

void kill_(pid_t pid, int port, int socket, std::string name)
{
    ES_INFO_LOG(ES_STRING("terminating %s (port %d)"), ESString(name).c_str() , port);

    if (0 <= socket){
        if (0 != close (socket)) {
            ES_WARM_LOG(ES_STRING("close %s"), ESString(strerror(errno)).c_str());
        }
    }
    if (1 >= pid) { // nothing left to do
        return;
    }
    if (0 != kill (pid, SIGHUP)) {
        ES_ERROR_LOG(ES_STRING("kill %s"), ESString(strerror(errno)).c_str());
    }

    int status = 0;
    if (pid == waitpid (pid, &status, 0)) {
        ES_INFO_LOG(ES_STRING("waitpid %d"), status);
    } else {
        ES_ERROR_LOG(ES_STRING("waitpid %s"), ESString(strerror(errno)).c_str());
    }
}

} // namespace ipc
