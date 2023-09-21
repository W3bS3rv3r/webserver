#include "request_utils.hpp"
#include "../HTTPException/HTTPException.hpp"
#include <sys/poll.h>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

std::string getHeaderValue(const std::string& request, const std::string& headerKey) {
	std::string headerValue;
	
	size_t headerPos = request.find(headerKey + ": ");
	if (headerPos == std::string::npos)
		return ("");

	size_t valueStart = headerPos + headerKey.size() + 2;
	size_t valueEnd = request.find("\r\n", valueStart);
	if (valueEnd == std::string::npos)
		valueEnd = request.size();

	headerValue = request.substr(valueStart, valueEnd - valueStart);
	return (headerValue);
}

std::string getRequestURI(const std::string& request) {
	size_t startPos = request.find(" ") + 1;
	size_t endPos = request.find(" ", startPos);
	if (startPos == std::string::npos || endPos == std::string::npos)
		return ("");
	return (request.substr(startPos, endPos - startPos));
}

std::string getScriptFilename(const std::string& requestURI) {
	size_t lastSlashPos = requestURI.find_last_of("/");
	if (lastSlashPos == std::string::npos)
		return (requestURI);
	return (requestURI.substr(lastSlashPos + 1));
}

std::string getServerName(const std::string& request) {
	std::string host = getHeaderValue(request, "Host");
	size_t pos = host.find(":");
	if (pos != std::string::npos)
		return host.substr(0, pos);
	return host;
}

std::string getServerPort(const std::string& request) {
	std::string host = getHeaderValue(request, "Host");
	size_t pos = host.find(":");
	if (pos != std::string::npos && pos + 1 < host.size()) 
		return host.substr(pos + 1);
	return "";
}

std::string	getFileContent(const std::string& path, std::string host) {
	std::string		content;
	char			buff[BUFFER_SIZE + 1];
	int				n;
	int				fd = open(path.c_str(), O_NONBLOCK);

	if (fd < 0 || !pollFdIn(fd))
		throw InternalServerErrorException(host);
	memset(buff, 0, BUFFER_SIZE + 1);
	while ((n = read(fd, buff, BUFFER_SIZE - 1)) > 0) {
		try {
			content += buff;
		}
		catch (const std::exception& e) {
			close(fd);
			throw InternalServerErrorException(host);
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	if (n < 0)
		throw InternalServerErrorException(host);
	close(fd);
	return (content);
}

bool	pollFdIn(int fd) {
	struct pollfd	temp;

	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN | POLLOUT;
	temp.fd = fd;
	poll(&temp, 1, 0);
	if (temp.revents & POLLIN)
		return (true);
	return (false);
}

bool	pollFdOut(int fd) {
	struct pollfd	temp;

	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN | POLLOUT;
	temp.fd = fd;
	poll(&temp, 1, 0);
	if (temp.revents & POLLOUT)
		return (true);
	return (false);
}
