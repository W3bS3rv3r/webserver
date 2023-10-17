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
	_written(0),
	_socket(socket),
	_done(false){}

Connection::~Connection(void) {
	close(_fd);
}

//METHODS
void	Connection::readRequest(void) {
	try {
		if (_requests.empty())
			_requests.push(getRequest(_fd, *_socket));
		else if (_requests.front().str().empty()) {
			_done = true;
			_requests.pop();
			return ;
		}
		else if (!_requests.front().ready()) {
			_requests.front().read();
		}
		if (_requests.front().ready()) {
			std::cout << _fd << ':' << _socket->_port << " <- ";
			std::cout << _requests.front() << std::endl;
		}
	}
	catch(const HTTPException& e) {
		if (!_requests.empty()) {
			std::cout << _fd << ':' << _socket->_port << " <- ";
			std::cout << _requests.front() << std::endl;
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
	this->sendResponse();
}

void	Connection::sendResponse(void) {
	int	sent = 0;
	if (_written == 0) {
		std::cout << _fd << ':' << _socket->_port << " -> ";
		std::cout << _responses.front().getStatus() << std::endl;
	}
	sent = send(_fd, _responses.front().getResponse() + _written,
				_responses.front().size() - _written, MSG_DONTWAIT);
	_written += sent;
	if (_written == _responses.front().size()) {
		_responses.pop();
		_written = 0;
	}
}

bool	Connection::done(void) const { return _done; }
unsigned short Connection::getPort(void) { return _socket->_port; }
int	Connection::getFd(void) { return _fd; }
