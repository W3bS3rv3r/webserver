#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>
#include "../Response/Response.hpp"

#define BUFFER_SIZE 1024

std::string	getRequest(const int client_fd);
Response	getResponse(const std::string& request, const std::string& root,
		const std::string& suffix);

#endif
