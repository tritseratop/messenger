#include "TCPClient.h"

int main() {
	Client client;
	client.Initialize();
	client.Create();
	client.Connect();
	client.StartChating();
	client.Shutdown();
	return 0;
}