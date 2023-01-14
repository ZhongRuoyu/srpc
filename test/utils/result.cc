#include "utils/result.h"

#include <memory>
#include <utility>

#include <gtest/gtest.h>

#include "protocol/integers.h"

TEST(Utils, Result) {
  auto result1 = srpc::Result<srpc::i32>::Ok(42);
  ASSERT_TRUE(result1.OK());
  ASSERT_EQ(result1.Value(), 42);

  auto result2 = srpc::Result<srpc::i32>::Err("error test");
  ASSERT_FALSE(result2.OK());
  ASSERT_EQ(result2.Error(), "error test");

  auto result3 = srpc::Result<srpc::i32, srpc::i64>::Ok(3141592);
  ASSERT_TRUE(result3.OK());
  ASSERT_EQ(result3.Value(), 3141592);

  auto result4 = srpc::Result<srpc::i32, srpc::i64>::Err(6535897);
  ASSERT_FALSE(result4.OK());
  ASSERT_EQ(result4.Error(), 6535897);

  auto result5 =
      srpc::Result<std::unique_ptr<int>>::Ok(std::make_unique<int>(2023));
  ASSERT_TRUE(result5.OK());
  ASSERT_NE(result5.Value(), nullptr);
  ASSERT_EQ(*result5.Value(), 2023);
  auto moved_out = std::move(result5.Value());
  ASSERT_TRUE(result5.OK());
  ASSERT_EQ(result5.Value(), nullptr);
  ASSERT_NE(moved_out, nullptr);
  ASSERT_EQ(*moved_out, 2023);
}