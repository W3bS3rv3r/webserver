#ifndef CGI_HPP
# define CGI_HPP

#include <string>

class CGI {
public:
	CGI(void);
	~CGI(void);

	CGI(const std::string& path, const std::string& root, const std::string& request);

	bool isPathValid();
	void runCGI();

private:
	std::string		_cgi_path;
	std::string		_full_req;

	CGI(const CGI& src);
	CGI&	operator=(const CGI& src);

};

#endif