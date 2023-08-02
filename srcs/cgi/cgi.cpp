#include "cgi.hpp"
#include "../http/error_codes.hpp"
#include <cstdlib>
#include <exception>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#include <iostream>

Response	cgiGet(std::string path) {
	Response	resp;
	int			fd[2];

	if (pipe(fd))
		throw InternalServerErrorException();
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException();
	else if (pid == 0) {
		std::vector<const char*>	argv;
		argv.push_back(path.c_str());
		argv.push_back(NULL);
		dup2(fd[0],0);
		dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
		execve(path.c_str(), const_cast<char* const*>(argv.data()), NULL);
		std::string	response = InternalServerErrorException().what();
		write(1, response.c_str(), response.size());
		exit(0);
	}
	else {
		close(fd[1]);
		resp.setFd(fd[0]);
		resp.setPid(pid);
	}
	return (resp);
}
