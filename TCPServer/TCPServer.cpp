#include "TCPServer.h"

#include <iostream>
#include <thread>
#include "Utility.h"

using LOG = logger::FileLogger::e_logType;

void Server::Initialize() {
    WSADATA wsadata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (result != 0) {
        *log << LOG::LOG_ERROR << "Failed to start up the winsock API";
        return;
    }
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        *log << LOG::LOG_ERROR << "Could not find a usable version of the winsock API dll";
        return;
    }
    *log << LOG::LOG_INFO << "Winsock API successfully initialized";
}

void Server::Shutdown() { // TODO что делать с void?
    WSACleanup();
}

void Server::Create() {
    main_socket.Create();

}

void Server::Close() {
    main_socket.Close();
}

Result Server::AddClient(Socket& client) { // TODO разобраться с копированием в функции
    clients[client.GetSocketHandle()] = client;

    client.Send("\\login " + client.getLogin());
    *log << LOG::LOG_INFO << "Client #" + client.getLogin() + " is connected";

    std::string welcomeMsg = "Welcome to the Awesome Chat Server!"; // TODO receive from js
    client.Send(welcomeMsg);

    if (!clientContainer->getMessageHistory().empty()) {
        client.Send(createMessageFromQueue(clientContainer->getMessageHistory()));
    }
    return Result::Success;
}

Result Server::StartListen(Endpoint endpoint) {
    return main_socket.Listen(endpoint);
}

void Server::StartListen() {
    main_socket.Listen(config.TCP_HOST, config.TCP_PORT);
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
                    *log << LOG::LOG_ERROR << "Failed to connect client #" + client.getLogin();
                    int error = WSAGetLastError();
                    break;
                }

                FD_SET(client.GetSocketHandle(), &master);
                int id;
                bool isAdded = clientContainer->addSocket(type, id);
                client.setLogin(std::to_string(id));

                if (isAdded) {
                    AddClient(client);
                }
                else {
                    waiting_clients.push(client);
                    *log << LOG::LOG_INFO << "Client #" + client.getLogin() + " is added to waiting";
                    std::string msg = "The maximum number of messenger clients was reached. Please wait...";
                    client.Send(msg);
                }
            }
            else {
                std::string message;
                if (sock.Recv(message) != Result::Success) {
                    DeleteSocket(sock);
                }
                else {
                    if (message[0] == '\\') continue;
                    SendToAll(message, sock);
                    if (webserver != nullptr) {
                        webserver->sendToAll(message);
                    }
                }
            }
        }
    }

    return; // TODO придумать закрытие бесконечного цикла
}

Result Server::SendToAll(std::string msg, const Socket& from) {
    clientContainer->updateMessageHistory(msg);
    for (auto& s : clients) {
        if (from.GetSocketHandle() != s.second.GetSocketHandle()) {            
            if (s.second.Send(msg) != Result::Success) { // TODO use move
                DeleteSocket(s.second);
            }
        }
    }
    *log << LOG::LOG_MESSAGE << msg;
    return Result::Success;
}

Result Server::sendToAll(const std::string& msg) {
    for (auto& s : clients) {
        if (s.second.Send(msg) != Result::Success) { // TODO use move
            DeleteSocket(s.second);
        }
    }
    return Result::Success;
}

void Server::DeleteSocket(Socket& s) {
    int handle = s.GetSocketHandle();
    FD_CLR(handle, &master);
    s.Close();
    *log << LOG::LOG_INFO << "Client #" + s.getLogin() + " is disconnected";
    clients.erase(handle);
    if (clientContainer->deleteSocket(type)) {
        popWaiting();
    }
    else {
        webserver->popWaiting();
    }
}

Server::Server(const Configure& config_) : config(config_)
{
    log = logger::FileLogger::getInstance();
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
        clientContainer->popWaiting();
        AddClient(waiting_clients.front());
        *log << LOG::LOG_INFO << "Client #" + waiting_clients.front().getLogin() + " is added to chat";
        waiting_clients.pop();
    }
}

