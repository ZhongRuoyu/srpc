#ifndef SRPC_NETWORK_SERVER_SOCKET_H_
#define SRPC_NETWORK_SERVER_SOCKET_H_

#include <functional>
#include <memory>

#include "srpc/network/socket.h"
#include "srpc/network/tcp_ip.h"
#include "srpc/protocol/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

class ServerSocket {
 public:
  [[nodiscard]] static Result<std::unique_ptr<ServerSocket>> New(u16 port);

  ServerSocket() = delete;
  ServerSocket(const ServerSocket &) = delete;
  ServerSocket(ServerSocket &&other) noexcept;

  ServerSocket &operator=(const ServerSocket &) = delete;
  ServerSocket &operator=(ServerSocket &&other) noexcept;

  ~ServerSocket();

  [[nodiscard]] const SocketAddress &Address() const;

  void Listen(
      const std::function<void(std::unique_ptr<Socket>)> &handler) const;

 private:
  ServerSocket(struct SocketAddress address, int descriptor);

  struct SocketAddress address_;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SERVER_SOCKET_H_
