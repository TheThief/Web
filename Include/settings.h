#pragma once

#include <string>

class Settings {
public:
	Settings();
	void load(std::string filePath);
	void save(std::string filePath);
	int port;
private:
	std::string strToLower(std::string str);
	bool strToBool(std::string str);
};
