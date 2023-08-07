#ifndef SERVER_HPP
# define SERVER_HPP

#include <map>
#include <vector>
#include <poll.h>
#include "../Socket/Socket.hpp"

class Server {
public:
	Server(void);
	virtual	~Server(void);

	void	init(std::string file);
	void	addSocket(unsigned short port, std::string root);
	void	run(void);

	struct DuplicateException: std::exception {
		const char*	what(void) const throw();
	};
	struct NoHomeException: std::exception {
		const char*	what(void) const throw();
	};
	struct AllPortsFailedException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::map<int, Socket*>		_sockets;
	std::map<int, Connection*>	_connections;
	std::vector<struct pollfd>	_polls;
	bool						_changed;

	void	startListening(void);
	void	closeSocket(int socket_fd);
	void	addConnection(int socket_fd);
	void	closeConnection(int fd);
	void	handleSocket(struct pollfd spoll);
	void	handleConnection(struct pollfd spoll);
	void	organizePoll(void);
	void	handlePoll(void);

	Server(const Server& src);
	Server&	operator=(const Server& src);
};

#endif
