#include "Socket.hpp"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

//Constructors
Socket::Socket(unsigned short port) : _is_listening(false) {
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0 || fcntl(_listen_fd, F_SETFL, O_NONBLOCK))
		throw Socket::CantCreateSocketException();
	bzero(&_server_addr, sizeof(_server_addr));
	_server_addr.sin_family			= AF_INET;
	_server_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	_server_addr.sin_port			= port;
}

Socket::~Socket(void) {
	close(_listen_fd);
}

// Methods
void	Socket::listen(void) {
	if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&_server_addr),
		sizeof(_server_addr)))
	{
		throw Socket::CantBindSocketException();
	}
	if (::listen(_listen_fd, 10))
		throw Socket::CantListenOnSocketException();
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
