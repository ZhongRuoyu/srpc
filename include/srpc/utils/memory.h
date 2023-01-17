#ifndef SRPC_UTILS_MEMORY_H_
#define SRPC_UTILS_MEMORY_H_

#include <cstddef>

namespace srpc {

inline constexpr std::byte *Memcpy(std::byte *dst, const std::byte *src,
                                   std::size_t cnt) {
  std::byte *result = dst;
  for (size_t i = 0; i < cnt; ++i) {
    *(dst++) = *(src++);
  }
  return result;
}

}  // namespace srpc

#endif  // SRPC_UTILS_MEMORY_H_
