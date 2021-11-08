#include "TCPClient.h"
#include <thread>
#include <future>
#include <iostream>

Client::Client() {}

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
	return main_socket.Connect(Endpoint(IP, PORT));
}

// ÆÈÅÑÒÜ
Result Client::StartChating() {
	std::thread native_thread = std::thread(
		[this]() {
			while (true) {
				std::string* message = new std::string();
				if (main_socket.Recv(*message) == Result::Error) {
					int error = WSAGetLastError();
					return;
				}
				std::cout << *message << std::endl;
			}
		}
	);
	std::string send_msg;
	while (true) {
		send_msg = "hello";
		std::getline(std::cin, send_msg);
		if (main_socket.Send(send_msg) == Result::Error) {
			int error = WSAGetLastError();
			native_thread.join();
			return Result::Error;
		}
	}
	native_thread.join();
	return Result::Error;
}
