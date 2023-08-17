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
