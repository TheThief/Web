#pragma once
#include <string>
#include <map>
using namespace std;

class Mimetypes{
public:
	void load(std::string filePath);
	std::string getFullType(std::string extension);
	std::string getType(std::string extension);
	std::string getSubType(std::string extension);
private:
	map<string,string> typeMap;
	std::string strToLower(std::string str);
};