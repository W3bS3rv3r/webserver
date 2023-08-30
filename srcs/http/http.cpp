#include "http.hpp"
#include "../HTTPException/HTTPException.hpp"
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

// Helper functions
namespace {
	std::string	getHeaders(int fd);
	std::string	getBody(int fd, unsigned long content_length, std::string host);
}

std::string	getRequest(const int client_fd) {
	std::string			request;

	request += getHeaders(client_fd);
	std::string contentLengthStr = getHeaderValue(request, "Content-Length");
	if (contentLengthStr.empty())
		return (request);
	int	content_length = strtol(contentLengthStr.c_str(), NULL, 10);
	request += getBody(client_fd, content_length, getHeaderValue(request, "Host"));
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

// Helper functions definitions
namespace {
std::string	getHeaders(int fd) {
	int					n;
	char				buff[BUFFER_SIZE + 1];
	std::string			headers, delimiter("\r\n\r\n");

	memset(buff, 0, BUFFER_SIZE + 1);
	while ((n = recv(fd, buff, BUFFER_SIZE - 1, MSG_PEEK | MSG_DONTWAIT)) > 0) {
		char*	i = std::search(buff, buff + n, delimiter.begin(), delimiter.end());
		try {
			if (i == buff + n) {
				headers += buff;
				recv(fd, buff, n, MSG_DONTWAIT);
			}
			else {
				recv(fd, buff, i - buff + delimiter.size(), MSG_DONTWAIT);
				headers += buff;
				break ;
			}
		}
		catch (const std::exception& e) {
			std::string	host = getHeaderValue(headers, "Host");
			throw InternalServerErrorException(host);
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	return (headers);
}

std::string	getBody(int fd, unsigned long content_length, std::string host) {
	std::string	body;
	int			n = 1;
	char		buff[BUFFER_SIZE + 1];
	unsigned long	size;

	memset(buff, 0, BUFFER_SIZE + 1);
	while (body.size() < content_length && n > 0) {
		size = std::min(static_cast<unsigned long>(BUFFER_SIZE - 1), content_length - body.size());
		if ((n = recv(fd, buff, size, MSG_DONTWAIT)) <= 0)
			break;
		try {
			body += buff;
		}
		catch (const std::exception& e) {
			throw InternalServerErrorException(host);
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	return (body);
}
}
