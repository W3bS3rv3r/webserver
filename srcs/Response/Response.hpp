#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <cstddef>
#include <string>
#include <sys/types.h>
#include "../Cgi/Cgi.hpp"

class Response {
public:
	Response(std::string response = "");
	Response(const Response& src);
	virtual	~Response(void);

	Response&	operator=(const Response& src);

	bool	ready(void);

	const char*	getResponse(void) const;
	void		setResponse(const std::string& resp);

	void	setCgi(const Cgi& cgi);

	std::string	getStatus(void) const;
	size_t		size(void) const;

private:
	std::string	_response;
	Cgi			_cgi;
};

#endif
