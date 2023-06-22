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

// Exceptions
const char*	Socket::CantCreateSocketException::what(void) const throw() {
	return ("Unable to create socket");
}
