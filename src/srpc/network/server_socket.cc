#include "srpc/network/server_socket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <functional>
#include <memory>
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

Result<std::unique_ptr<ServerSocket>> ServerSocket::New(u16 port) {
  addrinfo hints{
      .ai_flags = AI_PASSIVE,
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_STREAM,
  };
  addrinfo *head = nullptr;
  if (int err =
          getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &head);
      err != 0) {
    return Result<std::unique_ptr<ServerSocket>>::Err(gai_strerror(err));
  }

  for (addrinfo *p = head; p != nullptr; p = p->ai_next) {
    int descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (descriptor == -1) {
      continue;
    }
    {
      int yes = 1;
      if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
          -1) {
        close(descriptor);
        freeaddrinfo(head);
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        return Result<std::unique_ptr<ServerSocket>>::Err(std::strerror(errno));
      }
    }
    if (bind(descriptor, p->ai_addr, p->ai_addrlen) == -1) {
      close(descriptor);
      continue;
    }

    auto addr = GetSocketAddress(p->ai_addr);
    freeaddrinfo(head);
    if (listen(descriptor, SOMAXCONN) == -1) {
      close(descriptor);
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return Result<std::unique_ptr<ServerSocket>>::Err(std::strerror(errno));
    }

    return Result<std::unique_ptr<ServerSocket>>::Ok(
        std::unique_ptr<ServerSocket>(
            new ServerSocket(std::move(addr), descriptor)));
  }

  freeaddrinfo(head);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return Result<std::unique_ptr<ServerSocket>>::Err(std::strerror(errno));
}

ServerSocket::ServerSocket(ServerSocket &&other) noexcept
    : address_(std::move(other.address_)), descriptor_(other.descriptor_) {
  other.descriptor_ = -1;
}

ServerSocket &ServerSocket::operator=(ServerSocket &&other) noexcept {
  this->address_ = std::move(other.address_);
  this->descriptor_ = other.descriptor_;
  other.descriptor_ = -1;
  return *this;
}

ServerSocket::~ServerSocket() {
  if (this->descriptor_ != -1) {
    close(this->descriptor_);
  }
}

const SocketAddress &ServerSocket::Address() const { return this->address_; }

void ServerSocket::Listen(
    const std::function<void(std::unique_ptr<Socket>)> &handler) const {
  assert(this->descriptor_ != -1);

  for (;;) {
    sockaddr_storage from;
    socklen_t from_len = sizeof(from);
    int descriptor = accept(this->descriptor_, (sockaddr *)&from, &from_len);
    if (descriptor == -1) {
      continue;
    }

    auto from_addr = GetSocketAddress((sockaddr *)&from);
    std::unique_ptr<Socket> socket(
        new Socket(std::move(from_addr), descriptor));
    std::thread work(handler, std::move(socket));
    work.detach();
  }
}

ServerSocket::ServerSocket(SocketAddress address, int descriptor)
    : address_(std::move(address)), descriptor_(descriptor) {}

}  // namespace srpc
