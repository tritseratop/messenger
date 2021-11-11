#include "Chat.hpp"
#include "Utility.h"

using LOG = logger::FileLogger::e_logType;

Chat::Chat() {
	log = logger::FileLogger::getInstance();
}

bool Chat::addClient(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) {
	if (clientContainer->addSocket(type)) {
		auto client = std::make_shared<ClientListener>(socket, this, SOCKETS++);
		socket->setListener(client);
		std::string msg = "\\login " + std::to_string(client->getClientId());
		{
			std::lock_guard<std::mutex> m(m_writeMessage);
			client->sendMessageAsync(oatpp::String(msg.c_str()));
		}
		*log << LOG::LOG_INFO << "Client #" + std::to_string(client->getClientId()) + " is connected";

		idToClient[client->getClientId()] = client;
		if (!clientContainer->getMessageHistory().empty()) {
			std::lock_guard<std::mutex> m(m_writeMessage);
			client->sendMessageAsync(oatpp::String(createMessageFromQueue(clientContainer->getMessageHistory()).c_str()));
		}
		return true;
	}

	else {
		return false;
	}
}

void Chat::popWaiting() {
	if (!waitingClient.empty()) {
		addClient(waitingClient.front());
		waitingClient.pop();
	}
}

void Chat::SetTcpserver(IServerObserver* server) {
	tcpserver = server;
}

void Chat::setClientContainer(ClientContainer* clients) {
	clientContainer = clients;
}

void Chat::removeClientById(v_int64 id) {
	*log << LOG::LOG_INFO << "Client #" + std::to_string(id) + " is disconnected";
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

void Chat::sendMessageToAllAsync(v_int64 source_id, const oatpp::String& message) {
	clientContainer->updateMessageHistory(message->std_str());
	tcpserver->sendToAll(message.get()->std_str());
	std::lock_guard<std::mutex> m(m_writeMessage);
	for (auto& pair : idToClient) {
		if (pair.first != source_id) {
			pair.second->sendMessageAsync(message);
		}
	}
	*log << LOG::LOG_MESSAGE << message.get()->std_str();
}

void Chat::sendMessageToAllAsync(const oatpp::String& message) {
	std::lock_guard<std::mutex> m(m_writeMessage);
	for (auto& pair : idToClient) {
		pair.second->sendMessageAsync(message);
	}
}

std::atomic<v_int32> Chat::SOCKETS(0); 

void Chat::onAfterCreate_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) {
	OATPP_LOGD(TAG, "New incoming connection. Connection count = %d", SOCKETS.load());
	if (!addClient(socket)) {
		waitingClient.push(socket);
		*log << LOG::LOG_INFO << "WS client #NULL is added to waiting";
	}
}
void Chat::onBeforeDestroy_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) {
	//SOCKETS--;
	auto client = std::static_pointer_cast<ClientListener>(socket->getListener());
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