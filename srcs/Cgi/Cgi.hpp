#ifndef CGI_HPP
# define CGI_HPP

#include <sys/types.h>
#include <string>

class Cgi {
public:
	Cgi(pid_t pid = 0, int _fd = 0);
	Cgi(const Cgi& src);
	virtual	~Cgi(void);

	Cgi&	operator=(const Cgi& src);

	bool		active(void) const;
	bool		done(void);
	std::string	readResponse(void);

	void	setActive(void);
private:
	pid_t	_pid;
	int		_fd;
	int		_status;
	bool	_done;
};

#endif
