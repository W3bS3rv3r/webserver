#include "Cgi.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "../http/request_utils.hpp"
#include <cstddef>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>

Cgi::Cgi(void) : _pid(0), _request(NULL) {}

Cgi::Cgi(pid_t pid, int fd_out, int fd_in, const std::string* request) :
	_pid(pid),
	_response_fd(fd_out),
	_request_fd(fd_in),
	_status(0),
	_done(true),
	_bytes_written(0),
	_request(request)
{
	_start = time(NULL);
	if (_request) {
		_host = getHeaderValue(*request, "Host");
		std::string::size_type	headers_end = request->find("\r\n\r\n");
		if (headers_end == std::string::npos)
			throw BadRequestException(_host);
		_body = request->substr(headers_end + 4);
	}
}

Cgi::Cgi(const Cgi& src) : _request(src._request) { *this = src; }

Cgi::~Cgi(void) {}

Cgi&	Cgi::operator=(const Cgi& src) {
	if (this != &src) {
		_pid = src._pid;
		_response_fd = src._response_fd;
		_request_fd = src._request_fd;
		_status = src._status;
		_done = src._done;
		_start = src._start;
		_bytes_written = src._bytes_written;
		_host = src._host;
		_body = src._body;
	}
	return (*this);
}

bool	Cgi::isActive(void) const { return (_pid != 0); }

bool	Cgi::done(void) {
	int	status = 0;
	if (_bytes_written != _body.size())
		this->writeToCgi();
	pid_t	return_value = waitpid(_pid, &status, WNOHANG);
	if (return_value == -1) {
		close(_response_fd);
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
		close(_response_fd);
		throw BadGatewayException(_host);
	}
	if (!pollFdIn(_response_fd))
		throw InternalServerErrorException(_host);
	char		buff[1025];
	int			n;
	std::string	content;
	memset(buff, 0, 1025);
	while((n = read(_response_fd, buff, 1024)) > 0) {
		try {
			content.append(buff, n);
		}
		catch (const std::exception& e) {
			close(_response_fd);
			throw InternalServerErrorException(_host);
		}
	}
	close(_response_fd);
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

void	Cgi::writeToCgi(void) {
	if (!pollFdOut(_request_fd))
		return ;
	ssize_t	written = write(_request_fd, _body.c_str() + _bytes_written,
							_body.size() - _bytes_written);
	if (written < 0) {
		kill(_pid, SIGKILL);
		waitpid(_pid, NULL, WNOHANG);
		throw InternalServerErrorException(_host);
	}
	_bytes_written += written;
	if (_bytes_written == _body.size())
		close(_request_fd);
}

void	Cgi::setActive(void) { 
	_done = false;
	if (_request)
		this->writeToCgi();
}
