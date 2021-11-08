#pragma once
#include "IncludeMe.h"

class IClient {
public:
	virtual Result setLogin(std::string login_) = 0;
};