#include "srpc/network/server.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

#include "srpc/network/server_socket.h"
#include "srpc/network/tcp_ip.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

Result<std::unique_ptr<Server>> Server::New(u16 port) {
  auto server_socket_res = ServerSocket::New(port);
  if (!server_socket_res.OK()) {
    return std::move(server_socket_res.Error());
  }
  auto server =
      std::unique_ptr<Server>(new Server(std::move(server_socket_res.Value())));
  return std::move(server);
}

void Server::Listen(const std::function<std::optional<std::vector<std::byte>>(
                        const SocketAddress &, Result<std::vector<std::byte>>)>
                        &handler) const {
  this->server_socket_->Listen([handler](std::unique_ptr<Socket> socket) {
    auto req = socket->Receive();
    auto res = handler(socket->Address(), req);
    if (res.has_value()) {
      std::ignore = socket->Send(*res);
    }
  });
}

Server::Server(std::unique_ptr<ServerSocket> server_socket)
    : server_socket_(std::move(server_socket)) {}

}  // namespace srpc
