#include "TCPClient.h"
#include "WSClient.hpp"

int main() {
	Configure configure(SetConfig("D:/Develop/network cpp_/vs2019/messenger_oatpp/config.json"));
	std::string command = "0";
	std::cout << "Enter 0 to run TCP-client\nEnter 1 to run WebSocket-client\n> ";
	std::getline(std::cin, command);
	if (command == "0") {
		Client client(configure);
		client.Initialize();
		client.Create();
		client.Connect();
		client.StartChating();
		client.Shutdown();
	}
	else {
		WSClient wsclient(configure);
		wsclient.run();
	}

	return 0;
}