#include "http.hpp"
#include "error_codes.hpp"
#include "get.hpp"
#include "post.hpp"
#include "delete.hpp"
#include <cstring>
#include <exception>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <sstream>
#include <iostream>

std::string	getRequest(const int client_fd) {
	int					n;
	char				buff[BUFFER_SIZE + 1];
	std::string			headers;
	int					contentLength;
	size_t				contentLengthPos;
	std::string			body;
	std::string			request;
	const std::string	delimiter("\r\n\r\n");
	

	memset(buff, 0, BUFFER_SIZE + 1);
	while ((n = recv(client_fd, buff, BUFFER_SIZE - 1, MSG_PEEK | MSG_DONTWAIT)) > 0) {
		char*	i = std::search(buff, buff + n, delimiter.begin(), delimiter.end());
		if (i == buff + n) {
			try {
				headers += buff;
			}
			catch (const std::exception& e) {
				throw InternalServerErrorException();
			}
			recv(client_fd, buff, n, 0);
		}
		else {
			recv(client_fd, buff, i - buff + delimiter.size(), 0);
			try {
				headers += buff;
			}
			catch (const std::exception& e) {
				throw InternalServerErrorException();
			}
			break ;
		}
		memset(buff, 0, BUFFER_SIZE);
	}

	request += headers;

	// find content-length
	contentLengthPos = headers.find("Content-Length: ");
	contentLength = strtol(headers.c_str() + contentLengthPos + 16, NULL, 10); // 16 = "Content-Length: ""
	std::cout << "C-Len: " << contentLength << std::endl;
	
	// read body until content-length
	int bodyBytes = 0;
            
	while (bodyBytes < contentLength) {
		memset(buff, 0, BUFFER_SIZE);
		if (n = recv(client_fd, buff, std::min(BUFFER_SIZE - 1, contentLength - bodyBytes), MSG_DONTWAIT) <= 0)
			break;
		body += buff;
		bodyBytes += n;
	}

	request += body;
	
	std::cout << "\nUMA\n";

	return (request);
}

Response	getResponse(const std::string& request, const std::string& root,
		const std::string& extension)
{
	std::stringstream	stream(request);
	std::string			method, path;
	Response			response;

	stream >> method;
	stream >> path;
	if (method == "GET") {
		if (path.rfind(extension) == path.size() - extension.size())
			response = cgiGet(root + path);
		else
			response.setResponse(get(root + path));
	}
	else if (method == "DELETE")
		response.setResponse(del(path, root));
	else if (method == "POST") {
		if (path.rfind(extension) == path.size() - extension.size()) {
			response = cgiPost(root + path, request);
			return (response);
		}
		else
			throw MethodNotAllowedException();
	}
	else if (method == "HEAD" || method == "PUT" || method == "CONNECT"
			|| method == "OPTIONS" || method == "TRACE")
		throw ServiceUnavailableException();
	else
		throw BadRequestException();
	return (response);
}
