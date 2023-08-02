#include "get.hpp"
#include "error_codes.hpp"
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <iostream>

std::string	get(std::string path) {
	DIR*			dir;

	if ( (dir = opendir(path.c_str())) ) {
		closedir(dir);
		path += "/index.html";
	}
	if (access(path.c_str(), F_OK))
		throw NotFoundException();
	else if (access(path.c_str(), R_OK))
		throw ForbiddenException();
	std::fstream	file(path.c_str(), std::ios_base::in);
	std::string		response = ("HTTP/1.1 200 OK\r\n\r\n");
	std::string		buff;
	while(std::getline(file, buff)) {
			try {
				response += buff;
			}
			catch (const std::exception& e) {
				file.close();
				throw InternalServerErrorException();
			}
	}
	file.close();
	return (response);
}
