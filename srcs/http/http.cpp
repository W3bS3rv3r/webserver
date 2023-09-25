#include "http.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "request_utils.hpp"
#include "get.hpp"
#include "post.hpp"
#include "delete.hpp"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <sstream>
#include <iostream>
#include <utility>

// Helper functions
namespace {
	//std::string		getHeaders(int fd);
	long			getChunkSize(int fd, std::string host);
}

Request	getRequest(const int client_fd, const Socket& socket) {
	Request	request(client_fd, socket);
	request.read();
	return (request);
}

//Request	getRequest(const int client_fd, const Socket& socket) {
//	Request	request;
//
//	request.append(getHeaders(client_fd));
//	if (request.str().empty())
//		return (request);
//	std::string	host = getHeaderValue(request.str(), "Host");
//	if (request.str().find("HTTP/1.1") == std::string::npos)
//		throw HTTPVersionNotSupportedException(host);
//	const VirtualServer&	vserver = socket.getVServer(host);
//
//	std::string	transfer_enconding = getHeaderValue(request.str(), "Transfer-Encoding");
//	if (transfer_enconding == "chunked")
//		request.setChunked();
//	request.setFd(client_fd);
//	request.setMaxBodySize(vserver.getBodySize());
//	request.setHost(host);
//	return (request);
//}

std::string	readHeader(int fd) {
	int					n;
	char				buff[BUFFER_SIZE + 1];
	std::string			header, delimiter("\r\n\r\n");

	memset(buff, 0, BUFFER_SIZE + 1);
	while ((n = recv(fd, buff, BUFFER_SIZE - 1, MSG_PEEK | MSG_DONTWAIT)) > 0) {
		char*	i = std::search(buff, buff + n, delimiter.begin(), delimiter.end());
		try {
			if (i == buff + n) {
				header += buff;
				recv(fd, buff, n, MSG_DONTWAIT);
			}
			else {
				recv(fd, buff, i - buff + delimiter.size(), MSG_DONTWAIT);
				*(i + delimiter.size()) = '\0';
				header += buff;
				break ;
			}
		}
		catch (const std::exception& e) {
			std::string	host = getHeaderValue(header, "Host");
			throw InternalServerErrorException(host);
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	return (header);
}

std::string	readBody(int fd, unsigned long content_length, std::string host) {
	std::string	body;
	int			n = 1;
	char		buff[BUFFER_SIZE + 1];
	unsigned long	size;

	if (!content_length)
		return ("");
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

std::pair<std::string, bool> readChunk(int fd, std::string host) {
	std::string		chunk;
	std::string		delimiter("\r\n");
	char			buff[BUFFER_SIZE + 1];
	long			chunk_size;
	bool			done = false;

	chunk_size = getChunkSize(fd, host);
	if (chunk_size) {
		memset(buff, 0, BUFFER_SIZE + 1);
		if (recv(fd, buff, chunk_size, MSG_DONTWAIT) != chunk_size)
			throw InternalServerErrorException(host);
		try {
			chunk += buff;
		}
		catch (const std::exception& e) {
			throw InternalServerErrorException(host);
		}
		if (chunk.empty())
			return (std::make_pair(chunk, done));
	}
	else if (chunk_size < 0)
		return (std::make_pair(chunk, done));
	else
		done = true;
	memset(buff, 0, BUFFER_SIZE + 1);
	recv(fd, buff, 2, MSG_PEEK | MSG_DONTWAIT);
	if (delimiter == buff)
		recv(fd, buff, 2, MSG_DONTWAIT);
	else if (buff[0] != '\0')
		throw BadRequestException(host);
	return (std::make_pair(chunk, done));
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

namespace {
long	getChunkSize(int fd, std::string host) {
	int			n;
	char		buff[BUFFER_SIZE + 1];
	std::string	delimiter("\r\n");
	std::string	size;

	memset(buff, 0, BUFFER_SIZE + 1);
	while ((n = recv(fd, buff, BUFFER_SIZE - 1, MSG_PEEK | MSG_DONTWAIT)) > 0) {
		char*	i = std::search(buff, buff + n, delimiter.begin(), delimiter.end());
		try {
			if (i == buff + n) {
				size += buff;
				recv(fd, buff, n, MSG_DONTWAIT);
			}
			else {
				recv(fd, buff, i - buff + delimiter.size(), MSG_DONTWAIT);
				*(i + delimiter.size()) = '\0';
				size += buff;
				break ;
			}
		}
		catch (const std::exception& e) {
			throw InternalServerErrorException(host);
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	if (!size.empty() && !std::isdigit(size[0])) {
		throw BadRequestException(host);
	}
	else if (size.empty())
		return (-1);
	return (strtol(size.c_str(), NULL, 16));
}
}
