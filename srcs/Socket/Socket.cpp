#include "Socket.hpp"
#include "../http/http.hpp"
#include "../http/error_codes.hpp"
#include <exception>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>

//Constructors

Socket::Socket(const unsigned short port, std::string root) :
	_root(root),
	_is_listening(false),
	_port(port)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw Socket::CantCreateSocketException();
	bzero(&_socket, sizeof(_socket));
	_socket.sin_family		= AF_INET;
	_socket.sin_addr.s_addr	= htonl(INADDR_ANY);
	_socket.sin_port		= htons(_port);
}

Socket::~Socket(void) {
	close(_fd);
}

// Methods
void	Socket::listen(void) {
	int	reuse = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)))
		throw Socket::CantSetSocketOptionException();
	if (bind(_fd, (struct sockaddr *) &_socket, sizeof(_socket)))
		throw Socket::CantBindSocketException();
	if (::listen(_fd, 10))
		throw Socket::CantListenOnSocketException();
	_is_listening = true;
}

Connection*	Socket::acceptConnection(void) {
	if (!_is_listening)
		throw Socket::InactiveSocketException();
	const int	client_fd = accept(_fd, NULL, NULL);
	if (client_fd < 0)
		throw Socket::CantAcceptConnectionException();
	return (new Connection(client_fd, _port, _root));
}

//void	Socket::handleRequest(void) {
//	if (!_is_listening)
//		throw Socket::InactiveSocketException();
//	const int	client_fd = accept(_fd, NULL, NULL);
//	if (client_fd < 0)
//		throw Socket::CantAcceptConnectionException();
//	std::string	response;
//	try {
//		const std::string	request = getRequest(client_fd);
//		response = getResponse(request, _root);
//	}
//	catch (const std::exception& e) {
//		response = e.what();
//	}
//	send(client_fd, response.c_str(), response.size(), 0);
//	close(client_fd);
//}

int				Socket::getFd(void) const { return _fd; }
unsigned short	Socket::getPort(void) const { return _port; }

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
const char*	Socket::CantSetSocketOptionException::what(void) const throw() {
	return ("Unable to set socket options");
}
