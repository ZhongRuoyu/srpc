#include "srpc/network/server.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "srpc/network/server_socket.h"
#include "srpc/utils/result.h"

namespace srpc {

Result<std::unique_ptr<Server>> Server::New(int port) {
  auto server_socket_res = ServerSocket::New(port);
  if (!server_socket_res.OK()) {
    return Result<std::unique_ptr<Server>>::Err(
        std::move(server_socket_res.Error()));
  }
  auto server =
      std::unique_ptr<Server>(new Server(std::move(server_socket_res.Value())));
  return Result<std::unique_ptr<Server>>::Ok(std::move(server));
}

void Server::Listen(const std::function<std::vector<std::byte>(
                        const SocketAddress &, Result<std::vector<std::byte>>)>
                        &handler) const {
  this->server_socket_->Listen([handler](std::unique_ptr<Socket> socket) {
    auto req = socket->Receive();
    auto res = handler(socket->SocketAddress(), req);
    std::ignore = socket->Send(res);
  });
}

Server::Server(std::unique_ptr<ServerSocket> server_socket)
    : server_socket_(std::move(server_socket)) {}

}  // namespace srpc
