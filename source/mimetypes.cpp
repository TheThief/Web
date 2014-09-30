#include "mimetypes.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cctype> 
#include <stdlib.h>
#include <stdio.h>
#include <map>



void Mimetypes::load(std::string filePath){
	string line,ext,type;
	ifstream file(filePath.c_str());
	if (file.is_open()){
		while(!file.eof()){
			getline(file,line);
			if (!line.empty() && line[0] != '#' && line.find(" ")){
				ext = line.substr(0,line.find_first_of(" "));
				type = line.substr(line.find_first_of(" ")+1);
				ext = Mimetypes::strToLower(ext);
				if (typeMap.find(ext) == typeMap.end())
					typeMap.insert(make_pair(ext,type));
				else
					printf("Duplicate setting for \"%s\"\n", ext.c_str());
			}
		}
		file.close();
		printf("%i mime types loaded\n", typeMap.size());
	}
}

std::string Mimetypes::getFullType(std::string extension){
	map<string,string>::iterator iter = typeMap.find(extension);
	if(iter != typeMap.end()){
		return iter->second;
	}else{
		return "application/octet-stream";
	}
}
std::string Mimetypes::getType(std::string extension){
	string str = getFullType(extension);
	return str.substr(0,str.find_first_of("/"));
}

std::string Mimetypes::getSubType(std::string extension){
	string str = getFullType(extension);
	return str.substr(str.find_first_of("/")+1);
}

std::string Mimetypes::strToLower(std::string str){
	const size_t length = str.length();
		for(size_t i=0; i!=length; ++i){
			str[i] = std::tolower(str[i]);
		}
	return str;
}