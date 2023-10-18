#ifndef HTTPEXCEPTION_HPP
# define HTTPEXCEPTION_HPP

#include <exception>
#include <string>
#include "../Socket/Socket.hpp"

class HTTPException : public std::exception {
public:
	HTTPException(std::string h, std::string s_l);
	virtual ~HTTPException(void) throw();

	virtual const char*	what(void) const throw() = 0;
	std::string			getResponse(const Socket& socket) const;
	std::string			getErrorCode(void) const;
	void				setHost(std::string host);

private:
	std::string	_host;
	const std::string	_status_line;
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

struct ContentTooLargeException : public HTTPException {
	ContentTooLargeException(std::string h);
	const char*	what(void) const throw();
};

struct HTTPVersionNotSupportedException : public HTTPException {
	HTTPVersionNotSupportedException(std::string h);
	const char*	what(void) const throw();
};

struct NotImplementedException : public HTTPException {
	NotImplementedException(std::string h);
	const char*	what(void) const throw();
};

#endif
