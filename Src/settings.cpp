#include "../include/settings.h"
#include "../include/virtual_file.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cctype> 
#include <stdlib.h>
#include <stdio.h>

using namespace std;

Settings::Settings(){
	version = 1;
	port = 10000;
	physical_webroot = new PhysicalFolder("","webroot",0,NULL);
}

void Settings::load(string filePath){
	string line,varName,varValue;
	int fileVersion = 0;
	ifstream file(filePath.c_str());
	if (file.is_open()){
		while(!file.eof()){
			getline(file,line);
			if (line[0] != '#' && !line.empty() && line.find(" ")){
				varName = line.substr(0,line.find_first_of(" "));
				varValue = line.substr(line.find_first_of(" ")+1);
				varName = Settings::strToLower(varName);
				if (varName == "port") port = atoi(varValue.c_str());
				if (varName == "webroot"){
					physical_webroot->FilePath = varValue.c_str();
					physical_webroot->FilePath.clonebuffer();
				}
				if (varName == "version") fileVersion = atoi(varValue.c_str());
			}
		}
		file.close();
	}
	if (fileVersion < version) save(filePath);
}

void Settings::save(string filePath){
	FILE *file;
	file = fopen(filePath.c_str(),"w");
	fprintf(file, "#Settings file version\n");
	fprintf(file, "version %d\n",version);
	fprintf(file, "#Port Number to start on\n");
	fprintf(file, "port %d\n",port);
	fprintf(file, "#Folder to serve from\n");
	fprintf(file, "webroot %s\n",(const char*)physical_webroot->FilePath);
	fclose(file);
}

VirtualFolder* Settings::getVirtualFolder(){
	return physical_webroot;
}

std::string Settings::strToLower(std::string str){
	const size_t length = str.length();
		for(size_t i=0; i!=length; ++i){
			str[i] = std::tolower(str[i]);
		}
	return str;
}

bool Settings::strToBool(std::string str){
	str = Settings::strToLower(str);
	if (str == "on" || str == "yes" || str == "1" || str == "true") return true;
	return false;
}
