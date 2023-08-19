#ifndef POST_HPP
# define POST_HPP

#include <string>
#include "../Response/Response.hpp"

Response	cgiPost(std::string path, std::string request);

#endif
