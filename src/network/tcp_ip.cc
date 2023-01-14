#include "network/tcp_ip.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cassert>
#include <string>

namespace srpc {

std::string GetInAddr(const sockaddr *addr) {
  char in_addr[INET6_ADDRSTRLEN];
  switch (addr->sa_family) {
    case AF_INET:
      inet_ntop(addr->sa_family, &(((const sockaddr_in *)addr)->sin_addr),
                in_addr, sizeof(in_addr));
      break;
    case AF_INET6:
      inet_ntop(addr->sa_family, &(((const sockaddr_in6 *)addr)->sin6_addr),
                in_addr, sizeof(in_addr));
      break;
    default: assert(false);
  }
  return in_addr;
}

int GetInPort(const sockaddr *addr) {
  switch (addr->sa_family) {
    case AF_INET: return ntohs(((const sockaddr_in *)addr)->sin_port);
    case AF_INET6: return ntohs(((const sockaddr_in6 *)addr)->sin6_port);
    default: assert(false);
  }
}

}  // namespace srpc
