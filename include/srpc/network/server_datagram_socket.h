#ifndef SRPC_NETWORK_SERVER_DATAGRAM_SOCKET_H_
#define SRPC_NETWORK_SERVER_DATAGRAM_SOCKET_H_

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

#include "srpc/network/tcp_ip.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

class ServerDatagramSocket {
 public:
  [[nodiscard]] static Result<std::unique_ptr<ServerDatagramSocket>> New(
      u16 port);

  ServerDatagramSocket() = delete;
  ServerDatagramSocket(const ServerDatagramSocket &) = delete;
  ServerDatagramSocket(ServerDatagramSocket &&other) noexcept;

  ServerDatagramSocket &operator=(const ServerDatagramSocket &) = delete;
  ServerDatagramSocket &operator=(ServerDatagramSocket &&other) noexcept;

  ~ServerDatagramSocket();

  [[nodiscard]] const SocketAddress &Address() const;

  void Listen(
      const std::function<std::vector<std::byte>(
          const SocketAddress &, std::vector<std::byte>)> &handler) const;

 private:
  ServerDatagramSocket(SocketAddress address, int descriptor);

  SocketAddress address_;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SERVER_DATAGRAM_SOCKET_H_
