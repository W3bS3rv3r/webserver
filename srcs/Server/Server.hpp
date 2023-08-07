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
	struct NoUserException: std::exception {
		const char*	what(void) const throw();
	};
	struct AllPortsFailedException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::map<int, Socket*>		_sockets;
	std::map<int, Connection*>	_connections;
	std::vector<struct pollfd>	_polls;
	std::vector<int>			_removed_fds;
	std::vector<int>			_new_fds;
	bool						_changed;

	void	startListening(void);
	void	closeSocket(int socket_fd);
	void	addConnection(int socket_fd);
	void	closeConnection(int fd);
	void	handleSocket(struct pollfd spoll);
	void	handleConnection(struct pollfd spoll);
	void	removeOldFds(void);
	void	addNewFds(void);
	void	setPoll(void);
	void	handlePoll(void);
	bool	isSocket(int fd) const;
	bool	isConnection(int fd) const;

	Server(const Server& src);
	Server&	operator=(const Server& src);
};

#endif
