#pragma once
#include <string>
#include <map>

class Mimetypes{
public:
	void load(std::string filePath);
	std::string getFullType(std::string extension);
	std::string getType(std::string extension);
	std::string getSubType(std::string extension);
private:
	std::map<std::string, std::string> typeMap;
	std::string strToLower(std::string str);
};