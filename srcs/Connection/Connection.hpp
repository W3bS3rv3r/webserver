#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <string>
#include <queue>
#include "../Response/Response.hpp"

class Socket;

class Connection {
public:
	Connection(int	fd, const Socket& socket);
	virtual	~Connection(void);

	void			readRequest(void);
	void			writeResponse(void);
	bool			done(void) const;
	int				getFd(void);
	unsigned short	getPort(void);

private:
	int						_fd;
	const unsigned short	_port;
	const std::string		_root;
	const std::string		_suffix;

	bool					_done;
	std::queue<std::string>	_requests;
	std::queue<Response>	_responses;

	Connection&	operator=(const Connection& src);
	Connection(const Connection& src);
};

#endif
