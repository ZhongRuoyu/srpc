#include "srpc/network/socket.h"

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

#include "srpc/network/message.h"
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

Result<std::unique_ptr<Socket>> Socket::New(const std::string &address,
                                            u16 port) {
  int socktype = 0;
  addrinfo hints{
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_STREAM,
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
    return std::unique_ptr<Socket>(new Socket(std::move(addr), descriptor));
  }

  freeaddrinfo(head);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return std::string{std::strerror(errno)};
}

Socket::Socket(Socket &&other) noexcept
    : address_(std::move(other.address_)), descriptor_(other.descriptor_) {
  other.descriptor_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  this->address_ = std::move(other.address_);
  this->descriptor_ = other.descriptor_;
  other.descriptor_ = -1;
  return *this;
}

Socket::~Socket() {
  if (this->descriptor_ != -1) {
    close(this->descriptor_);
  }
}

const SocketAddress &Socket::Address() const { return this->address_; }

Result<i64> Socket::Send(const std::vector<std::byte> &data) const {
  assert(this->descriptor_ != -1);

  auto msg = MakeMessage(data);

  i64 data_sent = 0;
  while (data_sent < msg.size()) {
    i64 res = send(this->descriptor_, msg.data() + data_sent,
                   msg.size() - data_sent, 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }
    data_sent += res;
  }

  return data_sent;
}

Result<std::vector<std::byte>> Socket::Receive() const {
  assert(this->descriptor_ != -1);

  i64 msg_size = 0;
  {
    std::array<std::byte, sizeof(i64)> header;
    i64 res = recv(this->descriptor_, header.data(), header.size(), 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }
    msg_size = Unmarshal<i64>{}(header);
  }

  std::vector<std::byte> msg(msg_size);
  i64 data_received = 0;
  while (data_received < msg.size()) {
    i64 res = recv(this->descriptor_, msg.data() + data_received,
                   msg.size() - data_received, 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return std::string{std::strerror(errno)};
    }
    data_received += res;
  }

  return std::move(msg);
}

Socket::Socket(SocketAddress address, int descriptor)
    : address_(std::move(address)), descriptor_(descriptor) {}

}  // namespace srpc
