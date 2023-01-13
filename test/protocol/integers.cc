#include "protocol/integers.h"

#include <array>
#include <cstddef>

#include <gtest/gtest.h>

TEST(Protocol, ByteswapSignedInts) {
  ASSERT_EQ(srpc::i64(0x0df0ad8b0df0ad8b),
            srpc::Byteswap(srpc::i64(0x8badf00d8badf00d)));
  ASSERT_EQ(srpc::i32(0x0df0ad8b), srpc::Byteswap(srpc::i32(0x8badf00d)));
  ASSERT_EQ(srpc::i16(0x0df0), srpc::Byteswap(srpc::i16(0xf00d)));
  ASSERT_EQ(srpc::i8(0xf0), srpc::Byteswap(srpc::i8(0xf0)));
}

TEST(Protocol, ByteswapUnsignedInts) {
  ASSERT_EQ(srpc::u64(0xefbeaddeefbeadde),
            srpc::Byteswap(srpc::u64(0xdeadbeefdeadbeef)));
  ASSERT_EQ(srpc::u32(0xefbeadde), srpc::Byteswap(srpc::u32(0xdeadbeef)));
  ASSERT_EQ(srpc::u16(0xefbe), srpc::Byteswap(srpc::u16(0xbeef)));
  ASSERT_EQ(srpc::u8(0xbe), srpc::Byteswap(srpc::u8(0xbe)));
}

template <typename... Ts>
static std::array<std::byte, sizeof...(Ts)> MakeBytes(Ts... bytes) {
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
            srpc::Serialize(srpc::i64(0xdeadbeefdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde),
            srpc::Serialize(srpc::i32(0xdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe), srpc::Serialize(srpc::i16(0xbeef)));
  assert_eq(MakeBytes(0xbe), srpc::Serialize(srpc::i8(0xbe)));
}

TEST(Protocol, SerializeUnsignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde),
            srpc::Serialize(srpc::u64(0xdeadbeefdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde),
            srpc::Serialize(srpc::u32(0xdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe), srpc::Serialize(srpc::u16(0xbeef)));
  assert_eq(MakeBytes(0xbe), srpc::Serialize(srpc::u8(0xbe)));
}

TEST(Protocol, DeserializeSignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  ASSERT_EQ(srpc::i64(0xdeadbeefdeadbeef),
            srpc::Deserialize<srpc::i64>(
                MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(srpc::i32(0xdeadbeef),
            srpc::Deserialize<srpc::i32>(MakeBytes(0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(srpc::i16(0xbeef),
            srpc::Deserialize<srpc::i16>(MakeBytes(0xef, 0xbe)));
  ASSERT_EQ(srpc::i8(0xbe), srpc::Deserialize<srpc::i8>(MakeBytes(0xbe)));
}

TEST(Protocol, DeserializeUnsignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  ASSERT_EQ(srpc::u64(0xdeadbeefdeadbeef),
            srpc::Deserialize<srpc::u64>(
                MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(srpc::u32(0xdeadbeef),
            srpc::Deserialize<srpc::u32>(MakeBytes(0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(srpc::u16(0xbeef),
            srpc::Deserialize<srpc::u16>(MakeBytes(0xef, 0xbe)));
  ASSERT_EQ(srpc::u8(0xbe), srpc::Deserialize<srpc::u8>(MakeBytes(0xbe)));
}
