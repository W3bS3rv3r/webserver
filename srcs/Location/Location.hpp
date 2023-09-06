#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <set>

class Location {
public:
	Location(void);
	Location(const Location& src);
	virtual	~Location(void);

	Location&	operator=(const Location& src);

	void	interpretAttribute(std::string line);
	bool	checkIntegrity(void) const;
private:
	std::string	_root;
	std::string	_extension;

	static const char*					_fields_array[];
	static const std::set<std::string>	_fields;

	void	insertGeneralField(std::string field, std::string content);
};

#endif
