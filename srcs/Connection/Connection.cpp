#include "Connection.hpp"
#include "../http/http.hpp"
#include "../Socket/Socket.hpp"
#include "../Response/Response.hpp"
#include "../HTTPException/HTTPException.hpp"
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
		if (_requests.empty()) {
			_requests.push(getRequest(_fd, *_socket));
			if (_requests.front().str().empty()) {
				_done = true;
				_requests.pop();
				return ;
			}
			else if (!_requests.front().chunked())
				_requests.front().getBody();
		}
		else if (_requests.front().valid())
			_requests.front().getBody();
		if (!_requests.front().ready())
			return ;
		std::cout << _fd << ':' << _socket->_port << " <- " << _requests.front() << std::endl;
	}
	catch(const HTTPException& e) {
		if (!_requests.empty()) {
			std::cout << _fd << ':' << _socket->_port << " <- " << _requests.front() << std::endl;
			_requests.pop();
		}
		_responses.push(Response(e.getResponse(*_socket)));
		if (e.getErrorCode() == "413")
			_done = true;
	}
}

void	Connection::writeResponse(void) {
	if (_responses.empty() && (_requests.empty() || !_requests.front().ready()))
		return ;
	else if (_responses.empty()) {
		try {
			_responses.push(getResponse(_requests.front().str(), *_socket));
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
		_responses.front() = Response(e.getResponse(*_socket));
	}
	std::cout << _fd << ':' << _socket->_port << " -> ";
	std::cout << _responses.front().getStatus() << std::endl;
	send(_fd, _responses.front().getResponse(), _responses.front().size(), MSG_DONTWAIT);
	_responses.pop();
}

bool	Connection::done(void) const { return _done; }
unsigned short Connection::getPort(void) { return _socket->_port; }
int	Connection::getFd(void) { return _fd; }
