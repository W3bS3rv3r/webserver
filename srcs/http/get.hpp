#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "../Response/Response.hpp"

Response	cgiGet(std::string path, const std::string& request);
std::string	get(std::string path);

#endif
