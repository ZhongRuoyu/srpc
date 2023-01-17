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
      const std::string &address, u16 port);

  Socket() = delete;
  Socket(const Socket &) = delete;
  Socket(Socket &&other) noexcept;

  Socket &operator=(const Socket &) = delete;
  Socket &operator=(Socket &&other) noexcept;

  ~Socket();

  [[nodiscard]] const SocketAddress &Address() const;

  [[nodiscard]] Result<i64> Send(const std::vector<std::byte> &data) const;
  [[nodiscard]] Result<std::vector<std::byte>> Receive() const;

 private:
  Socket(SocketAddress address, int descriptor);

  SocketAddress address_;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SOCKET_H_
