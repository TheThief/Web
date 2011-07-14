#pragma once

#include "virtual_file.h"
#include "auto_ptr.h"
#include <string>

class Settings {
public:
	Settings();
	void load(std::string filePath);
	void save(std::string filePath);
	VirtualFolder* getVirtualFolder();
	int port;
	dynamic_array<dynamic_string> hostnames;
	std::string mimeTypesFile;
	std::string defaultCharset;
	bool bDebugLog;
	int maxConnections;
private:
	PhysicalFolder *physical_webroot;
	std::string strToLower(std::string str);
	bool strToBool(std::string str);
protected:
	int version;
};
