#ifndef WSClient_hpp
#define WSClient_hpp

#include "WSListener.hpp"

class WSClient {
private:
	std::string login;

public:
	void run();
	void runWithoutCoroutine();
	void setLogin(std::string login_);
};

#endif //WSClient_hpp 

