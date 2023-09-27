#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include "../Response/Response.hpp"

class Request;
class Socket;

Request			getRequest(const int client_fd, const Socket& socket);
Response		getResponse(const std::string& request, const Socket& socket);
unsigned long	getChunkSize(int fd, std::string host);
std::string		readBody(int fd, unsigned long content_length, std::string host);
std::string		readHeader(int fd);

#endif
