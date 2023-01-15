#include "srpc/network/tcp_ip.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cassert>

#include "srpc/protocol/integers.h"

namespace srpc {

SocketAddress GetSocketAddress(const sockaddr *addr) {
  switch (addr->sa_family) {
    case AF_INET: {
      char sin_addr[INET_ADDRSTRLEN];
      inet_ntop(addr->sa_family, &(((const sockaddr_in *)addr)->sin_addr),
                sin_addr, sizeof(sin_addr));
      u16 sin_port = ntohs(((const sockaddr_in *)addr)->sin_port);
      return {
          .protocol = IPv4,
          .address = sin_addr,
          .port = sin_port,
      };
    }
    case AF_INET6: {
      char sin6_addr[INET6_ADDRSTRLEN];
      inet_ntop(addr->sa_family, &(((const sockaddr_in6 *)addr)->sin6_addr),
                sin6_addr, sizeof(sin6_addr));
      u16 sin6_port = ntohs(((const sockaddr_in6 *)addr)->sin6_port);
      return {
          .protocol = IPv6,
          .address = sin6_addr,
          .port = sin6_port,
      };
    }
  }
  assert(false);
}

}  // namespace srpc
