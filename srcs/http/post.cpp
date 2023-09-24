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
#include <arpa/inet.h>
#include <fcntl.h>

static std::vector	<char*> setCgiEnv(const std::string& request, const Socket& socket) {
	std::vector<char*> env;
	std::string requestURI;
	std::string serverName;
	std::string serverPort;
	std::string clientIp;
	std::string clientPort;

	requestURI = getRequestURI(request);
	serverName = getServerName(request);
	serverPort = getServerPort(request);
	clientIp = inet_ntoa(socket.getClientInfo().sin_addr);
	clientPort = ntohs(socket.getClientInfo().sin_port);

	env.push_back(strdup(("CONTENT_TYPE=" + getHeaderValue(request, "Content-Type")).c_str()));
	env.push_back(strdup(("CONTENT_LENGTH=" + getHeaderValue(request, "Content-Length")).c_str()));
	env.push_back(strdup(("REQUEST_URI=" + requestURI).c_str()));
	env.push_back(strdup(("SCRIPT_NAME=" + requestURI.substr(1)).c_str()));
	env.push_back(strdup(("SCRIPT_FILENAME=" + getScriptFilename(requestURI)).c_str()));

	env.push_back(strdup(("REMOTE_ADDR=" + clientIp + ":" + clientPort).c_str()));
	env.push_back(strdup(("SERVER_NAME=" + serverName).c_str()));
	env.push_back(strdup(("SERVER_PORT=" + serverPort).c_str()));

	env.push_back(strdup("AUTH_TYPE=Basic"));
	env.push_back(strdup("REQUEST_METHOD=POST"));
	env.push_back(strdup("REDIRECT_STATUS=200"));
	env.push_back(strdup("DOCUMENT_ROOT=./"));
	env.push_back(strdup("GATEWAY_INTERFACE=CGI/1.1"));
	env.push_back(strdup("PATH_INFO="));
	env.push_back(strdup("PATH_TRANSLATED=.//"));
	env.push_back(strdup("QUERY_STRING="));
	env.push_back(strdup("SERVER_PROTOCOL=HTTP/1.1"));
	env.push_back(strdup("SERVER_SOFTWARE=AMANIX"));
	env.push_back(NULL);

	return (env);
}

void	makePipesNonBlocking(int* pip1, int* pip2) {
	fcntl(pip1[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip1[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip2[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pip2[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

#include <iostream>

Response	cgiPost(std::string path, std::string request,
					const Socket& socket, std::string upload)
{
	Response				resp;
	int						fd_req[2];
	int						fd_res[2];
	std::string				req_body;
	std::string::size_type	headers_end;

	if (pipe(fd_req) || pipe(fd_res))
		throw InternalServerErrorException("");
	makePipesNonBlocking(fd_req, fd_req);
	if (!pollFdOut(fd_req[1]))
		throw InternalServerErrorException("");
	headers_end = request.find("\r\n\r\n");
	if (headers_end != std::string::npos) {
        req_body = request.substr(headers_end + 4);
		std::cout << req_body << std::endl;
        write(fd_req[1], req_body.c_str(), req_body.size());
    } else {
        write(fd_req[1], "", 0);
	}
	pid_t	pid = fork();
	if (pid == -1)
		throw InternalServerErrorException("");
	else if (pid == 0) {
		chdir(upload.c_str());
		std::vector<const char*>	argv;
		argv.push_back("/usr/bin/python3");
		argv.push_back(path.c_str());
		argv.push_back(NULL);
		dup2(fd_req[0], STDIN_FILENO);
		dup2(fd_res[1], STDOUT_FILENO);
		close(fd_req[1]);
		close(fd_res[0]);
		std::vector<char*> env = setCgiEnv(request, socket);
		execve(argv[0], const_cast<char* const*>(argv.data()), env.data());
		exit(1);
	}
	else {
		Cgi	cgi(pid, fd_res[0], time(NULL));
		close(fd_req[0]);
		close(fd_res[1]);
		cgi.setActive();
		close(fd_req[1]);
		resp.setCgi(cgi);
	}
	return (resp);
}
