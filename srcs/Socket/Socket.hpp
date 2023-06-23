#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <netinet/in.h>
#include <exception>
#include <string>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

class Socket {
public:
	Socket(void);
	Socket(const unsigned short port);
	Socket(const Socket& src);
	~Socket(void);

	Socket&	operator=(const Socket& src);

	void	listen(void);
	void	handleRequest(void);

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
private:
	bool				_is_listening;
	int					_listen_fd;
	const socklen_t		_socket_size;
	struct sockaddr_in	_socket_addr;
};

#endif
