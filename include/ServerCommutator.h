#pragma once
#include "Result.h"
#include "Constants.h"
#include <string>
#include <set>
#include <queue>

enum ServerType {
	TCP,
	Websocket,
};

class ClientContainer {
public:
	int activeClientNumber;
	int clientId;
	bool addSocket(ServerType type);
	bool deleteSocket(ServerType type);

	ClientContainer() : activeClientNumber(0) {}
private:
	std::queue<ServerType> waitingClients;	// waiting websocket clients
};

class IServerObserver {
public:
	virtual Result sendToAll(const std::string& message) = 0;
	virtual void popWaiting() = 0;
	virtual void SetServer(IServerObserver* webserver_) = 0;
	virtual void setClientContainer(ClientContainer* clientContainer) = 0;
	//virtual Result moveFromWaiting() = 0;
	//TODO destructor
};

//class INotifier {
//public:
//	virtual void Attach(IServerObserver* observer) = 0;
//	virtual void Detach(IServerObserver* observer) = 0;
//	virtual void Notify(const std::string& msg) = 0;
//	//TODO destructor
//};
//
//class Notifier : public INotifier {
//public:
//	virtual void Attach(IServerObserver* server) override {
//		servers.push_back(server);
//	}
//	virtual void Detach(IServerObserver* server) override {
//		servers.remove(server);
//	}
//	virtual void Notify(const std::string& msg) override {
//		for (auto s : servers) {
//			s->sendToAll(msg);
//		}
//	}
//private:
//	std::list<IServerObserver*> servers;
//};