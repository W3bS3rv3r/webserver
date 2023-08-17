#include "VirtualServer.hpp"
#include <cstdlib>

// Constructors
VirtualServer::VirtualServer(std::map<std::string, std::string> parameters) {
	if (parameters.find("root") != parameters.end())
		_root = parameters["root"];
	else {
	    const char*  home = getenv("HOME");
		if (!home)
			throw VirtualServer::NoHomeException();
		_root += home;
		_root += "/webserver";
	}
	if (parameters.find("cgi_extension") != parameters.end())
		_extension = parameters["cgi_extension"];
	if (parameters.find("server_name") != parameters.end())
		_name = parameters["server_name"];
}

VirtualServer::VirtualServer(const VirtualServer& src) { *this = src; }

VirtualServer::~VirtualServer(void) {}

// Operators
VirtualServer&	VirtualServer::operator=(const VirtualServer& src) {
	if (this != &src) {
		_root = src._root;
		_extension = src._extension;
		_name = src._name;
	}
	return (*this);
}

// Methods
std::string	VirtualServer::getName(void) const { return _name; }
std::string	VirtualServer::getRoot(void) const { return _root; }
std::string	VirtualServer::getExtension(void) const { return _extension; }

// Exceptions
const char*	VirtualServer::NoHomeException::what(void) const throw() {
	return ("HOME environment variable not set");
}
