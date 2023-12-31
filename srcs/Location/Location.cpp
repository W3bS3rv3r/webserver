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

namespace {
	std::string searchAndReplace(std::string inString, const std::string& origStr, const std::string& newStr);
	std::string decodeHexInUri(const std::string &inString);
}

//Constructors

Location::Location(std::string name) : _name(name) {
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
		_name = src._name;
		_index = src._index;
		_autoindex = src._autoindex;
		_redirect = src._redirect;
		_upload = src._upload;
		_methods = src._methods;
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
		_autoindex = (content == "on");
	else if (field == "return")
		_redirect = content;
	else if (field == "upload") {
		if (content.find("../") != std::string::npos)
			throw InvalidSyntaxException();
		_upload = content;
	}
}

bool	Location::checkIntegrity(void) const {
	if (_root.empty())
		return (false);
	return (true);
}

Response	Location::handleRequest(std::string method, std::string route,
		const std::string& request, const Socket& socket) const
{
	size_t		argsPos;
	std::string	arguments = "";

	if (!_redirect.empty())
		return (Response(this->redirectResponse()));
	if (!_methods.empty() && _methods.find(method) == _methods.end())
		throw MethodNotAllowedException("");
	std::string path = decodeHexInUri(this->buildPath(route));
	argsPos = path.find('?');
	if (argsPos != std::string::npos)
	{
		arguments = path.substr(argsPos + 1);
		path = path.substr(0, argsPos);
	}
	if (method == "GET")
		return (this->callGet(path, request, arguments));
	else if (method == "DELETE")
		return (this->callDelete(path));
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

Response	Location::callDelete(std::string path) const {
	if (_upload.empty() || path.substr(_root.size() + 1, _upload.size()) != _upload)
		throw MethodNotAllowedException("");
	return (Response(del(path)));
}

Response	Location::callGet(std::string path, const std::string& request, const std::string& arguments) const {
	DIR*		dir;
	
	if (!_extension.empty() &&
		path.rfind(_extension) == path.size() - _extension.size())
	{
		return (cgiGet(path, request, arguments, _root + "/" + _upload));
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
	if (_extension.empty())
		throw NotImplementedException("");
	else if (_upload.empty())
		throw MethodNotAllowedException("");
	DIR* dir = opendir((_root + "/" + _upload).c_str());
	if (!dir)
		throw InternalServerErrorException("");
	closedir(dir);
	if (path.rfind(_extension) == path.size() - _extension.size())
		return (cgiPost(path, request, socket, _root + "/" + _upload));
	throw MethodNotAllowedException("");
}

std::string	Location::buildPath(std::string route) const {
	route.erase(0, _name.size());
	std::string	path = _root + "/" + route;
	DIR*		dir;

	if ( (dir = opendir(path.c_str())) ) {
		closedir(dir);
		if (!_index.empty() && !access((path + "/" + _index).c_str(), F_OK))
			path += "/" + _index;
		else if (!_autoindex)
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
	"autoindex",
	"upload",
	"methods",
	"return",
	"upload"
};

const std::set<std::string>	Location::_fields(_fields_array, _fields_array + 7);

namespace {
std::string searchAndReplace(std::string inString, const std::string& origStr, const std::string& newStr)
{
	std::string	inStringCopy = inString;
	std::string	resultString;
	size_t		curPos = 0;

	while (true) {
		curPos = inStringCopy.find(origStr, curPos);
		if (curPos == std::string::npos) {
			resultString += inStringCopy;
			break;
		}
		resultString += inStringCopy.substr(0, curPos) + newStr;
		curPos += origStr.length();
		inStringCopy = inStringCopy.substr(curPos);
		curPos = 0;
	}
	return (resultString);
}

std::string decodeHexInUri(const std::string &inString)
{
	std::string	copyString = inString;

	for (size_t i = 0; i < copyString.size(); ++i) {
		int	intValue;
		if (copyString[i] == '%' && i + 2 < copyString.size()) {
			std::string			hexStr = copyString.substr(i + 1, 2);
			std::stringstream	ss;
			char				c;

			ss << std::hex << hexStr;
			ss >> intValue;
			c = static_cast<char>(intValue);
			copyString = searchAndReplace(copyString, "%" + hexStr, std::string(1, c));
		}
	}
	return (copyString);
}
}