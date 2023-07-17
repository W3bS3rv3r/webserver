#ifndef HTTP_HPP
# define HTTP_HPP

#include <string>

#define BUFFER_SIZE 1024

std::string	getRequest(const int client_fd);
std::string	getResponse(const std::string& request, const std::string& root);

#endif
