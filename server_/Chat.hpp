#ifndef Chat_hpp
#define Chat_hpp

#include "ws_listener/ClientListener.hpp"
#include <map>

class Chat : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener {
private:
	static constexpr const char* TAG = "Server_WSInstanceListener";
	std::map<v_int64, std::shared_ptr<ClientListener>> idToClient;
public:
	static std::atomic<v_int32> SOCKETS;
public:
	void addClient(const std::shared_ptr<ClientListener>& client);
	void removeClientById(v_int64 id);
	std::shared_ptr<ClientListener> getClientById(v_int64 id);

	void sendMessageToAllAsync(const oatpp::String& message);

	void onAfterCreate_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) override;
	void onBeforeDestroy_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) override;
};

#endif // !Chat_hpp
