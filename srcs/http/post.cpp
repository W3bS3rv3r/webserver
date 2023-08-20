#include "post.hpp"
#include "error_codes.hpp"
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

	env.push_back(strdup("AUTH_TYPE=Basic"));
	env.push_back(strdup("REQUEST_METHOD=POST"));
	env.push_back(strdup(("CONTENT_TYPE=" + getHeaderValue(request, "Content-Type")).c_str()));
	env.push_back(strdup(("CONTENT_LENGTH=" + getHeaderValue(request, "Content-Length")).c_str()));
	env.push_back(strdup("REDIRECT_STATUS=200"));
	env.push_back(strdup("DOCUMENT_ROOT=./"));
	env.push_back(strdup("GATEWAY_INTERFACE=CGI/1.1"));
	env.push_back(strdup("PATH_INFO="));
	env.push_back(strdup("PATH_TRANSLATED=.//"));
	env.push_back(strdup("QUERY_STRING="));
	env.push_back(strdup("REMOTE_ADDR=localhost:4242"));
	env.push_back(strdup("REQUEST_URI=/cgi-bin/upload.py"));
	env.push_back(strdup("SCRIPT_FILENAME=upload.py"));
	env.push_back(strdup("SCRIPT_NAME=cgi-bin/upload.py"));
	env.push_back(strdup("SERVER_NAME=localhost"));
	env.push_back(strdup("SERVER_PORT=4242"));
	env.push_back(strdup("SERVER_PROTOCOL=HTTP/1.1"));
	env.push_back(strdup("SERVER_SOFTWARE=AMANIX"));
	env.push_back(NULL);

	return (env);
}


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
		// dup2(fd[1], STDOUT_FILENO);  // this makes CGI hang
		close(fd[0]);
		close(fd[1]);

        std::vector<char*> env = setCgiEnv(request);
		execve(argv[0], const_cast<char* const*>(argv.data()), env.data());

		for (std::vector<char*>::iterator it = env.begin(); it != env.end(); ++it)
			free(*it);
		env.clear();

		exit(1);
	}
	else {
		Cgi	cgi(pid, fd[0], time(NULL));
		cgi.setActive();
		write(fd[1], request.c_str(), request.size());
		close(fd[0]);
		close(fd[1]);
		resp.setCgi(cgi);
		waitpid(pid, NULL, 0);
	}
	return (resp);
}

