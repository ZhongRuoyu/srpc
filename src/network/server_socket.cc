#include "network/server_socket.h"

#include <netdb.h>
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

#include "network/socket.h"
#include "network/tcp_ip.h"
#include "utils/result.h"

namespace srpc {

Result<std::unique_ptr<ServerSocket>> ServerSocket::New(int port) {
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

    auto address = GetInAddr(p->ai_addr);
    freeaddrinfo(head);
    if (listen(descriptor, SOMAXCONN) == -1) {
      close(descriptor);
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return Result<std::unique_ptr<ServerSocket>>::Err(std::strerror(errno));
    }

    return Result<std::unique_ptr<ServerSocket>>::Ok(
        std::unique_ptr<ServerSocket>(
            new ServerSocket(std::move(address), port, descriptor)));
  }

  freeaddrinfo(head);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return Result<std::unique_ptr<ServerSocket>>::Err(std::strerror(errno));
}

ServerSocket::ServerSocket(ServerSocket &&other) noexcept
    : address_(std::move(other.address_)),
      port_(other.port_),
      descriptor_(other.descriptor_) {
  other.descriptor_ = -1;
}

ServerSocket &ServerSocket::operator=(ServerSocket &&other) noexcept {
  this->address_ = std::move(other.address_);
  this->port_ = other.port_;
  this->descriptor_ = other.descriptor_;
  other.descriptor_ = -1;
  return *this;
}

ServerSocket::~ServerSocket() {
  if (*this != -1) {
    close(*this);
  }
}

ServerSocket::operator int() const { return this->descriptor_; }

const std::string &ServerSocket::Address() const { return this->address_; }

int ServerSocket::Port() const { return this->port_; }

void ServerSocket::Listen(
    const std::function<void(std::unique_ptr<Socket>)> &handler) const {
  assert(*this != -1);

  for (;;) {
    sockaddr_storage from;
    socklen_t from_len = sizeof(from);
    int descriptor = accept(*this, (sockaddr *)&from, &from_len);
    if (descriptor == -1) {
      continue;
    }

    auto from_addr = GetInAddr((sockaddr *)&from);
    auto from_port = GetInPort((sockaddr *)&from);
    std::unique_ptr<Socket> socket(
        new Socket(std::move(from_addr), from_port, descriptor));
    std::thread work(handler, std::move(socket));
    work.detach();
  }
}

ServerSocket::ServerSocket(std::string address, int port, int descriptor)
    : address_(std::move(address)), port_(port), descriptor_(descriptor) {}

}  // namespace srpc
