#include "post.hpp"
#include "error_codes.hpp"
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <sstream>

Response	cgiPost(std::string path, std::string request) {
	Response	resp;
	int			fd[2];

	if (pipe(fd))
		throw InternalServerErrorException();
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException();
	else if (pid == 0) {
		std::vector<const char*>	argv;
        argv.push_back("/usr/bin/python3");
		argv.push_back(path.c_str());
		argv.push_back(NULL);
        dup2(fd[0], STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve(argv[0], const_cast<char* const*>(argv.data()), NULL);
		exit(0);
	}
	else {
		Cgi	cgi(pid, fd[0], time(NULL));
		cgi.setActive();
		write(fd[1], request.c_str(), request.size());
		close(fd[1]);
		resp.setCgi(cgi);
	}
	return (resp);
}
