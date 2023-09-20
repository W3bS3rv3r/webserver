#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include "../http/http.hpp"

class Request {
public:
	Request(void);
	Request(const Request& src);
	virtual	~Request(void);

	Request&	operator=(const Request& src);

	const std::string&	str(void) const;
	void				append(std::string);
	void				setFd(int fd);
	void				setMaxBodySize(unsigned long n);
	void				setHost(std::string host);
	void				getBody(void);
	bool				ready(void) const;
private:
	std::string		_request;
	std::string		_host;
	bool			_ready;
	int				_fd;
	unsigned long	_body_limit;
};

#endif
