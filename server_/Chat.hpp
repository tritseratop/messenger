#ifndef Chat_hpp
#define Chat_hpp

#include "ws_listener/ClientListener.hpp"
#include "ServerCommutator.h"
#include "logger/Logger.h"
#include <map>
#include <queue>

class Chat : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener {
private:
	IServerObserver* tcpserver;
	ClientContainer* clientContainer;
	ServerType type = ServerType::Websocket;
	static constexpr const char* TAG = "Server_WSInstanceListener";
	std::map<v_int64, std::shared_ptr<ClientListener>> idToClient;
	std::queue<std::shared_ptr<ClientListener>> waitingClient;
	std::mutex m_writeMessage;
	logger::FileLogger* log;
public:
	static std::atomic<v_int32> SOCKETS; // for id generation
public:
	Chat();
	void addClient(const std::shared_ptr<ClientListener>& socket);
	void popWaiting();
	void SetTcpserver(IServerObserver* server);
	void setClientContainer(ClientContainer* clients);
	void removeClientById(v_int64 id);
	bool isClienAdded(v_int64 id);
	std::shared_ptr<ClientListener> getClientById(v_int64 id);

	void sendMessageToAllAsync(const oatpp::String& message);
	void sendMessageToAllAsync(v_int64 source_id, const oatpp::String& message);

	void onAfterCreate_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) override;
	void onBeforeDestroy_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) override;
};

#endif // !Chat_hpp
