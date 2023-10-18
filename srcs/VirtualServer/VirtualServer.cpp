#include "VirtualServer.hpp"
#include "../http/request_utils.hpp"
#include "../parser/parser.hpp"
#include <cctype>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <fstream>

// Constructors
VirtualServer::VirtualServer(void) : _body_size(1024) {}

VirtualServer::VirtualServer(const VirtualServer& src) { *this = src; }

VirtualServer::~VirtualServer(void) {}

// Operators
VirtualServer&	VirtualServer::operator=(const VirtualServer& src) {
	if (this != &src) {
		_name = src._name;
		_error_pages = src._error_pages;
		_body_size = src._body_size;
		_locations = src._locations;
	}
	return (*this);
}

// Helper Functions Declarations
namespace {
	int						getPort(std::stringstream& stream);
	bool					validClientSize(const std::string& s);
	unsigned long			bodySizeToBytes(const std::string& s);
	std::string::size_type	matchingCharacters(std::string s1, std::string s2);
}

// Methods
std::string	VirtualServer::getName(void) const { return _name; }

unsigned long	VirtualServer::getBodySize(void) const { return _body_size; }

const Location&	VirtualServer::getLocation(std::string route) const {
	std::string::size_type	max = 0;
	std::string				best_match;
	std::map<std::string, Location>::const_iterator i;

	for	(i = _locations.begin(); i != _locations.end(); ++i) {
		if (max > i->first.size())
			continue ;
		std::string::size_type	tmp = matchingCharacters(route, i->first);
		if (tmp > max || best_match.empty()) {
			max = tmp;
			best_match = i->first;
		}
		if (max == route.size())
			break ;
	}
	return (_locations.at(best_match));
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
	_locations.insert(std::make_pair(path, ::getLocation(file, path)));
}

void	VirtualServer::insertGeneralField(std::string field, std::stringstream& stream) {
	std::string			content;

	stream >> content;
	stream >> std::ws;
	if (!stream.eof() || content.empty())
		throw std::exception();
	if (field == "server_name")
		_name = content;
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

// Static variables
const char*	VirtualServer::_fields_array[] = {
	"port",
	"server_name",
	"error_page",
	"body_size",
	"location"
};

const std::set<std::string>	VirtualServer::_fields(_fields_array, _fields_array + 5);

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

std::string::size_type	matchingCharacters(std::string s1, std::string s2) {
	std::string::size_type	i = 0;
	if (s2[s2.size() - 1] != '/')
		s2.push_back('/');
	while (s1[i] == s2[i] && i < s1.size() && i < s2.size())
		++i;
	if (!s2[i] || (s2[i] == '/' && s2.size() == i +1 && (s1[i] == '/' || !s1[i])))
		return (i);
	return (0);
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
