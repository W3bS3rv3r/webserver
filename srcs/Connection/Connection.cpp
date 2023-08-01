#include "Connection.hpp"
#include "../http/http.hpp"
#include "../Socket/Socket.hpp"
#include "../Response/Response.hpp"
#include "../http/error_codes.hpp"
#include <exception>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <signal.h>

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
			_responses.push(getResponse(_requests.front(), _root, _suffix));
		}
		catch(const std::exception& e) {
			_responses.push(Response(e.what()));
		}
		_requests.pop();
	}
	if (!this->responseReady())
		return ;
	std::cout << _fd << ':' << _port << " -> ";
	std::cout << _responses.front().getStatus() << std::endl;
	send(_fd, _responses.front().getResponse(), _responses.front().size(), 0);
	_responses.pop();
	if (_responses.empty() && _requests.empty())
		_done = true;
}

#include <iostream>
bool	Connection::responseReady(void) {
	Response	resp = _responses.front();
	if (resp.getPid() == 0)
		return (true);
	else {
		pid_t	status = waitpid(resp.getPid(), NULL, WNOHANG);
		if (status == -1) {
			_responses.front().setResponse(InternalServerErrorException().what());
			close(resp.getFd());
			kill(resp.getPid(), SIGTERM);
			return (true);
		}
		else if (!status)
			return (false);
		else {
			char		buff[1025];
			std::string	str;
			memset(buff, 0, 1025);
			while(read(resp.getFd(), buff, 1024)) {
				try {
					str += buff;
				}
				catch (const std::exception& e) {
					_responses.front().setResponse(InternalServerErrorException().what());
					close(resp.getFd());
					return (true);
				}
			}
			close(resp.getFd());
			_responses.front().setResponse(str);
			return (true);
		}
	}
}

bool	Connection::done(void) const { return _done; }
unsigned short Connection::getPort(void) { return _port; }
int	Connection::getFd(void) { return _fd; }
