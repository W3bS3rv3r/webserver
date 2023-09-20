#include "Request.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "../http/request_utils.hpp"
#include <cstdlib>

// Constructors
Request::Request(void) : _ready(false) {}

Request::Request(const Request& src) { *this = src; }

Request::~Request(void) {}

// Operators
Request&	Request::operator=(const Request& src) {
	if (this != &src) {
		_request = src._request;
		_host = src._host;
		_ready = src._ready;
		_fd = src._fd;
		_body_limit = src._body_limit;
	}
	return (*this);
}

// Methods
void	Request::append(std::string str) { _request += str; }

const std::string&	Request::str(void) const { return (_request); }

bool	Request::ready(void) const { return _ready; }

void	Request::setFd(int fd) { _fd = fd; }

void	Request::setHost(std::string host) { _host = host; };

void	Request::setMaxBodySize(unsigned long n) { _body_limit = n; }

void	Request::getBody(void) {
	std::string cont_len_str = getHeaderValue(_request, "Content-Length");
	if (cont_len_str.empty()) {
		_ready = true;
	}
	unsigned long	cont_len = strtoul(cont_len_str.c_str(), NULL, 10);
	if (_body_limit != 0 && cont_len > _body_limit)
		throw ContentTooLargeException(_host);
	_request += readBody(_fd, cont_len, _host);
	_ready = true;
}
