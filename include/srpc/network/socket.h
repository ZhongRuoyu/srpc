#ifndef SRPC_NETWORK_SOCKET_H_
#define SRPC_NETWORK_SOCKET_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "srpc/network/tcp_ip.h"
#include "srpc/protocol/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

class Socket {
 public:
  friend class ServerSocket;

  [[nodiscard]] static Result<std::unique_ptr<Socket>> New(
      const std::string &address, int port);

  Socket() = delete;
  Socket(const Socket &) = delete;
  Socket(Socket &&other) noexcept;

  Socket &operator=(const Socket &) = delete;
  Socket &operator=(Socket &&other) noexcept;

  ~Socket();

  [[nodiscard]] operator int() const;

  [[nodiscard]] const SocketAddress &SocketAddress() const;

  [[nodiscard]] Result<i64> Send(const std::vector<std::byte> &msg) const;
  [[nodiscard]] Result<std::vector<std::byte>> Receive() const;

 private:
  Socket(struct SocketAddress socket_address, int descriptor);

  struct SocketAddress socket_address_;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SOCKET_H_
