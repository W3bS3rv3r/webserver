#include "Cgi.hpp"
#include "../http/error_codes.hpp"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>

Cgi::Cgi(pid_t pid, int fd) : _pid(pid), _fd(fd), _status(0), _done(true) {}

Cgi::Cgi(const Cgi& src) { *this = src; }

Cgi::~Cgi(void) {}

Cgi&	Cgi::operator=(const Cgi& src) {
	if (this != &src) {
		_pid = src._pid;
		_fd = src._fd;
	}
	return (*this);
}

bool	Cgi::active(void) const { return (_pid != 0); }

bool	Cgi::done(void) {
	int	status = 0;
	pid_t	return_value = waitpid(_pid, &status, WNOHANG);
	if (return_value == -1) {
		close(_fd);
		throw InternalServerErrorException();
	}
	else if (return_value != 0) {
		_done = true;
	}
	_status = status;
	return (_done);
}

std::string	Cgi::readResponse(void) {
	if (_status) {
		close(_fd);
		throw BadGatewayException();
	}
	char		buff[1025];
	std::string	resp = "HTTP/1.1 200 OK\n";
	memset(buff, 0, 1025);
	while(read(_fd, buff, 1024)) {
		try {
			resp += buff;
		}
		catch (const std::exception& e) {
			close(_fd);
			throw InternalServerErrorException();
		}
	}
	close(_fd);
	return (resp);
}

void	Cgi::setActive(void) { _done = false; }
