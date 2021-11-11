#pragma once
#include <string>
#include <fstream>
#include <deque>
#include "Result.h"
#include "ParserInclude.h"
#include "Constants.h"

Result ReadTextFile(const std::string& path, std::string& result);

Configure ParseJsonToConfig(const std::string& json);

Configure SetConfig(const std::string& path);

void ParseMessage(const std::string& input, std::string& command, std::string& message);

Commands GetCommand(std::string command);

std::string genMessage(const std::string login, const std::string& message);

std::string createMessageFromQueue(const std::deque<std::string>& deq);