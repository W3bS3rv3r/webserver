#include "Socket.hpp"
#include "../http/http.hpp"
#include <cstring>
#include <cstdlib>
#include <unistd.h>

//Constructors
Socket::Socket(void) :
	_root("/webserver"),
	_is_listening(false),
	_socket_size(sizeof(struct sockaddr_in))
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw Socket::CantCreateSocketException();
	bzero(&_socket_addr, _socket_size);
	_socket_addr.sin_family			= AF_INET;
	_socket_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	_socket_addr.sin_port			= htons(80);
}

Socket::Socket(const unsigned short port) :
	_root("/webserver"),
	_is_listening(false),
	_socket_size(sizeof(struct sockaddr_in))
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw Socket::CantCreateSocketException();
	bzero(&_socket_addr, _socket_size);
	_socket_addr.sin_family			= AF_INET;
	_socket_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	_socket_addr.sin_port			= htons(port);
}

Socket::Socket(std::string root) :
	_root(root),
	_is_listening(false),
	_socket_size(sizeof(struct sockaddr_in))
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw Socket::CantCreateSocketException();
	bzero(&_socket_addr, _socket_size);
	_socket_addr.sin_family			= AF_INET;
	_socket_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	_socket_addr.sin_port			= htons(80);
}

Socket::Socket(const unsigned short port, std::string root) :
	_root(root),
	_is_listening(false),
	_socket_size(sizeof(struct sockaddr_in))
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw Socket::CantCreateSocketException();
	bzero(&_socket_addr, _socket_size);
	_socket_addr.sin_family			= AF_INET;
	_socket_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	_socket_addr.sin_port			= htons(port);
}

Socket::~Socket(void) {
	close(_listen_fd);
}

// Methods
void	Socket::listen(void) {
	if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&_socket_addr),
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
