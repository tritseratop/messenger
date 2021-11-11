#pragma once
#include "IncludeMe.h"
#include "ServerCommutator.h"
#include "logger/Logger.h"
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <vector>


class Server : public IServerObserver {
public:
	Server(const Configure& config);

	void Initialize();	// bool
	void Shutdown();	// void
	void Create();		// number
	void Close();
	virtual void SetServer(IServerObserver* webserver_); // TODO вынести в интерфейс
	virtual void setClientContainer(ClientContainer* clients);
	virtual void popWaiting();

	Result StartListen(Endpoint endpoint);
	void StartListen();
	void HandleClients();
	Result AddClient(Socket& client);
	Result SendToAll(std::string msg, const Socket& from);
	virtual Result sendToAll(const std::string& msg) override;
private:
	ServerType type = ServerType::TCP;

	std::ofstream file;
	logger::FileLogger* log;

	fd_set master;
	Socket main_socket;
	IServerObserver* webserver;

	std::list<Socket> client_sockets;
	std::map<int, Socket> clients;
	std::map<int, std::string> client_logins;
	//std::map<int, std::string> name_to_id;
	std::queue<Socket> waiting_clients;
	ClientContainer* clientContainer;

	std::deque<std::string> message_history;
	const Configure config;
	size_t client_count = 0;
	void DeleteSocket(Socket& s);
	void DeleteSocket(int handle);
};