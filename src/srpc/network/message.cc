#include "srpc/network/message.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "srpc/types/integers.h"

namespace srpc {

std::optional<std::vector<std::byte>> RemoveMessageHeader(
    const std::vector<std::byte> &msg) {
  if (msg.size() < sizeof(i64)) {
    return {};
  }

  i64 data_size = Unmarshal<i64>{}(std::span<const std::byte, sizeof(i64)>{
      msg.data(), msg.data() + sizeof(i64)});
  if (sizeof(i64) + data_size != msg.size()) {
    return {};
  }

  std::vector<std::byte> data(msg.begin() + 8, msg.end());
  return data;
}

std::vector<std::byte> MakeMessage(const std::vector<std::byte> &data) {
  std::vector<std::byte> msg(sizeof(i64) + data.size());
  Marshal<i64>{}(std::ssize(data), std::span<std::byte, sizeof(i64)>{
                                       msg.data(), msg.data() + sizeof(i64)});
  std::copy(data.begin(), data.end(), msg.data() + sizeof(i64));
  return msg;
}

}  // namespace srpc
