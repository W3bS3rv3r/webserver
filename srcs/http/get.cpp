#include "get.hpp"
#include "../HTTPException/HTTPException.hpp"
#include <fstream>
#include "request_utils.hpp"
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <sstream>

std::string	get(std::string path, std::string request) {
	DIR*			dir;
	std::string		host = getHeaderValue(request, "Host");

	if ( (dir = opendir(path.c_str())) ) {
		closedir(dir);
		path += "/index.html";
	}
	if (access(path.c_str(), F_OK))
		throw NotFoundException(host);
	else if (access(path.c_str(), R_OK))
		throw ForbiddenException(host);
	std::string	content = getFileContent(path, host);
	std::stringstream	response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Length: " << content.size() << "\r\n\r\n";
	response << content;
	return (response.str());
}

Response	cgiGet(std::string path, std::string request) {
	Response	resp;
	int			fd[2];
	std::string	host = getHeaderValue(request, "Host");

	if (pipe(fd))
		throw InternalServerErrorException(host);
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException(host);
	else if (pid == 0) {
		std::vector<const char*>	argv;
		argv.push_back(path.c_str());
		argv.push_back(NULL);
		dup2(fd[0],0);
		dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
		execve(path.c_str(), const_cast<char* const*>(argv.data()), NULL);
		exit(1);
	}
	else {
		Cgi	cgi(pid, fd[0], time(NULL));
		cgi.setActive();
		close(fd[1]);
		resp.setCgi(cgi);
	}
	return (resp);
}
