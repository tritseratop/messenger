#pragma once
#include "IncludeMe.h"
#include "ClientCommutator.h"
#include <iostream>
#include <list>
#include <queue>
#include <string>

class Client : public IClient {
public:
	Client();

	Result Initialize();	// bool
	Result Shutdown();	// void
	Result Create();		// number
	Result Close();

	Result Connect(Endpoint endpoint);
	Result Connect();
	Result StartChating();
	Result setLogin(std::string login_);
	Result defineLogin();
private:
	void handleCommand(std::string msg);
	Socket main_socket;
	std::string login;
	size_t client_count = 0;
};
