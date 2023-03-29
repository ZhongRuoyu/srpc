#include "srpc/network/server_datagram_socket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "srpc/network/socket.h"
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

Result<std::unique_ptr<ServerDatagramSocket>> ServerDatagramSocket::New(
    u16 port) {
  addrinfo hints{
      .ai_flags = AI_PASSIVE,
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_DGRAM,
  };
  addrinfo *head = nullptr;
  if (int err =
          getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &head);
      err != 0) {
    return std::string{gai_strerror(err)};
  }

  for (addrinfo *p = head; p != nullptr; p = p->ai_next) {
    int descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (descriptor == -1) {
      continue;
    }
    if (bind(descriptor, p->ai_addr, p->ai_addrlen) == -1) {
      close(descriptor);
      continue;
    }

    auto addr = GetSocketAddress(p->ai_addr);
    freeaddrinfo(head);

    return std::unique_ptr<ServerDatagramSocket>(
        new ServerDatagramSocket(std::move(addr), descriptor));
  }

  freeaddrinfo(head);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return std::string{std::strerror(errno)};
}

ServerDatagramSocket::ServerDatagramSocket(
    ServerDatagramSocket &&other) noexcept
    : address_(std::move(other.address_)), descriptor_(other.descriptor_) {
  other.descriptor_ = -1;
}

ServerDatagramSocket &ServerDatagramSocket::operator=(
    ServerDatagramSocket &&other) noexcept {
  this->address_ = std::move(other.address_);
  this->descriptor_ = other.descriptor_;
  other.descriptor_ = -1;
  return *this;
}

ServerDatagramSocket::~ServerDatagramSocket() {
  if (this->descriptor_ != -1) {
    close(this->descriptor_);
  }
}

const SocketAddress &ServerDatagramSocket::Address() const {
  return this->address_;
}

void ServerDatagramSocket::Listen(
    const std::function<std::optional<std::vector<std::byte>>(
        const SocketAddress &, Result<std::vector<std::byte>>)> &handler)
    const {
  assert(this->descriptor_ != -1);

  for (;;) {
    sockaddr_storage from;
    socklen_t from_len = sizeof(from);
    std::vector<std::byte> msg(65536);
    i64 res = recvfrom(this->descriptor_, msg.data(), msg.size(), 0,
                       (sockaddr *)&from, &from_len);
    if (res == -1) {
      continue;
    }
    msg.resize(res);
    auto from_addr = GetSocketAddress((sockaddr *)&from);
    std::thread work([handler, from_addr = std::move(from_addr),
                      data = std::move(msg), descriptor = this->descriptor_,
                      from, from_len]() mutable {
      auto res = handler(from_addr, std::move(data));
      if (res.has_value()) {
        i64 data_sent = 0;
        while (data_sent < res->size()) {
          i64 sendto_res =
              sendto(descriptor, res->data() + data_sent,
                     res->size() - data_sent, 0, (sockaddr *)&from, from_len);
          if (sendto_res == -1) {
            return;
          }
          data_sent += sendto_res;
        }
      }
    });
    work.detach();
  }
}

ServerDatagramSocket::ServerDatagramSocket(SocketAddress address,
                                           int descriptor)
    : address_(std::move(address)), descriptor_(descriptor) {}

}  // namespace srpc
