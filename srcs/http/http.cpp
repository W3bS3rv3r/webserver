#include "http.hpp"
#include "error_codes.hpp"
#include "request_utils.hpp"
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
				std::string	host = getHeaderValue(request, "Host");
				throw InternalServerErrorException(host);
			}
			recv(client_fd, buff, n, 0);
		}
		else {
			recv(client_fd, buff, i - buff + delimiter.size(), 0);
			try {
				headers += buff;
			}
			catch (const std::exception& e) {
				std::string	host = getHeaderValue(request, "Host");
				throw InternalServerErrorException(host);
			}
			break ;
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	request += headers;

	std::string contentLengthStr = getHeaderValue(headers, "Content-Length");
	if (contentLengthStr.empty())
		return (headers);
	contentLength = strtol(contentLengthStr.c_str(), NULL, 10);

	int bodyBytes = 0;
	while (bodyBytes < contentLength) {
		memset(buff, 0, BUFFER_SIZE);
		if ((n = recv(client_fd, buff, std::min(BUFFER_SIZE - 1, contentLength - bodyBytes), 0)) <= 0)
			break;
		body += buff;
		bodyBytes += n;
	}
	request += body;
	return (request);
}

Response	getResponse(const std::string& request, const Socket& socket) {
	std::stringstream	stream(request);
	std::string			method, route, path;
	Response			response;

	std::string				host = getHeaderValue(request, "Host");
	const VirtualServer&	server = socket.getVServer(host);
	stream >> method;
	stream >> route;
	path = server.buildPath(route);
	if (method == "GET") {
		if (server.isCgi(route))
			response = cgiGet(path, request);
		else
			response.setResponse(get(path, request));
	}
	else if (method == "DELETE")
		response.setResponse(del(path, request));
	else if (method == "POST") {
		if (server.isCgi(route))
			response = cgiPost(path, request);
		else
			throw MethodNotAllowedException(host);
	}
	else if (method == "HEAD" || method == "PUT" || method == "CONNECT"
			|| method == "OPTIONS" || method == "TRACE")
		throw ServiceUnavailableException(host);
	else
		throw BadRequestException(host);
	return (response);
}
