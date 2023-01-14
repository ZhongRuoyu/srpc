#ifndef SRPC_NETWORK_CLIENT_H_
#define SRPC_NETWORK_CLIENT_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "srpc/network/socket.h"
#include "srpc/utils/result.h"

namespace srpc {

class Client {
 public:
  [[nodiscard]] static Result<std::unique_ptr<Client>> New(std::string address,
                                                           int port);

  Client() = delete;
  Client(const Client &) = delete;
  Client(Client &&) noexcept = default;

  Client &operator=(const Client &) = delete;
  Client &operator=(Client &&) noexcept = default;

  ~Client() = default;

  [[nodiscard]] Result<std::vector<std::byte>> SendAndReceive(
      const std::vector<std::byte> &data) const;

 private:
  explicit Client(std::unique_ptr<Socket> socket);

  std::unique_ptr<Socket> socket_;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_CLIENT_H_
