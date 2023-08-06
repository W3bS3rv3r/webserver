#include "error_codes.hpp"

const char*	MethodNotAllowedException::what(void) const throw() {
	return ( \
		"HTTP/1.1 405 Method Not Allowed\r\n"								\
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

const char*	NotFoundException::what(void) const throw() {
	return ( \
		"HTTP/1.1 404 Not Found\r\n"										\
		"Content-Length: 187\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">404 Not Found</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

const char*	ForbiddenException::what(void) const throw() {
	return ( \
		"HTTP/1.1 403 Forbidden\r\n"										\
		"Content-Length: 187\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">403 Forbidden</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

const char*	BadRequestException::what(void) const throw() {
	return ( \
		"HTTP/1.1 400 Bad Request\r\n"										\
		"Content-Length: 189\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">400 Bad Request</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}

const char*	ServiceUnavailableException::what(void) const throw() {
	return ( \
		"HTTP/1.1 503 Service Unavailable\r\n"										\
		"Content-Length: 197\r\n\r\n"												\
		"<html>"																	\
		"<body style=\"background-color:292D39\">"									\
		"<div>"																		\
		"<h1 align=\"center\", style=\"color:00BABC\">503 Service Unavailable</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"			\
		"</div></body></html>"														\
	);
}

const char*	InternalServerErrorException::what(void) const throw() {
	return ( \
		"HTTP/1.1 500 Internal Server Error\r\n"										\
		"Content-Length: 199\r\n\r\n"													\
		"<html>"																		\
		"<body style=\"background-color:292D39\">"										\
		"<div>"																			\
		"<h1 align=\"center\", style=\"color:00BABC\">500 Internal Server Error</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"				\
		"</div></body></html>"															\
	);
}

const char*	BadGatewayException::what(void) const throw() {
	return ( \
		"HTTP/1.1 502 Bad Gateway\r\n"										\
		"Content-Length: 189\r\n\r\n"										\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">502 Bad Gateway</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
	
}
