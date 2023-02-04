#ifndef SRPC_NETWORK_DATAGRAM_SOCKET_H_
#define SRPC_NETWORK_DATAGRAM_SOCKET_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "srpc/network/tcp_ip.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

class DatagramSocket {
 public:
  friend class ServerSocket;

  [[nodiscard]] static Result<std::unique_ptr<DatagramSocket>> New(
      const std::string &address, u16 port);

  DatagramSocket() = delete;
  DatagramSocket(const DatagramSocket &) = delete;
  DatagramSocket(DatagramSocket &&other) noexcept;

  DatagramSocket &operator=(const DatagramSocket &) = delete;
  DatagramSocket &operator=(DatagramSocket &&other) noexcept;

  ~DatagramSocket();

  [[nodiscard]] const SocketAddress &Address() const;

  [[nodiscard]] Result<i64> Send(const std::vector<std::byte> &msg) const;
  [[nodiscard]] Result<std::vector<std::byte>> Receive() const;

 private:
  DatagramSocket(SocketAddress address, int descriptor);

  SocketAddress address_;
  int descriptor_ = -1;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_DATAGRAM_SOCKET_H_
