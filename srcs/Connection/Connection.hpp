#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <queue>
#include "../Response/Response.hpp"
#include "../Request/Request.hpp"

class Socket;

class Connection {
public:
	Connection(int	fd, const Socket* socket);
	virtual	~Connection(void);

	void			readRequest(void);
	void			writeResponse(void);
	bool			done(void) const;
	int				getFd(void);
	unsigned short	getPort(void);

private:
	int						_fd;
	unsigned int			_written;
	const Socket*			_socket;
	bool					_done;
	std::queue<Request>		_requests;
	std::queue<Response>	_responses;

	void	sendResponse(void);
};

#endif
