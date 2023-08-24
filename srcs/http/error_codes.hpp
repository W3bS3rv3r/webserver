#ifndef ERROR_CODES_HPP
# define ERROR_CODES_HPP

#include <exception>
#include <string>

struct HTTPException : public std::exception {
	HTTPException(std::string h, std::string s_l);
	virtual ~HTTPException(void) throw();
	const std::string	host;
	const std::string	status_line;
	virtual const char*	what(void) const throw() = 0;
};

struct RequestTimeoutException : public HTTPException {
	RequestTimeoutException(std::string h);
	const char*	what(void) const throw();
};

struct MethodNotAllowedException : public HTTPException {
	MethodNotAllowedException(std::string h);
	const char*	what(void) const throw();
};

struct NotFoundException : public HTTPException {
	NotFoundException(std::string h);
	const char*	what(void) const throw();
};

struct ForbiddenException : public HTTPException {
	ForbiddenException(std::string h);
	const char*	what(void) const throw();
};

struct BadRequestException : public HTTPException {
	BadRequestException(std::string h);
	const char*	what(void) const throw();
};

struct ServiceUnavailableException : public HTTPException {
	ServiceUnavailableException(std::string h);
	const char*	what(void) const throw();
};

struct InternalServerErrorException : public HTTPException {
	InternalServerErrorException(std::string h);
	const char*	what(void) const throw();
};

struct BadGatewayException : public HTTPException {
	BadGatewayException(std::string h);
	const char*	what(void) const throw();
};

#endif
