#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <cstddef>
#include <string>
#include <sys/types.h>

class Response {
public:
	Response(std::string response = "");
	Response(const Response& src);
	virtual	~Response(void);

	Response&	operator=(const Response& src);

	bool	ready(void);

	const char*	getResponse(void) const;
	void		setResponse(const std::string& resp);

	size_t	getPid(void) const;
	void	setPid(const size_t pid);

	int		getFd(void) const;
	void	setFd(const int pid);

	std::string	getStatus(void) const;
	size_t		size(void) const;

private:
	std::string	_response;
	pid_t		_pid;
	int			_fd;
};

#endif
