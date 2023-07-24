#include "Server.hpp"
#include <exception>
#include <iostream>
#include <utility>

//CONSTRUCTORS
Server::Server(void) {}

Server::~Server(void) {
	std::map<int, Socket*>::iterator	i = _sockets.begin();
	while (i != _sockets.end()) {
		delete i->second;
		++i;
	}
}

//METHODS
void	Server::addSocket(unsigned short port, std::string root) {
	Socket*	socket = NULL;
	try {
		std::pair<std::map<int, Socket*>::iterator, bool>	p;
		socket = new Socket(port, root);
		p = _sockets.insert(std::make_pair(port, socket));
		if (!p.second)
			throw Server::DuplicateException();
	}
	catch (const std::exception& e) {
		delete socket;
		std::cerr << "failed to create socket on port: " << port << std::endl;
		std::cerr << "Error: " << e.what() << std::endl; 
	}
}

//EXCEPTIONS
const char*	Server::DuplicateException::what(void) const throw() {
	return ("Duplicate port");
}
