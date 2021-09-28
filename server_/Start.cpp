#include "controller/Controller.hpp"
#include "AppComponent.hpp"
#include "oatpp/network/Server.hpp"
//#include "Chat.hpp"

void run() {
	AppComponent components;
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	auto myController = std::make_shared<Controller>();
	myController->addEndpointsToRouter(router);
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler, "http");
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	oatpp::network::Server server(connectionProvider, connectionHandler);
	OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());
	server.run();
}

int main() {
	oatpp::base::Environment::init();

	run();

	oatpp::base::Environment::destroy();

	return 0;
}