#include "Location.hpp"
#include <sstream>
#include <dirent.h>
#include <cstdlib>
#include <unistd.h>
#include "../http/get.hpp"
#include "../http/post.hpp"
#include "../http/delete.hpp"
#include "../HTTPException/HTTPException.hpp"

//Constructors

Location::Location(void) : _index("index.html") {
    const char*  home = getenv("HOME");
	if (!home)
		throw Location::NoHomeException();
	_root += home;
	_root += "/webserver";
}

Location::Location(const Location& src) { *this = src; }

Location::~Location(void) {}

//Operators
Location&	Location::operator=(const Location& src) {
	if (this != &src) {
		_root = src._root;
		_extension = src._extension;
		_index = src._index;
	}
	return (*this);
}

//Methods
void	Location::interpretAttribute(std::string line) {
	std::string			field, content;
	std::stringstream	stream;

	stream << line;
	stream >> field;
	if (_fields.find(field) == _fields.end())
		throw std::exception();
	stream >> content;
	stream >> std::ws;
	if (!stream.eof() || content.empty())
		throw std::exception();
	this->insertGeneralField(field, content);
}

void	Location::insertGeneralField(std::string field, std::string content) {
	if (field == "root")
		_root = content;
	else if (field == "cgi_extension")
		_extension = content;
	else if (field == "index")
		_index = content;
}

bool	Location::checkIntegrity(void) const {
	if (_root.empty())
		return (false);
	return (true);
}

Response	Location::handleRequest(std::string method, std::string route,
		const std::string& request) const
{
	std::string	path = this->buildPath(route);
	if (method == "GET")
		return (this->callGet(path, request));
	else if (method == "DELETE")
		return (Response(del(path)));
	else if (method == "POST")
		return (this->callPost(path, request));
	else
		throw ServiceUnavailableException("");
}

Response	Location::callGet(std::string path, const std::string& request) const {
	if (!_extension.empty() &&
		path.rfind(_extension) == path.size() - _extension.size())
	{
		return (cgiGet(path, request));
	}
	return (Response(get(path)));
}

Response	Location::callPost(std::string path, const std::string& request) const {
	if (!_extension.empty() &&
		path.rfind(_extension) == path.size() - _extension.size())
	{
		return (cgiPost(path, request));
	}
	throw MethodNotAllowedException("");
}

std::string	Location::buildPath(std::string route) const {
	std::string	path = _root + route;
	DIR*		dir;

	if ( (dir = opendir(path.c_str())) ) {
		closedir(dir);
		if (!access((path + _index).c_str(), F_OK))
			path += _index;
		else 
			throw ForbiddenException("");
	}
	return (path);
}

// Exceptions
const char*	Location::NoHomeException::what(void) const throw() {
	return ("HOME environment variable not set");
}


//Static variables
const char*	Location::_fields_array[] = {
	"root",
	"cgi_extension",
	"index"
};

const std::set<std::string>	Location::_fields(_fields_array, _fields_array + 3);
