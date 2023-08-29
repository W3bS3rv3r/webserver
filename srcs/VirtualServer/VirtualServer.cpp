#include "VirtualServer.hpp"
#include <cstdlib>
#include <exception>
#include <sstream>
#include <utility>
#include <fstream>

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
		_error_pages = src._error_pages;
	}
	return (*this);
}

// Methods
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
}

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
}

std::string	VirtualServer::getCustomError(std::string code) const {
	std::string	path;
	try {
		path = _error_pages.at(code);
		std::string		content;
		std::string		buff;
		std::fstream	file(path.c_str(), std::ios_base::in);
		while(std::getline(file, buff)) {
			try {
				content += buff;
			}
			catch (const std::exception& e) {
				file.close();
				throw std::exception();
			}
		}
		file.close();
		if (content.empty())
			throw std::exception();
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
	"error_page"
};

const std::set<std::string>	VirtualServer::_fields(_fields_array, _fields_array + 5);
