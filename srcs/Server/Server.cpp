#include "Server.hpp"
#include "../parser/parser.hpp"
#include <exception>
#include <iostream>
#include <sys/poll.h>
#include <utility>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <fstream>

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
void	Server::init(std::string path) {
	std::string			buff;
	std::stringstream	str;
	std::fstream		file(path.c_str());

	while(std::getline(file >> std::ws, buff)) {
		if (buff == "server {")
			this->addSocket(getVServer(file));
		else {
			std::cerr << "at line: '" << buff << "'" << std::endl;
			throw InvalidSyntaxException();
		}
	}
	std::cout << "Config file parsed" << std::endl;
}

void	Server::addSocket(std::pair<unsigned short, VirtualServer> p) {
	for (std::map<int, Socket*>::iterator i = _sockets.begin(); i != _sockets.end(); ++i) {
		if (i->second->getPort() == p.first) {
			i->second->addVserver(p.second);
			return ;
		}
	}
	Socket*	socket = new Socket(p.first, p.second);
	 _sockets.insert(std::make_pair(socket->getFd(), socket));
}

void	Server::run(void) {
	this->startListening();
	if (_sockets.empty())
		throw AllPortsFailedException();
	std::cout << "Ports are now listening" << std::endl;
	this->setPoll();
	while (1) {
		poll(_polls.data(), _polls.size(), 1000);
		this->handlePoll();
		if (_changed) {
			this->removeOldFds();
			this->addNewFds();
			_changed = false;
		}
	}
}

void	Server::setPoll(void) {
	struct pollfd	temp;
	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN | POLLOUT;
	for (std::map<int, Socket*>::iterator i = _sockets.begin();
		i != _sockets.end(); ++i)
	{
		temp.fd = i->first;
		_polls.push_back(temp);
	}
}

void	Server::removeOldFds(void) {
	struct pollfd	temp;
	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN | POLLOUT;

	for (std::vector<int>::size_type i = 0; i < _removed_fds.size(); ++i) {
		for (std::vector<struct pollfd>::size_type j = 0; j < _polls.size(); ++j) {
			if (_polls[j].fd == _removed_fds[i])
				_polls.erase(_polls.begin() + j);
		}
	}
	_removed_fds.clear();
}

void	Server::addNewFds(void) {
	struct pollfd	temp;
	memset(&temp, 0, sizeof(temp));
	temp.events = POLLIN | POLLOUT;

	for (std::vector<int>::size_type i = 0; i < _new_fds.size(); ++i) {
		temp.fd = _new_fds[i];
		_polls.push_back(temp);
	}
	_new_fds.clear();
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
	_removed_fds.push_back(fd);
}

void	Server::handlePoll(void) {
	for (std::vector<struct pollfd>::iterator i = _polls.begin();
			 i != _polls.end(); ++i)
	{
		if (this->isSocket(i->fd))
			this->handleSocket(*i);
		else if (this->isConnection(i->fd))
			this->handleConnection(*i);
		else
			std::cerr << "Impossible file descriptor found: " << i->fd << std::endl;
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
	std::pair<std::map<int, Connection*>::iterator, bool>	ret_val;
	Connection*	cn = _sockets.at(socket_fd)->acceptConnection();
	ret_val = _connections.insert(std::make_pair(cn->getFd(), cn));
	_changed = true;
	_new_fds.push_back(ret_val.first->second->getFd());
}

void	Server::closeConnection(int fd) {
	delete _connections.at(fd);
	_connections.erase(fd);
	_changed = true;
	_removed_fds.push_back(fd);
}

//EXCEPTIONS
const char*	Server::DuplicateException::what(void) const throw() {
	return ("FD already in use");
}
const char*	Server::AllPortsFailedException::what(void) const throw() {
	return ("No single port is working");
}
