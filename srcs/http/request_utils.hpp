#ifndef REQ_UTILS_HPP
# define REQ_UTILS_HPP

#include <string>

std::string	getHeaderValue(const std::string& request, const std::string& headerName);

#endif