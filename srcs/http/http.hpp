#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include "../Response/Response.hpp"

class Request;
class Socket;

Response		getResponse(const std::string& request, const Socket& socket);

#endif
