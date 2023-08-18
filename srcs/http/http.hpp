#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include "../Response/Response.hpp"
#include "../Socket/Socket.hpp"

#define BUFFER_SIZE 1024

std::string	getRequest(const int client_fd);
Response	getResponse(const std::string& request, const Socket& socket);

#endif
