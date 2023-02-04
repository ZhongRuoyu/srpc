#include "srpc/network/message.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <vector>

#include "srpc/types/integers.h"

namespace srpc {

std::optional<std::vector<std::byte>> RemoveMessageHeader(
    const std::vector<std::byte> &msg) {
  if (msg.size() < sizeof(i64)) {
    return {};
  }

  i64 data_size = 0;
  {
    std::array<std::byte, sizeof(i64)> header;
    std::copy(msg.begin(), msg.begin() + sizeof(i64), header.begin());
    data_size = Unmarshal<i64>{}(header);
  }
  if (sizeof(i64) + data_size != msg.size()) {
    return {};
  }

  std::vector<std::byte> data(msg.begin() + 8, msg.end());
  return data;
}

std::vector<std::byte> MakeMessage(const std::vector<std::byte> &data) {
  std::vector<std::byte> msg;
  auto header = Marshal<i64>{}(std::ssize(data));
  std::copy(header.begin(), header.end(), std::back_inserter(msg));
  std::copy(data.begin(), data.end(), std::back_inserter(msg));
  return msg;
}

}  // namespace srpc
