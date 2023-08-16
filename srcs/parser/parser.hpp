#ifndef PARSER_HPP
# define PARSER_HPP

#include <fstream>
#include <map>
#include <exception>
#include "../Socket/Socket.hpp"

Socket*								newSocket(std::fstream& file);
bool								validFieldName(std::string field_name);
std::map<std::string, std::string>	readParameters(std::fstream& file);

struct InvalidSyntaxException: std::exception {
	const char*	what(void) const throw();
};

#endif // !PARSER_HPP
