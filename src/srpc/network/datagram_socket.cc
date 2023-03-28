#include "srpc/network/datagram_socket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "srpc/network/tcp_ip.h"
#include "srpc/types/integers.h"
#include "srpc/utils/result.h"

namespace srpc {

static SocketAddress GetSocketAddress(const sockaddr *addr) {
  switch (addr->sa_family) {
    case AF_INET: {
      char sin_addr[INET_ADDRSTRLEN];
      inet_ntop(addr->sa_family, &(((const sockaddr_in *)addr)->sin_addr),
                sin_addr, sizeof(sin_addr));
      u16 sin_port = ntohs(((const sockaddr_in *)addr)->sin_port);
      return {
          .protocol = kIPv4,
          .address = sin_addr,
          .port = sin_port,
      };
    }
    case AF_INET6: {
      char sin6_addr[INET6_ADDRSTRLEN];
      inet_ntop(addr->sa_family, &(((const sockaddr_in6 *)addr)->sin6_addr),
                sin6_addr, sizeof(sin6_addr));
      u16 sin6_port = ntohs(((const sockaddr_in6 *)addr)->sin6_port);
      return {
          .protocol = kIPv6,
          .address = sin6_addr,
          .port = sin6_port,
      };
    }
  }
  assert(false);
}

Result<std::unique_ptr<DatagramSocket>> DatagramSocket::New(
    const std::string &address, u16 port) {
  int socktype = 0;
  addrinfo hints{
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_DGRAM,
  };
  addrinfo *head = nullptr;
  if (int err = getaddrinfo(address.c_str(), std::to_string(port).c_str(),
                            &hints, &head);
      err != 0) {
    return std::string{gai_strerror(err)};
  }

  for (addrinfo *p = head; p != nullptr; p = p->ai_next) {
    int descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (descriptor == -1) {
      continue;
    }
    if (connect(descriptor, p->ai_addr, p->ai_addrlen) == -1) {
      close(descriptor);
      continue;
    }

    auto addr = GetSocketAddress(p->ai_addr);
    freeaddrinfo(head);
    return std::unique_ptr<DatagramSocket>(
        new DatagramSocket(std::move(addr), descriptor));
  }

  freeaddrinfo(head);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return std::string{std::strerror(errno)};
}

DatagramSocket::DatagramSocket(DatagramSocket &&other) noexcept
    : address_(std::move(other.address_)), descriptor_(other.descriptor_) {
  other.descriptor_ = -1;
}

DatagramSocket &DatagramSocket::operator=(DatagramSocket &&other) noexcept {
  this->address_ = std::move(other.address_);
  this->descriptor_ = other.descriptor_;
  other.descriptor_ = -1;
  return *this;
}

DatagramSocket::~DatagramSocket() {
  if (this->descriptor_ != -1) {
    close(this->descriptor_);
  }
}

const SocketAddress &DatagramSocket::Address() const { return this->address_; }

Result<i64> DatagramSocket::Send(const std::vector<std::byte> &msg) const {
  assert(this->descriptor_ != -1);

  if (msg.size() > 65536) {
    return std::string{"Message is too large"};
  }

  constexpr int retry_times = 8;
  int retry_us = 10'000;  // 10 milliseconds
  for (int i = 0; i <= retry_times; ++i) {
    timeval tv{
        .tv_sec = retry_us / 1'000'000,
        .tv_usec = retry_us % 1'000'000,
    };
    if (setsockopt(this->descriptor_, SOL_SOCKET, SO_SNDTIMEO, &tv,
                   sizeof(tv)) == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }

    i64 res = send(this->descriptor_, msg.data(), msg.size(), 0);
    if (res == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // send has timed out. See socket(7).
        retry_us *= 2;
        continue;
      }
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }

    if (i == 0) {
      return res;
    }
    return {
        res,
        std::string{"Send succeeded after " + std::to_string(i) + " retries"},
    };
  }

  return std::string{"Send failed after " + std::to_string(retry_times) +
                     " retries"};
}

Result<std::vector<std::byte>> DatagramSocket::Receive() const {
  assert(this->descriptor_ != -1);

  constexpr int retry_times = 8;
  int retry_us = 10'000;  // 10 milliseconds
  for (int i = 0; i <= retry_times; ++i) {
    timeval tv{
        .tv_sec = retry_us / 1'000'000,
        .tv_usec = retry_us % 1'000'000,
    };
    if (setsockopt(this->descriptor_, SOL_SOCKET, SO_RCVTIMEO, &tv,
                   sizeof(tv)) == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }

    std::vector<std::byte> msg(65536);
    i64 res = recv(this->descriptor_, msg.data(), msg.size(), 0);
    if (res == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // recv has timed out. See socket(7).
        retry_us *= 2;
        continue;
      }
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }

    msg.resize(res);
    if (i == 0) {
      return std::move(msg);
    }
    return {
        std::move(msg),
        std::string{"Receive succeeded after " + std::to_string(i) +
                    " retries"},
    };
  }

  return std::string{"Receive failed after " + std::to_string(retry_times) +
                     " retries"};
}

DatagramSocket::DatagramSocket(SocketAddress address, int descriptor)
    : address_(std::move(address)), descriptor_(descriptor) {}

}  // namespace srpc
