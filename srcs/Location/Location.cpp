#include "Location.hpp"
#include <sstream>

//Constructors

Location::Location(void) {}

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
		_extension == content;
}

bool	Location::checkIntegrity(void) const {
	if (_root.empty())
		return (false);
	return (true);
}

//Static variables
const char*	Location::_fields_array[] = {
	"root",
	"cgi_extension"
};

const std::set<std::string>	Location::_fields(_fields_array, _fields_array + 2);
