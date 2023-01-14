#include "srpc/protocol/floats.h"

#include <cmath>
#include <limits>

#include <gtest/gtest.h>

TEST(Protocol, SerializeAndDeserializeFloats) {
  auto check = [](auto val) {
    auto tmp = srpc::Serialize(val);
    ASSERT_EQ(sizeof(val), tmp.size());
    if (std::isnan(val)) {
      ASSERT_TRUE(std::isnan(srpc::Deserialize<decltype(val)>(tmp)));
    } else {
      ASSERT_FLOAT_EQ(val, srpc::Deserialize<decltype(val)>(tmp));
    }
  };
  check(3.141593F);
  check(3.141593);
  check(std::numeric_limits<srpc::f32>::infinity());
  check(std::numeric_limits<srpc::f64>::infinity());
  check(-std::numeric_limits<srpc::f32>::infinity());
  check(-std::numeric_limits<srpc::f64>::infinity());
  check(std::numeric_limits<srpc::f32>::quiet_NaN());
  check(std::numeric_limits<srpc::f64>::quiet_NaN());
}
