#ifndef SRPC_NETWORK_SOCKET_H_
#define SRPC_NETWORK_SOCKET_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "protocol/integers.h"
#include "utils/result.h"

namespace srpc {

class Socket {
 public:
  friend class ServerSocket;

  [[nodiscard]] static Result<std::unique_ptr<Socket>> New(std::string address,
                                                           int port);

  Socket() = delete;
  Socket(const Socket &) = delete;
  Socket(Socket &&other) noexcept;

  Socket &operator=(const Socket &) = delete;
  Socket &operator=(Socket &&other) noexcept;

  ~Socket();

  [[nodiscard]] operator int() const;

  [[nodiscard]] const std::string &Address() const;
  [[nodiscard]] int Port() const;

  [[nodiscard]] Result<i64> Send(const std::vector<std::byte> &msg) const;
  [[nodiscard]] Result<std::vector<std::byte>> Receive() const;

 private:
  Socket(std::string address, int port, int descriptor);

  std::string address_;
  int port_ = -1;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SOCKET_H_
