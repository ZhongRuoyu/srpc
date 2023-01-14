#include "network/client.h"

#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "network/socket.h"
#include "utils/result.h"

namespace srpc {

Result<std::unique_ptr<Client>> Client::New(std::string address, int port) {
  auto socket_res = Socket::New(std::move(address), port);
  if (!socket_res.OK()) {
    return Result<std::unique_ptr<Client>>::Err(socket_res.Error());
  }
  auto client =
      std::unique_ptr<Client>(new Client(std::move(socket_res.Value())));
  return Result<std::unique_ptr<Client>>::Ok(std::move(client));
}

Result<std::vector<std::byte>> Client::SendAndReceive(
    const std::vector<std::byte> &data) const {
  std::ignore = this->socket_->Send(data);
  auto res = this->socket_->Receive();
  return res;
}

Client::Client(std::unique_ptr<Socket> socket) : socket_(std::move(socket)) {}

}  // namespace srpc
