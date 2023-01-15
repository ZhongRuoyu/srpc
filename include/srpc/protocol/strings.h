#ifndef SRPC_PROTOCOL_STRINGS_H_
#define SRPC_PROTOCOL_STRINGS_H_

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace srpc {

[[nodiscard]] std::vector<std::byte> Serialize(const std::string &str);

[[nodiscard]] std::vector<std::byte> Serialize(const char *str);
[[nodiscard]] std::vector<std::byte> Serialize(char *str);

[[nodiscard]] std::optional<std::string> Deserialize(
    const std::vector<std::byte> &data);

}  // namespace srpc

#endif  // SRPC_PROTOCOL_STRINGS_H_
