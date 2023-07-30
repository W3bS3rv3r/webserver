#include "CGI.hpp"
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>



CGI::CGI(void)
{

}

CGI::~CGI(void)
{

}

CGI::CGI(const std::string& path, const std::string& root, const std::string& request)
{
	this->_cgi_path = root + path;
	this->_full_req = request;
}

bool CGI::isPathValid()
{
	std::ifstream file(this->_cgi_path.c_str());
	return file.good();
}

void CGI::runCGI()
{
	int fd[2];
    pipe(fd);

    if (fork() == 0)
	{
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execl("/usr/bin/python3", "python3", this->_cgi_path.c_str(), NULL);
    }
	else
	{
        close(fd[0]);
        write(fd[1], this->_full_req.c_str(), this->_full_req.size());
        close(fd[1]);
        wait(NULL);
    }
}
