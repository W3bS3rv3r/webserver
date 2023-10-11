#include "get.hpp"
#include "../HTTPException/HTTPException.hpp"
#include <cstring>
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

namespace {
void	makePipesNonBlocking(int* pip1, int* pip2) {
	fcntl(pip1[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip1[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip2[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip2[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}
}

Response	cgiGet(std::string path, const std::string& request, const std::string& arguments) {
	Response	resp;
	int			fd_req[2];
	int			fd_res[2];
	std::string	host = getHeaderValue(request, "Host");

	if (pipe(fd_req) || pipe(fd_res))
		throw InternalServerErrorException(host);
	makePipesNonBlocking(fd_req, fd_req);
	if (!pollFdOut(fd_req[1]))
		throw InternalServerErrorException("");
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException(host);
	else if (pid == 0) {
		std::vector<const char*>	argv;
		std::vector<char*>			env;
		env.push_back(strdup(("QUERY_STRING=" + arguments).c_str()));
		env.push_back(NULL);
		argv.push_back("/usr/bin/python3");
		argv.push_back(path.c_str());
		argv.push_back(arguments.c_str());
		argv.push_back(NULL);
		dup2(fd_req[0], STDIN_FILENO);
		dup2(fd_res[1], STDOUT_FILENO);
		close(fd_req[0]);
		close(fd_res[1]);
		execve(path.c_str(), const_cast<char* const*>(argv.data()), env.data());
		exit(1);
	}
	else {
		if (!pollFdOut(fd_req[1]))
			throw InternalServerErrorException("");
		if ( (write(fd_req[1], arguments.c_str(), arguments.size()) < 0 ) )
			throw InternalServerErrorException("");
		close(fd_req[0]);
		close(fd_res[1]);
		close(fd_req[1]);
		Cgi	cgi(pid, fd_res[0], 0, NULL);
		cgi.setActive();
		resp.setCgi(cgi);
	}
	return (resp);
}
