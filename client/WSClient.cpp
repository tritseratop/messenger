#include "WSClient.hpp"
#include "AppComponent.hpp"

#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"

#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/core/async/Executor.hpp"

#include <thread>
#include <iostream>

void WSClient::run() {
    oatpp::base::Environment::init();
	AppComponent component(config);
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

    auto connector = oatpp::websocket::Connector::createShared(connectionProvider);

    executor->execute<ClientCoroutine>(connector, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    /*oatpp::websocket::Config config;
    config.maskOutgoingMessages = true;
    config.readBufferSize = 64;
    ClientCoroutine::SOCKET->setConfig(config);*/
    std::mutex socketWriteMutex;
    //executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, &socketWriteMutex);
    std::thread test_handler([&executor, this, &socketWriteMutex] {
        while (true) {
            std::string msg;
            std::cin >> msg;
            msg = genMessage(login, msg);
            std::lock_guard<std::mutex> lock(socketWriteMutex);
            executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, oatpp::String(msg.c_str()));
        }
    });
    test_handler.join();
    executor->join();
    oatpp::base::Environment::destroy();
}

void WSClient::setLogin(std::string login_) {
    login = login_;
}

void WSClient::runWithoutCoroutine() {
    AppComponent component(config);
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

    auto connector = oatpp::websocket::Connector::createShared(connectionProvider);
    auto connection = connector->connect("v3/channel_1?api_key=oCdCMcMPQpbvNjUIzqtvF1d2X2okWpDQj4AwARJuAgtjhzKxVEjQU6IdCjwm&notify_self");
    auto m_socket = oatpp::websocket::AsyncWebSocket::createShared(connection, true);
    //m_socket->setListener(std::make_shared<WSListener>());
    //m_socket->setListener(std::make_shared<WSListener>(client));

    executor->execute<ClientCoroutine>(connector, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    /*oatpp::websocket::Config config;
    config.maskOutgoingMessages = true;
    config.readBufferSize = 64;
    ClientCoroutine::SOCKET->setConfig(config);*/
    executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET);
    std::thread test_handler([&executor, this] {
        while (true) {
            std::string msg;
            std::cin >> msg;
            msg = genMessage(login, msg);
            executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, oatpp::String(msg.c_str()));
        }
        });
    test_handler.join();
    executor->join();
}