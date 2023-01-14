#ifndef SRPC_NETWORK_SERVER_H_
#define SRPC_NETWORK_SERVER_H_

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

#include "network/server_socket.h"
#include "utils/result.h"

namespace srpc {

class Server {
 public:
  static Result<std::unique_ptr<Server>> New(int port);

  Server() = delete;
  Server(const Server &) = delete;
  Server(Server &&) noexcept = default;

  Server &operator=(const Server &) = delete;
  Server &operator=(Server &&) noexcept = default;

  ~Server() = default;

  void Listen(const std::function<std::vector<std::byte>(
                  Result<std::vector<std::byte>>)> &handler) const;

 private:
  explicit Server(std::unique_ptr<ServerSocket> server_socket);

  std::unique_ptr<ServerSocket> server_socket_;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_SERVER_H_
