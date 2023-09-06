#include "Location.hpp"
#include <sstream>
#include <cstdlib>
#include "../http/get.hpp"
#include "../http/post.hpp"
#include "../http/delete.hpp"
#include "../HTTPException/HTTPException.hpp"

//Constructors

Location::Location(void) {
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
}

bool	Location::checkIntegrity(void) const {
	if (_root.empty())
		return (false);
	return (true);
}

Response	Location::handleRequest(std::string method, std::string route,
		const std::string& request) const
{
	if (method == "GET")
		return (this->callGet(route, request));
	else if (method == "DELETE")
		return (Response(del(_root + route, request)));
	else if (method == "POST")
		return (this->callPost(route, request));
	else
		throw ServiceUnavailableException("");
}

Response	Location::callGet(std::string route, const std::string& request) const {
	if (!_extension.empty() &&
		route.rfind(_extension) == route.size() - _extension.size())
	{
		return (cgiGet(_root + route, request));
	}
	return (Response(get(_root + route, request)));
}

Response	Location::callPost(std::string route, const std::string& request) const {
	if (!_extension.empty() &&
		route.rfind(_extension) == route.size() - _extension.size())
	{
		return (cgiPost(_root + route, request));
	}
	throw MethodNotAllowedException("");
}

// Exceptions
const char*	Location::NoHomeException::what(void) const throw() {
	return ("HOME environment variable not set");
}


//Static variables
const char*	Location::_fields_array[] = {
	"root",
	"cgi_extension"
};

const std::set<std::string>	Location::_fields(_fields_array, _fields_array + 2);
