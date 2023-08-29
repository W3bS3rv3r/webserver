#include "HTTPException.hpp"

//HTTP CLASS CODE
HTTPException::HTTPException(std::string h, std::string s_l) :
	_host(h),
	_status_line(s_l)
{}

HTTPException::~HTTPException(void) throw() {}

std::string	HTTPException::getResponse(const Socket& socket) const {
	VirtualServer vserver		= socket.getVServer(_host);
	std::string	error_message	= vserver.getCustomError(this->getErrorCode());
	if (error_message.empty())
		error_message = this->what();
	return (_status_line + error_message);
}

std::string	HTTPException::getErrorCode(void) const {
	std::string::size_type	begining = _status_line.find(" ") + 1;
	std::string	code = _status_line.substr(begining, 3);
	return (code);
}

//HTTP ERRORS
RequestTimeoutException::RequestTimeoutException(std::string h) :
	HTTPException(h, "HTTP/1.1 408 Request Timeout\r\n"){}
const char*	RequestTimeoutException::what(void) const throw() {
	return ( \
		"Content-Length: 193\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">408 Request Timeout</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

MethodNotAllowedException::MethodNotAllowedException(std::string h) :
	HTTPException(h, "HTTP/1.1 405 Method Not Allowed\r\n"){}
const char*	MethodNotAllowedException::what(void) const throw() {
	return ( \
		"Content-Length: 196\r\n"											\
		"Allow: GET, DELETE, POST\r\n"										\
		"\r\n"																\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">405 Method Not Allowed</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

NotFoundException::NotFoundException(std::string h) :
	HTTPException(h, "HTTP/1.1 404 Not Found\r\n"){}
const char*	NotFoundException::what(void) const throw() {
	return ( \
		"Content-Length: 187\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">404 Not Found</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

ForbiddenException::ForbiddenException(std::string h) :
	HTTPException(h, "HTTP/1.1 403 Forbidden\r\n"){}
const char*	ForbiddenException::what(void) const throw() {
	return ( \
		"Content-Length: 187\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">403 Forbidden</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

BadRequestException::BadRequestException(std::string h) :
	HTTPException(h, "HTTP/1.1 400 Bad Request\r\n"){}
const char*	BadRequestException::what(void) const throw() {
	return ( \
		"Content-Length: 189\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">400 Bad Request</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

ServiceUnavailableException::ServiceUnavailableException(std::string h) :
	HTTPException(h, "HTTP/1.1 503 Service Unavailable\r\n"){}
const char*	ServiceUnavailableException::what(void) const throw() {
	return ( \
		"Content-Length: 197\r\n\r\n"												\
		"<html>"																	\
		"<body style=\"background-color:292D39\">"									\
		"<div>"																		\
		"<h1 align=\"center\", style=\"color:00BABC\">503 Service Unavailable</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"			\
		"</div></body></html>"														\
	);
}

InternalServerErrorException::InternalServerErrorException(std::string h) :
	HTTPException(h, "HTTP/1.1 500 Internal Server Error\r\n"){}
const char*	InternalServerErrorException::what(void) const throw() {
	return ( \
		"Content-Length: 199\r\n\r\n"													\
		"<html>"																		\
		"<body style=\"background-color:292D39\">"										\
		"<div>"																			\
		"<h1 align=\"center\", style=\"color:00BABC\">500 Internal Server Error</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"				\
		"</div></body></html>"															\
	);
}

BadGatewayException::BadGatewayException(std::string h) :
	HTTPException(h, "HTTP/1.1 502 Bad Gateway\r\n"){}
const char*	BadGatewayException::what(void) const throw() {
	return ( \
		"Content-Length: 189\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">502 Bad Gateway</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
	
}
