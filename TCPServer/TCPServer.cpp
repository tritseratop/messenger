#include "TCPServer.h"

#include <iostream>
#include <thread>
#include "Utility.h"

using LOG = logger::FileLogger::e_logType;

void ServerClientHandler(SOCKET client) {
}

void Server::Initialize() {
    WSADATA wsadata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (result != 0) {
        log << LOG::LOG_ERROR << "Failed to start up the winsock API";
        return;
    }
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        log << LOG::LOG_ERROR << "Could not find a usable version of the winsock API dll";
        return;
    }
    log << LOG::LOG_INFO << "Winsock API successfully initialized";
}

void Server::Shutdown() { // TODO ��� ������ � void?
    WSACleanup();
}

void Server::Create() {
    main_socket.Create();
}

void Server::Close() {
    main_socket.Close();
}

std::string createMessageFromQueue(const std::deque<std::string>& deq) {
    std::string res;
    for (const auto& d : deq) {
        res += d + "\n";
    }
    res.pop_back();
    return res;
}

Result Server::AddClient(Socket& client) { // TODO ����������� � ������������ � �������
    if (clientContainer->addSocket(type)) {
        client_it[client.GetSocketHandle()] = client_sockets.insert(client_sockets.end(), client);
        FD_SET(client.GetSocketHandle(), &master);
        std::string welcomeMsg = "Welcome to the Awesome Chat Server!"; // TODO receive from js
        client.Send(welcomeMsg);

        if (!message_history.empty()) {
            client.Send(createMessageFromQueue(message_history));
        }
        return Result::Success;
    }
    else {
        waiting_clients.push(client);
        log << LOG::LOG_INFO << "Client #" + std::to_string(client.GetSocketHandle()) + " is added to waiting";
        std::string msg = "The maximum number of messenger clients was reached. Please wait...";
        client.Send(msg);
        return Result::Success;
    }
}

Result Server::StartListen(Endpoint endpoint) {
    return main_socket.Listen(endpoint);
}

void Server::StartListen() {
    main_socket.Listen(config.getIp(), config.getPort());
    return;
}

void Server::HandleClients() {
    FD_ZERO(&master);
    FD_SET(main_socket.GetSocketHandle(), &master);

    while (true) {
        fd_set copy = master;
        int socket_count = select(0, &copy, nullptr, nullptr, nullptr);
        for (int i = 0; i < socket_count; ++i) {
            Socket sock(copy.fd_array[i]);
            if (sock.GetSocketHandle() == main_socket.GetSocketHandle()) {
                Socket client;
                Result res = main_socket.Accept(client);
                if (res == Result::Error) {
                    log << LOG::LOG_ERROR << "Failed to connect client #" + std::to_string(client.GetSocketHandle());
                    int error = WSAGetLastError();
                    break;
                }
                AddClient(client);
                log << LOG::LOG_INFO << "Client #" + std::to_string(client.GetSocketHandle()) + " is connected";
            }
            else {
                std::string message;
                if (sock.Recv(message) != Result::Success) {
                    DeleteSocket(sock);
                }
                else {
                    std::string msg_to_send = "Client #" + std::to_string(sock.GetSocketHandle()) + " " + message;
                    SendToAll(msg_to_send, sock);
                    webserver->sendToAll(msg_to_send);
                }
            }
        }
    }

    return; // TODO ��������� �������� ������������ �����
}

Result Server::SendToAll(std::string msg, const Socket& from) {
    for (auto& s : client_sockets) {
        if (from.GetSocketHandle() != s.GetSocketHandle()) {
            //std::string msg_to_send = "Client #" + std::to_string(from.GetSocketHandle()) + " " + msg;
            log << LOG::LOG_MESSAGE << msg;
            if (message_history.size() == MAX_MESSAGE_BUF_COUNT) {
                message_history.pop_front();
            }
            message_history.push_back(msg);
            if (s.Send(msg) != Result::Success) { // TODO use move
                DeleteSocket(s);
            }
        }
    }
    return Result::Success;
}

Result Server::sendToAll(const std::string& msg) {
    for (auto& s : client_sockets) {
        log << LOG::LOG_MESSAGE << msg;
        if (message_history.size() == MAX_MESSAGE_BUF_COUNT) {
            message_history.pop_front();
        }
        message_history.push_back(msg);
        if (s.Send(msg) != Result::Success) { // TODO use move
            DeleteSocket(s);
        }
    }
    return Result::Success;
}

void Server::DeleteSocket(Socket& s) {
    FD_CLR(s.GetSocketHandle(), &master);
    int handle = s.GetSocketHandle();
    s.Close();
    client_sockets.erase(client_it.at(handle)); //� ����� �������?
    client_it.erase(handle);
    log << LOG::LOG_INFO << "Client #" + std::to_string(handle) + " is disconnected";
    if (clientContainer->deleteSocket(type)) {
        popWaiting();
    }
    else {
        webserver->popWaiting();
    }
}

Server::Server()
    : log("Messenger Logger")
{
    SetConfig("D:/Develop/nodejs/vs2019/messenger_with_debug/build/Debug/config.json");
    //TODO delete
    //webserver.se("TCP server connected");
}

void Server::SetServer(IServerObserver* webserver_) {
    webserver = webserver_;
}

void Server::setClientContainer(ClientContainer* clients) {
    clientContainer = clients;
}

void Server::popWaiting() {
    if (!waiting_clients.empty()) {
        AddClient(waiting_clients.front());
        log << LOG::LOG_INFO << "Client #" + std::to_string(waiting_clients.front().GetSocketHandle()) + " is added to chat";
        waiting_clients.pop();
    }
}

void Server::SetConfig(const std::string& path) {
    std::string json;
    if (ReadTextFile(path, json) == Result::Success) {
        config = ParseJsonToConfig(json);
    }
}
