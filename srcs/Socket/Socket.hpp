#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <netinet/in.h>
#include <exception>
#include <string>
#include <map>
#include <sys/socket.h>
#include "../Connection/Connection.hpp"
#include "../VirtualServer/VirtualServer.hpp"

class Socket {
public:
	Socket(unsigned short port, std::string root, std::string extension);
	Socket(unsigned short port, VirtualServer vserver);
	Socket(std::map<std::string, std::string> parameters);
	~Socket(void);

	void			listen(void);
	Connection*		acceptConnection(void);
	int				getFd(void) const;
	unsigned short	getPort(void) const;

	struct CantAcceptConnectionException: std::exception {
		const char*	what(void) const throw();
	};
	struct CantCreateSocketException : std::exception {
		const char*	what(void) const throw();
	};
	struct CantBindSocketException : std::exception {
		const char*	what(void) const throw();
	};
	struct InactiveSocketException: std::exception {
		const char*	what(void) const throw();
	};
	struct CantListenOnSocketException : std::exception {
		const char*	what(void) const throw();
	};
	struct CantSetSocketOptionException : std::exception {
		const char*	what(void) const throw();
	};
private:
	std::string								_root;
	std::string								_extension;
	bool									_is_listening;
	int										_fd;
	unsigned short							_port;
	struct sockaddr_in						_socket;
	std::string								_default_vserver;
	std::map<std::string, VirtualServer>	_vservers;

	Socket(const Socket& src);
	Socket&	operator=(const Socket& src);
	
	friend class Connection;
};

#endif
