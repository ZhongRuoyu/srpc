#include <cstdlib>
#include <iostream>
#include <utility>

#include "srpc/network/datagram_client.h"
#include "srpc/network/message.h"
#include "srpc/types/strings.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <address> <port> <message>"
              << std::endl;
    return -1;
  }

  auto client_res = srpc::DatagramClient::New(argv[1], std::atoi(argv[2]));
  if (!client_res.OK()) {
    std::cerr << client_res.Error() << std::endl;
    return -1;
  }
  auto client = std::move(client_res.Value());

  auto req_data = srpc::Marshal<std::string>{}(argv[3]);
  auto req_msg = srpc::MakeMessage(req_data);

  auto res_msg_res = client->SendAndReceive(req_msg);
  if (!res_msg_res.OK()) {
    std::cerr << res_msg_res.Error() << std::endl;
    return -1;
  }
  auto maybe_res_data = srpc::RemoveMessageHeader(res_msg_res.Value());
  if (!maybe_res_data.has_value()) {
    std::cerr << "deserialization failure" << std::endl;
    return -1;
  }
  auto res_data = srpc::Unmarshal<std::string>{}(*maybe_res_data);
  if (!res_data.second.has_value()) {
    std::cerr << "deserialization failure" << std::endl;
    return -1;
  }
  auto res = *res_data.second;
  std::cout << "client received response: " << res << std::endl;
  if (!res_msg_res.Error().empty()) {
    std::cout << res_msg_res.Error() << std::endl;
  }

  return 0;
}
