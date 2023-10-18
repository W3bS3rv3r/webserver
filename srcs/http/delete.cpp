#include "delete.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "request_utils.hpp"
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>

std::string	del(const std::string& path) {
	if (access(path.c_str(), F_OK))
		throw NotFoundException("");
	else if (access(path.c_str(), R_OK))
		throw ForbiddenException("");
	std::string		response = ("HTTP/1.1 204 OK\r\nContent-Length: 0\r\n\r\n");
	if (remove(path.c_str()))
		throw InternalServerErrorException("");
	return (response);
}
