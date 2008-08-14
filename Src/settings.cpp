#include "../include/settings.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cctype> 
#include <stdlib.h>
#include <stdio.h>

using namespace std;

Settings::Settings(){
	port = 10000;
}

void Settings::load(string filePath){
	string line,varName,varValue;
	ifstream file(filePath.c_str());
	if (file.is_open()){
		while(!file.eof()){
			getline(file,line);
			if (line[0] != '#' && !line.empty() && line.find(" ")){
				varName = line.substr(0,line.find_first_of(" "));
				varValue = line.substr(line.find_first_of(" ")+1);
				varName = Settings::strToLower(varName);
				if (varName == "port") port = atoi(varValue.c_str());
			}
		}
		file.close();
	}
}

void Settings::save(string filePath){
	FILE *file;
	file = fopen(filePath.c_str(),"w");
	fprintf(file, "#Port Number to start on\n");
	fprintf(file, "port %d\n",port);
	fclose(file);
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
