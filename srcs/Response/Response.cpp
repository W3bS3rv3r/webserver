#include "Response.hpp"
#include <cstddef>

Response::Response(std::string response) : _response(response) {}

Response::Response(const Response& src) { *this = src; }

Response::~Response(void) {}

Response&	Response::operator=(const Response& src) {
	if (this != &src) {
		_response = src._response;
	}
	return (*this);
}

std::string	Response::getStatus(void) const {
	return (_response.substr(0, _response.find('\n')));
}

const char*	Response::getResponse(void) const { return _response.c_str(); }

void	Response::setResponse(const std::string& resp) { _response = resp; }

size_t	Response::size(void) const { return _response.size(); }
