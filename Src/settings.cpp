#include <string>
#include <iostream>
#include <fstream>
#include <cctype> 
#include <stdlib.h>
#include <stdio.h>

#include "settings.h"
#include "virtual_file.h"

using namespace std;

extern void error(char *msg);

Settings::Settings(){
	version = 6;
	port = 10000;
	hostnames.SetSize(0);
	hostnames.AddItem(dynamic_string("localhost"));
	hostnames.AddItem(dynamic_string("127.0.0.1"));
	physical_webroot = new PhysicalFolder("","webroot",0,NULL);
	mimeTypesFile = "mimetypes.conf";
	defaultCharset = "ISO-8859-1";
	bDebugLog = false;
	maxConnections = 8;
}

void Settings::load(string filePath){
	string line,varName,varValue;
	int fileVersion = 0;
	ifstream file(filePath.c_str());
	if (file.is_open()){
		while(!file.eof()){
			getline(file,line);
			if (!line.empty() && line[0] != '#' && line.find(" ")){
				varName = line.substr(0,line.find_first_of(" "));
				varValue = line.substr(line.find_first_of(" ")+1);
				varName = Settings::strToLower(varName);
				if (varName == "port") port = atoi(varValue.c_str());
				if (varName == "hostnames")
				{
					hostnames.SetSize(0);
					for(size_t i = 0;;)
					{
						size_t j = varValue.find(',', i);
						if (j == std::string::npos)
						{
							hostnames.AddItem(dynamic_string(varValue.c_str() + i, varValue.length() - i));
							break;
						}
						else
							hostnames.AddItem(dynamic_string(varValue.c_str() + i, j - i));
						i = j + 1;
					}
				}
				if (varName == "mimetypesfile") mimeTypesFile = varValue;
				if (varName == "webroot"){
					physical_webroot->FilePath = varValue;
				}
				if (varName == "version") fileVersion = atoi(varValue.c_str());
				if (varName == "defaultcharset") defaultCharset = varValue;
				if (varName == "debuglog") bDebugLog = strToBool(varValue);
				if (varName == "maxconn") maxConnections = atoi(varValue.c_str());
			}
		}
		file.close();
	}
	if (fileVersion < version){
		printf("Automatically updating settings file to version %i\n", version);
		save(filePath);
	}
	else if (fileVersion > version){
		error("Settings file is from a newer version");
	}
}

void Settings::save(string filePath){
	FILE *file;
	fopen_s(&file,filePath.c_str(),"w");
	fprintf(file, "#Settings file version\n");
	fprintf(file, "version %d\n", version);
	fprintf(file, "\n");
	fprintf(file, "#Port Number to start on\n");
	fprintf(file, "port %d\n", port);
	fprintf(file, "\n");
	fprintf(file, "#Host names\n");
	fprintf(file, "hostnames ");
	for (int i = 0; i < (int)hostnames.Num(); i++)
	{
		if (i == 0)
			fprintf(file, "%s", (const char*)hostnames[i]);
		else
			fprintf(file, ",%s", (const char*)hostnames[i]); 
	}
	fprintf(file, "\n");
	fprintf(file, "\n");
	fprintf(file, "#Folder to serve from\n");
	fprintf(file, "webroot %s\n", (const char*)physical_webroot->FilePath);
	fprintf(file, "\n");
	fprintf(file, "#File to load Mime types from\n");
	fprintf(file, "mimetypesfile %s\n", mimeTypesFile.c_str());
	fprintf(file, "\n");
	fprintf(file, "#Maximum number of simultaneous connections\n");
	fprintf(file, "maxconn %i\n", maxConnections);
	fprintf(file, "\n");
	fprintf(file, "#Default character set\n");
	fprintf(file, "defaultcharset %s\n", defaultCharset.c_str());
	fprintf(file, "\n");
	fprintf(file, "#Enable Debug Logging\n");
	fprintf(file, "debuglog %s\n", bDebugLog ? "on" : "off" );
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
