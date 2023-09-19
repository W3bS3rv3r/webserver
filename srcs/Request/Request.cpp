#include "Request.hpp"

// Constructors
Request::Request(void) {}

Request::Request(const Request& src) { *this = src; }

Request::~Request(void) {}

// Operators
Request&	Request::operator=(const Request& src) {
	if (this != &src) {
		_request = src._request;
	}
	return (*this);
}

// Methods
void	Request::append(std::string str) { _request += str; }

const std::string&	Request::str(void) const { return (_request); }
