#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <sstream>
#include <set>
#include "../Response/Response.hpp"

class Socket;

class Location {
public:
	Location(std::string name);
	Location(const Location& src);
	virtual	~Location(void);

	Location&	operator=(const Location& src);

	void		interpretAttribute(std::string line);
	bool		checkIntegrity(void) const;
	Response	handleRequest(std::string method, std::string route,
				const std::string& request, const Socket& socket) const;

	struct NoHomeException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::string				_root;
	std::string				_extension;
	std::string				_name;
	std::string				_index;
	std::string				_autoindex;
	std::string				_redirect;
	std::string				_upload;
	std::set<std::string>	_methods;

	static const char*					_fields_array[];
	static const std::set<std::string>	_fields;

	void		insertGeneralField(std::string field, std::stringstream& stream);
	void		insertMethods(std::stringstream& stream);
	Response	callDelete(std::string route) const;
	Response	callGet(std::string route, const std::string& request, const std::string& arguments) const;
	Response	callPost(std::string route, const std::string& request, const Socket& socket) const;
	std::string	buildPath(std::string route) const;
	std::string	redirectResponse(void) const;
};

#endif
