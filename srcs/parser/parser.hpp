#ifndef PARSER_HPP
# define PARSER_HPP

#include <fstream>
#include <exception>
#include "../Socket/Socket.hpp"

Socket*	newSocket(std::fstream& file);
bool	validFieldName(std::string field_name);

struct InvalidSyntaxException: std::exception {
	const char*	what(void) const throw();
};

#endif // !PARSER_HPP
