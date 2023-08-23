#include "VirtualServer.hpp"
#include <cstdlib>
#include <sstream>

// Constructors
VirtualServer::VirtualServer(void) {
    const char*  home = getenv("HOME");
	if (!home)
		throw VirtualServer::NoHomeException();
	_root += home;
	_root += "/webserver";
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

std::string	VirtualServer::buildPath(std::string route) const {
	return _root + route;
}

bool	VirtualServer::isCgi(std::string route) const {
	if (!_extension.empty() &&
		route.rfind(_extension) == route.size() - _extension.size())
	{
		return (true);
	}
	return (false);
}

int	VirtualServer::interpretLine(std::string str) {
	std::string			field, content;
	std::stringstream	stream;

	stream << str;
	stream >> field;
	if (_fields.find(field) == _fields.end())
		return (-1);
	stream >> content;
	stream >> std::ws;
	if (!stream.eof() || content.empty())
		return (-1);
	if (field == "port")
		return (std::atoi(content.c_str()));
	this->insertGeneralField(field, content);
	return (0);
}

void	VirtualServer::insertGeneralField(std::string field, std::string content) {
	if (field == "root")
		_root = content;
	else if (field == "server_name")
		_name = content;
	else if (field == "cgi_extension")
		_extension = content;
}

// Exceptions
const char*	VirtualServer::NoHomeException::what(void) const throw() {
	return ("HOME environment variable not set");
}

// Static variables
const char*	VirtualServer::_fields_array[] = {
	"port",
	"root",
	"cgi_extension",
	"server_name"
};

const std::set<std::string>	VirtualServer::_fields(_fields_array, _fields_array + 4);
