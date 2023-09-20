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
	void				setChunked(void);
	void				getBody(void);
	bool				ready(void) const;
	bool				valid(void) const;
	bool				chunked(void) const;
private:
	std::string		_request;
	std::string		_host;
	bool			_ready;
	bool			_chunked;
	bool			_valid;
	int				_fd;
	unsigned long	_body_limit;
	unsigned long	_body_size;
};

#endif
