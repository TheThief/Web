#pragma once

#include "../include/virtual_file.h"
#include <string>

class Settings {
public:
	Settings();
	void load(std::string filePath);
	void save(std::string filePath);
	VirtualFolder* getVirtualFolder();
	int port;
private:
	PhysicalFolder *physical_webroot;
	std::string strToLower(std::string str);
	bool strToBool(std::string str);
protected:
	int version;
};
