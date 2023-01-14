#ifndef SRPC_NETWORK_TCP_IP_H_
#define SRPC_NETWORK_TCP_IP_H_

#include <sys/socket.h>

#include <string>

namespace srpc {

std::string GetInAddr(const sockaddr *addr);

int GetInPort(const sockaddr *addr);

}  // namespace srpc

#endif  // SRPC_NETWORK_TCP_IP_H_
