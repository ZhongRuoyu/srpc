#include "srpc/network/datagram_server.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

#include "srpc/network/server_datagram_socket.h"
#include "srpc/network/tcp_ip.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

Result<std::unique_ptr<DatagramServer>> DatagramServer::New(u16 port) {
  auto server_socket_res = ServerDatagramSocket::New(port);
  if (!server_socket_res.OK()) {
    return std::move(server_socket_res.Error());
  }
  auto server = std::unique_ptr<DatagramServer>(
      new DatagramServer(std::move(server_socket_res.Value())));
  return std::move(server);
}

void DatagramServer::Listen(
    const std::function<std::optional<std::vector<std::byte>>(
        const SocketAddress &, std::vector<std::byte>)> &handler) const {
  this->server_datagram_socket_->Listen(handler);
}

DatagramServer::DatagramServer(
    std::unique_ptr<ServerDatagramSocket> server_datagram_socket)
    : server_datagram_socket_(std::move(server_datagram_socket)) {}

}  // namespace srpc
