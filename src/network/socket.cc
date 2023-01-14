#include "network/socket.h"

#include <netdb.h>
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

#include "protocol/integers.h"
#include "utils/result.h"

namespace srpc {

Result<std::unique_ptr<Socket>> Socket::New(std::string address, int port) {
  addrinfo hints{
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_STREAM,
  };
  addrinfo *head = nullptr;
  if (int err = getaddrinfo(address.c_str(), std::to_string(port).c_str(),
                            &hints, &head);
      err != 0) {
    return Result<std::unique_ptr<Socket>>::Err(gai_strerror(err));
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

    freeaddrinfo(head);
    return Result<std::unique_ptr<Socket>>::Ok(std::unique_ptr<Socket>(
        new Socket(std::move(address), port, descriptor)));
  }

  freeaddrinfo(head);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return Result<std::unique_ptr<Socket>>::Err(std::strerror(errno));
}

Socket::Socket(Socket &&other) noexcept
    : address_(std::move(other.address_)),
      port_(other.port_),
      descriptor_(other.descriptor_) {
  other.descriptor_ = -1;
}

Socket::operator int() const { return this->descriptor_; }

Socket &Socket::operator=(Socket &&other) noexcept {
  this->address_ = std::move(other.address_);
  this->port_ = other.port_;
  this->descriptor_ = other.descriptor_;
  other.descriptor_ = -1;
  return *this;
}

Socket::~Socket() {
  if (*this != -1) {
    close(*this);
  }
}

const std::string &Socket::Address() const { return this->address_; }

int Socket::Port() const { return this->port_; }

Result<i64> Socket::Send(const std::vector<std::byte> &msg) const {
  assert(*this != -1);

  {
    auto header = Serialize(i64(std::ssize(msg)));
    i64 res = send(*this, header.data(), header.size(), 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return Result<i64>::Err(std::strerror(errno));
    }
  }

  i64 msg_sent = 0;
  while (msg_sent < msg.size()) {
    i64 res = send(*this, msg.data() + msg_sent, msg.size() - msg_sent, 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return Result<i64>::Err(std::strerror(errno));
    }
    msg_sent += res;
  }

  return Result<i64>::Ok(i64(sizeof(i64)) + msg_sent);
}

Result<std::vector<std::byte>> Socket::Receive() const {
  assert(*this != -1);

  i64 msg_size = 0;
  {
    std::array<std::byte, sizeof(i64)> header;
    i64 res = recv(*this, header.data(), sizeof(header), 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return Result<std::vector<std::byte>>::Err(std::strerror(errno));
    }
    msg_size = Deserialize<i64>(header);
  }

  std::vector<std::byte> msg(msg_size);
  i64 msg_received = 0;
  while (msg_received < msg.size()) {
    i64 res =
        recv(*this, msg.data() + msg_received, msg_size - msg_received, 0);
    if (res == -1) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      return Result<std::vector<std::byte>>::Err(std::strerror(errno));
    }
    msg_received += res;
  }

  return Result<std::vector<std::byte>>::Ok(std::move(msg));
}

Socket::Socket(std::string address, int port, int descriptor)
    : address_(std::move(address)), port_(port), descriptor_(descriptor) {}

}  // namespace srpc
