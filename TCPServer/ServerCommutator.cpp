#include "ServerCommutator.h"


bool ClientContainer::addSocket(ServerType type) {
	if (activeClientNumber < MAX_CLIENT_COUNT) {
		++activeClientNumber;
		bool isAdded = true;
		return isAdded;
	}
	else {
		waitingClients.push(type);
		bool isAdded = false;
		return isAdded;
	}
}

bool ClientContainer::deleteSocket(ServerType type) {
	activeClientNumber = activeClientNumber == 0 ? 0 : activeClientNumber - 1;
	if (waitingClients.empty()) {
		bool isReadyToAdd = false;
		return isReadyToAdd;
	}
	else {
		bool isReadyToAdd = waitingClients.front() == type;
		waitingClients.pop();
		return isReadyToAdd;
	}
}
