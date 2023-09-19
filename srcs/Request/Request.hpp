#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>

class Request {
public:
	Request(void);
	Request(const Request& src);
	virtual	~Request(void);

	Request&	operator=(const Request& src);

	void				append(std::string);
	const std::string&	str(void) const;
private:
	std::string	_request;
};

#endif
