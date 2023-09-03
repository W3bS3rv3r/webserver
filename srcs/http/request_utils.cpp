#include "request_utils.hpp"

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
