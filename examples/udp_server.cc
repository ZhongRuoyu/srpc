#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "srpc/network/datagram_server.h"
#include "srpc/network/message.h"
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
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return -1;
  }

  auto server_res = srpc::DatagramServer::New(std::atoi(argv[1]));
  if (!server_res.OK()) {
    std::cerr << server_res.Error() << std::endl;
    return -1;
  }

  auto server = std::move(server_res.Value());
  std::random_device rand;
  server->Listen(
      [&rand](const srpc::SocketAddress &from_addr,
              const std::vector<std::byte> &req_msg) -> std::vector<std::byte> {
        auto req_data_res = srpc::RemoveMessageHeader(req_msg);
        if (!req_data_res.has_value()) {
          std::cerr << "deserialization failure" << std::endl;
          return srpc::Marshal<std::string>{}("deserialization failure");
        }
        auto maybe_req = srpc::Unmarshal<std::string>{}(*req_data_res);
        if (!maybe_req.has_value()) {
          std::cerr << "deserialization failure" << std::endl;
          return srpc::Marshal<std::string>{}("deserialization failure");
        }
        auto req = std::move(*maybe_req);
        std::cout << "server received request from " << from_addr << ": " << req
                  << std::endl;

        // Wait for a random period of time (between 0 and 1000 milliseconds)
        // before sending back the result.
        std::this_thread::sleep_for(std::chrono::milliseconds(
            std::uniform_int_distribution<std::chrono::milliseconds::rep>{
                0, 1000}(rand)));

        std::stringstream res;
        res << "Hi " << from_addr << ", server received your request \"" << req
            << "\".";
        return srpc::MakeMessage(srpc::Marshal<std::string>{}(res.str()));
      });

  return 0;
}
