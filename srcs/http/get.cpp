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
#include <algorithm>
#include <fcntl.h>

std::string	get(std::string path) {
	if (access(path.c_str(), F_OK))
		throw NotFoundException("");
	else if (access(path.c_str(), R_OK))
		throw ForbiddenException("");
	std::string	content = getFileContent(path, "");
	std::stringstream	response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Length: " << content.size() << "\r\n\r\n";
	response << content;
	return (response.str());
}

std::string	getDir(std::string path) {
	DIR*						dir = opendir(path.c_str());
	std::string					content;
	std::stringstream			response;
	struct dirent*				buff;
	std::vector<std::string>	v;

	if (!dir)
		throw InternalServerErrorException("");
	while((buff = readdir(dir)))
		v.push_back(buff->d_name);
	closedir(dir);
	std::sort(v.begin(), v.end());
	content += "<div><ul>";
	for(std::vector<std::string>::size_type i = 0; i < v.size(); ++i)
		content += "<li>" + v[i] + "</li>";
	content += "</ul></div>";
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Length: " << content.size() << "\r\n\r\n";
	response << content;
	return (response.str());
}

void	makePipeNonBlocking(int* pip) {
	fcntl(pip[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

Response	cgiGet(std::string path, const std::string& request, const std::string& arguments) {
	Response	resp;
	int			fd[2];
	std::string	host = getHeaderValue(request, "Host");

	if (pipe(fd))
		throw InternalServerErrorException(host);
	makePipeNonBlocking(fd);
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException(host);
	else if (pid == 0) {
		std::vector<const char*>	argv;
		argv.push_back(path.c_str());
		argv.push_back(arguments.c_str());
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
