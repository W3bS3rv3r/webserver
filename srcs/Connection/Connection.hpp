#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <string>
#include <queue>

class Connection {
public:
	Connection(int	fd, unsigned short port, std::string root);
	virtual	~Connection(void);

	void			readRequest(void);
	void			writeResponse(void);
	bool			done(void) const;
	int				getFd(void);
	unsigned short	getPort(void);

private:
	int						_fd;
	unsigned short			_port;
	const std::string		_root;
	bool					_done;
	std::queue<std::string>	_requests;
	std::queue<std::string>	_responses;

	Connection&	operator=(const Connection& src);
	Connection(const Connection& src);
};

#endif
