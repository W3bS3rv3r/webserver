#include "Request.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "../http/request_utils.hpp"
#include <cstdlib>
#include <exception>

// Constructors
Request::Request(int fd, const Socket& socket) :
	_ready(false),
	_chunked(false),
	_valid(true),
	_read_header(false),
	_fd(fd),
	_header_size(0),
	_cont_len(0),
	_socket(socket)
{}

Request::Request(const Request& src) : _socket(src._socket) { *this = src; }

Request::~Request(void) {}

// Operators
Request&	Request::operator=(const Request& src) {
	if (this != &src) {
		_request = src._request;
		_host = src._host;
		_ready = src._ready;
		_chunked = src._chunked;
		_read_header = src._read_header;
		_fd = src._fd;
		_header_size = src._header_size;
		_cont_len = src._cont_len;
	}
	return (*this);
}

std::ostream&	operator<<(std::ostream& stream, const Request& req) {
	stream << req._request.substr(0, req._request.find('\n'));
	return (stream);
}


// Methods
void	Request::append(std::string str) { _request.append(str.c_str(), str.size()); }

const std::string&	Request::str(void) const { return (_request); }

bool	Request::ready(void) const { return _ready; }

bool	Request::valid(void) const { return _valid; }

bool	Request::chunked(void) const { return _chunked; }

void	Request::setFd(int fd) { _fd = fd; }

void	Request::setHost(std::string host) { _host = host; };

void	Request::setMaxBodySize(unsigned long n) { _body_limit = n; }

void	Request::setChunked(void) { _chunked = true; }

void	Request::read(void) {
	int					n;
	char				buff[BUFFER_SIZE];
	std::string			header, delimiter("\r\n\r\n");

	n = recv(_fd, buff, BUFFER_SIZE, MSG_DONTWAIT);
	if (n <= 0) {
		_request.clear();
		return ;
	}
	else {
		try {
			_request.append(buff, n);
		}
		catch (...) {
			throw InternalServerErrorException(getHeaderValue(header, "Host"));
		}
	}
	this->checkRequestStatus();
}

void	Request::checkRequestStatus(void) {
	this->updateHeaderStatus();
	if (_read_header) {
		if (_request.size() - _header_size > _body_limit)
			throw ContentTooLargeException(_host);
		this->checkBody();
	}
}

void	Request::checkBody(void) {
	if (_chunked)
		this->unchunkRequest();
	else {
		if (_request.size() == _header_size + _cont_len)
			_ready = true;
	}
}

void Request::unchunkRequest(void) {
	if (_request.substr(_request.size() - 5) != "0\r\n\r\n")
		return ;
	unsigned int	chunk_size;
	unsigned int	erase_size;
	unsigned int	i = _header_size - 1;
	while (_request[i]) {
		chunk_size = strtoul(_request.c_str() + i, NULL, 16);
		erase_size = _request.find("\r\n", i);
		if (!erase_size)
			throw BadRequestException(_host);
		else
			erase_size = erase_size - i + 2;
		_request.erase(i, erase_size);
		i += chunk_size;
		if (_request[i] != '\r' || _request[i + 1] != '\n')
			throw BadRequestException(_host);
		else
			_request.erase(i, 2);
	}
	_ready = true;
}

void	Request::updateHeaderStatus(void) {
	if (_read_header)
		return ;
	else if (unsigned long i = _request.find("\r\n\r\n")) {
		_read_header = true;
		_header_size = i + 4;
		this->getHeaderValues();
	}
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
