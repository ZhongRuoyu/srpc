#include <cstdlib>
#include <iostream>
#include <utility>

#include "srpc/network/client.h"
#include "srpc/types/strings.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <address> <port> <message>"
              << std::endl;
    return -1;
  }

  auto client_res = srpc::Client::New(argv[1], std::atoi(argv[2]));
  if (!client_res.OK()) {
    std::cerr << client_res.Error() << std::endl;
    return -1;
  }
  auto client = std::move(client_res.Value());

  auto req_data = srpc::Marshal<std::string>{}(argv[3]);
  auto res_data_res = client->SendAndReceive(req_data);
  if (!res_data_res.OK()) {
    std::cerr << res_data_res.Error() << std::endl;
    return -1;
  }
  auto res_data = srpc::Unmarshal<std::string>{}(res_data_res.Value());
  if (!res_data.second.has_value()) {
    std::cerr << "deserialization failure" << std::endl;
    return -1;
  }
  auto res = *res_data.second;
  std::cout << "client received response: " << res << std::endl;

  return 0;
}
