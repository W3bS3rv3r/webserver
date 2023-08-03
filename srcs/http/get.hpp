#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "../Response/Response.hpp"

Response	cgiGet(std::string path);
std::string	get(std::string path);

#endif
