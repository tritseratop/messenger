#include "ServerCommutator.h"


bool ClientContainer::addSocket(ServerType type) {
	if (activeClientNumber < MAX_CLIENT_COUNT) {
		++activeClientNumber;
		bool isAdded = true;
		return isAdded;
	}
	else {
		{
			std::lock_guard<std::mutex> m(m_changeWaiting);
			waitingClients.push(type);
		}
		bool isAdded = false;
		return isAdded;
	}
}

bool ClientContainer::addSocket(ServerType type, int& id) {
	id = clientId++;
	if (activeClientNumber < MAX_CLIENT_COUNT) {
		++activeClientNumber;
		bool isAdded = true;
		return isAdded;
	}
	else {
		{
			std::lock_guard<std::mutex> m(m_changeWaiting);
			waitingClients.push(type);
		}
		bool isAdded = false;
		return isAdded;
	}
}

void ClientContainer::popWaiting() {
	++activeClientNumber;
}

bool ClientContainer::deleteSocket(ServerType type) {
	activeClientNumber = activeClientNumber == 0 ? 0 : activeClientNumber - 1;
	if (waitingClients.empty()) {
		bool isReadyToAdd = false;
		return isReadyToAdd;
	}
	else {
		bool isReadyToAdd = waitingClients.front() == type;
		{
			std::lock_guard<std::mutex> m(m_changeWaiting);
			waitingClients.pop();
		}
		return isReadyToAdd;
	}
}

void ClientContainer::updateMessageHistory(const std::string& msg) {
	if (messageHistory.size() == MAX_MESSAGE_BUF_COUNT) {
		{
			std::lock_guard<std::mutex> m(m_changeHistory);
			messageHistory.pop_front();
		}
	}
	{
		std::lock_guard<std::mutex> m(m_changeWaiting);
		messageHistory.push_back(msg);
	}
}

std::deque<std::string> ClientContainer::getMessageHistory() {
	return messageHistory;
}