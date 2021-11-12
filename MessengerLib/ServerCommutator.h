#pragma once
#include "Result.h"
#include "Constants.h"
#include <string>
#include <set>
#include <queue>
#include <mutex>
#include <atomic>

enum ServerType {
	TCP,
	Websocket,
};

class ClientContainer {
public:
	bool addSocket(ServerType type);
	bool addSocket(ServerType type, int& id);
	void popWaiting();
	bool deleteSocket(ServerType type);
	void updateMessageHistory(const std::string& msg);
	std::deque<std::string> getMessageHistory();

	ClientContainer(int max_count) 
		: activeClientNumber(0)
		, MAX_CLIENT_COUNT(max_count)
		, clientId(0)
	{}
private:
	std::atomic<int> activeClientNumber;
	std::atomic<int> clientId;
	const int MAX_CLIENT_COUNT;

	std::queue<ServerType> waitingClients;	// waiting common clients
	std::deque<std::string> messageHistory;

	std::mutex m_changeWaiting;
	std::mutex m_changeHistory;
};

class IServerObserver {
public:
	virtual Result sendToAll(const std::string& message) = 0;
	virtual void popWaiting() = 0;
	virtual void SetServer(IServerObserver* webserver_) = 0;
	virtual void setClientContainer(ClientContainer* clientContainer) = 0;
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