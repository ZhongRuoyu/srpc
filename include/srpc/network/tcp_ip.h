#ifndef SRPC_NETWORK_TCP_IP_H_
#define SRPC_NETWORK_TCP_IP_H_

#include <string>

#include "srpc/protocol/integers.h"

namespace srpc {

enum InternetProtocol {
  kIPv4 = 4,
  kIPv6 = 6,
};

struct SocketAddress {
  InternetProtocol protocol;
  std::string address;
  u16 port;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_TCP_IP_H_
