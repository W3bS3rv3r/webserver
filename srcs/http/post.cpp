#include "post.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "request_utils.hpp"
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <string.h>

static std::vector	<char*> setCgiEnv(const std::string& request) {
	std::vector<char*> env;
	std::string requestURI;
	
	requestURI = getRequestURI(request);


	env.push_back(strdup(("CONTENT_TYPE=" + getHeaderValue(request, "Content-Type")).c_str()));
	env.push_back(strdup(("CONTENT_LENGTH=" + getHeaderValue(request, "Content-Length")).c_str()));
	env.push_back(strdup(("REQUEST_URI=" + requestURI).c_str()));
	env.push_back(strdup(("SCRIPT_NAME=" + requestURI.substr(1)).c_str()));

	env.push_back(strdup("AUTH_TYPE=Basic"));
	env.push_back(strdup("REQUEST_METHOD=POST"));
	env.push_back(strdup("REDIRECT_STATUS=200"));
	env.push_back(strdup("DOCUMENT_ROOT=./"));
	env.push_back(strdup("GATEWAY_INTERFACE=CGI/1.1"));
	env.push_back(strdup("PATH_INFO="));
	env.push_back(strdup("PATH_TRANSLATED=.//"));
	env.push_back(strdup("QUERY_STRING="));
	env.push_back(strdup("REMOTE_ADDR=localhost:4242"));
	env.push_back(strdup("SCRIPT_FILENAME=upload.py"));	
	env.push_back(strdup("SERVER_NAME=localhost"));
	env.push_back(strdup("SERVER_PORT=4242"));
	env.push_back(strdup("SERVER_PROTOCOL=HTTP/1.1"));
	env.push_back(strdup("SERVER_SOFTWARE=AMANIX"));
	env.push_back(NULL);

	return (env);
}

#include <iostream>
Response	cgiPost(std::string path, std::string request) {
	Response	resp;
	int			fd_req[2];
	int			fd_res[2];
	std::string	host = getHeaderValue(request, "Host");
	std::cout << "VAI\n";
	std::cout << request;
	std::cout << "END\n";
	if (pipe(fd_req) || pipe(fd_res))
		throw InternalServerErrorException(host);
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException(host);
	else if (pid == 0) {
	
		std::vector<const char*>	argv;
		argv.push_back("/usr/bin/python3");
		argv.push_back(path.c_str());
		argv.push_back(NULL);
		
		dup2(fd_req[0], STDIN_FILENO);
		dup2(fd_res[1], STDOUT_FILENO);
		close(fd_req[1]);
		close(fd_res[0]);

        std::vector<char*> env = setCgiEnv(request);
		execve(argv[0], const_cast<char* const*>(argv.data()), env.data());
		exit(1);
	}
	else {
		Cgi	cgi(pid, fd_res[0], time(NULL));
		close(fd_req[0]);
		close(fd_res[1]);
		cgi.setActive();
		write(fd_req[1], request.c_str(), request.size());
		close(fd_req[1]);
		resp.setCgi(cgi);
	}
	return (resp);
}
