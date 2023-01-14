#ifndef SRPC_UTILS_RESULT_H_
#define SRPC_UTILS_RESULT_H_

#include <cassert>
#include <optional>
#include <string_view>
#include <utility>

namespace srpc {

template <typename T, typename E = std::string_view>
class Result {
 public:
  Result() = delete;
  Result(const Result &other) = default;
  Result(Result &&other) noexcept = default;

  Result &operator=(const Result &other) = default;
  Result &operator=(Result &&other) noexcept = default;

  ~Result() = default;

  [[nodiscard]] constexpr bool OK() const { return this->value_.has_value(); }

  [[nodiscard]] constexpr T &Value() & {
    assert(this->OK());
    return *(this->value_);
  }
  [[nodiscard]] constexpr const T &Value() const & {
    assert(this->OK());
    return *(this->value_);
  }

  [[nodiscard]] constexpr T &&Value() && {
    assert(this->OK());
    return *(this->value_);
  }
  [[nodiscard]] constexpr const T &&Value() const && {
    assert(this->OK());
    return *(this->value_);
  }

  [[nodiscard]] constexpr E &Error() {
    assert(!this->OK());
    return *(this->error_);
  }
  [[nodiscard]] constexpr const E &Error() const {
    assert(!this->OK());
    return *(this->error_);
  }

  static Result<T, E> Ok(T value) { return Result<T, E>(std::move(value), {}); }

  static Result<T, E> Err(E error) {
    return Result<T, E>({}, std::move(error));
  }

 private:
  Result(std::optional<T> value, std::optional<E> error)
      : value_(std::move(value)), error_(std::move(error)) {}

  std::optional<T> value_;
  std::optional<E> error_;
};

}  // namespace srpc

#endif  // SRPC_UTILS_RESULT_H_
