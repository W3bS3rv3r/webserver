#ifndef PARSER_HPP
# define PARSER_HPP

#include <fstream>
#include <map>
#include <exception>
#include "../Socket/Socket.hpp"

std::pair<unsigned short, VirtualServer>	getVServer(std::fstream& file);


bool	validFieldName(std::string field_name);

struct InvalidSyntaxException : std::exception {
	const char*	what(void) const throw();
};

#endif // !PARSER_HPP
