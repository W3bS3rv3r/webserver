#include "Response.hpp"
#include <cstddef>

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

size_t	Response::size(void) const { return _response.size(); }

const char*	Response::getResponse(void) const { return _response.c_str(); }
void	Response::setResponse(const std::string& resp) { _response = resp; }

size_t	Response::getPid(void) const { return _pid; }
void	Response::setPid(const size_t pid) { _pid = pid; }

int		Response::getFd(void) const { return _fd; }
void	Response::setFd(const int fd) { _fd= fd; }
