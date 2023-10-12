#ifndef CGI_HPP
# define CGI_HPP

#include <cstddef>
#include <sys/types.h>
#include <string>
#include <ctime>
#define TIMELIMIT 30

class Cgi {
public:
	Cgi(void);
	Cgi(pid_t pid, int fd_out, int fd_in, const std::string* request);
	Cgi(const Cgi& src);
	virtual	~Cgi(void);

	Cgi&	operator=(const Cgi& src);

	bool		isActive(void) const;
	bool		done(void);
	std::string	readResponse(void);

	void	setActive(void);
private:
	pid_t					_pid;
	int						_response_fd;
	int						_request_fd;
	int						_status;
	bool					_done;
	time_t					_start;
	size_t					_bytes_written;
	std::string				_host;
	std::string				_body;
	const std::string*		_request;

	void	writeToCgi(void);
};

#endif
