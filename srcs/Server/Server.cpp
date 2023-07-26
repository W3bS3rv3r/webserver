#include "Server.hpp"
#include <exception>
#include <iostream>
#include <sys/poll.h>
#include <utility>
#include <unistd.h>
#include <cstring>

//CONSTRUCTORS
Server::Server(void) : _changed(false) {}

Server::~Server(void) {
	for(std::map<int, Socket*>::iterator i = _sockets.begin();
			i != _sockets.end(); ++i)
	{
		delete i->second;
	}
	for(std::map<int, Connection*>::iterator i = _connections.begin();
			i != _connections.end(); ++i)
	{
		delete i->second;
	}
}

//HELPER FUNTIONS
namespace {
	inline bool	pendingSocket(int events, int revents) {
		return (revents & POLLIN && events == POLLIN);
	}
	inline bool pendingRead(int events, int revents) {
		return (revents & POLLIN && events == (POLLIN | POLLOUT));
	}
	inline bool pendingWrite(int events, int revents) {
		return (revents & POLLIN && events == (POLLIN | POLLOUT));
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
	std::cout << "Config file parsed" << std::endl;
}

void	Server::run(void) {
	this->startListening();
	std::cout << "Ports listening" << std::endl;
	while (1) {
		poll(_polls.data(), _polls.size(), 1000);
		this->handlePoll();
		if (_changed) {
			this->organizePoll();
			_changed = false;
		}
	}
}

void	Server::startListening(void) {
	struct pollfd	temp;

	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN;
	for(std::map<const int, Socket*>::iterator i = _sockets.begin();
			i != _sockets.end(); ++i)
	{
		temp.fd = i->second->getFd();
		_polls.push_back(temp);
		i->second->listen();
	}
}

void	Server::handlePoll(void) {
	for (std::vector<struct pollfd>::iterator i = _polls.begin();
			 i != _polls.end(); ++i)
	{
		if (i->events == POLLIN)
			this->handleSocket(*i);
		else
			this->handleConnection(*i);
	}
}

void	Server::handleSocket(struct pollfd spoll) {
	if (spoll.revents & POLLIN)
		this->addConnection(spoll.fd);
}

void	Server::handleConnection(struct pollfd spoll) {
	//std::cout << "handle" << std::endl;
	if (spoll.revents & POLLIN)
		_connections.at(spoll.fd)->readRequest();
	if (spoll.revents & POLLOUT) {
		_connections.at(spoll.fd)->writeResponse();
		if (_connections.at(spoll.fd)->done())
			this->closeConnection(spoll.fd);
	}
}

void	Server::addConnection(int socket_fd) {
	Connection*	cn = _sockets.at(socket_fd)->acceptConnection();
	_connections.insert(std::make_pair(cn->getFd(), cn));
	_changed = true;
}

void	Server::closeConnection(int fd) {
	delete _connections.at(fd);
	_connections.erase(fd);
	_changed = true;
}

// this is dumb, needs to be refactored later
void	Server::organizePoll(void) {
	struct pollfd	temp;
	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN;
	_polls.clear();
	for (std::map<int, Socket*>::iterator i = _sockets.begin();
		i != _sockets.end(); ++i)
	{
		temp.fd = i->first;
		_polls.push_back(temp);
	}
	temp.events = POLLIN | POLLOUT;
	for (std::map<int, Connection*>::iterator i = _connections.begin();
		i != _connections.end(); ++i)
	{
		temp.fd = i->first;
		_polls.push_back(temp);
	}
}

//EXCEPTIONS
const char*	Server::DuplicateException::what(void) const throw() {
	return ("FD already in use");
}
const char*	Server::NoUserException::what(void) const throw() {
	return ("USER environment variable not set");
}
