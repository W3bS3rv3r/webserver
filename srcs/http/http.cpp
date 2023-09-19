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

Request	getRequest(const int client_fd, const Socket& socket) {
	Request	request;

	request.append(getHeaders(client_fd));
	std::string contentLengthStr = getHeaderValue(request.str(), "Content-Length");
	if (contentLengthStr.empty())
		return (request);
	std::string	host = getHeaderValue(request.str(), "Host");
	if (request.str().find("HTTP/1.1") == std::string::npos)
		throw HTTPVersionNotSupportedException(host);
	const VirtualServer&	vserver = socket.getVServer(host);
	unsigned long	content_length = strtoul(contentLengthStr.c_str(), NULL, 10);
	if (vserver.getBodySize()!= 0 && content_length > vserver.getBodySize())
		throw ContentTooLargeException(host);
	request.append(getBody(client_fd, content_length, host));
	return (request);
}

Response	getResponse(const std::string& request, const Socket& socket) {
	std::stringstream		stream(request);
	std::string				method, route, path;
	Response				response;
	std::string				host = getHeaderValue(request, "Host");
	const VirtualServer&	server = socket.getVServer(host);

	stream >> method;
	stream >> route;
	const Location&	location = server.getLocation(route);
	try {
		response = location.handleRequest(method, route, request, socket);
	}
	catch (HTTPException& e) {
		e.setHost(host);
		throw ;
	}
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
				*(i + delimiter.size()) = '\0';
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
