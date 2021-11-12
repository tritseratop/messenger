#include "TCPClient.h"
#include "Utility.h"
#include <thread>
#include <future>
#include <iostream>

Result Client::Initialize() {
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0) {
		std::cerr << "Failed to start up the winsock API." << std::endl;
		return Result::Error;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		std::cerr << "Could not find a usable version of the winsock API dll." << std::endl;
		return Result::Error;
	}
	std::cout << "Winsock API successfully initialized!" << std::endl;
	return Result::Success;
}

Result Client::Shutdown() {
	WSACleanup();
	return Result::Success;
}

Result Client::Create() {
	return main_socket.Create();
}

Result Client::Close() {
	return main_socket.Close();
}

Result Client::Connect(Endpoint endpoint) {
	return main_socket.Connect(endpoint);
}

Result Client::Connect() {
	return main_socket.Connect(Endpoint(config.TCP_HOST.c_str(), config.TCP_PORT));
}

Result Client::defineLogin() {
	std::string message = "Enter your login: ";
	while (true) {
		std::cout << message;
		std::string login;
		std::getline(std::cin, login);
		if (login.empty()) { continue; }
		if (main_socket.Send(login) == Result::Error) {
			int error = WSAGetLastError();
			return Result::Error;
		}
		std::string response;
		if (main_socket.Recv(response) == Result::Error) {
			int error = WSAGetLastError();
			return Result::Error;
		}
		if (response == login) { 
			return Result::Success; 
		}
		else {
			message = "This login has already used.\nPlease, enter new login: ";
		}
	}
}

Result Client::setLogin(std::string login_) {
	login = login_;
	return Result::Success;
}

// ÆÈÅÑÒÜ
Result Client::StartChating() {
	std::thread native_thread = std::thread(
		[this]() {
			while (true) {
				std::string message;
				if (main_socket.Recv(message) == Result::Error) {
					int error = WSAGetLastError();
					return;
				}
				if (message[0] == '\\') {
					handleCommand(message);
				}
				else {
					std::cout << message << std::endl;
				}
			}
		}
	);
	std::string send_msg;
	while (true) {
		send_msg = "hello";
		std::getline(std::cin, send_msg);
		if (login.empty()) continue;
		if (main_socket.Send(genMessage(login, send_msg)) == Result::Error) {
			int error = WSAGetLastError();
			return Result::Error;
		}
	}
	native_thread.join();
	return Result::Error;
}

void Client::handleCommand(std::string msg) {
	std::string command, login;
	ParseMessage(msg, command, login);
	switch (GetCommand(command)) {
	case Commands::LOGIN:
		setLogin(login);
		break;
	default:
		return;
	}
}