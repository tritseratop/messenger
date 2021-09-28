#include "Chat.hpp"

void Chat::addClient(const std::shared_ptr<ClientListener>& client) {
	idToClient[client->getClientId()] = client;
}

void Chat::removeClientById(v_int64 id) {
	if (idToClient.count(id) != 0) {
		idToClient.erase(id);
	}
}
std::shared_ptr<ClientListener> Chat::getClientById(v_int64 id) {
	if (idToClient.count(id) != 0) {
		return idToClient[id];
	}
	return nullptr;
}
void Chat::sendMessageToAllAsync(const oatpp::String& message) {
	for (auto& pair : idToClient) {
		pair.second->sendMessageAsync(message);
	}
}

std::atomic<v_int32> Chat::SOCKETS(0);

void Chat::onAfterCreate_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) {
	OATPP_LOGD(TAG, "New incoming connection. Connection count = %d", SOCKETS.load());
	auto client = std::make_shared<ClientListener>(socket, this, SOCKETS++);
	socket->setListener(client);
	addClient(client);
}
void Chat::onBeforeDestroy_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) {
	//SOCKETS--;
	auto client = std::static_pointer_cast<ClientListener>(socket->getListener()); // TODO откуда он берет именно тот объект??
	removeClientById(client->getClientId());
	client->invalidateSocket();
	OATPP_LOGD(TAG, "Connection closed. Connection count = %d", SOCKETS.load());
}