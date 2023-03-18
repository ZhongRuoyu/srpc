#ifndef SRPC_UTILS_RESULT_H_
#define SRPC_UTILS_RESULT_H_

#include <cassert>
#include <optional>
#include <string>
#include <utility>

namespace srpc {

template <typename T, typename E = std::string>
class Result {
 public:
  Result() = delete;
  Result(const Result &) = default;
  Result(Result &&) noexcept = default;

  Result(const T &value) : value_(value), error_() {}
  Result(T &&value) : value_(std::move(value)), error_() {}

  Result(const E &error) : value_(), error_(error) {}
  Result(E &&error) noexcept : value_(), error_(std::move(error)) {}

  template <typename U, typename F>
  Result(U &&value, F &&error)
      : value_(std::forward<U>(value)), error_(std::forward<F>(error)) {}

  Result &operator=(const Result &) = default;
  Result &operator=(Result &&) noexcept = default;

  ~Result() = default;

  [[nodiscard]] constexpr bool OK() const { return this->value_.has_value(); }

  [[nodiscard]] constexpr T &Value() & { return *(this->value_); }
  [[nodiscard]] constexpr const T &Value() const & { return *(this->value_); }
  [[nodiscard]] constexpr T &&Value() && { return *(this->value_); }
  [[nodiscard]] constexpr const T &&Value() const && { return *(this->value_); }

  [[nodiscard]] constexpr E &Error() & { return *(this->error_); }
  [[nodiscard]] constexpr const E &Error() const & { return *(this->error_); }
  [[nodiscard]] constexpr E &&Error() && { return *(this->error_); }
  [[nodiscard]] constexpr const E &&Error() const && { return *(this->error_); }

 private:
  Result(std::optional<T> value, std::optional<E> error)
      : value_(std::move(value)), error_(std::move(error)) {}

  std::optional<T> value_;
  std::optional<E> error_;
};

}  // namespace srpc

#endif  // SRPC_UTILS_RESULT_H_
