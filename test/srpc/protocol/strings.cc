#include "srpc/protocol/strings.h"

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "srpc/protocol/integers.h"

using namespace srpc;

template <typename... Ts>
static std::vector<std::byte> MakeBytes(Ts... bytes) {
  return std::vector<std::byte>{std::byte(bytes)...};
};

TEST(Protocol, MarshalStrings) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e',
                      'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'),
            Marshal<std::string>{}("hello, world"));
  assert_eq(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0), Marshal<std::string>{}(""));
}

TEST(Protocol, UnmarshalStrings) {
  ASSERT_EQ(std::string("hello, world"),
            Unmarshal<std::string>{}(MakeBytes(
                (u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e', 'l',
                'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd')));
  ASSERT_EQ(std::string(""),
            Unmarshal<std::string>{}(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0)));
}
