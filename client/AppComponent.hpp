#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/core/macro/component.hpp"

class AppComponent {
public:
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
		return std::make_shared<oatpp::async::Executor>(1, 1, 1);
		}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider)([] {
		return oatpp::network::tcp::client::ConnectionProvider::createShared({ "localhost", 8000, oatpp::network::Address::IP_4 });
		}());
	
};

#endif // !AppComponent_hpp
