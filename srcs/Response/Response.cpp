#include "Response.hpp"
#include "../http/request_utils.hpp"
#include "../HTTPException/HTTPException.hpp"

Response::Response(std::string response) : _response(response) {}

Response::Response(const Response& src) { *this = src; }

Response::~Response(void) {}

Response&	Response::operator=(const Response& src) {
	if (this != &src) {
		_response = src._response;
		_cgi = src._cgi;
	}
	return (*this);
}

std::string	Response::getStatus(void) const {
	return (_response.substr(0, _response.find('\n')));
}

bool	Response::isReady(void) {
	if (!_cgi.isActive())
		return (true);
	else if (_cgi.done()) {
		_response = _cgi.readResponse();
		return (true);
	}
	return (false);
}

size_t	Response::size(void) const { return _response.size(); }

const char*	Response::getResponse(void) const { return _response.c_str(); }

void	Response::setResponse(const std::string& resp) { _response = resp; }

void	Response::setCgi(const Cgi &cgi) {	_cgi = cgi; }
