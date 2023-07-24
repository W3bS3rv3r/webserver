#include "Server.hpp"
#include <exception>
#include <iostream>
#include <utility>
#include <unistd.h>
#include <poll.h>
#include <cstring>

//CONSTRUCTORS
Server::Server(void) {}

Server::~Server(void) {
	for(std::map<int, Socket*>::iterator i = _sockets.begin();
			i != _sockets.end(); ++i)
	{
		delete i->second;
	}
}

//METHODS
void	Server::addSocket(unsigned short port, std::string root) {
	Socket*	socket = NULL;
	try {
		std::pair<std::map<int, Socket*>::iterator, bool>	p;
		socket = new Socket(port, root);
		p = _sockets.insert(std::make_pair(socket->getFd(), socket));
		if (!p.second)
			throw Server::DuplicateException();
	}
	catch (const std::exception& e) {
		delete socket;
		std::cerr << "failed to create socket on port: " << port << std::endl;
		std::cerr << "Error: " << e.what() << std::endl; 
	}
}

void	Server::init(std::string file) {
    const char* user_name = getlogin();

	if (!user_name)
		throw Server::NoUserException();
	std::string path = "/home/";
    path += user_name;
    path += "/webserver";

	std::cout << "Default path: " << path << std::endl;
	/*FILE INTERPRETATION HERE ON THE FUTURE*/
	(void)file;
	this->addSocket(4242, path);
	this->addSocket(8484, path);
	this->addSocket(2121, path);
}

void	Server::run(void) {
	struct pollfd	server_poll[_sockets.size()];
	memset(&server_poll, 0, sizeof(server_poll));
	int										j;
	std::map<const int, Socket*>::iterator	i;

	for(i = _sockets.begin(), j = 0; i != _sockets.end(); ++i, ++j)
	{
		server_poll[j].fd = i->second->getFd();
		server_poll[j].events = POLLIN;
		i->second->listen();
	}
	while (1) {
		poll(server_poll, _sockets.size(), 100);
		for (unsigned long int i = 0; i < _sockets.size(); ++i) {
			if (server_poll[i].revents & POLLIN) {
				_sockets.at(server_poll[i].fd)->handleRequest();
			}
		}
	}
	return ;
}

//EXCEPTIONS
const char*	Server::DuplicateException::what(void) const throw() {
	return ("FD already in use");
}
const char*	Server::NoUserException::what(void) const throw() {
	return ("USER environment variable not set");
}
