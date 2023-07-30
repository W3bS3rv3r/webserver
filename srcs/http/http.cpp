#include "http.hpp"
#include "error_codes.hpp"
#include "get.hpp"
#include "delete.hpp"
#include <cstring>
#include <exception>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <sstream>
#include <iostream>
#include "../CGI/CGI.hpp"

std::string	getRequest(const int client_fd) {
	int					n;
	char				buff[BUFFER_SIZE + 1];
	std::string			request;
	const std::string	delimiter("\r\n\r\n");

	memset(buff, 0, BUFFER_SIZE);
	while ((n = recv(client_fd, buff, BUFFER_SIZE - 1, MSG_PEEK)) > 0) {
		char*	i = std::search(buff, buff + n, delimiter.begin(),
								delimiter.end());
		if (i == buff + n) {
			try {
				request += buff;
			}
			catch (const std::exception& e) {
				throw InternalServerErrorException();
			}
			recv(client_fd, buff, n, 0);
		}
		else {
			recv(client_fd, buff, i - buff + delimiter.size(), 0);
			try {
				request += buff;
			}
			catch (const std::exception& e) {
				throw InternalServerErrorException();
			}
			break ;
		}
		memset(buff, 0, BUFFER_SIZE);
	}
	return (request);
}

std::string	getResponse(const std::string& request, const std::string& root) {
	std::stringstream	stream(request);
	std::string			method, path; 
	stream >> method;
	stream >> path;
	if (method == "GET")
		return (get(path, root));
	else if (method == "DELETE")
		return (del(path, root));
	else if (method == "HEAD" || method == "PUT" || method == "CONNECT"
			|| method == "OPTIONS" || method == "TRACE")
		throw ServiceUnavailableException();
	else if (method == "POST")
	{
		CGI *cgiReq = new CGI(path, root, request);
		if (cgiReq->isPathValid())
			cgiReq->runCGI();
		delete cgiReq;
		return ("TESTE");
	}
	else
		throw BadRequestException();
		
}
