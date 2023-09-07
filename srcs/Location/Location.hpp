#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <set>
#include "../Response/Response.hpp"

class Location {
public:
	Location(void);
	Location(const Location& src);
	virtual	~Location(void);

	Location&	operator=(const Location& src);

	void		interpretAttribute(std::string line);
	bool		checkIntegrity(void) const;
	Response	handleRequest(std::string method, std::string route, const std::string& request) const;

	struct NoHomeException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::string	_root;
	std::string	_extension;
	std::string	_name;
	std::string	_index;

	static const char*					_fields_array[];
	static const std::set<std::string>	_fields;

	void		insertGeneralField(std::string field, std::string content);
	Response	callGet(std::string route, const std::string& request) const;
	Response	callPost(std::string route, const std::string& request) const;
	std::string	buildPath(std::string route) const;
};

#endif
