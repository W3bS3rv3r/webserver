#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>

class Response {
public:
	Response(std::string response);
	Response(const Response& src);
	virtual	~Response(void);

	Response&	operator=(const Response& src);

	const char*	getResponse(void) const;
	std::string	getStatus(void) const;
	void		setResponse(const std::string& resp);
	size_t		size(void) const;
private:
	std::string	_response;
};

#endif
