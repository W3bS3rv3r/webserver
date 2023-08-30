#ifndef REQ_UTILS_HPP
# define REQ_UTILS_HPP

#include <string>

#define BUFFER_SIZE 1024

std::string	getHeaderValue(const std::string& request, const std::string& headerName);
std::string	getFileContent(const std::string& path, std::string host);

#endif
