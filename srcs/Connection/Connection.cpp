#include "Connection.hpp"
#include "../http/http.hpp"
#include "../Socket/Socket.hpp"
#include "../Response/Response.hpp"
#include "../http/error_codes.hpp"
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

//CONSTRUCTORS
Connection::Connection(int fd, const Socket* socket) :
	_fd(fd),
	_socket(socket),
	_done(false){}

Connection::~Connection(void) {
	close(_fd);
}

//METHODS
void	Connection::readRequest(void) {
	try {
		_requests.push(getRequest(_fd));
		if (_requests.front().empty()) {
			_done = true;
			_requests.pop();
			return ;
		}
		std::cout << _fd << ':' << _socket->_port << " <- ";
		std::cout << _requests.back().substr(0, _requests.back().find('\n'));
		std::cout << std::endl;
	}
	catch(const HTTPException& e) {
		_responses.push(Response(e.getResponse(*_socket)));
	}
}

void	Connection::writeResponse(void) {
	if (_responses.empty() && _requests.empty())
		return ;
	else if (_responses.empty()) {
		try {
			_responses.push(getResponse(_requests.front(), *_socket));
		}
		catch(const HTTPException& e) {
			_responses.push(Response(e.getResponse(*_socket)));
		}
		_requests.pop();
	}
	try {
		if (!_responses.front().isReady())
			return ;
	}
	catch(const HTTPException& e) {
		_responses.push(Response(e.getResponse(*_socket)));
	}
	std::cout << _fd << ':' << _socket->_port << " -> ";
	std::cout << _responses.front().getStatus() << std::endl;
	send(_fd, _responses.front().getResponse(), _responses.front().size(), 0);
	_responses.pop();
}

bool	Connection::done(void) const { return _done; }
unsigned short Connection::getPort(void) { return _socket->_port; }
int	Connection::getFd(void) { return _fd; }
