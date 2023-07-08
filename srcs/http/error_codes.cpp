#include "error_codes.hpp"

const char*	NotFoundException::what(void) const throw() {
	return ( \
		"HTTP/1.1 404 Not Found)\r\n\r\n"									\
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
		"HTTP/1.1 403 Forbidden)\r\n\r\n"									\
		"<html>"															\
		"<body style=\"background-color:292D39\">"							\
		"<div>"																\
		"<h1 align=\"center\", style=\"color:00BABC\">403 Forbidden</h1>"	\
		"<p align=\"center\", style=\"color:00BABC\">W3B53RB3RU5 1.0</p>"	\
		"</div></body></html>"												\
	);
}
