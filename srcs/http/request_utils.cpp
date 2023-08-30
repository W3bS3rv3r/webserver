#include "request_utils.hpp"
#include "../HTTPException/HTTPException.hpp"
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

std::string	getFileContent(const std::string& path, std::string host) {
	std::string		content;
	char			buff[BUFFER_SIZE + 1];
	int				n;
	int				fd = open(path.c_str(), O_NONBLOCK);

	if (fd < 0)
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
