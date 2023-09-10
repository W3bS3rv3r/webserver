#include "Socket.hpp"
#include "../http/http.hpp"
#include "../HTTPException/HTTPException.hpp"
#include <exception>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h> 

//Constructors
Socket::Socket(unsigned short port, VirtualServer vserver) {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw Socket::CantCreateSocketException();
	_port = port;
	if (_vservers.empty())
		_default_vserver = vserver.getName();
	_vservers.insert(std::make_pair(vserver.getName(), vserver));
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
}

Connection*	Socket::acceptConnection(void) {
	_client_info_len = sizeof(_client_info); 
	const int	client_fd = accept(_fd, (struct sockaddr*)&_client_info, &_client_info_len);
	if (client_fd < 0)
		throw Socket::CantAcceptConnectionException();
	return (new Connection(client_fd, this));
}

void	Socket::addVserver(const VirtualServer& server) {
	std::pair<std::map<std::string, VirtualServer>::iterator, bool> p;

	p = _vservers.insert(std::make_pair(server.getName(), server));
	if (!p.second)
		throw DuplicateException();
}

int				Socket::getFd(void) const { return _fd; }
unsigned short	Socket::getPort(void) const { return _port; }

VirtualServer	Socket::getVServer(std::string host_header) const {
	std::map<std::string, VirtualServer>::const_iterator	i;
	host_header = host_header.substr(0, host_header.find_last_of(":"));

	if ((i = _vservers.find(host_header)) != _vservers.end())
		return (i->second);
	return _vservers.begin()->second;
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
const char*	Socket::CantAcceptConnectionException::what(void) const throw() {
	return ("Unable to accept connection on socket");
}
const char*	Socket::CantSetSocketOptionException::what(void) const throw() {
	return ("Unable to set socket options");
}
const char*	Socket::DuplicateException::what(void) const throw() {
	return ("Duplicate server name on the same port in the config file");
}
