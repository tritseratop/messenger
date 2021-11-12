#include "WebsockServer.h"

void WebsockServer::run(IServerObserver* server_, ClientContainer* clients) {
	//run();
	AppComponent components(config);
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	auto myController = std::make_shared<Controller>();
	myController->addEndpointsToRouter(router);
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler, "http");
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	oatpp::network::Server server(connectionProvider, connectionHandler);

	OATPP_COMPONENT(std::shared_ptr<Chat>, chat);
	chat->SetTcpserver(server_);
	chat->setClientContainer(clients);
	server.run();
}
	
Result WebsockServer::sendToAll(const std::string& message) {
	OATPP_COMPONENT(std::shared_ptr<Chat>, chat);
	chat->sendMessageToAllAsync(oatpp::String(message.c_str()));
	return Result::Success;
}

void WebsockServer::SetServer(IServerObserver* server) {
	OATPP_COMPONENT(std::shared_ptr<Chat>, chat);
	chat->SetTcpserver(server);
}

void WebsockServer::setClientContainer(ClientContainer* clientContainer) {
	OATPP_COMPONENT(std::shared_ptr<Chat>, chat);
	chat->setClientContainer(clientContainer);
}

void WebsockServer::popWaiting() {
	OATPP_COMPONENT(std::shared_ptr<Chat>, chat);
	chat->popWaiting();
}



//int main() {
//	oatpp::base::Environment::init();
//
//	//run();
//	WebsockServer server;
//	server.run();
//
//	oatpp::base::Environment::destroy();
//
//	return 0;
//}