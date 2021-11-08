#pragma once
#include "IncludeMe.h"
#include <iostream>
#include <list>
#include <queue>
#include <string>

class Client {
public:
	Client();

	Result Initialize();	// bool
	Result Shutdown();	// void
	Result Create();		// number
	Result Close();

	Result Connect(Endpoint endpoint);
	Result Connect();
	Result StartChating();
private:
	Socket main_socket;
	std::list<Socket> client_sockets;
	std::queue<Socket> waiting_clients; // TODO очередь ожидания
	size_t client_count = 0;
};
