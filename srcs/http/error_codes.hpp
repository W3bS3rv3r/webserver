#ifndef ERROR_CODES_HPP
# define ERROR_CODES_HPP

#include <exception>

struct NotFoundException: std::exception {
	const char*	what(void) const throw();
};

struct ForbiddenException: std::exception {
	const char*	what(void) const throw();
};

struct BadRequestException: std::exception {
	const char*	what(void) const throw();
};

struct ServiceUnavailableException: std::exception {
	const char*	what(void) const throw();
};

#endif
