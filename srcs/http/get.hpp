#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "../Response/Response.hpp"

Response	cgiGet(std::string path, const std::string& request, const std::string& arguments, std::string upload);
std::string	get(std::string path);
std::string	getDir(std::string path);

#endif
