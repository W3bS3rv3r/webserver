#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <ostream>
#include "../http/http.hpp"

class Socket;

class Request {
public:
	Request(int fd, const Socket& Socket);
	Request(const Request& src);
	virtual	~Request(void);

	Request&	operator=(const Request& src);

	const std::string&	str(void) const;
	void				read(void);
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
	std::string		_chunk;
	std::string		_host;
	bool			_ready;
	bool			_chunked;
	bool			_valid;
	bool			_read_header;
	int				_fd;
	unsigned long	_header_size;
	unsigned long	_body_limit;
	unsigned long	_cont_len;
	unsigned long	_chunk_len;
	const Socket&	_socket;

	void	checkRequestStatus(void);
	void	updateHeaderStatus(void);
	void	getHeaderValues(void);
	void	checkBody(void);
	void	unchunkRequest(void);

	friend std::ostream&	operator<<(std::ostream& stream, const Request& req);
};

std::ostream&	operator<<(std::ostream& stream, const Request& req);

#endif
