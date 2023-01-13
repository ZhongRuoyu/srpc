#include "protocol/string.h"

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "protocol/integers.h"

template <typename... Ts>
static std::vector<std::byte> MakeBytes(Ts... bytes) {
  return std::vector<std::byte>{std::byte(bytes)...};
};

TEST(Protocol, SerializeStrings) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(
      MakeBytes((srpc::u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e',
                'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'),
      srpc::Serialize(std::string("hello, world")));
  assert_eq(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0),
            srpc::Serialize(std::string("")));
}

TEST(Protocol, DeserializeStrings) {
  ASSERT_EQ(std::string("hello, world"),
            srpc::Deserialize(MakeBytes((srpc::u8)strlen("hello, world"), 0, 0,
                                        0, 0, 0, 0, 0, 'h', 'e', 'l', 'l', 'o',
                                        ',', ' ', 'w', 'o', 'r', 'l', 'd')));
  ASSERT_EQ(std::string(""),
            srpc::Deserialize(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0)));
}
