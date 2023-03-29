#ifndef SRPC_NETWORK_DATAGRAM_SERVER_H_
#define SRPC_NETWORK_DATAGRAM_SERVER_H_

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "srpc/network/server_datagram_socket.h"
#include "srpc/network/tcp_ip.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

class DatagramServer {
 public:
  [[nodiscard]] static Result<std::unique_ptr<DatagramServer>> New(u16 port);

  DatagramServer() = delete;
  DatagramServer(const DatagramServer &) = delete;
  DatagramServer(DatagramServer &&) noexcept = default;

  DatagramServer &operator=(const DatagramServer &) = delete;
  DatagramServer &operator=(DatagramServer &&) noexcept = default;

  ~DatagramServer() = default;

  void Listen(const std::function<std::optional<std::vector<std::byte>>(
                  const SocketAddress &, Result<std::vector<std::byte>>)>
                  &handler) const;

 private:
  explicit DatagramServer(
      std::unique_ptr<ServerDatagramSocket> server_datagram_socket);

  std::unique_ptr<ServerDatagramSocket> server_datagram_socket_;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_DATAGRAM_SERVER_H_
