#ifndef SRPC_NETWORK_MESSAGE_H_
#define SRPC_NETWORK_MESSAGE_H_

#include <cstddef>
#include <optional>
#include <vector>

namespace srpc {

std::optional<std::vector<std::byte>> RemoveMessageHeader(
    const std::vector<std::byte> &msg);

std::vector<std::byte> MakeMessage(const std::vector<std::byte> &data);

}  // namespace srpc

#endif  // SRPC_NETWORK_MESSAGE_H_
