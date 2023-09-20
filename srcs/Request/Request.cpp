#include "Request.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "../http/request_utils.hpp"
#include <cstdlib>

// Constructors
Request::Request(void) :
	_ready(false), _chunked(false), _valid(true), _body_limit(0), _body_size(0)
{}

Request::Request(const Request& src) { *this = src; }

Request::~Request(void) {}

// Operators
Request&	Request::operator=(const Request& src) {
	if (this != &src) {
		_request = src._request;
		_host = src._host;
		_ready = src._ready;
		_chunked = src._chunked;
		_valid = src._valid;
		_fd = src._fd;
		_body_limit = src._body_limit;
		_body_size = src._body_size;
	}
	return (*this);
}

std::ostream&	operator<<(std::ostream& stream, const Request& req) {
	stream << req._request.substr(0, req._request.find('\n'));
	return (stream);
}


// Methods
void	Request::append(std::string str) { _request += str; }

const std::string&	Request::str(void) const { return (_request); }

bool	Request::ready(void) const { return _ready; }

bool	Request::valid(void) const { return _valid; }

bool	Request::chunked(void) const { return _chunked; }

void	Request::setFd(int fd) { _fd = fd; }

void	Request::setHost(std::string host) { _host = host; };

void	Request::setMaxBodySize(unsigned long n) { _body_limit = n; }

void	Request::setChunked(void) { _chunked = true; }

void	Request::getBody(void) {
	try {
		if (_chunked) {
			std::string	chunk = readChunk(_fd, _host);
			_body_size += chunk.size();
			if (_body_limit != 0 && _body_size > _body_limit)
				throw ContentTooLargeException(_host);
			_request += chunk;
			if (chunk.empty())
				_ready = true;
		}
		else {
			std::string cont_len_str = getHeaderValue(_request, "Content-Length");
			unsigned long	cont_len = strtoul(cont_len_str.c_str(), NULL, 10);
			if (_body_limit != 0 && cont_len > _body_limit)
				throw ContentTooLargeException(_host);
			_request += readBody(_fd, cont_len, _host);
			_ready = true;
		}
	}
	catch (...) {
		_valid = false;
		_ready = true;
		throw ;
	}
}
