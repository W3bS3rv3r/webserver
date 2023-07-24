#ifndef SERVER_HPP
# define SERVER_HPP

#include <map>
#include "../Socket/Socket.hpp"

class Server {
public:
	Server(void);
	virtual	~Server(void);

	void	addSocket(unsigned short port, std::string root);

	struct DuplicateException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::map<int, Socket*>	_sockets;

	Server(const Server& src);
	Server&	operator=(const Server& src);
};

#endif
