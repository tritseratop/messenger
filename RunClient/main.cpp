#include "TCPClient.h"
#include "WSClient.hpp"

int main() {
	Client client;
	client.Initialize();
	client.Create();
	client.Connect();
	client.StartChating();
	client.Shutdown();

	//WSClient wsclient;
	//wsclient.run();
	return 0;
}