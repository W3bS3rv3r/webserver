#include "get.hpp"
#include "error_codes.hpp"
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <iostream>

std::string	get(const std::string& path, const std::string& root) {
	std::string		open_string(root + path);
	DIR*			dir;

	if ( (dir = opendir(open_string.c_str())) ) {
		closedir(dir);
		open_string += "/index.html";
	}
	if (access(open_string.c_str(), F_OK))
		throw NotFoundException();
	else if (access(open_string.c_str(), R_OK))
		throw ForbiddenException();
	std::fstream	file(open_string.c_str(), std::ios_base::in);
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
