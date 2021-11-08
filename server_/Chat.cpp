#include "Chat.hpp"

void Chat::addClient(const std::shared_ptr<ClientListener>& client) {
	idToClient[client->getClientId()] = client;
}

void Chat::popWaiting() {
	auto client = std::make_shared<ClientListener>(waitingClient.front(), this, SOCKETS++);
	waitingClient.front()->setListener(client);
	addClient(client);
	waitingClient.pop();
}

void Chat::SetTcpserver(IServerObserver* server) {
	tcpserver = server;
}

void Chat::setClientContainer(ClientContainer* clients) {
	clientContainer = clients;
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
	tcpserver->sendToAll(message.get()->std_str());
	for (auto& pair : idToClient) {
		pair.second->sendMessageAsync(message);
	}
}

std::atomic<v_int32> Chat::SOCKETS(0); 

void Chat::onAfterCreate_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) {
	OATPP_LOGD(TAG, "New incoming connection. Connection count = %d", SOCKETS.load());
	if (clientContainer->addSocket(type)) {
		auto client = std::make_shared<ClientListener>(socket, this, SOCKETS++);
		socket->setListener(client);
		addClient(client);
	}
	else {
		waitingClient.push(socket);
	}
}
void Chat::onBeforeDestroy_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) {
	//SOCKETS--;
	auto client = std::static_pointer_cast<ClientListener>(socket->getListener()); // TODO откуда он берет именно тот объект??
	removeClientById(client->getClientId());
	client->invalidateSocket();
	if (clientContainer->deleteSocket(type)) {
		popWaiting();
	}
	else {
		tcpserver->popWaiting();
	}
	OATPP_LOGD(TAG, "Connection closed. Connection count = %d", SOCKETS.load());
}