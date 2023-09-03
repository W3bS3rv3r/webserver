#include "delete.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "request_utils.hpp"
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>

std::string	del(const std::string& path, std::string request) {
	std::string		open_string(path);
	DIR*			dir;
	std::string		host = getHeaderValue(request, "Host");

	if ( (dir = opendir(open_string.c_str())) ) {
		closedir(dir);
		open_string += "/index.html";
	}
	if (access(open_string.c_str(), F_OK))
		throw NotFoundException(host);
	else if (access(open_string.c_str(), W_OK))
		throw ForbiddenException(host);
	std::string		response = ("HTTP/1.1 204 OK\r\nContent-Length: 0\r\n\r\n");
	try {
		std::remove(open_string.c_str());
	}
	catch (const std::exception& e) {
		throw InternalServerErrorException(host);
	}
	return (response);
}
