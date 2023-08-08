#include "Cgi.hpp"
#include "../http/error_codes.hpp"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>
#include <iostream>

Cgi::Cgi(pid_t pid, int fd, time_t start) :
	_pid(pid),
	_fd(fd),
	_status(0),
	_done(true),
	_start(start) {}

Cgi::Cgi(const Cgi& src) { *this = src; }

Cgi::~Cgi(void) {}

Cgi&	Cgi::operator=(const Cgi& src) {
	if (this != &src) {
		_pid = src._pid;
		_fd = src._fd;
		_done = src._done;
	}
	return (*this);
}

bool	Cgi::isActive(void) const { return (_pid != 0); }

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
	time_t now = time(NULL);
	double timespan = difftime(now, _start);
	if (timespan >= TIMELIMIT)
		throw RequestTimeoutException();
	return (_done);
}

std::string	Cgi::readResponse(void) {
	if (_status) {
		close(_fd);
		throw BadGatewayException();
	}
	char		buff[1025];
	std::string	content;
	memset(buff, 0, 1025);
	while(read(_fd, buff, 1024)) {
		try {
			content += buff;
		}
		catch (const std::exception& e) {
			close(_fd);
			throw InternalServerErrorException();
		}
	}
	close(_fd);
	if (content.empty())
		throw InternalServerErrorException();
	std::string::size_type	i = content.find("\r\n\r\n");
	if (i == std::string::npos)
		throw InternalServerErrorException();
	std::stringstream	response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Length: " << content.substr(i + 4).size() << "\r\n";
	response << content.substr(0, i + 4);
	response << content.substr(i + 4);
	return (response.str());
}

void	Cgi::setActive(void) { _done = false; }
