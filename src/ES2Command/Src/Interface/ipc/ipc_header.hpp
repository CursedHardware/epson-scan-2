#pragma once

#include <cstdint>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace ipc {

enum {
  ipc_hdr_type_open = 4,
  ipc_hdr_type_close,
  ipc_hdr_type_write,
  ipc_hdr_type_read,
  ipc_hdr_type_status,
  ipc_hdr_type_ext_transfer_start_with_block,
  ipc_hdr_type_ext_transfer_read,
  ipc_hdr_type_ext_transfer_abort,
};
enum{
  status_interrupt_supported = 1,
  status_extended_transfer_supported
};

enum interrupt_event_type : uint32_t {
  event_reserved_by_host = 1,
	event_did_press_button,
  event_request_start_scanning,
  event_request_stop_scanning,
  event_request_start_or_stop,
  event_request_stop,
  event_did_timeout = 100,
  event_did_disconnect,
  event_receive_server_err,
  event_device_comunication_err,

  ask_is_should_prevent_timeout = 200,
};

#define IPC_INTERRUPT_DATA_FILE "interrupt.dat"
#define IPC_INTERRUPT_DATA_SIZE (32)
#define IPC_SHARED_ID    (30)
#define IPC_SEMAHORE_KEY (30)

#pragma pack(push, 1)
struct ipc_header
{
  uint32_t _token;
  uint32_t _type;
  uint32_t _error;
  int32_t _size;
  uint32_t _extension;
};

struct ext_transfer_block_info{
  uint32_t _blocks;
  uint32_t _bytes_per_block;
  uint32_t _bytes_of_lastblock;
};

struct ipc_interrupt_event_data
{
  interrupt_event_type _type;
  uint8_t _data[IPC_INTERRUPT_DATA_SIZE];

  uint32_t _recv_result;
};
#pragma pack(pop)

inline void hdr_token(struct ipc_header& hdr, uint32_t token){
  hdr._token = htonl(token);
}
inline uint32_t hdr_token(struct ipc_header& hdr){
  return ntohl(hdr._token);
}
inline void hdr_type(struct ipc_header& hdr, uint32_t type){
  hdr._type = htonl(type);
}
inline uint32_t hdr_type(struct ipc_header& hdr){
  return ntohl(hdr._type);
}
inline void hdr_error(struct ipc_header& hdr, uint32_t error){
  hdr._error = htonl(error);
}
inline uint32_t hdr_error(struct ipc_header& hdr){
  return ntohl(hdr._error);
}
inline void hdr_size(struct ipc_header& hdr, int32_t size){
  hdr._size = htonl(size);
}
inline int32_t hdr_size(struct ipc_header& hdr){
  return ntohl(hdr._size);
}
inline void hdr_extension(struct ipc_header& hdr, uint32_t extension){
  hdr._extension = htonl(extension);
}
inline uint32_t hdr_extension(struct ipc_header& hdr){
  return ntohl(hdr._extension);
}

inline void ntoh(struct ext_transfer_block_info& info)
{
  info._blocks             = ntohl(info._blocks);
  info._bytes_per_block    = ntohl (info._bytes_per_block);
  info._bytes_of_lastblock = ntohl(info._bytes_of_lastblock);
}

inline void hton(struct ext_transfer_block_info& info)
{
  info._blocks             = htonl(info._blocks);
  info._bytes_per_block    = htonl (info._bytes_per_block);
  info._bytes_of_lastblock = htonl(info._bytes_of_lastblock);
}

}; // namespace ipc