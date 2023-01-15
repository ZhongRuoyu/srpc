#include "srpc/protocol/integers.h"

#include <array>
#include <cstddef>

#include <gtest/gtest.h>

using namespace srpc;

TEST(Protocol, ByteswapSignedInts) {
  ASSERT_EQ(i64(0x0df0ad8b0df0ad8b), Byteswap(i64(0x8badf00d8badf00d)));
  ASSERT_EQ(i32(0x0df0ad8b), Byteswap(i32(0x8badf00d)));
  ASSERT_EQ(i16(0x0df0), Byteswap(i16(0xf00d)));
  ASSERT_EQ(i8(0xf0), Byteswap(i8(0xf0)));
}

TEST(Protocol, ByteswapUnsignedInts) {
  ASSERT_EQ(u64(0xefbeaddeefbeadde), Byteswap(u64(0xdeadbeefdeadbeef)));
  ASSERT_EQ(u32(0xefbeadde), Byteswap(u32(0xdeadbeef)));
  ASSERT_EQ(u16(0xefbe), Byteswap(u16(0xbeef)));
  ASSERT_EQ(u8(0xbe), Byteswap(u8(0xbe)));
}

template <typename... Ts>
static constexpr std::array<std::byte, sizeof...(Ts)> MakeBytes(Ts... bytes) {
  return std::array<std::byte, sizeof...(Ts)>{std::byte(bytes)...};
};

TEST(Protocol, SerializeSignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde),
            Serialize(i64(0xdeadbeefdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde), Serialize(i32(0xdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe), Serialize(i16(0xbeef)));
  assert_eq(MakeBytes(0xbe), Serialize(i8(0xbe)));
}

TEST(Protocol, SerializeUnsignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde),
            Serialize(u64(0xdeadbeefdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde), Serialize(u32(0xdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe), Serialize(u16(0xbeef)));
  assert_eq(MakeBytes(0xbe), Serialize(u8(0xbe)));
}

TEST(Protocol, DeserializeSignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  ASSERT_EQ(i64(0xdeadbeefdeadbeef),
            Deserialize<i64>(
                MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(i32(0xdeadbeef),
            Deserialize<i32>(MakeBytes(0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(i16(0xbeef), Deserialize<i16>(MakeBytes(0xef, 0xbe)));
  ASSERT_EQ(i8(0xbe), Deserialize<i8>(MakeBytes(0xbe)));
}

TEST(Protocol, DeserializeUnsignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  ASSERT_EQ(u64(0xdeadbeefdeadbeef),
            Deserialize<u64>(
                MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(u32(0xdeadbeef),
            Deserialize<u32>(MakeBytes(0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(u16(0xbeef), Deserialize<u16>(MakeBytes(0xef, 0xbe)));
  ASSERT_EQ(u8(0xbe), Deserialize<u8>(MakeBytes(0xbe)));
}
