#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

#include <string>
#include <map>
#include <set>
#include <exception>

class VirtualServer {
public:
	VirtualServer(void);
	VirtualServer(const VirtualServer& src);
	virtual	~VirtualServer(void);

	VirtualServer&	operator=(const VirtualServer& src);

	std::string	getName(void) const;
	std::string	buildPath(std::string route) const;
	bool		isCgi(std::string route) const;
	int			interpretLine(std::string str);

	struct NoHomeException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::string				_root;
	std::string				_extension;
	std::string				_name;

	void		insertGeneralField(std::string field, std::string content);

	static const char*	_fields_array[];
	static const std::set<std::string>	_fields;
};

#endif
