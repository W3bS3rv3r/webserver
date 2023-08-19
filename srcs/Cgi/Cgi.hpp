#ifndef CGI_HPP
# define CGI_HPP

#include <sys/types.h>
#include <string>
#include <ctime>
#define TIMELIMIT 30

class Cgi {
public:
	Cgi(pid_t pid = 0, int _fd = 0, time_t start = time(NULL));
	Cgi(const Cgi& src);
	virtual	~Cgi(void);

	Cgi&	operator=(const Cgi& src);

	bool		isActive(void) const;
	bool		done(void);
	std::string	readResponse(void);

	void	setActive(void);
private:
	pid_t	_pid;
	int		_fd;
	int		_status;
	bool	_done;
	time_t	_start;
};

#endif
