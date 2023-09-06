#include "VirtualServer.hpp"
#include "../http/request_utils.hpp"
#include "../parser/parser.hpp"
#include <cctype>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <utility>
#include <fstream>

// Constructors
VirtualServer::VirtualServer(void) : _body_size(1024) {
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
		_error_pages = src._error_pages;
		_body_size = src._body_size;
	}
	return (*this);
}

// Helper Functions Declarations
namespace {
	int				getPort(std::stringstream& stream);
	bool			validClientSize(const std::string& s);
	unsigned long	bodySizeToBytes(const std::string& s);
}

// Methods
std::string	VirtualServer::getName(void) const { return _name; }

unsigned long	VirtualServer::getBodySize(void) const { return _body_size; }

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

int	VirtualServer::interpretAttribute(std::string str, std::fstream& file) {
	std::string			field;
	std::stringstream	stream;

	stream << str;
	stream >> field;
	if (_fields.find(field) == _fields.end())
		throw std::exception();
	else if (field == "port")
		return (getPort(stream));
	if (field == "error_page")
		this->insertErrorCode(stream);
	else if (field == "location")
		this->insertLocation(stream, file);
	else
		this->insertGeneralField(field, stream);
	return (0);
}

void	VirtualServer::insertErrorCode(std::stringstream& stream) {
	std::string			error, path;

	stream >> error;
	stream >> path;
	stream >> std::ws;
	if (error.empty() || path.empty() || !stream.eof())
		throw std::exception();
	_error_pages.insert(std::make_pair(error, path));
}

void	VirtualServer::insertLocation(std::stringstream& stream, std::fstream& file) {
	std::string			path, bracket;

	stream >> path;
	stream >> bracket;
	stream >> std::ws;
	if (!stream.eof() || path.empty() || bracket.empty() || bracket != "{")
		throw std::exception();
	_locations.insert(std::make_pair(path, getLocation(file)));
}

void	VirtualServer::insertGeneralField(std::string field, std::stringstream& stream) {
	std::string			content;

	stream >> content;
	stream >> std::ws;
	if (!stream.eof() || content.empty())
		throw std::exception();
	if (field == "root")
		_root = content;
	else if (field == "server_name")
		_name = content;
	else if (field == "cgi_extension")
		_extension = content;
	else if (field == "body_size") {
		if(!validClientSize(content))
			throw std::exception();
		else
			_body_size = bodySizeToBytes(content);
	}
}

std::string	VirtualServer::getCustomError(std::string code) const {
	std::string	path;
	try {
		path = _error_pages.at(code);
		std::string			content = getFileContent(path, "");
		std::stringstream	response;
		response << "Content-Length: " << content.size() << "\r\n\r\n";
		response << content;
		return (response.str());
	}
	catch(...) {
		return ("");
	}
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
	"server_name",
	"error_page",
	"body_size",
	"location"
};

const std::set<std::string>	VirtualServer::_fields(_fields_array, _fields_array + 7);

// Helper functions implementation
namespace {
int	getPort(std::stringstream& stream) {
	std::string	str;

	stream >> str;
	stream >> std::ws;
	if (str.empty() || !stream.eof())
		throw std::exception();
	int port = std::atoi(str.c_str());
	if (!port)
		throw std::exception();
	return (port);
}

bool	validClientSize(const std::string& s) {
	if (*(s.end() - 1) != 'M' && *(s.end() - 1) != 'K')
		return (false);
	for (std::string::size_type i = 0; i < s.size() - 1; ++i) {
		if (!std::isdigit(s[i]))
			return (false);
	}
	return (true);
}

unsigned long	bodySizeToBytes(const std::string& s) {
	unsigned long	bytes = std::strtoul(s.c_str(), NULL, 10) * 1024;
	if (*(s.end() - 1) == 'M')
		bytes *= 1024;
	return (bytes);
}
}
