#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <netinet/in.h>
#include <exception>
#include <string>

#define BUFFER_SIZE 1024

class Socket {
public:
	Socket(void);
	Socket(unsigned short port);
	Socket(const Socket& src);
	~Socket(void);

	Socket&	operator=(const Socket& src);

	struct CantCreateSocketException : std::exception {
		const char*	what(void) const throw();
	};

private:
	bool				_is_listening;
	int					_listen_fd;
	struct sockaddr_in	_server_addr;
};

#endif
