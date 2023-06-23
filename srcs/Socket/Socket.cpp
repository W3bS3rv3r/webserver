#include "Socket.hpp"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>

//Constructors
Socket::Socket(const unsigned short port) :
	_is_listening(false),
	_socket_size(sizeof(struct sockaddr_in))
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0 /*|| fcntl(_listen_fd, F_SETFL, O_NONBLOCK)*/)
		throw Socket::CantCreateSocketException();
	bzero(&_server_addr, _socket_size);
	_server_addr.sin_family			= AF_INET;
	_server_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	_server_addr.sin_port			= htons(port);
}

Socket::~Socket(void) {
	close(_listen_fd);
}

// Helper Functions
namespace {
	std::string	getRequest(const int client_fd);
	std::string	getResponse(const std::string& request);
}

// Methods
void	Socket::listen(void) {
	if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&_server_addr),
		_socket_size))
	{
		throw Socket::CantBindSocketException();
	}
	if (::listen(_listen_fd, 10))
		throw Socket::CantListenOnSocketException();
	_is_listening = true;
}

void	Socket::handleRequest(void) {
	if (!_is_listening)
		throw Socket::InactiveSocketException();
	const int	client_fd = accept(_listen_fd, NULL, NULL);
	if (client_fd < 0)
		throw Socket::CantAcceptConnectionException();
	const std::string	request = getRequest(client_fd);
	const std::string	response = getResponse(request);
	send(client_fd, response.c_str(), response.size(), 0);
	close(client_fd);
}

// Exceptions
const char*	Socket::CantCreateSocketException::what(void) const throw() {
	return ("Unable to create socket");
}
const char*	Socket::CantBindSocketException::what(void) const throw() {
	return ("Unable to bind socket");
}
const char*	Socket::CantListenOnSocketException::what(void) const throw() {
	return ("Unable to listen on socket");
}
const char*	Socket::InactiveSocketException::what(void) const throw() {
	return ("Handled connection on inactive socket");
}
const char*	Socket::CantAcceptConnectionException::what(void) const throw() {
	return ("Unable to accept connection on socket");
}

// Helper Functions
namespace {
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
}
