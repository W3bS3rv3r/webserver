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

	std::string		getName(void) const;
	std::string		buildPath(std::string route) const;
	std::string		getCustomError(std::string code) const;
	bool			isCgi(std::string route) const;
	int				interpretLine(std::string str);
	unsigned long	getBodySize(void) const;

	struct NoHomeException: std::exception {
		const char*	what(void) const throw();
	};
private:
	std::map<std::string, std::string>	_error_pages;
	std::string							_root;
	std::string							_extension;
	std::string							_name;
	unsigned long						_body_size;

	void	insertGeneralField(std::string field, std::stringstream& stream);
	void	insertErrorCode(std::stringstream& stream);

	static const char*					_fields_array[];
	static const std::set<std::string>	_fields;
};

#endif
