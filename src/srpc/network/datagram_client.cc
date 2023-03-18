#include "srpc/network/datagram_client.h"

#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "srpc/network/datagram_socket.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

Result<std::unique_ptr<DatagramClient>> DatagramClient::New(
    const std::string &address, u16 port) {
  auto socket_res = DatagramSocket::New(address, port);
  if (!socket_res.OK()) {
    return std::move(socket_res.Error());
  }
  auto client = std::unique_ptr<DatagramClient>(
      new DatagramClient(std::move(socket_res.Value())));
  return std::move(client);
}

Result<std::vector<std::byte>> DatagramClient::SendAndReceive(
    const std::vector<std::byte> &msg) const {
  std::ignore = this->socket_->Send(msg);
  auto res = this->socket_->Receive();
  return res;
}

DatagramClient::DatagramClient(std::unique_ptr<DatagramSocket> socket)
    : socket_(std::move(socket)) {}

}  // namespace srpc
