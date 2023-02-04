#ifndef SRPC_NETWORK_DATAGRAM_CLIENT_H_
#define SRPC_NETWORK_DATAGRAM_CLIENT_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "srpc/network/datagram_socket.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

class DatagramClient {
 public:
  [[nodiscard]] static Result<std::unique_ptr<DatagramClient>> New(
      const std::string &address, u16 port);

  DatagramClient() = delete;
  DatagramClient(const DatagramClient &) = delete;
  DatagramClient(DatagramClient &&) noexcept = default;

  DatagramClient &operator=(const DatagramClient &) = delete;
  DatagramClient &operator=(DatagramClient &&) noexcept = default;

  ~DatagramClient() = default;

  [[nodiscard]] Result<std::vector<std::byte>> SendAndReceive(
      const std::vector<std::byte> &msg) const;

 private:
  explicit DatagramClient(std::unique_ptr<DatagramSocket> socket);

  std::unique_ptr<DatagramSocket> socket_;
};

}  // namespace srpc

#endif  // SRPC_NETWORK_DATAGRAM_CLIENT_H_
