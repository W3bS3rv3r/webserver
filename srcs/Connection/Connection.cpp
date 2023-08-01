#include "Connection.hpp"
#include "../http/http.hpp"
#include "../Socket/Socket.hpp"
#include "../Response/Response.hpp"
#include <exception>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>

//CONSTRUCTORS
Connection::Connection(int fd, const Socket& socket) :
	_fd(fd),
	_port(socket._port),
	_root(socket._root),
	_suffix(socket._suffix),
	_done(false){}

Connection::~Connection(void) {
	close(_fd);
}

//METHODS
void	Connection::readRequest(void) {
	try {
		_requests.push(getRequest(_fd));
		std::cout << _fd << ':' << _port << " <- ";
		std::cout << _requests.back().substr(0, _requests.back().find('\n'));
		std::cout << std::endl;
	}
	catch(const std::exception& e) {
		_responses.push(Response(e.what()));
	}
}

void	Connection::writeResponse(void) {
	if (_responses.empty() && _requests.empty())
		return ;
	else if (_responses.empty()) {
		try {
			_responses.push(getResponse(_requests.front(), _root));
		}
		catch(const std::exception& e) {
			_responses.push(Response(e.what()));
		}
		_requests.pop();
	}
	std::cout << _fd << ':' << _port << " -> ";
	std::cout << _responses.front().getStatus() << std::endl;
	send(_fd, _responses.front().getResponse(), _responses.front().size(), 0);
	_responses.pop();
	if (_responses.empty() && _requests.empty())
		_done = true;
}

bool	Connection::done(void) const { return _done; }
unsigned short Connection::getPort(void) { return _port; }
int	Connection::getFd(void) { return _fd; }
