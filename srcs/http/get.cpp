#include "get.hpp"
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <iostream>

std::string	get(const std::string& path, const std::string& root) {
	std::fstream	file;
	std::string		open_string(root + path);
	DIR*			dir;

	if ( (dir = opendir(open_string.c_str())) ) {
		closedir(dir);
		open_string += "/index.html";
	}
	if (access(open_string.c_str(), F_OK))
		return ("HTTP/1.1 404 Not Found)\r\n\r\n<h1>File not found</h1>");
	else if (access(open_string.c_str(), R_OK))
		return ("HTTP/1.1 403 Forbidden)\r\n\r\n<h1>Bad permission</h1>");
	file.open(open_string.c_str(), std::ios_base::in);
	std::string		response = ("HTTP/1.1 200 OK\r\n\r\n");
	std::string		buff;
	while(std::getline(file, buff))
		response += buff;
	return (response);
}
