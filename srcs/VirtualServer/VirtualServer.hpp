#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

#include <fstream>
#include <string>
#include <map>
#include <set>
#include <exception>
#include "../Location/Location.hpp"

class VirtualServer {
public:
	VirtualServer(void);
	VirtualServer(const VirtualServer& src);
	virtual	~VirtualServer(void);

	VirtualServer&	operator=(const VirtualServer& src);

	std::string		getName(void) const;
	std::string		getCustomError(std::string code) const;
	int				interpretAttribute(std::string str, std::fstream& file);
	unsigned long	getBodySize(void) const;
	const Location&	getLocation(std::string route) const;


private:
	std::map<std::string, std::string>	_error_pages;
	std::map<std::string, Location>		_locations;
	std::string							_name;
	unsigned long						_body_size;

	void	insertGeneralField(std::string field, std::stringstream& stream);
	void	insertErrorCode(std::stringstream& stream);
	void	insertLocation(std::stringstream& stream, std::fstream& file);

	static const char*					_fields_array[];
	static const std::set<std::string>	_fields;
};

#endif
