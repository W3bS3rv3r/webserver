#include "Request.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "../http/request_utils.hpp"
#include <cstdlib>

// Constructors
Request::Request(int fd, const Socket& socket) :
	_ready(false),
	_chunked(false),
	_valid(true),
	_read_header(false),
	_fd(fd),
	_body_limit(0),
	_body_size(0),
	_cont_len(0),
	_socket(socket)
{}

Request::Request(const Request& src) : _socket(src._socket) { *this = src; }

Request::~Request(void) {}

// Operators
Request&	Request::operator=(const Request& src) {
	if (this != &src) {
		_read_header = src._read_header;
		_request = src._request;
		_host = src._host;
		_ready = src._ready;
		_chunked = src._chunked;
		_valid = src._valid;
		_fd = src._fd;
		_body_limit = src._body_limit;
		_body_size = src._body_size;
		_cont_len = src._cont_len;
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

void	Request::read(void) {
	if (!_read_header) {
		_request += readHeader(_fd);
		this->updateHeaderStatus();
		if (_read_header){
			getHeaderValues();
		}
	}
	if (_read_header) {
		try {
			this->getBody();
		}
		catch (...) {
			_ready = true;
			throw ;
		}
	}
}

void	Request::updateHeaderStatus(void) {
	if (_request.rfind("\r\n\r\n") == _request.size() - 4)
		_read_header = true;
}

void	Request::getHeaderValues(void) {
	_host = getHeaderValue(_request, "Host");
	if (_request.find("HTTP/1.1") == std::string::npos)
		throw HTTPVersionNotSupportedException(_host);
	std::string	transfer_enconding = getHeaderValue(_request, "Transfer-Encoding");
	if (transfer_enconding == "chunked")
		_chunked = true;
	const VirtualServer&	vserver = _socket.getVServer(_host);
	_body_limit = vserver.getBodySize();
	std::string cont_len_str = getHeaderValue(_request, "Content-Length");
	_cont_len = strtoul(cont_len_str.c_str(), NULL, 10);
}

void	Request::getBody(void) {
	if (_chunked) {
		std::pair<std::string, bool> p = readChunk(_fd, _host);
		_body_size += p.first.size();
		if (_body_limit != 0 && _body_size > _body_limit)
			throw ContentTooLargeException(_host);
		_request += p.first;
		if (p.second)
			_ready = true;
	}
	else {
		std::string	chunk = readBody(_fd, _cont_len - _body_size, _host);
		_body_size += chunk.size();
		if (_body_limit != 0 && _body_size > _body_limit)
			throw ContentTooLargeException(_host);
		_request += chunk;
		if (_body_size == _cont_len) {
			_ready = true;
		}
	}
}
