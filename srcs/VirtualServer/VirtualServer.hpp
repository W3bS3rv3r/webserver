#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

#include <string>
#include <map>
#include <exception>

class VirtualServer {
public:
	VirtualServer(std::map<std::string, std::string> parameters);
	VirtualServer(const VirtualServer& src);
	virtual	~VirtualServer(void);

	VirtualServer&	operator=(const VirtualServer& src);

	std::string	getName(void) const;
	std::string	buildPath(std::string route) const;
	bool		isCgi(std::string route) const;

	struct NoHomeException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::string				_root;
	std::string				_extension;
	std::string				_name;
};

#endif
