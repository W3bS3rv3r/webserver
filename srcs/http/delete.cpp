#include "delete.hpp"
#include "error_codes.hpp"
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>

std::string	del(const std::string& path, const std::string& root) {
	std::string		open_string(root + path);
	DIR*			dir;

	if ( (dir = opendir(open_string.c_str())) ) {
		closedir(dir);
		open_string += "/index.html";
	}
	if (access(open_string.c_str(), F_OK))
		throw NotFoundException();
	else if (access(open_string.c_str(), W_OK))
		throw ForbiddenException();
	std::string		response = ("HTTP/1.1 204 OK\r\n\r\n");
	try {
		std::remove(open_string.c_str());
	}
	catch (const std::exception& e) {
		throw InternalServerErrorException();
	}
	return (response);
}
