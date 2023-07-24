#ifndef SERVER_HPP
# define SERVER_HPP

#include <map>
#include "../Socket/Socket.hpp"

class Server {
public:
	Server(void);
	virtual	~Server(void);

private:
	std::map<int, Socket>	_sockets;

	Server(const Server& src);
	Server&	operator=(const Server& src);
};

#endif
