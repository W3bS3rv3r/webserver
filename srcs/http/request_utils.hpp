#ifndef REQ_UTILS_HPP
# define REQ_UTILS_HPP

#define BUFFER_SIZE 1024

#include <string>

std::string	getHeaderValue(const std::string& request, const std::string& headerName);
std::string	getRequestURI(const std::string& request);
std::string	getScriptFilename(const std::string& requestURI);
std::string	getServerName(const std::string& request);
std::string	getServerPort(const std::string& request);
std::string	getFileContent(const std::string& path, std::string host);
bool		pollFd(int fd);

#endif
