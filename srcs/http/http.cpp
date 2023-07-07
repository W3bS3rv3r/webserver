#include "http.hpp"
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>

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
std::string	getResponse(const std::string& request) {
	(void)request;
	return ("HTTP/1.0 200 OK\r\n\r\nHello World");
}
