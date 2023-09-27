#include "Cgi.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "../http/request_utils.hpp"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>

Cgi::Cgi(pid_t pid, int fd, time_t start, std::string host) :
	_pid(pid),
	_fd(fd),
	_status(0),
	_done(true),
	_start(start),
	_host(host) {}

Cgi::Cgi(const Cgi& src) { *this = src; }

Cgi::~Cgi(void) {}

Cgi&	Cgi::operator=(const Cgi& src) {
	if (this != &src) {
		_pid = src._pid;
		_fd = src._fd;
		_status = src._status;
		_done = src._done;
		_start = src._start;
		_host = src._host;
	}
	return (*this);
}

bool	Cgi::isActive(void) const { return (_pid != 0); }

bool	Cgi::done(void) {
	int	status = 0;
	pid_t	return_value = waitpid(_pid, &status, WNOHANG);
	if (return_value == -1) {
		close(_fd);
		throw InternalServerErrorException(_host);
	}
	else if (return_value != 0) {
		_done = true;
	}
	_status = status;
	time_t now = time(NULL);
	double timespan = difftime(now, _start);
	if (timespan >= TIMELIMIT)
		throw RequestTimeoutException(_host);
	return (_done);
}

std::string	Cgi::readResponse(void) {
	if (_status) {
		close(_fd);
		throw BadGatewayException(_host);
	}
	if (!pollFdIn(_fd))
		throw InternalServerErrorException(_host);
	char		buff[1025];
	int			n;
	std::string	content;
	memset(buff, 0, 1025);
	while((n = read(_fd, buff, 1024)) > 0) {
		try {
			content.append(buff, n);
		}
		catch (const std::exception& e) {
			close(_fd);
			throw InternalServerErrorException(_host);
		}
	}
	close(_fd);
	if (content.empty())
		throw InternalServerErrorException(_host);
	std::string::size_type	i = content.find("\r\n\r\n");
	if (i == std::string::npos) {
		i = content.find("\n\n");
		if (i == std::string::npos)
			throw InternalServerErrorException(_host);
	}
	std::stringstream	response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Length: " << content.substr(i + 4).size() << "\r\n";
	return (response.str().append(content.c_str(), content.size()));
}

void	Cgi::setActive(void) { _done = false; }
