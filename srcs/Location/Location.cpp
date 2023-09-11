#include "Location.hpp"
#include <dirent.h>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include "../parser/parser.hpp"
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
		_methods = src._methods;
		_autoindex = src._autoindex;
		_redirect = src._redirect;
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
	if (field == "methods")
		this->insertMethods(stream);
	else
		this->insertGeneralField(field, stream);
}

void	Location::insertMethods(std::stringstream& stream) {
	std::string	temp;

	stream >> std::ws;
	if (stream.eof())
		throw InvalidSyntaxException();
	while (!stream.eof()) {
		stream >> temp;
		stream >> std::ws;
		_methods.insert(temp);
	}
}

void	Location::insertGeneralField(std::string field, std::stringstream& stream) {
	std::string	content;

	stream >> content;
	stream >> std::ws;
	if (!stream.eof() || content.empty())
		throw InvalidSyntaxException();
	if (field == "root")
		_root = content;
	else if (field == "cgi_extension")
		_extension = content;
	else if (field == "index")
		_index = content;
	else if (field == "autoindex")
		_autoindex = ((content == "on") ? content : "");
	else if (field == "return")
		_redirect = content;
}

bool	Location::checkIntegrity(void) const {
	if (_root.empty())
		return (false);
	return (true);
}

Response	Location::handleRequest(std::string method, std::string route,
		const std::string& request, const Socket& socket) const
{
	if (!_redirect.empty())
		return (Response(this->redirectResponse()));
	if (!_methods.empty() && _methods.find(method) == _methods.end())
		throw MethodNotAllowedException("");
	std::string	path = this->buildPath(route);
	if (method == "GET")
		return (this->callGet(path, request));
	else if (method == "DELETE")
		return (Response(del(path)));
	else if (method == "POST")
		return (this->callPost(path, request, socket));
	else
		throw NotImplementedException("");
}

std::string	Location::redirectResponse(void) const {
	std::stringstream	response;

	response << "HTTP/1.1 301 Moved Permanently\r\n";
	response << "Location: " + _redirect + "\r\n";
	response << "Content-Length: 0\r\n\r\n";
	return (response.str());
}

Response	Location::callGet(std::string path, const std::string& request) const {
	DIR*		dir;

	if (!_extension.empty() &&
		path.rfind(_extension) == path.size() - _extension.size())
	{
		return (cgiGet(path, request));
	}
	else if ((dir = opendir(path.c_str()))) {
		closedir(dir);
		return (Response(getDir(path)));
	}
	return (Response(get(path)));
}

Response	Location::callPost(std::string path, const std::string& request,
		const Socket& socket) const
{
	if (!_extension.empty() &&
		path.rfind(_extension) == path.size() - _extension.size())
	{
		return (cgiPost(path, request, socket));
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
		else if (_autoindex.empty())
			throw ForbiddenException("");
	}
	return (path);
}

// Exceptions
const char*	Location::NoHomeException::what(void) const throw() {
	return ("HOME environment variable not set, unable to construct default path");
}


//Static variables
const char*	Location::_fields_array[] = {
	"root",
	"cgi_extension",
	"index",
	"methods",
	"autoindex",
	"return"
};

const std::set<std::string>	Location::_fields(_fields_array, _fields_array + 6);
