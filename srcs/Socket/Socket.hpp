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
	void			addVserver(const VirtualServer& server);
	Connection*		acceptConnection(void);
	int				getFd(void) const;
	unsigned short	getPort(void) const;
	VirtualServer	getVServer(std::string host_header) const;

	struct CantAcceptConnectionException: std::exception {
		const char*	what(void) const throw();
	};
	struct CantCreateSocketException : std::exception {
		const char*	what(void) const throw();
	};
	struct CantBindSocketException : std::exception {
		const char*	what(void) const throw();
	};
	struct CantListenOnSocketException : std::exception {
		const char*	what(void) const throw();
	};
	struct CantSetSocketOptionException : std::exception {
		const char*	what(void) const throw();
	};
	struct DuplicateException : std::exception {
		const char*	what(void) const throw();
	};
private:
	int										_fd;
	struct sockaddr_in						_client_info;
	socklen_t								_client_info_len;
	unsigned short							_port;
	struct sockaddr_in						_socket;
	std::string								_default_vserver;
	std::map<std::string, VirtualServer>	_vservers;

	Socket(const Socket& src);
	Socket&	operator=(const Socket& src);
	
	friend class Connection;
};

#endif
