#include "get.hpp"
#include "error_codes.hpp"
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>

std::string	get(std::string path) {
	DIR*			dir;

	if ( (dir = opendir(path.c_str())) ) {
		closedir(dir);
		path += "/index.html";
	}
	if (access(path.c_str(), F_OK))
		throw NotFoundException();
	else if (access(path.c_str(), R_OK))
		throw ForbiddenException();
	std::fstream	file(path.c_str(), std::ios_base::in);
	std::string		response = ("HTTP/1.1 200 OK\r\n\r\n");
	std::string		buff;
	while(std::getline(file, buff)) {
			try {
				response += buff;
			}
			catch (const std::exception& e) {
				file.close();
				throw InternalServerErrorException();
			}
	}
	file.close();
	return (response);
}

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
		exit(0);
	}
	else {
		close(fd[1]);
		resp.setCgi(Cgi(pid, fd[0]));
	}
	return (resp);
}
