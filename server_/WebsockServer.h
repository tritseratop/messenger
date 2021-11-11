#pragma once
#include "controller/Controller.hpp"
#include "AppComponent.hpp"
#include "oatpp/network/Server.hpp"
#include "ServerCommutator.h"

void run();

class WebsockServer : public IServerObserver {
public:
	WebsockServer(const Configure& config_) : config(config_) {}
	void run(IServerObserver* server_, ClientContainer* clients);
	virtual Result sendToAll(const std::string& message) override;
	virtual void SetServer(IServerObserver* server);
	virtual void setClientContainer(ClientContainer* clientContainer);
	virtual void popWaiting();
private:
	const Configure config;
};