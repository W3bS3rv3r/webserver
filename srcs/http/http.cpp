#include "http.hpp"
#include <cstring>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <iostream>
#include "get.hpp"

std::string	getRequest(const int client_fd) {
	int					n;
	char				buff[BUFFER_SIZE + 1];
	std::string			request;
	const std::string	delimiter("\r\n\r\n");

	memset(buff, 0, BUFFER_SIZE);
	while ((n = recv(client_fd, buff, BUFFER_SIZE - 1, MSG_PEEK)) > 0) {
		char*	i = std::search(buff, buff + n, delimiter.begin(),
								delimiter.end());
		if (i == buff + n)
			recv(client_fd, buff, n, 0);
		else {
			recv(client_fd, buff, i - buff + delimiter.size(), 0);
			request += buff;
			break ;
		}
		request += buff;
		memset(buff, 0, BUFFER_SIZE);
	}
	return (request);
}

std::string	getResponse(const std::string& request, const std::string& root) {
	std::string	reqln = request.substr(0, request.find('\n'));
	size_t		space_one = reqln.find(' ');
	size_t		space_two = reqln.find(' ', space_one + 1);
	std::string	method = reqln.substr(0, space_one);
	std::string	path = reqln.substr(space_one + 1, space_two - space_one - 1);
	if (method == "GET")
		return (get(path, root));
	return ("HTTP/1.1 200 OK\r\n\r\nNO MEHTOD YET");
}
