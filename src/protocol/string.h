#ifndef SRPC_PROTOCOL_STRING_H_
#define SRPC_PROTOCOL_STRING_H_

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace srpc {

std::vector<std::byte> Serialize(const std::string &str);

std::optional<std::string> Deserialize(const std::vector<std::byte> &data);

}  // namespace srpc

#endif  // SRPC_PROTOCOL_STRING_H_
