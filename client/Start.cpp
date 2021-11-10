#include "WSClient.hpp"
#include "AppComponent.hpp"

#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"

#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/core/async/Executor.hpp"

#include <thread>
#include <iostream>

namespace {

    const char* TAG = "websocket-client";

    bool finished = false;

    void socketTask(const std::shared_ptr<oatpp::websocket::WebSocket>& websocket) {
        websocket->listen();
        OATPP_LOGD(TAG, "SOCKET CLOSED!!!");
        finished = true;
    }

}

void run() {

    OATPP_LOGI(TAG, "Application Started");
    AppComponent component;
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

    auto connector = oatpp::websocket::Connector::createShared(connectionProvider);

    executor->execute<ClientCoroutine>(connector);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    OATPP_LOGI(TAG, "Connected")
    /*oatpp::websocket::Config config;
    config.maskOutgoingMessages = true;
    config.readBufferSize = 64;
    ClientCoroutine::SOCKET->setConfig(config);*/
    executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET);
    std::thread test_handler([&executor] {
        while (true) {
            std::string msg;
            std::cin >> msg;
            executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, oatpp::String(msg.c_str()));
        }
    });
    test_handler.join();
    executor->join();
}

int main() {
    oatpp::base::Environment::init();
    WSClient client;
    client.run();
    oatpp::base::Environment::destroy();
    return 0;
}