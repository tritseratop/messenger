#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "Chat.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

class AppComponent {
public:
	// ������� ����������� executor
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
		return std::make_shared<oatpp::async::Executor>(
			4, // ���-�� ������� data-processing 
			1, // ���-�� ������� �����/������
			1  // ���-�� ������� ��������
			);
		}());

	// ������� ���������, ������� ������� ����
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
		return oatpp::network::tcp::server::ConnectionProvider::createShared({ "localhost", 8000, oatpp::network::Address::IP_4 });
		}());

	// ������� ������
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
		return oatpp::web::server::HttpRouter::createShared();
		}());

	// ������� ���������, ������� ������������ (��������������?) ������� � ������� �������
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)("http", [] {
		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
		OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
		return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
		}());

	// ������������/�������������� DTO 
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
		return oatpp::parser::json::mapping::ObjectMapper::createShared();
		}());

	// ����������� ����� ������� ���� SocketListener, ����� �� ������ ����� ����������� �� ����������� ����� 
	OATPP_CREATE_COMPONENT(std::shared_ptr<Chat>, chat)([] {
		return std::make_shared<Chat>();
		}());

	// ���������� ����������� �� ��������� websocket
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)("websocket", [] {
		OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
		OATPP_COMPONENT(std::shared_ptr<Chat>, chat);
		auto connectionHanler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
		connectionHanler->setSocketInstanceListener(chat);
		return connectionHanler;
		}());
};

#endif // !AppComponent_hpp
