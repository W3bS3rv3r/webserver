#ifndef POST_HPP
# define POST_HPP

#include <string>
#include "../Response/Response.hpp"
#include "../Socket/Socket.hpp"

Response	cgiPost(std::string path, const std::string& request,
					const Socket& socket, std::string upload);

#endif
