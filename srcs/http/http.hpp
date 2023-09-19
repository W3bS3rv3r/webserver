#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include "../Response/Response.hpp"
#include "../Request/Request.hpp"
#include "../Socket/Socket.hpp"

Request		getRequest(const int client_fd, const Socket& socket);
Response	getResponse(const std::string& request, const Socket& socket);

#endif
