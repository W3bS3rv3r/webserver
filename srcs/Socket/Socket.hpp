#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <netinet/in.h>
#include <exception>
#include <string>
#include <sys/socket.h>

class Socket {
public:
	Socket(void);
	Socket(const unsigned short port);
	Socket(std::string root);
	Socket(const unsigned short port, std::string root);
	~Socket(void);

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
	std::string			_root;
	bool				_is_listening;
	int					_fd;
	struct sockaddr_in	_socket;

	Socket(const Socket& src);
	Socket&	operator=(const Socket& src);
};

#endif
