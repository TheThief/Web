#pragma once

#include <string>

#include "virtual_file.h"
#include "dynamic_array.h"
#include "dynamic_string.h"

class Settings {
public:
	enum eDebugLog
	{
		debuglog_off,
		debuglog_on,
		debuglog_errors,
	};

	Settings();
	void load(std::string filePath);
	void save(std::string filePath);
	VirtualFolder* getVirtualFolder();
	int port;
	dynamic_array<dynamic_string> hostnames;
	std::string mimeTypesFile;
	std::string defaultCharset;
	eDebugLog bDebugLog;
	int maxConnections;
private:
	PhysicalFolder *physical_webroot;
	std::string strToLower(std::string str);
	bool strToBool(std::string str);
protected:
	int version;
};
