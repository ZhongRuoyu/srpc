#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

#include "srpc/network/server.h"
#include "srpc/network/tcp_ip.h"
#include "srpc/types/strings.h"
#include "srpc/utils/result.h"

template <typename CharT, typename Traits>
static std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &ostream,
    const srpc::SocketAddress &addr) {
  switch (addr.protocol) {
    case srpc::kIPv4: return ostream << addr.address << ":" << addr.port;
    case srpc::kIPv6:
      return ostream << "[" << addr.address << "]:" << addr.port;
  }
  assert(false);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return -1;
  }

  auto server_res = srpc::Server::New(std::atoi(argv[1]));
  if (!server_res.OK()) {
    std::cerr << server_res.Error() << std::endl;
    return -1;
  }

  auto server = std::move(server_res.Value());
  server->Listen([](const srpc::SocketAddress &from_addr,
                    srpc::Result<std::vector<std::byte>> req_data_res)
                     -> std::vector<std::byte> {
    if (!req_data_res.OK()) {
      std::cerr << req_data_res.Error() << std::endl;
      return srpc::Marshal<std::string>{}(req_data_res.Error());
    }
    auto maybe_req = srpc::Unmarshal<std::string>{}(req_data_res.Value());
    if (!maybe_req.second.has_value()) {
      std::cerr << "deserialization failure" << std::endl;
      return srpc::Marshal<std::string>{}("deserialization failure");
    }
    auto req = std::move(*maybe_req.second);
    std::cout << "server received request from " << from_addr << ": " << req
              << std::endl;

    std::stringstream res;
    res << "Hi " << from_addr << ", server received your request \"" << req
        << "\".";
    return srpc::Marshal<std::string>{}(res.str());
  });

  return 0;
}
