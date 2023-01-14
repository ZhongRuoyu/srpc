#ifndef SRPC_NETWORK_SERVER_SOCKET_H_
#define SRPC_NETWORK_SERVER_SOCKET_H_

#include <functional>
#include <memory>
#include <string>

#include "network/socket.h"
#include "utils/result.h"

namespace srpc {

class ServerSocket {
 public:
  [[nodiscard]] static Result<std::unique_ptr<ServerSocket>> New(int port);

  ServerSocket() = delete;
  ServerSocket(const ServerSocket &) = delete;
  ServerSocket(ServerSocket &&other) noexcept;

  ServerSocket &operator=(const ServerSocket &) = delete;
  ServerSocket &operator=(ServerSocket &&other) noexcept;

  ~ServerSocket();

  [[nodiscard]] operator int() const;

  [[nodiscard]] const std::string &Address() const;
  [[nodiscard]] int Port() const;

  void Listen(
      const std::function<void(std::unique_ptr<Socket>)> &handler) const;

 private:
  ServerSocket(std::string address, int port, int descriptor);

  std::string address_;
  int port_ = -1;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SERVER_SOCKET_H_
