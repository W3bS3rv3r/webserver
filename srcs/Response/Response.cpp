#include "Response.hpp"
#include "../http/error_codes.hpp"
#include <cstddef>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>

Response::Response(std::string response) : _response(response), _pid(0) {}

Response::Response(const Response& src) { *this = src; }

Response::~Response(void) {}

Response&	Response::operator=(const Response& src) {
	if (this != &src) {
		_response = src._response;
		_pid = src._pid;
		_fd= src._fd;
	}
	return (*this);
}

std::string	Response::getStatus(void) const {
	return (_response.substr(0, _response.find('\n')));
}

bool	Response::ready(void) {
	if (_pid == 0)
		return (true);
	else {
		int	exit_status = 0;
		pid_t	status = waitpid(_pid, &exit_status, WNOHANG);
		if (status == -1) {
			_response = InternalServerErrorException().what();
			close(_fd);
			kill(_pid, SIGTERM);
			return (true);
		}
		else if (!status)
			return (false);
		else {
			if (exit_status) {
				_response = BadGatewayException().what();
				close(_fd);
				return (true);
			}
			char		buff[1025];
			std::string	str = "HTTP/1.1 200 OK\n";
			memset(buff, 0, 1025);
			while(read(_fd, buff, 1024)) {
				try {
					str += buff;
				}
				catch (const std::exception& e) {
					_response = InternalServerErrorException().what();
					close(_fd);
					return (true);
				}
			}
			close(_fd);
			_response = str;
			return (true);
		}
	}
}

size_t	Response::size(void) const { return _response.size(); }

const char*	Response::getResponse(void) const { return _response.c_str(); }
void	Response::setResponse(const std::string& resp) { _response = resp; }

size_t	Response::getPid(void) const { return _pid; }
void	Response::setPid(const size_t pid) { _pid = pid; }

int		Response::getFd(void) const { return _fd; }
void	Response::setFd(const int fd) { _fd= fd; }
