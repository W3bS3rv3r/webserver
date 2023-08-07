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

//METHODS
void	Server::addSocket(unsigned short port, std::string root) {
	Socket*	socket = NULL;
	try {
		std::pair<std::map<int, Socket*>::iterator, bool>	p;
		socket = new Socket(port, root, ".py");
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
	/*FILE INTERPRETATION HERE ON THE FUTURE*/
    const char* user_name = getlogin();
	if (!user_name)
		throw Server::NoUserException();
	std::string path = "/home/";
    path += user_name;
    path += "/webserver";
	std::cout << "Default path: " << path << std::endl;
	(void)file;

	this->addSocket(4242, path);
	this->addSocket(8484, path);
	this->addSocket(2121, path);
	std::cout << "Config file parsed" << std::endl;
}

void	Server::run(void) {
	this->startListening();
	if (_sockets.empty())
		throw AllPortsFailedException();
	std::cout << "Ports are now listening" << std::endl;
	this->organizePoll();
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
	std::vector<int>	failed_sockets;
	for(std::map<const int, Socket*>::iterator i = _sockets.begin();
			i != _sockets.end(); ++i)
	{
		try {
			i->second->listen();
		}
		catch (const std::exception& e) {
			std::cerr << "Error on port " << i->second->getPort();
			std::cerr << ": " << e.what() << std::endl;
			failed_sockets.push_back(i->second->getFd());
		}
	}
	for (unsigned long i = 0; i < failed_sockets.size(); ++i)
		this->closeSocket(failed_sockets[i]);
}

void	Server::closeSocket(int fd) {
	delete _sockets.at(fd);
	_sockets.erase(fd);
}

void	Server::handlePoll(void) {
	for (std::vector<struct pollfd>::iterator i = _polls.begin();
			 i != _polls.end(); ++i)
	{
		if (isSocket(i->fd))
			this->handleSocket(*i);
		else if (isConnection(i->fd))
			this->handleConnection(*i);
		else
			std::cerr << "Impossble file descriptor found: " << i->fd << std::endl;
	}
}

bool	Server::isSocket(int fd) const {
	try {
		_sockets.at(fd);
		return (true);
	}
	catch (std::exception& e) {
		return (false);
	}
}

bool	Server::isConnection(int fd) const {
	try {
		_connections.at(fd);
		return (true);
	}
	catch (std::exception& e) {
		return (false);
	}
}

void	Server::handleSocket(struct pollfd spoll) {
	if (spoll.revents & POLLIN)
		this->addConnection(spoll.fd);
}

void	Server::handleConnection(struct pollfd spoll) {
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

// maybe refactor this later?
void	Server::organizePoll(void) {
	struct pollfd	temp;
	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN | POLLOUT;
	_polls.clear();
	for (std::map<int, Socket*>::iterator i = _sockets.begin();
		i != _sockets.end(); ++i)
	{
		temp.fd = i->first;
		_polls.push_back(temp);
	}
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
const char*	Server::AllPortsFailedException::what(void) const throw() {
	return ("No single port is working");
}
