#include "TCPServer.h"
#include "WebsockServer.h"

int main() {
	oatpp::base::Environment::init();
	ClientContainer clients;
	Server tcpserver;
	WebsockServer websockserver;
	tcpserver.setClientContainer(&clients);
	tcpserver.SetServer(&websockserver);
	//server.SetTcpserver(&tcpserver);

	std::thread tcp_thread([](Server& tcpserver) {
		tcpserver.Initialize();
		tcpserver.Create();
		tcpserver.StartListen();
		tcpserver.HandleClients();
	}, std::ref(tcpserver));

	//WebsockServer server;
	websockserver.run(&tcpserver, &clients);

	oatpp::base::Environment::destroy();

	//tcp_thread.join();
	return 0;
}